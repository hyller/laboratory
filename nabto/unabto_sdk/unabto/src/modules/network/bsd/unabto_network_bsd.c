/*
 * Copyright (C) 2008-2013 Nabto - All Rights Reserved.
 */
#define NABTO_LOG_MODULE_CURRENT NABTO_LOG_MODULE_NETWORK

#include <unabto/unabto_env_base.h>
#include <unabto/unabto_external_environment.h>
#include <modules/network/bsd/unabto_network_bsd.h>
#include <modules/network/poll/unabto_network_poll_api.h>
#include <unabto/unabto_context.h>
#include <unabto/unabto_main_contexts.h>
#include <unabto/unabto_common_main.h>

#include <modules/list/utlist.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#ifdef __MACH__
#include <net/if.h>
#else
#include <linux/if.h>
#endif

#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct socketListElement {
    nabto_socket_t socket;
    struct socketListElement *prev;
    struct socketListElement *next;
} socketListElement;

static NABTO_THREAD_LOCAL_STORAGE struct socketListElement* socketList = 0;



bool nabto_init_socket(uint32_t localAddr, uint16_t* localPort, nabto_socket_t* sock) {
    
    nabto_socket_t sd;
    nabto_main_context* nmc;
    const char* interface;
    socketListElement* se;
    
    NABTO_LOG_TRACE(("Open socket: ip=" PRIip ", port=%u", MAKE_IP_PRINTABLE(localAddr), (int)*localPort));
    
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd == -1) {
        NABTO_LOG_ERROR(("Unable to create socket: (%i) '%s'.", errno, strerror(errno)));
        return false;
    }

#ifdef SO_BINDTODEVICE
    // Used for multiple interfaces
    nmc = unabto_get_main_context();
    interface = nmc->nabtoMainSetup.interfaceName;                
    if (interface != NULL) {
        if (setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, (void *)interface, strlen(interface)+1) < 0) {
            NABTO_LOG_FATAL(("Unable to bind to interface: %s, '%s'.", interface, strerror(errno)));
        }
        NABTO_LOG_TRACE(("Bound to device '%s'.", interface));
    }

    {
        // only reuse when user has explicitly set local port and bound to a specific device as this indicates user
        // knows what he is doing...
        int allowReuse = (localPort && *localPort != 0 && interface != NULL);
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (void *) &allowReuse, sizeof(int)))
        {
            NABTO_LOG_FATAL(("Unable to set option: (%i) '%s'.", errno, strerror(errno)));
            return false;
        }
    }
#endif    
    {
        struct sockaddr_in sa;
        int status;
        memset(&sa, 0, sizeof(sa));
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(localAddr);
        sa.sin_port = htons(*localPort);
        
        status = bind(sd, (struct sockaddr*)&sa, sizeof(sa));
        
        if (status < 0) {
            NABTO_LOG_ERROR(("Unable to bind socket: (%i) '%s'.", errno, strerror(errno)));
            close(sd);
            return false;
        }
#if UNABTO_NETWORK_BSD_NONBLOCKING
        nabto_bsd_set_nonblocking(&sd);
#endif
        *sock = sd;
    }
    {
        struct sockaddr_in sao;
        socklen_t len = sizeof(sao);
        if ( getsockname(*sock, (struct sockaddr*)&sao, &len) != -1) {
            *localPort = htons(sao.sin_port);
        } else {
            NABTO_LOG_ERROR(("Unable to get local port of socket: (%i) '%s'.", errno, strerror(errno)));
        }
    }
    
    NABTO_LOG_TRACE(("Socket opened: ip=" PRIip ", port=%u", MAKE_IP_PRINTABLE(localAddr), (int)*localPort));
    
    
    se = (socketListElement*)malloc(sizeof(socketListElement));
    
    if (!se) {
        NABTO_LOG_FATAL(("Malloc of a single small list element should not fail!"));
        close(sd);
        return false;
    }

    se->socket = sd;
    DL_APPEND(socketList, se);
    
    return true;
}

void nabto_close_socket(nabto_socket_t* sock) {
    if (sock && *sock != NABTO_INVALID_SOCKET) {
        socketListElement* se;
        socketListElement* found = 0;
        DL_FOREACH(socketList,se) {
            if (se->socket == *sock) {
                found = se;
                break;
            }
        }
        if (!found) {
            NABTO_LOG_ERROR(("Socket %i Not found in socket list", *sock));
        } else {
            DL_DELETE(socketList, se);
            free(se);
        }

        close(*sock);
        *sock = NABTO_INVALID_SOCKET;

    }
}

