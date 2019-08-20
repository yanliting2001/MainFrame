#include "PadControl.h"
#include "BaseThread.h"
#include "BaseApp.h"
#include "xmlanalytic.hpp"
#include "control.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "../DlnaTypeDef.h"
int giListen_fd;
int giThreadExit = 0;
static CBaseThread gPadControlServerThread;

struct SConnetInfo
{
	int acceptSock;
	char conIP[MAX_IP_LEN];
	int conPort;
};
int giConnetCount;		//tcpÁ¬½ÓÊýÄ¿
struct SConnetInfo gsConnetInfo[MAX_CONNECTION];	//tcpÁ¬½ÓÐÅÏ¢

char gBuf[MAX_BUFFER];

LPVOID PadControlServerThreadFunc(LPVOID lpParam)
{
	int sockfd = -1;
	int cmd = -1;
	int ret;
	while(giThreadExit == 0)
	{
		GetNewConnect();
		memset(gBuf,0,sizeof(gBuf));
		sockfd = ReciveMsgFromPAD(gBuf);
		if(sockfd >0)
		{
#ifdef DEBUG
			printf("\r\n---------------------\r\n");
			printf("receive len is %d. the msg is:\n",strlen(gBuf));
			for(unsigned int i=0; i<strlen(gBuf); i++)
			{
				printf("%c",gBuf[i]);
			}
			printf("\r\n---------------------\r\n");
#endif
			ret = gpXml->InitXmlDoc(gBuf, strlen(gBuf)+1);
			if(ret != -1)
			{
				ret = gpXml->GetCmd(&cmd);
				if(ret != -1)
				{
					gpControl->HandlePAD(sockfd,cmd);
				}
				gpXml->FreeXmlDoc();
			}
		}
		usleep(100000);
	}

	return 0;
}

void DlnaPad_Init()
{

	for(int i=0; i<MAX_CONNECTION; i++)
	{
		gsConnetInfo[i].acceptSock = -1;
		memset(gsConnetInfo[i].conIP,0,sizeof(gsConnetInfo[i].conIP));
		gsConnetInfo[i].conPort = -1;
	}
	if(InitPadControlServer() != 0)
	{
		DbgOutput(DBG_ERR,"#Error: InitPadControl failed.\n");
	}
	DbgOutput(DBG_ERR,"#Listen:%d port succeed.\n",LISTEN_PORT);

	gpXml = new CXmlAnalytic();
	if(gpXml == NULL)
	{
		DbgOutput(DBG_ERR,"#Error: new xmlAnalytci failed.\n");
	}
	gpControl = new CControl();
	if(gpControl == NULL)
	{
		DbgOutput(DBG_ERR,"#Error: new control failed.\n");
	}
	gpControl->Init();

	gPadControlServerThread.StartThread(PadControlServerThreadFunc, NULL, "PadControlServerThreadFunc");
}
void DlnaPad_DeInit()
{
	giThreadExit = 1;
	DeInitPadControlServer();
	if(gpXml != NULL)
	{
		delete gpXml;
		gpXml = NULL;
	}

	if(gpControl != NULL)
	{
		delete gpControl;
		gpControl = NULL;
	}
}

