/* 
 * CScout ANSI C generic include file 
 * $Id: string.h 1.1 2003/06/16 20:00:57 dds Exp $
 */

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)string.h	5.10 (Berkeley) 3/9/91
 */

#ifndef _STRING_H_
#define	_STRING_H_

#include <_ansi.h>

#ifdef	_CSCOUT_SIZE_T_
typedef	_CSCOUT_SIZE_T_	size_t;
#undef	_CSCOUT_SIZE_T_
#endif

#ifndef	NULL
#define	NULL	0
#endif

void	*memchr (const void *, int, size_t);
int	 memcmp (const void *, const void *, size_t);
void	*memcpy (void *, const void *, size_t);
void	*memmove (void *, const void *, size_t);
void	*memset (void *, int, size_t);
char	*strcat (char *, const char *);
char	*strchr (const char *, int);
int	 strcmp (const char *, const char *);
int	 strcoll (const char *, const char *);
char	*strcpy (char *, const char *);
size_t	 strcspn (const char *, const char *);
char	*strerror (int);
size_t	 strlen (const char *);
char	*strncat (char *, const char *, size_t);
int	 strncmp (const char *, const char *, size_t);
char	*strncpy (char *, const char *, size_t);
char	*strpbrk (const char *, const char *);
char	*strrchr (const char *, int);
size_t	 strspn (const char *, const char *);
char	*strstr (const char *, const char *);
char	*strtok (char *, const char *);
char	*strtok_r (char *, const char *, char **);
size_t	 strxfrm (char *, const char *, size_t);


#endif /* _STRING_H_ */
