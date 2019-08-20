#ifndef _COMMAPI_H
#define _COMMAPI_H
#include "types.h"
#include "commdef.h"
#include <stdlib.h>

#define msleep(x) usleep(x*1000)

#define DELAYCLICK(i)				\
{									\
	static UINT64 uTimeOld = GetTickCount() - ((i) + 200 );	\
	UINT64 uTimeNew = GetTickCount();						\
	if ( (uTimeNew-uTimeOld) < (i) )						\
	{														\
		return ;											\
	}														\
	uTimeOld = uTimeNew;									\
}

BOOL SystemCmd(const char *cmd);

BOOL IsInMonkeyMode();

//检测 /media  /media2  链接文件的真正路径
void DetectSongParentPath();

char *ConvertLink2Path(char *cDiskPath, const char *cLinkPath, int iDiskLen);

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

extern char g_cMainSongParentPath[32];

#endif
