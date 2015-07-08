/* ************************************
*《精通Windows API》
* 示例代码
* client.c
* 14.1 socket通信
**************************************/
/* 头文件 */
#include <stdio.h>
#include "winsock2.h"
/* 常量 */
#define RECV_BUFFER_SIZE 8192

/*************************************
* main
* 功能 socket通信客户端
**************************************/
void main(int argc, char* argv[])
{
	// 变量定义
	SOCKADDR_IN clientService;// 地址
	SOCKET ConnectSocket;// socket
	WSADATA wsaData;// 库
	LPVOID recvbuf;// 接收缓存
	int bytesSent;
	int bytesRecv = 0;
	char sendbuf[32] = "get information";// 默认发送的数据

	// 初始化socket库，	保存ws2_32.dll已经加载
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");

	// 创建socket
	ConnectSocket = socket(AF_INET, // IPv4
		SOCK_STREAM, // 顺序的、可靠的、基于连接的、双向的数据流通信
		IPPROTO_TCP// 使用TCP协议
		);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// 设置服务端的通信协议、IP地址、端口
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
	clientService.sin_port = htons( 10000 );

	// 连接到服务端
	if ( connect(
		ConnectSocket, // socket
		(SOCKADDR*) &clientService, // 地址
		sizeof(clientService) // 地址的大小
		) == SOCKET_ERROR)
	{
		printf( "Failed to connect(%d)\n",WSAGetLastError() );
		WSACleanup();
		return;
	}
	// 准备发送数据
	// 如果输入参数是-d，那么发送的数据是“download file”否则是"get information"
	if(argc ==2 && (!lstrcmp(argv[1], "-d")))
	{
		lstrcpyn(sendbuf, "download file", 32);
	}
	// 向服务端发送数据
	bytesSent = send( ConnectSocket, // socket
		sendbuf,// 发送的数据 
		lstrlen(sendbuf)+1,// 数据长度
		0 );// 无标志

	if(bytesSent == SOCKET_ERROR)
	{
		printf( "send error (%d)\n", WSAGetLastError());
		closesocket(ConnectSocket);
		return;
	}
	printf( "Bytes Sent: %ld\n", bytesSent );

	// 准备接收数据
	recvbuf = HeapAlloc(GetProcessHeap(), 0, RECV_BUFFER_SIZE);
	// 循环接收
	while( bytesRecv != SOCKET_ERROR )
	{
		//Sleep(50);
		bytesRecv = recv( ConnectSocket, // socket
			recvbuf, // 接收数据缓存
			RECV_BUFFER_SIZE,// 缓存大小
			0 );// 无标志
		if ( bytesRecv == 0 )
		{
			printf( "Connection Closed.\n");
			break;
		}
		// TODO，处理接收的数据，这里只简单的将收到的数据大小显示
		printf( "Bytes Recv: %ld\n", bytesRecv );
	}
	HeapFree(GetProcessHeap(), 0, recvbuf);
	WSACleanup();
	return;
}