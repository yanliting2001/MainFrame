#include "BCM_PLT_CTRL.h"
#if  USE_MODULE_BCM
#if defined(__mips__)
#include "damd_porting_nvram.h"
#include "damd_porting_sm39.h"
#endif
#include "damd_porting_i2c.h"
#include "CommApi.h"

#include "BCM_Control.h"
//#include "BCM_File_IO.h"

CBCM_PLTCtrl g_PLYCtrl;

static int BCMP_SendStartBCMRequest(const char *msg)
{
	int sock = Connect(PLAYERAPI_SOCKET_ADDR, PLAYERAPI_SOCKET_CLIENTPORT);
	if ( sock <= 0 )
	{
		printf("==>Err[%s]: Connct port:%d failed.\n", __FUNCTION__, PLAYERAPI_SOCKET_CLIENTPORT);
		return -1;
	}

	CBaseStringA tmp;
	tmp.Format("%s,%s,%s,\n", "RMC", "PLY", msg);
	if ( SendMsg(sock, tmp.GetString(), tmp.GetLength()) < 0 )
		DbgOutput(DBG_INFO, "Warn: send:%s failed.\n", tmp.GetString());
	CloseSock(sock);

	return 0;
}

int BCMP_SendNextRequest()
{
	int sock = Connect(PLAYERAPI_SOCKET_ADDR, PLAYERAPI_SOCKET_SERVERPORT);
	if ( sock <= 0 )
	{
		printf("==>Err[%s]: Connct port:%d failed.\n", __FUNCTION__, PLAYERAPI_SOCKET_SERVERPORT);
		return -1;
	}

	if ( SendMsg(sock, "RMC,RequestNext", 20) < 0 )
		DbgOutput(DBG_INFO, "Warn: send:RMC,RequestNext failed.\n");
	CloseSock(sock);

	return 0;
}

static int MainPlayCallBack(PLY_EVENT_TYPE_E event, void *buf)
{
	DbgOutput(DBG_INFO, "====Main event type is %d====.\n", event);
	switch( event )
	{
	case STREAM_PLAYBACK_COMPLETE:
	case STREAM_ERROR:    
  case STREAM_PARSE_ERROR:  
		if( g_PLYCtrl.GetInitVideoState() )
		{
			g_PLYCtrl.SetInPlayingInitVideo(FALSE);
			BCMP_SendStartBCMRequest(OTHER_START_BCM);
		}
		//usleep(1000000);
		//CBaseRecord::Stop_Record();
		//WSD_AL_PLAYER_FlushBuffer(PLAYER_MAIN);
		BCMP_SendNextRequest();
		break;

	case STREAM_SHOW_PIC:
	    BCMP_SendStartBCMRequest(OTHER_DECODE_OK);
		//CBaseRecord::Start_Record();
	    break;

	default:
		break;
	}
	return 0;
}

static int PipPlayCallBack(PLY_EVENT_TYPE_E event, void *buf)
{
	DbgOutput(DBG_DEBUG, "====Pip event type is %d====.\n", event);
    switch( event )
	{
    case STREAM_PLAYBACK_COMPLETE:
    	//g_PLYCtrl.Stop(PLAYER_PIP);
    	break;

    case STREAM_ERROR 			:
    	//g_PLYCtrl.Stop(PLAYER_PIP);
		break;

	case STREAM_SHOW_PIC:
//		if ( g_pPlayerCtrl )
//		{
//			//g_pPlayerCtrl->SetPreviewVisible(TRUE, g_pPlayerCtrl->mPreviewRect);
//		}
	break;

    default:
    	break;
	}
    return 0;
}

static int PcmPlayCallBack(PLY_EVENT_TYPE_E event, void *buf)
{
	DbgOutput(DBG_INFO, "====Pcm event type is %d====.\n", event);
	switch( event )
	{
	case STREAM_PLAYBACK_COMPLETE:
		break;

	case STREAM_ERROR:
		break;

	default:
		break;
	}
	return 0;
}

