#include "CommApi.h"
#include "DbgOutput.h"
#include "Song.h"
#include "FileOperate.h"
#include <sys/types.h>
#include <dirent.h>
#include "CurlApi.h"
#include "BaseThread.h"
#include "InternetCtrl/NetCtrlApi.h"
#include "../CommControl/MainCtrlPage.h"
#include "CSGetMacAddr.h"
#include "JsonParser/cJsonParser.h"
#include <openssl/md5.h>
#include <iconv.h>
#include <errno.h>

#define ROOT_BASEPATH	"/stb/disk/sd"

char g_cMainSongParentPath[32] = "/stb/config/media";
static char g_cSubSongParentPath[32] = "/stb/disk/sdb1";

BOOL SystemCmd(const char *cmd)
{
	BOOL bOK = FALSE;
	pid_t status = 0;

	if ( cmd == NULL )
	{
		DbgOutput(DBG_ERR, "the parameter is null.\n");
		return FALSE;
	}
	status = system(cmd);

	if ( (status == -1) || !WIFEXITED(status) || (WEXITSTATUS(status) != 0) )
		bOK = FALSE;
	else
		bOK = TRUE;

	return bOK;
}

BOOL IsInMonkeyMode()
{
	BOOL bIs = FALSE;

	if ( getenv("MONKEY") != NULL )
		bIs = TRUE;

	return bIs;
}

void DetectSongParentPath()
{
	char cRoot[32] = {0};
	char cPath[32] = {0};
	int j = 'a';
	int idx = 0;

	for ( j = 'a'; j < ('a'+5); j++ )
	{
		for ( idx = 0; idx < 10; idx++ )
		{
			if ( idx == 0 )
				snprintf(cRoot, sizeof(cRoot), "%s%c",  ROOT_BASEPATH, j);
			else
				snprintf(cRoot, sizeof(cRoot), "%s%c%d", ROOT_BASEPATH, j, idx);

			snprintf(cPath, sizeof(cPath), "%s/songs", cRoot);
			if ( IsFileExist(cPath) )
			{
				// 存在songs文件夹
				snprintf(cPath, sizeof(cPath), "%s/languang", cRoot);
				if ( IsFileExist(cPath) )
					memcpy(g_cMainSongParentPath, cRoot, sizeof(g_cMainSongParentPath));
				else
					memcpy(g_cSubSongParentPath, cRoot, sizeof(g_cSubSongParentPath));
			}
		}
	}
}

char *ConvertLink2Path(char *cDiskPath, const char *cLinkPath, int iDiskLen)
{
	if ( !cDiskPath || !cLinkPath )
		return NULL;

	const char *pMedia = cLinkPath;

	if ( (pMedia = strstr(cLinkPath, "/media/")) != NULL )
	{
		snprintf(cDiskPath, iDiskLen, "%s/%s", g_cMainSongParentPath, pMedia+7);
	}
	else if ( (pMedia = strstr(cLinkPath, "/media2/")) != NULL )
	{
		snprintf(cDiskPath, iDiskLen, "%s/%s", g_cSubSongParentPath, pMedia+8);
	}
	else
	{
		snprintf(cDiskPath, iDiskLen, "%s", cLinkPath);
	}

	return cDiskPath;
}

static int code_convert(const char *from_charset, const char *to_charset,
				 char *inbuf, size_t inlen,
			 	 char *outbuf, size_t outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if ( cd == 0 )
	{
		printf("iconv_open: charset from: %s to: %s failed.\n", from_charset, to_charset);
		return -1;
	}
	memset(outbuf, 0x00, outlen);
	if ( iconv(cd, pin, &inlen, pout, &outlen) == -1 )
	{
		printf("iconv failed:%s\n", strerror(errno));
		iconv_close(cd);
		return -1;
	}
	iconv_close(cd);
	return 0;
}

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}
