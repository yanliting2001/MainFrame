#include "PlayerApi.h"
#include "DbgOutput.h"
#include "BaseString.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 		/*for struct sockaddr_in*/
#include <arpa/inet.h>
#include "../DLNA/DlnaApi.h"
#include "../GlobalUIClass.h"
#include "CommApi.h"

#if !USER_NEW_SOCK
int gPlayerApiSocket = 0;

int PlayerApi_Init(void);
int PlayerApi_DeInit(void);

int PlayerApi_Init(void)
{
	struct sockaddr_in dest_addr;
	int err = PLAYERAPI_ERROR_NONE;

	err = PLAYERAPI_ERROR_SOCKET;
	do
	{
		/* 取得一个套接字*/
		gPlayerApiSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (gPlayerApiSocket == -1)
		{
			DbgOutput(DBG_ERR, "play api client socket create fail, errno=%d, %s\n",
				errno, strerror(errno));
			err = PLAYERAPI_ERROR_SOCKET;
			break;
		}

		/* 设置远程连接的信息*/
		dest_addr.sin_family = AF_INET; /* 注意主机字节顺序*/
		dest_addr.sin_port = htons(PLAYERAPI_SOCKET_CLIENTPORT); /* 远程连接端口, 注意网络字节顺序*/
		dest_addr.sin_addr.s_addr = inet_addr(PLAYERAPI_SOCKET_ADDR); /* 远程 IP 地址, inet_addr() 会返回网络字节顺序*/
		bzero(&(dest_addr.sin_zero), 8); /* 其余结构须置 0*/

		/* 连接远程主机，出错返回 -1*/
		if (connect(gPlayerApiSocket, (struct sockaddr *) &dest_addr,
			sizeof(struct sockaddr_in)) == -1)
		{
			DbgOutput(DBG_ERR, "play api client socket create fail, errno=%d, %s\n",
				errno, strerror(errno));
			err = PLAYERAPI_ERROR_SOCKET;
			break;
		}

		err = PLAYERAPI_ERROR_NONE;
	} while (0);

	if (err != PLAYERAPI_ERROR_NONE)
	{
		close(gPlayerApiSocket);
		gPlayerApiSocket = 0;
	}

	return err;
}

int PlayerApi_DeInit(void)
{
	if (gPlayerApiSocket > 0)
	{
		shutdown(gPlayerApiSocket, SHUT_RDWR);
		close(gPlayerApiSocket);
		gPlayerApiSocket = 0;
	}

	return PLAYERAPI_ERROR_NONE;
}

static int PlayerApi_SendMsg(const char *msg, int iLen)
{
	if ( !msg || iLen <= 0 )
		return -1;

	PlayerApi_Init();
	if ( gPlayerApiSocket <= 0 )
		return PLAYERAPI_ERROR_SOCKET;

	int err = PLAYERAPI_ERROR_NONE;
	do
	{
		if ( send(gPlayerApiSocket, msg, iLen, 0) == -1 )
		{
			DbgOutput(DBG_ERR, "%s send command fail, errno=%d, %s\n",__FUNCTION__, errno, strerror(errno));
			err = PLAYERAPI_ERROR_OTHER;
			break;;
		}
		// read return code
		char retcode = PLAYERAPI_ERROR_NONE;
		if ( recv(gPlayerApiSocket, &retcode, 1, 0) == -1 )
		{
			DbgOutput(DBG_ERR, "%s recv return code fail, errno=%d, %s\n",__FUNCTION__, errno, strerror(errno));
			err = PLAYERAPI_ERROR_OTHER;
			break;
		}

		if (retcode == '0')
			err = PLAYERAPI_ERROR_NONE;
		else
			err = PLAYERAPI_ERROR_OTHER;
	} while (0);

	PlayerApi_DeInit();

	return err;
}
#else
static int PlayerApi_SendMsg(const char *msg, int iLen)
{
	if ( !msg || iLen <= 0 )
		return -1;

	DbgOutput(DBG_INFO, "Send:%s \n", msg);
	int sock = Connect(PLAYERAPI_SOCKET_ADDR, PLAYERAPI_SOCKET_CLIENTPORT);
	if ( sock <= 0 )
		return PLAYERAPI_ERROR_SOCKET;

	int err = PLAYERAPI_ERROR_NONE;
	do
	{
		if ( SendMsg(sock, msg, iLen) <= 0 )
		{
			DbgOutput(DBG_ERR, "%s send command fail, errno:%s\n",__FUNCTION__, strerror(errno));
			err = PLAYERAPI_ERROR_OTHER;
			break;
		}
		// read return code
		char retcode = PLAYERAPI_ERROR_NONE;
		if ( RecvMsg(sock, &retcode, 1) <= 0 )
		{
			DbgOutput(DBG_ERR, "%s recv return code fail, errno=%s\n",__FUNCTION__, strerror(errno));
			err = PLAYERAPI_ERROR_OTHER;
			break;
		}

		if (retcode == '0')
			err = PLAYERAPI_ERROR_NONE;
		else
			err = PLAYERAPI_ERROR_OTHER;
	} while (0);

	CloseSock(sock);

	return err;
}
#endif