static int IOCallBack(PLY_EVENT_TYPE_E event, void *buf)
{
	DbgOutput(DBG_INFO, "====IO event type is %d, theBaseApp:%d====.\n", event, (int)theBaseApp);
	g_pBCM_Ctrl->StartShutDownTimer();
	return 0;
}

bool CheckVerbFromFlash()
{
	bool ret = true;
	char recvBuf[64] = {0};
	if ( g_pPlayerCtrl )
	{
		g_pPlayerCtrl->ReadFromFlash("board_id", recvBuf, sizeof(recvBuf));
		if ( strlen(recvBuf) == 0 || strstr(recvBuf, APP_VERB) == NULL )
		{
			ret = false;
		}
	}

	return ret;
}

int GetBoardType()
{
	int iType = 810;
	char recvBuf[64] = {0};

	if ( g_pPlayerCtrl )
	{
		g_pPlayerCtrl->ReadFromFlash("board_type", recvBuf, sizeof(recvBuf));
		if ( strlen(recvBuf) > 0 )
		{
			iType = atoi(recvBuf);
		}
	}
	printf("=========the board type:%d=============\n", iType);
	return iType;
}

#if BOARD_TYPE == BCM_BOARD_840

void SetAppVerbToEncryptDev(const char *cAppVerb, int iBufLen)
{
	if ( cAppVerb && iBufLen > 0 && g_pPlayerCtrl )
	{
		g_pPlayerCtrl->WriteToEncryptionChip(cAppVerb, iBufLen);
	}
}

char* GetAppVerbFormEncryptDev(char *cAppVerb, int iBufLen)
{
	char *cGetBuf = NULL;

	if ( cAppVerb && iBufLen > 0 && g_pPlayerCtrl )
	{
		cGetBuf = g_pPlayerCtrl->ReadFromEncryptionChip(cAppVerb, iBufLen);
	}

	return cGetBuf;
}

#endif


CBCM_PLTCtrl::CBCM_PLTCtrl()
{
	mbPlayingInitVideo 	= FALSE;
}

CBCM_PLTCtrl::~CBCM_PLTCtrl()
{
	DeInit();
}

int CBCM_PLTCtrl::Init(int iVerb)
{
	g_pPlayerCtrl = new CPlayerCtrl();
	if ( g_pPlayerCtrl == NULL )
		return D_FAILURE;

  

	OUTPUT_FORMAT_E output = (gs_SystemSetting.miOutputMode == 1) ? OUTPUT_FORMAT_NTSC : OUTPUT_FORMAT_1080P;
  

//#if defined(__mips__)
	g_pPlayerCtrl->Init(BOARD_ID_850, output);
//#else if defined(__arm__)
//	g_pPlayerCtrl->Init(BOARD_ID_850, output,
//			gs_SystemSetting.miTV_lightness, gs_SystemSetting.miTV_contrast, gs_SystemSetting.miTV_Saturation);
//#endif
	//g_pPlayerCtrl->Init(BOARD_ID_840, output);
	g_pPlayerCtrl->SetMainPlayerCallback(MainPlayCallBack);
	//g_pPlayerCtrl->SetPipPlayerCallback(PipPlayCallBack);
	//g_pPlayerCtrl->SetPcmPlayerCallback(PcmPlayCallBack);
	//if ( iVerb == BOARD_ID_840 )
	{
		//g_pPlayerCtrl->SetIOCallback(IOCallBack);
	}

	if ( gs_SystemSetting.mbHdmiAudioClose )
		g_pPlayerCtrl->SetHdmiMute(D_FALSE);

	return D_SUCCESS;
}

void CBCM_PLTCtrl::DeInit()
{
	if ( g_pPlayerCtrl != NULL )
	{
		g_pPlayerCtrl->Player_Stop();
		g_pPlayerCtrl->Preview_Stop();
		g_pPlayerCtrl->PCM_Stop();
		g_pPlayerCtrl->SetIOLed(D_FALSE);
		g_pPlayerCtrl->Standby();
		delete g_pPlayerCtrl;
		g_pPlayerCtrl = NULL;
	}
}

