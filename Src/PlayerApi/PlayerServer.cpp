#include "PlayerServer.h"
#include "DbgOutput.h"
#include "BaseString.h"
#include "BaseThread.h"
#include "UserMsgDefine.h"
#include "../GlobalUIClass.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /*for struct sockaddr_in*/
#include <arpa/inet.h>
#include <signal.h>
  #include <netdb.h>
    #include <sys/socket.h>

static int gPlayerServerSocket = 0;
static int gPlayerClientSocket = 0;

static CBaseThread gCmdServerThread;

#if !USER_NEW_SOCK

void ParseCommand(
	const char *cCommand)
{
	if ( gPlayerInfo != NULL )
		gPlayerInfo->PlayNext(FALSE);
}

LPVOID CmdServerThreadFunc(LPVOID lpParam)
{
	//printf("the pthread of CmdServerThreadFunc ID is:%d.\n",syscall(SYS_gettid));
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	while(1)
	{
		gPlayerClientSocket = accept(gPlayerServerSocket,
					(struct sockaddr*) &client_addr, &length);

		if (gPlayerClientSocket < 0)
		{
			DbgOutput(DBG_ERR, "play server socket accept fail, errno=%d, %s\n",
				errno, strerror(errno));

			break;
		}

		char cCommand[1024+1];

		while (gPlayerClientSocket > 0)
		{
			int length = recv(gPlayerClientSocket, cCommand, 1024, 0);
			if (length <= 0)
			{
				DbgOutput(DBG_ERR,
					"play server socket recv %d, errno=%d, %s\n",
					length, errno, strerror(errno));

				break;
			}

			cCommand[length+1] = '\0';

			DbgOutput(DBG_INFO, "recv(%d): %s\n", length, cCommand);

			ParseCommand(cCommand);
		}
	}

	return 0;
}

int PlayerServer_Init()
{
	int err = PLAYERAPI_ERROR_NONE;

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(PLAYERAPI_SOCKET_SERVERPORT);

	gPlayerServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (gPlayerServerSocket < 0)
	{
		DbgOutput(DBG_ERR, "play server socket create fail, errno=%d, %s\n",
			errno, strerror(errno));
		err = PLAYERAPI_ERROR_SOCKET;
	}

	if (bind(gPlayerServerSocket, (struct sockaddr*) &server_addr,
			sizeof(server_addr)))
	{
		DbgOutput(DBG_ERR, "play server socket bind fail, errno=%d, %s\n",
			errno, strerror(errno));
		err = PLAYERAPI_ERROR_SOCKET;
	}

	if (listen(gPlayerServerSocket, 10))
	{
		DbgOutput(DBG_ERR, "play server socket listen fail, errno=%d, %s\n",
			errno, strerror(errno));
		err = PLAYERAPI_ERROR_SOCKET;
	}

	gCmdServerThread.StartThread(CmdServerThreadFunc, NULL, "CmdServerThreadFunc");

	return err;
}

int PlayerServer_DeInit()
{
	if (gPlayerClientSocket > 0)
	{
		shutdown(gPlayerClientSocket, SHUT_RDWR);
		close(gPlayerClientSocket);
		gPlayerClientSocket = 0;
	}

	if (gPlayerServerSocket > 0)
	{
		shutdown(gPlayerServerSocket, SHUT_RDWR);
		close(gPlayerServerSocket);
		gPlayerServerSocket = 0;
	}

	gCmdServerThread.StopThread();

	return 0;
}
#else
void ParseCommand(const char *cCommand)
{
	if ( gPlayerInfo != NULL )
		gPlayerInfo->PlayNext(FALSE);
}

LPVOID CmdServerThreadFunc(LPVOID lpParam)
{
	//printf("the pthread of CmdServerThreadFunc ID is:%d.\n",syscall(SYS_gettid));
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	while(1)
	{
		gPlayerClientSocket = accept(gPlayerServerSocket,(struct sockaddr*) &client_addr, &length);
		if (gPlayerClientSocket < 0)
		{
			DbgOutput(DBG_ERR, "#Err[%s]: accept fail, errno=%s\n", __FUNCTION__, strerror(errno));
			break;
		}

		char cCommand[1024+1];
		while ( gPlayerClientSocket > 0 )
		{
			int length = RecvMsg(gPlayerClientSocket, cCommand, 1024);
			if (length <= 0)
			{
				// Close
				DbgOutput(DBG_DEBUG, "play server socket recv %d, errno=%d, %s\n",
					length, errno, strerror(errno));
				CloseSock(gPlayerClientSocket);
				break;
			}
			cCommand[length+1] = '\0';
			DbgOutput(DBG_INFO, "recv(%d): %s\n", length, cCommand);

			ParseCommand(cCommand);
			CloseSock(gPlayerClientSocket);
		}
	}

	return 0;
}

