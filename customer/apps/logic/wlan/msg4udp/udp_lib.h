/*============================================================================
//
// Copyright(c) 2006 Intel Corporation. All rights reserved.
//   All rights reserved.
// 
//   Redistribution and use in source and binary forms, with or without 
//   modification, are permitted provided that the following conditions 
//   are met:
// 
//     * Redistributions of source code must retain the above copyright 
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions and the following disclaimer in 
//       the documentation and/or other materials provided with the 
//       distribution.
//     * Neither the name of Intel Corporation nor the names of its 
//       contributors may be used to endorse or promote products derived 
//       from this software without specific prior written permission.
// 
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
//   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  File Name: udp_lib.h
//  Description: Header file for simple UDP socket for communication between
//               different components.
//
****************************************************************************/

#ifndef _UDPLIB_
#define _UDPLIB_
#include <netinet/in.h>

int udp_open();
int udp_bind(int fd, int portno);
void udp_close(int fd);
int udp_write(int fd, char * buf, int len, struct sockaddr_in * to);
int udp_read(int fd, char * buf, int len, struct sockaddr_in * from);
int udp_read_timed(int fd, char * buf, int len,
                   struct sockaddr_in * from, int timeout);
int get_mac_address(char *ifname, char *mac);


#endif // _UDPLIB_