int CBCM_PLTCtrl::Stop(int idx)
{
	if ( g_pPlayerCtrl )
	{
		switch( idx )
		{
		case PLAYER_MAIN:
			g_pPlayerCtrl->Player_Stop();
			break;

		case PLAYER_PIP:
			g_pPlayerCtrl->Preview_Stop();
			break;

		case PLAYER_PCM:
			g_pPlayerCtrl->PCM_Stop();
			break;
		}

	}

	return 0;
}

int CBCM_PLTCtrl::Pause(int idx)
{
	if ( g_pPlayerCtrl )
	{
		switch( idx )
		{
		case PLAYER_MAIN:
			g_pPlayerCtrl->Player_Pause();
			break;

		case PLAYER_PIP:
			g_pPlayerCtrl->Preview_Pause();
			break;

		case PLAYER_PCM:
			g_pPlayerCtrl->PCM_Pause();
			break;
		}

	}
//	WSD_AL_PLAYER_Pause(idx);
	return 0;
}

int CBCM_PLTCtrl::Resume(int idx)
{
	if ( g_pPlayerCtrl )
	{
		switch( idx )
		{
		case PLAYER_MAIN:
			g_pPlayerCtrl->Player_Resume();
			break;

		case PLAYER_PIP:
			g_pPlayerCtrl->Preview_Resume();
			break;

		case PLAYER_PCM:
			g_pPlayerCtrl->PCM_Resume();
			break;
		}

	}
	//WSD_AL_PLAYER_Resume(idx);
	return 0;
}

int CBCM_PLTCtrl::Replay(int idx)
{
	// 由上层负责播放当前文件
	return 0;
}

int CBCM_PLTCtrl::ForWard()					// 快进
{
	g_pPlayerCtrl->Player_ForWard();
	//WSD_AL_PLAYER_Forward(PLAYER_MAIN);
	return 0;
}

int CBCM_PLTCtrl::BackWard()					// 后退
{
	g_pPlayerCtrl->Player_BackWard();
	//WSD_AL_PLAYER_Backward(PLAYER_MAIN);
	return 0;
}

int CBCM_PLTCtrl::PlayBG(char *pBGFileName)	// 背景视频
{
	if ( pBGFileName == NULL )
			return -1;

	if ( !IsFileExist(pBGFileName) )
	{
		DbgOutput(DBG_INFO, "the file:%s not exist.\n", pBGFileName);
		return -1;
	}
	if ( g_pPlayerCtrl )
	{
		strcpy(mBkgroundViewFile, pBGFileName);
		mBkgroundViewFile[MAX_BUFFER_LEN-1] = '\0';

	//	if ( !mbPlayingInitVideo )
			g_pPlayerCtrl->Background_Play(PLYMODE_FILE, pBGFileName);
	}

    return 0;
}

void CBCM_PLTCtrl::RePlayBkgroundView()
{
	g_pPlayerCtrl->Background_Play(PLYMODE_FILE, mBkgroundViewFile);
}

int CBCM_PLTCtrl::StopBG()
{
	if ( g_pPlayerCtrl )
	{
		g_pPlayerCtrl->Background_Stop();
	}

	return 0;
}

int CBCM_PLTCtrl::PlayPCM(char *pcFileName, int iVol)	// 播放混音 如气氛，
{
	if ( pcFileName == NULL )
			return -1;

	if ( !IsFileExist(pcFileName) )
	{
		DbgOutput(DBG_DEBUG, "the file:%s not exist.\n", pcFileName);
		return -1;
	}

	printf("==========>Play PCM:%s\n", pcFileName);
	if ( g_pPlayerCtrl )
	{
		//g_pPlayerCtrl->PCM_Play(PLYMODE_STREAM, pcFileName);
		g_pPlayerCtrl->PCM_Play(PLYMODE_FILE, pcFileName);
		g_pPlayerCtrl->PCM_SetVolume(iVol);
	}

	return 0;
}

