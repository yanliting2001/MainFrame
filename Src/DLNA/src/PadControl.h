#ifndef _PADCONTROL_H
#define _PADCONTROL_H
#include "../DlnaApi.h"

#define LISTEN_PORT		50000	//
#define MAX_CONNECTION  32		//Maximium Connection A Serverr can support
#define MAX_IP_LEN      32		//IP Address buffer length
#define MAX_BUFFER		1024
#define PNGPATH			"./KTV_Data/osdimg/Ambience.bmp"

void DlnaPad_Init();
void DlnaPad_DeInit();
int InitPadControlServer();
int DeInitPadControlServer();
int GetNewConnect();
int ReciveMsgFromPAD(char *pBuf);
int ReciveMsgForLen(int sockfd,int iLen);
int SendMsgToPad(int sockfd,char *pBuf,int iLen);
int SendMsgToOtherPad(int sockfd, char *pBuf, int iLen);

int SendSongSyncToPadFormPhan(int sockfd);

#endif