int PlayerServer_Init()
{
	gPlayerServerSocket = CreateServer(PLAYERAPI_SOCKET_SERVERPORT);
	if ( gPlayerServerSocket < 0 )
	{
		DbgOutput(DBG_ERR, "#Err[%s]:create server fail, port=%d, %s\n", __FUNCTION__, PLAYERAPI_SOCKET_SERVERPORT, strerror(errno) );
		return PLAYERAPI_ERROR_SOCKET;
	}
	gCmdServerThread.StartThread(CmdServerThreadFunc, NULL, "CmdServerThreadFunc");
	return 0;
}

int PlayerServer_DeInit()
{
	shutdown(gPlayerClientSocket, SHUT_RDWR);
	shutdown(gPlayerServerSocket, SHUT_RDWR);
	CloseSock(gPlayerClientSocket);
	CloseSock(gPlayerServerSocket);
	gCmdServerThread.StopThread();
	return 0;
}
#endif

//////////////////////////////
//////yq////////////////////

// 返回socket id
// -1：出错
int CreateServer(int iPort)
{
	int sock = -1;
	struct sockaddr_in server_addr;

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(iPort);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if ( sock < 0 )
	{
		DbgOutput(DBG_ERR, "create socket fail, %s\n", strerror(errno));
		return -1;
	}

	int flags = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(int));

	if ( bind(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) )
	{
		DbgOutput(DBG_ERR, "bind fail,%s\n",strerror(errno));
		close(sock);
		return -1;
	}

	if ( listen(sock, 10) )
	{
		DbgOutput(DBG_ERR, "listen fail,%s\n",strerror(errno));
		close(sock);
		return -1;
	}

	return sock;
}

// 返回socket id
// -1：出错
int Connect(const char *pIP, int iPort)
{
	if ( !pIP || iPort <= 0 )
		return -1;

	unsigned long inaddr=0l;
	struct hostent *host = NULL;
	struct sockaddr_in dest_addr;
	int sock = -1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if ( sock < 0 )
	{
		DbgOutput(DBG_ERR, "client socket create fail, errno=%s\n", strerror(errno));
		return -1;
	}

	/* 设置远程连接的信息*/
	dest_addr.sin_family = AF_INET; /* 注意主机字节顺序*/
	dest_addr.sin_port = htons(iPort); /* 远程连接端口, 注意网络字节顺序*/
	//dest_addr.sin_addr.s_addr = inet_addr(pIP); /* 远程 IP 地址, inet_addr() 会返回网络字节顺序*/
	bzero(&(dest_addr.sin_zero), 8); /* 其余结构须置 0*/
	if ( (inaddr = inet_addr(pIP)) == INADDR_NONE )
	{
		if ( (host = gethostbyname(pIP)) == NULL )	//是主机名
		{
			DbgOutput(DBG_ERR, "gethostbyname fail, errno=%s\n", strerror(errno));
			close(sock);
			return -1;
		}
		memcpy( (char *)&dest_addr.sin_addr, host->h_addr, host->h_length);
	}
	else
		dest_addr.sin_addr.s_addr = inaddr;

	/* 连接远程主机，出错返回 -1*/
	if (connect(sock, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr_in)) < 0 )
	{
		DbgOutput(DBG_ERR, "connect fail, errno=%s\n", strerror(errno));
		close(sock);
		return -1;
	}

	return sock;
}

void CloseSock(int &sock)
{
	if ( sock > 0 )
	{
		//shutdown(sock, SHUT_RDWR);
		close(sock);
		sock = 0;
	}
}

int SendMsg(int sock, const char *Msg, int iLen)
{
	signal(SIGPIPE, SIG_IGN);		// 对端sock断开后，在发送会产生该信号
	return send(sock, Msg, iLen, 0);
}

int RecvMsg(int sock, char *Msg, int iLen)
{
	signal(SIGPIPE, SIG_IGN);		// 对端sock断开后，在发送会产生该信号
	return recv(sock, Msg, iLen, 0);
}