int InitPadControlServer()
{
	int ret = 0;
	int flag = 1;
	struct sockaddr_in client_addr;

	giListen_fd = socket(AF_INET,SOCK_STREAM,0);
	if(giListen_fd < 0)
	{
		DbgOutput(DBG_ERR,"#Error: the socket listen failed.\n");
		return -1;
	}

	memset(&client_addr,0,sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(LISTEN_PORT);
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	setsockopt(giListen_fd,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(int));

	if(bind(giListen_fd,(struct sockaddr *)&client_addr,sizeof(client_addr)) < 0)
	{
		DbgOutput(DBG_ERR,"#Error: bind failed.\n");
		return -1;
	}

	fcntl(giListen_fd,F_SETFL,O_NONBLOCK);
	if(listen(giListen_fd,MAX_CONNECTION) < 0)
	{
		DbgOutput(DBG_ERR,"#Error: listen failed.\n");
		return -1;
	}
	return ret;
}

int DeInitPadControlServer()
{
	int ret = 0;
	if(giListen_fd > 0)
	{
		close(giListen_fd);
		giListen_fd = -1;
	}
	return ret;
}


int GetNewConnect()
{
	int fd;
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);

	memset(&addr,0,sizeof(addr));

	if(giConnetCount > MAX_CONNECTION)
	{
		DbgOutput(DBG_ERR,"Warnint: the connection count:[%d]  is full.",giConnetCount);
		return -1;
	}
	fd = accept(giListen_fd,(struct sockaddr *)&addr,&len);
	if(fd > 0)
	{
		fcntl(fd,F_SETFL,O_NONBLOCK);
		//SetSockProp(fd);
#ifndef DEBUG
		printf("the %s %d has connect, the sock id is %d.\n",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port),fd);
#endif
		for(int i=0; i< MAX_CONNECTION; i++)
		{
			if(gsConnetInfo[i].acceptSock == -1)
			{
				gsConnetInfo[i].acceptSock = fd;
				memcpy(gsConnetInfo[i].conIP,(char *)inet_ntoa(addr.sin_addr),sizeof(gsConnetInfo[i].conIP));
				gsConnetInfo[i].conPort = ntohs(addr.sin_port);
				giConnetCount++;
				break;
			}
		}
	}

	return 0;
}

int ReciveMsgFromPAD(char *pBuf)
{
	int sockfd = -1;
	int iLen;
	int m_iSocketError;
	char buf[MAX_BUFFER];

	if(pBuf == NULL)
	{
		DbgOutput(DBG_ERR,"#Error:the param of ReciveMsgFormPAD is null.\n");
		return -1;
	}

	memset(buf,0,sizeof(buf));
	for(int i=0; i<MAX_CONNECTION; i++)
	{
		if(gsConnetInfo[i].acceptSock > 0)
		{
			iLen = read(gsConnetInfo[i].acceptSock,(char *)buf,MAX_BUFFER);
			if(iLen < 0)
			{
				m_iSocketError = errno;

				if((EWOULDBLOCK == m_iSocketError)
					|| (EINTR == m_iSocketError)
					|| (EAGAIN == m_iSocketError))
				{
		//			Print("the sock %d has no msg.",connetInfo[i].acceptSock);
					iLen = 0;
				}
				else
				{
#ifdef DEBUG
					printf("the client fd %d unusual closed.\n",gsConnetInfo[i].acceptSock);
#endif
					close(gsConnetInfo[i].acceptSock);
					gsConnetInfo[i].acceptSock = -1;
					giConnetCount--;
				}
			}
			else if(iLen == 0)
			{
#ifdef DEBUG
				printf("the client %d is closed.\n",gsConnetInfo[i].acceptSock);
#endif
				close(gsConnetInfo[i].acceptSock);
				gsConnetInfo[i].acceptSock = -1;
				giConnetCount--;
			}
			else
			{
				while((iLen > 0) && (buf[iLen-1] != '>'))
					iLen--;
				buf[iLen] = '\0';
				memcpy(pBuf,buf,sizeof(buf));
				sockfd = gsConnetInfo[i].acceptSock;
				break;
			}
		}
	}

	return sockfd;
}

int SendMsgToPad(int sockfd,char *pBuf,int iLen)
{
	//usleep(300000);
	int iRet,i;
	char *p;
	int m_iSocketError;

	if((pBuf == NULL) || (sockfd < 0))
	{
		printf("#Error: the param of SendMsgToPad is null.\n");
		return -1;
	}

	iRet = 0;
	i = 0;

	p = (char *)pBuf;

#if 1
	printf("=============>Response Msg:\n");
	printf("%s\n", pBuf);
	printf("=============>end response\n");
#endif


	while(i < iLen)
	{
		iRet = write(sockfd,p,iLen-i);
		if(iRet <= 0)
		{
			m_iSocketError = errno;
			if((EWOULDBLOCK == m_iSocketError)
				|| (EINTR == m_iSocketError)
				|| (EAGAIN == m_iSocketError))
			{
				iRet = 0;
			}
			else
			{
				close(sockfd);
				for(int i=0; i<MAX_CONNECTION; i++)
				{
					if(gsConnetInfo[i].acceptSock == sockfd)
					{
						gsConnetInfo[i].acceptSock = -1;
						break;
					}
				}
				return -1;
			}
		}
		m_iSocketError = 0;
		i += iRet;
		p += iRet;
	}

	return i;
}