ssize_t nabto_read(nabto_socket_t sock,
                   uint8_t*       buf,
                   size_t         len,
                   uint32_t*      addr,
                   uint16_t*      port)
{
    int opt = 1;
    ssize_t res;
    struct iovec iov[1];
    char cmbuf[256];
    struct sockaddr_in sa;
    struct cmsghdr *cmsg;
    nabto_endpoint ep;
    struct msghdr mh = {
        .msg_name = &sa,
        .msg_namelen = sizeof(sa),
        .msg_control = cmbuf,
        .msg_controllen = sizeof(cmbuf),
        .msg_iov = iov,
        .msg_iovlen = 1,
        .msg_flags = 0
    };

    iov[0].iov_base = buf;
    iov[0].iov_len = len;
    
    setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &opt, sizeof(opt));
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;


    res = recvmsg(sock, &mh, 0);
    if (res == -1) {
        return 0;
    } else if (mh.msg_flags & MSG_TRUNC) {
        NABTO_LOG_ERROR(("ERROR: buffer too small"));
        return 0;
    }

    *addr = ntohl(sa.sin_addr.s_addr);
    *port = ntohs(sa.sin_port);

    ep.addr = *addr;
    ep.port = *port;
    NABTO_NOT_USED(ep);
    NABTO_LOG_BUFFER(NABTO_LOG_SEVERITY_BUFFERS, ("nabto_read: " PRIep, MAKE_EP_PRINTABLE(ep)), buf, res);
  
    // iterate through all the control headers
    for ( cmsg=CMSG_FIRSTHDR(&mh); cmsg != NULL; cmsg = CMSG_NXTHDR(&mh, cmsg)) {
        if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_PKTINFO) {
            nabto_main_context* nmc;
            struct in_addr myAddr;
            myAddr = ((struct in_pktinfo*)CMSG_DATA(cmsg))->ipi_addr;
            // set our actual address instead of 0.0.0.0 when using inaddr_any to allow local clients to connect directly
            nmc = unabto_get_main_context();
            nmc->socketGSPLocalEndpoint.addr = ntohl(myAddr.s_addr);
        }
    }

    return res;
}



ssize_t nabto_write(nabto_socket_t sock,
                    const uint8_t* buf,
                    size_t         len,
                    uint32_t       addr,
                    uint16_t       port)
{
    int res;
    struct sockaddr_in sa;
    nabto_endpoint ep;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(addr);
    sa.sin_port = htons(port);
    ep.addr = addr;
    ep.port = port;
    NABTO_NOT_USED(ep);
    NABTO_LOG_BUFFER(NABTO_LOG_SEVERITY_BUFFERS, ("nabto_write size: %" PRIsize ", " PRIep, len, MAKE_EP_PRINTABLE(ep)),buf, len);
    res = sendto(sock, buf, (int)len, 0, (struct sockaddr*)&sa, sizeof(sa));
    if (res < 0) {
        int status = errno;
        NABTO_NOT_USED(status);
        NABTO_LOG_ERROR(("ERROR: %s (%i) in nabto_write()", strerror(status), (int) status));
    }
    return res;
}

/**
 * Return a list of sockets which can be read from.
 */
uint16_t nabto_read_events(nabto_socket_t* sockets, uint16_t maxSockets, int timeout) {
    int nfds;
    uint16_t resultN;
    fd_set read_fds;
    unsigned int max_fd = 0;
    struct timeval timeout_val;
    socketListElement* se;
        
    FD_ZERO(&read_fds);
    max_fd = 0;



    DL_FOREACH(socketList, se) {
        FD_SET(se->socket, &read_fds);
        max_fd = MAX(max_fd, se->socket);
    }

    timeout_val.tv_sec = timeout/1000;
    timeout_val.tv_usec = (timeout*1000)%1000000;
    
    nfds = select(max_fd+1, &read_fds, NULL, NULL, &timeout_val);

    resultN = 0;

    if (nfds == 0) {
        return 0;
    }

    if (nfds < 0) {
        NABTO_LOG_ERROR(("Select returned error"));
        return 0;
    }

    DL_FOREACH(socketList, se) {
        if (resultN >= maxSockets) {
            break;
        }

        if (FD_ISSET(se->socket, &read_fds)) {
            sockets[resultN] = se->socket;
            resultN++;
        }
    }
    return resultN;
}


void unabto_network_select_add_to_read_fd_set(fd_set* readFds, int* maxReadFd) {
    socketListElement* se;
    DL_FOREACH(socketList, se) {
        FD_SET(se->socket, readFds);
        *maxReadFd = MAX(*maxReadFd, se->socket);
    }
}

void unabto_network_select_read_sockets(fd_set* readFds) {
    socketListElement* se;
    DL_FOREACH(socketList, se) {
        if (FD_ISSET(se->socket, readFds)) {
            unabto_read_socket(se->socket);
        }
    }
}

struct pollfd* unabto_network_poll_add_to_set(struct pollfd* begin, struct pollfd* end)
{
    socketListElement* se;
    DL_FOREACH(socketList, se) {
        if (begin != end) {
            begin->fd = se->socket;
            begin->events = POLLIN;
            begin++;
        }
    }
    return begin;
}

void unabto_network_poll_read_sockets(struct pollfd* begin, struct pollfd* end)
{
    while (begin != end) {
        if (begin->revents == POLLIN) {
            unabto_read_socket(begin->fd);
        }
        begin++;
    }
}