int CBCM_PLTCtrl::StartRecord(char *pcFileName , int iVol)// 开始录音
{
//	if ( pcFileName == NULL )
//		return -1;
//	StopRecord();
//	WSD_AL_REC_PARAM_T Param;
//	memset(&Param, 0x00, sizeof(Param));
//
//	Param.RecType = WSD_AL_REC_MIC1_MUSIC_E;
//	strncpy((char *)Param.FileName, pcFileName, sizeof(Param.FileName)-1);
//	WSD_AL_RECORD_Start(&Param);
//	WSD_AL_RECORD_SetVolume(WSD_AL_REC_CHANNEL_LEFT_RIGHT_E, iVol);

	return 0;
}

void CBCM_PLTCtrl::StopRecord()				// 停止录音
{
//	WSD_AL_RECORD_Stop();
}

int CBCM_PLTCtrl::PlayRecord(char *pcFileName)// 播放录音
{
//	if ( pcFileName == NULL )
//		return -1;
//
////	WSD_AL_PLAYER_SetSource(PLAYER_PCM, FILE_STREAM, pcFileName);
////	WSD_AL_PLAYER_Play(PLAYER_PCM);
//	RECT rc = {0, 0, 0, 0};
//	PlayFile(PLAYER_MAIN, pcFileName, rc);

	return 0;
}

int CBCM_PLTCtrl::StartPhantom(RECT rc)		// 播放幻影，MTV等
{
	if ( g_pPlayerCtrl )
	{
		D_RECT rect;
		rect.Left 	= rc.left;
		rect.Top 	= rc.top;
		rect.Right	= rc.right;
		rect.Bottom	= rc.bottom;
		g_pPlayerCtrl->SetMtvVisible(D_TRUE,rect);
	}
	return 0;
}

int CBCM_PLTCtrl::StopPhantom()				// 停止幻影
{
	D_RECT rc;
	g_pPlayerCtrl->SetMtvVisible(D_FALSE, rc);
	//WSD_AL_PLAYER_ShowPlayWindow(PLAYER_MAIN, 0);
	return 0;
}

void CBCM_PLTCtrl::SetHdmi(BOOL bOpen)
{
	if ( g_pPlayerCtrl )
	{
		g_pPlayerCtrl->SetHdmiMute(bOpen);
	}
	//WSD_AL_AUDIO_SetHdmiMute(bOpen);
}

void CBCM_PLTCtrl::SetOutputMode(int iMode)
{
	if ( g_pPlayerCtrl )
	{
		OUTPUT_FORMAT_E output = (iMode == 1) ? OUTPUT_FORMAT_1080i : OUTPUT_FORMAT_NTSC;
		g_pPlayerCtrl->SetOutputMode(output);
	}
}

void CBCM_PLTCtrl::SetVolume(int iVol)
{
	//WSD_AL_AUDIO_SetVolume((WSD_AL_AUDIO_Volume)iVol);
	//printf("===========the volume:%d===========\n", iVol);
	//WSD_AL_AUDIO_SetVolume(iVol);
	g_pPlayerCtrl->Player_SetVolume(iVol);
}

void CBCM_PLTCtrl::SetMute(BOOL bMute)
{
	g_pPlayerCtrl->Player_SetMute(bMute);
	//WSD_AL_AUDIO_SetMute(bMute);
}

int	CBCM_PLTCtrl::GetAllAudioChannel(int &iNum)
{
	return g_pPlayerCtrl->Player_GetAudioCount(iNum);
//	iNum = 0;
//	long int ChanCount;
//	WSD_AL_PLAYER_GetAllAudioChannel(PLAYER_MAIN, &ChanCount);
//	iNum = ChanCount;
//	return iNum;
}

void CBCM_PLTCtrl::SetAudioTrack(int iTrack)
{
	g_pPlayerCtrl->Player_SetAudioTrack(iTrack);
	//WSD_AL_AUDIO_SetTrack((WSD_AL_AUDIO_TRACK_E)iTrack);
}