int SendMsgToOtherPad(int sockfd, char *pBuf, int iLen)
{
	int len = 0;

	if(pBuf == NULL)
	{
		printf("#Error: the param of SendMsgToPadByCmd is null.\n");
		return -1;
	}

	for(int i=0; i<MAX_CONNECTION; i++)
		if((gsConnetInfo[i].acceptSock > 0) && (gsConnetInfo[i].acceptSock != sockfd))
			len = SendMsgToPad(gsConnetInfo[i].acceptSock,pBuf,strlen(pBuf));
	return len;
}

int ReciveMsgForLen(int sockfd,int iLen)
{
	errno = 0;

	int len = -1;
	char *buf = NULL;

	FILE *fp = fopen(PNGPATH,"wb");
	if(fp == NULL)
	{
		printf("#Error: open the file %s failed.\n",PNGPATH);
		return -1;
	}

	buf = (char *)malloc(iLen * sizeof(char));
	if(buf == NULL)
	{
		printf("#Error: malloc failed. %s:%d.\n",__FILE__,__LINE__);
		return -1;
	}


    if((sockfd <0) || (iLen <= 0))
    {
        printf("the param of ReciveMsgForLen is wrong. sockfd:%d, len:%d.",sockfd,iLen);
        return 0;
    }

    int pos = 0;

	memset(buf,0,iLen * sizeof(char));
    while(pos < iLen)
    {
        len = read(sockfd,buf+pos,iLen-pos);     //read in 2 data,message length and type
        if(len > 0)
        {

			fwrite(buf+pos,len,1,fp);
			pos += len;
        }
        else if(len == 0)
        {
            printf("Readin length of packet error,retcode=%d.\n",len);
            return -1;
        }
        else if((errno == EWOULDBLOCK) || (errno == EINTR) || (errno == EAGAIN))
        {
			usleep(1);
			/*
            if(pos == 0)
            {
                return 0;
            }
            else
            {
                usleep(1);
            }
            */
        }
        else
        {
            printf("Readin length of packet error,retcode=%d.\n",len);
            return -1;
        }
    }

	fclose(fp);
	fp = NULL;

	free(buf);
	buf = NULL;

	printf("receive the picture leng:%d.\n",pos);
    return pos;
}


int SendSyncToPadFromUI(int iValue,SyncType eSyncType)
{
	for(int i = 0; i < MAX_CONNECTION; i++)
		if(gsConnetInfo[i].acceptSock > 0)
		{
			switch(eSyncType)
			{
			case SYNCTYPE_SONG:
				gpControl->SendSyncSongToPad(gsConnetInfo[i].acceptSock,SYNCSONG_NOTICE);
				break;

			case SYNCTYPE_SWITCH:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,SWITCHSONG_NOTICE,-1);
				break;

			case SYNCTYPE_REPLAY:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,REPLAY_NOTICE,-1);
				break;

			case SYNCTYPE_PAUSE:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,PAUSE_NOTICE,-1);
				break;

			case SYNCTYPE_RESUME:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,RESUME_NOTICE,-1);
				break;

			case SYNCTYPE_VOLUME:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,SETVOLUME_NOTICE,iValue);
				break;

			case SYNCTYPE_CHANNEL:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,SWITCHCHANNEL_NOTICE,-1);
				break;

			case SYNCTYPE_START_SCORE:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,START_SCORE_NOTICE,-1);
				break;

			case SYNCTYPE_STOP_SCORE:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,STOP_SCORE_NOTICE,-1);
				break;

			case SYNCTYPE_SITUATION:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,SET_SITUATION_NOTICE,-1);
				break;

			case SYNCTYPE_PUBLICSONG:
				gpControl->SendSyncPublicSongToPad(gsConnetInfo[i].acceptSock,SET_PUBLICSONG_NOTICE);
				break;

			case SYNCTYPE_PLAYUDISK:
				gpControl->SendSyncStatusToPad(gsConnetInfo[i].acceptSock,PLAY_UDISK_NOTICE,-1);
				break;

			case SYNCTYPE_SINGED:
				gpControl->SearchSingedSongReq(gsConnetInfo[i].acceptSock,SYNC_SINGED_NOTICE);
				break;

			default:
				break;
			}
		}
	return 0;
}
