#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <fcntl.h>
#include "CClientSocket.h"
CClientSocket::CClientSocket()  
{
  
}  
  
CClientSocket::~CClientSocket()  
{  
    if(!Close())  
    {  
        printf( "client socket close err\n");  
    }  
}  
  
bool CClientSocket::Close()  
{  
    if(clientsockfd < 0)  
        return false;  
    else  
    {  
        ::close(clientsockfd);  
        clientsockfd = -1;  
    }  
          
    return true;  
}  
  
bool CClientSocket::Connect(const char* ip, int port)  
{  
    if(ip==NULL || ip=="" || port < 0)  
        return false;  
    struct sockaddr_in sddr;  
    sddr.sin_family = AF_INET;  
    sddr.sin_addr.s_addr = inet_addr(ip);  
    sddr.sin_port = htons(port); 

    if((clientsockfd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)  
    {  
        printf("client socket create err\n");  
	 return false;
    }else {
	printf("client socket create clientsockfd = %d\r\n",clientsockfd);
    }

    struct timeval timeout={30,0};//30s
    int ret=setsockopt(clientsockfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
 
    if(::connect(clientsockfd, (struct sockaddr*)&sddr, sizeof(sddr)) < 0)  
    {  
        printf("connnect to server err\n");  
        return false;  
    }  
    return true;  
}  
bool CClientSocket::Send(char* sendbuf, int len)  
{  
    int ret = 0;   
    ret = ::send(clientsockfd, sendbuf, len, 0);
		if(ret<0){
			printf("tcp socket send error ret = %d,error=%d\r\n",ret,errno);
		}
    return ret == len;   
}  
  
int CClientSocket::Recv(char* recvbuf, int len)  
{  
    int ret = 0;  
    ret = ::recv(clientsockfd, recvbuf, len, 0);
    //printf("tcp socket recv  ret = %d,errno = %d,len = %d,data=%s\r\n",ret,errno,len,recvbuf);
    if(ret >0) return ret;
    else {
	/*
	if((ret<0) &&(errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR)){
	    	//printf("tcp socket recv normal error\r\n");	
	    return 0;
	}
	else*/
	{
      		printf("tcp socket recv error\r\n");
	    	return -1;
       }
    }  
}
char* CClientSocket::GetIpByName(const char* name)
{
	char ip[16];
	struct hostent *hptr = gethostbyname(name);  
    	if(hptr == NULL)  
    	{  
        	printf("gethostbyname  err\n");
		return NULL; 
    	}
	char **pptr = hptr->h_addr_list;  
      for(; *pptr != NULL; pptr++)  
      {  
        	inet_ntop(hptr->h_addrtype, *pptr, ip, sizeof(ip));	
     	}
	printf("addr = %s\r\n",ip);
	return ip;
}  