// song contrl
int PlayerApi_Play(const char *cID,
		const char *cServerName, const char *cFileName, int nVol,
		int nTrack, int nOriginal, int bForceSwitch,
		const char *cSongName,
		const char *cSinger,
		const char *cPicPath)
{
#if 0 && USE_MODULE_DLNA && USE_DLNA_PAD
	//printf("[%s,%d] %s,%s,%s\n",__FILE__,__LINE__,cFileName,cServerName,cSongName);
	char url[64]={0};
	char *p = strstr(cFileName,"http://");
	if(!p){
		sprintf(url,"http://%s%s",gPlayerInfo->GetIp(),cFileName);
		dmr_update_playing_info(url,"mp4",(char *)cSongName);
		
	}else{
		dmr_update_playing_info((char *)cFileName,"mp4",(char *)cSongName);
	}
#endif

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,%s,%s,%d,%d,%d,%s,%s,%s,\n", "RMC", "PLY",
		bForceSwitch ? "NEXTM" : "NEXTA",cID,cFileName, cServerName, nVol,
		nTrack, nOriginal,cSongName,cSinger,cPicPath);

	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());
	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Pause()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "PAUSE");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Resume()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "PAUSERESUME");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Stop()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "STOP");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Replay()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "REPEAT");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

// volume control
int PlayerApi_SetVolume(int nVol)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "PLY", "VOLSET", nVol);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

// audio control
int PlayerApi_SetOriginal(int nTrack, int nOriginal)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,%d,\n", "RMC", "PLY", "SWITCHAUDIO", nTrack, nOriginal);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

//add by yq
int PlayerApi_StartPreview(
	const char *cServerName,
	const char *cFileName,
	int x,
	int y,
	int w,
	int h)
{
	if((cServerName == NULL) || (cFileName == NULL))
	{
		DbgOutput(DBG_INFO, "#Error:%s--%s\n", __FILE__,__FUNCTION__);
		return -1;
	}

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,%s,%d,%d,%d,%d,\n", "RMC", "PLY", "STARTPREVIEW",cServerName,cFileName,x,y,w,h);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_PausePreview()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "PAUSEPREVIEW");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_ResumePreview()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "RESUMEPREVIEW");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_StopPreview()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "STOPPREVIEW");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_StartPhantom()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "PLAYPHANTOM");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_StopPhantom()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", "STOPPHANTOM");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_PlayMTV(int x,int y,int w,int h)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,%d,%d,%d,\n", "RMC","PLY", "PLAYMTV",x,y,w,h);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_PlayMTVFullScreen(int bFullScreen)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC","PLY", "PLAYMTVFULL",bFullScreen);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_StopMTV()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC","PLY", "STOPMTV");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Cheer(
	int nType)
{
	if((nType != 0) && (nType != 1))
	{
		DbgOutput(DBG_INFO, "#Error:%s--%s\n", __FILE__,__FUNCTION__);
		return -1;
	}

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "PLY", "CHEER", nType);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}


