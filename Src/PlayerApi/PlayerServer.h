#include "PlayerApiDef.h"

#define USER_NEW_SOCK	1

int PlayerServer_Init();

int PlayerServer_DeInit();

// 返回socket id
// -1：出错
int CreateServer(int iPort);

// 返回socket id
// -1：出错
int Connect(const char *pIP, int iPort);

void CloseSock(int &sock);

int SendMsg(int sock, const char *Msg, int iLen);

int RecvMsg(int sock, char *Msg, int iLen);