void CBCM_PLTCtrl::SetAudioChannel(int iChannel)
{
	g_pPlayerCtrl->Player_SetAudioChannel(iChannel);
	//WSD_AL_PLAYER_SetCurAudioChannel(PLAYER_MAIN, iChannel);
}

void CBCM_PLTCtrl::SetInPlayingInitVideo(BOOL bPlaying)
{
	mbPlayingInitVideo = bPlaying;
}

BOOL CBCM_PLTCtrl::GetInitVideoState()
{
	return mbPlayingInitVideo;
}

void CBCM_PLTCtrl::SetGuideState(BOOL bState)
{
	//mbGuideState = bState;
}

BOOL CBCM_PLTCtrl::GetGuideState()
{
	return FALSE;
	//return mbGuideState;
}

int CBCM_PLTCtrl::GetDuration(int *iFileTime)
{
	int ret = -1;
	if ( iFileTime )
	{
		g_pPlayerCtrl->Player_GetDuration(iFileTime);
		//WSD_AL_PLAYER_GetDuration(PLAYER_MAIN, (long int *)iFileTime);
		ret = 0;
	}
	return ret;
}

int CBCM_PLTCtrl::GetCurPosition(int *iFileTime)
{
	int ret = -1;
	if ( iFileTime )
	{
		g_pPlayerCtrl->Player_GetPosition(iFileTime);
		//WSD_AL_PLAYER_GetCurPosition(PLAYER_MAIN, (long int *)iFileTime);
		ret = 0;
	}
	return ret;
}

void CBCM_PLTCtrl::SeekTo(int iFileTime)
{
	g_pPlayerCtrl->Player_SeekTo(iFileTime);
	//WSD_AL_PLAYER_SeekTo(PLAYER_MAIN, iFileTime);
}

int	CBCM_PLTCtrl::GetPlayState(int idx)
{
	int ret = 0 ;
	switch ( idx )
	{
	case PLAYER_MAIN:
		ret = g_pPlayerCtrl->Player_GetPlayState();
		break;

	case PLAYER_PCM:
		ret = g_pPlayerCtrl->PCM_GetPlayState();
		break;

	case PLAYER_PIP:
		ret = g_pPlayerCtrl->Preview_GetPlayState();
		break;
	}

	return ret;
//	WSD_AL_PLAYER_STATUS_T Status;
//	WSD_AL_PLAYER_GetStatus(idx, &Status);
//	return Status.PlayState;
}

int CBCM_PLTCtrl::GetSubtitle(int idx, char* pSubtitle, unsigned long *pInterval)
{
	return g_pPlayerCtrl->GetSubtitle(pSubtitle, pInterval);

//	int iGetOK = -1;
//	long int vtime;
//	int ret = -1;
//	D_UINT8 Changed;
//
//	WSD_AL_PLAYER_GetVideoTime(idx, &vtime);
//	ret = WSD_MW_SUBTITLE_GetData(vtime, pSubtitle, &Changed, pInterval);
//	//printf("==============the Get Time:%d, Interval:%d, changed:%u\n", vtime, *pInterval, Changed);
//	if ( (ret == 0) && ( D_TRUE == Changed) )
//	{
//		iGetOK = 0;
//	}
//
//	return iGetOK;
}

void CBCM_PLTCtrl::SetSubtitleParse(char *pSubtitle)
{
	if ( pSubtitle )
	{
		//WSD_MW_SUBTITLE_Parse(pSubtitle, WSD_SUBTITLE_SRT);
		g_pPlayerCtrl->SetSubtitleParse(pSubtitle);
		printf("===Start parser %s.===\n", pSubtitle);
	}
}

void CBCM_PLTCtrl::Standby()
{
	DbgOutput(DBG_INFO, "====Standby now====.\n");
	theBaseApp->SuspendInput();
	theBaseApp->AddQuitMsg();
}
#endif