int PlayerApi_Ambience(int nFaceIndex)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD", "AMBIENCE",nFaceIndex);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Situation(
	const char *cServerName,
	const char *cFileName,
	int nVol,
	int nTrack,
	int nOriginal)
{
	if((cServerName == NULL) || (cFileName == NULL))
	{
		DbgOutput(DBG_INFO, "#Error:%s--%s\n", __FILE__,__FUNCTION__);
		return -1;
	}

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,%s,%d,%d,%d,\\n", "RMC", "PLY","SITUATION",
		cFileName, cServerName, nVol,nTrack, nOriginal);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Attunement(
	int nType)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "PLY", "ATTUNEMENT", nType);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_SongMark(
	int nType)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD", "SONGMARK",nType);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_Quit()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,\n", "RMC", "QUIT");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}
//end by yq



//add by yq,2012-12-04
int PlayerApi_UpdateMsgInfo(const char *playingName,const char *nextPlayName)
{
	if((playingName == NULL) || (nextPlayName == NULL))
	{
		DbgOutput(DBG_DEBUG, "#Error:%s--%s\n", __FILE__,__FUNCTION__);
		return -1;
	}

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,%s,\n", "RMC", "OSD","UPDATEMSGINFO",playingName,nextPlayName);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}
//end add by yq,2012-12-04

//add by yq,2012-13
int PlayerApi_SetTVLightness(
		int iLightness)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD","LIGHTNESS",iLightness);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}
int PlayerApi_SetTVContrast(int iContrast)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD","CONTRAST",iContrast);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}
int PlayerApi_SetTVSaturation(int iStaturation)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD","SATURATION",iStaturation);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}
//end add by yq,2012-12-13

int PlayerApi_ChangeBGVideo(int iBGVideoIndex)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD","CHANGEBGVEDIO",iBGVideoIndex);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_SetTvPutOutMode(int mode)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "OSD","TVFORMAT",mode);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_StartGuide()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "OSD","STARTGUIDE");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_StopGuide()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "OSD","STOPGUIDE");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayerApi_PlayMovie(
	const char *cServerName,
	const char *cFileName,
	int nVol,
	int nTrack,
	int nOriginal)
{
	if((cServerName == NULL) || (cFileName == NULL))
	{
		DbgOutput(DBG_INFO, "#Error:%s--%s\n", __FILE__,__FUNCTION__);
		return -1;
	}

	CBaseStringA tmp;
	char cgbkUsbName[128];
	u2g((char *)cFileName, strlen(cFileName), cgbkUsbName, sizeof(cgbkUsbName));
	tmp.Format("%s,%s,%s,%s,%s,%d,%d,%d,\\n", "RMC", "PLY","MOVIE",
			cgbkUsbName, cServerName, nVol,nTrack, nOriginal);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_ForwardMovie()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY","FORWARD");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_BackwardMovie()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY","BACKWARD");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_StartRecord(const char *recordName)
{
	if(NULL == recordName)
		return -1;

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,\n", "RMC", "OSD","STARTRECORD",recordName);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_StopRecord()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "OSD","STOPRECORD");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_PlayRecord(const char *filename,
		int value,
		int nOriginal)
{
	if(NULL == filename)
		return -1;

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,%d,%d,\n", "RMC", "OSD","PLAYRECORD",filename, value, nOriginal);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}
int PlayApi_ShowWatermark()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "OSD","SHOWWATERMARK");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_UnShowWatermark()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "OSD","UNSHOWWATERMARK");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_UpdateWatermark()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "OSD","UPDATEWATERMARK");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_PlayUrlStream(const char *url)
{
	if(NULL == url)
		return -1;

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,\n", "RMC", "PLY","URLLINK", url);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_StopPlay()
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY","STOPPLAY");
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_SetHdmiAudio(int bOpen)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "PLY","HdmiAudio", bOpen ? 1 : 0);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int OSDApi_ShowBarCode(const char *pFilePath, int bShow)
{
	if ( pFilePath == NULL )
		return -1;

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%s,%d,\n", "RMC", "OSD","BARCODE", pFilePath, bShow ? 1 : 0);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

int PlayApi_SeekTo(int iPos)
{
	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,%d,\n", "RMC", "PLY","SEEKTO", iPos);
	DbgOutput(DBG_DEBUG, "%s command is \"%s\"\n", __FUNCTION__, tmp.GetString());

	return PlayerApi_SendMsg(tmp.GetString(), tmp.GetLength());
}

