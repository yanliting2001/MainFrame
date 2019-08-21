#include "BCM_Control.h"
#include "../PlayerApi/PlayerInfo.h"
#include "../SQL/FastSearch.h"
#define XMLPATH		"./KTV_Data/xml/chs/OSDMsg.xml"

#define NRE_SETVOL_METHOD		1

#if USE_MODULE_BCM
BOOL				g_bExitBCM = FALSE;
CBCM_Ctrl   		*g_pBCM_Ctrl = NULL;
static CBaseLock	g_GetCurPositionLock;
CBaseStringA		gLogoText;

static void StartApp()
{
	XMLParser parser;
	if(false == parser.Load(XMLPATH))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", XMLPATH);
		return ;
	}

	const char *ptr = parser.GetStrValue("StartSysVideoFileName","path","./KTV_Data/osdmpg/StartTv.mpg");
	if ( ptr && IsFileExist(ptr) && g_pPlayerCtrl )
	{
		RECT rc = {1045, 945, 184, 121};
		//g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, ptr, FALSE, FALSE);
		g_pPlayerCtrl->Player_Play(PLYMODE_FILE, ptr, FALSE, FALSE);
		g_PLYCtrl.StartPhantom(rc);
		g_PLYCtrl.SetInPlayingInitVideo(TRUE);
		//g_PLYCtrl.SetInPlayingInitVideo(FALSE);
#if NRE_SETVOL_METHOD
		//g_pBCM_Ctrl->SelectAudio(miCurUITrack);
		int ival = (int)(gs_SystemSetting.miStartUp_vol * 0.8);
		g_pBCM_Ctrl->SetVolume(ival);
#endif
	}
	else
		g_pBCM_Ctrl->StartBCM();

  g_pBCM_Ctrl->StartAudioCompareInit();
}
#endif

void BCM_Init()
{
#if USE_MODULE_BCM
	if ( !g_pBCM_Ctrl )
	{
		g_pBCM_Ctrl = new CBCM_Ctrl();
		if ( !g_pBCM_Ctrl )
		{
			DbgOutput(DBG_ERR, "Err[%s]: New CBCM_Ctrl failed.You need to Quit the program\n", __FUNCTION__);
			return ;
		}
		g_pBCM_Ctrl->InitBCM();
	}
	StartApp();
#else
	return ;
#endif
}

void BCM_DeInit()
{
#if USE_MODULE_BCM
	if ( g_pBCM_Ctrl )
	{
		g_pBCM_Ctrl->DeInitBCM();
		delete g_pBCM_Ctrl;
		g_pBCM_Ctrl = NULL;
	}
#else
	return ;
#endif
}

#if USE_NEW_GEN_RANK
static void UpdateSongOrderTimes(const char *cID)
{
	if ( !cID || !gPlayerInfo )
		return ;

	if ( gPlayerInfo->IsSongInSelectedList(cID) )
		FastAdd_SongOrderTimes(cID);
}
#endif

#if WORKMODE == NET_WORKMODE

void BCM_RoomScrollInfo(int iTime)
{
#if USE_MODULE_BCM
	RGBQUAD textColor ;
	textColor.rgbBlue 	= gs_RoomOSDScroll.mrColor;
	textColor.rgbGreen	= gs_RoomOSDScroll.mgColor;
	textColor.rgbRed	= gs_RoomOSDScroll.mbColor;
	textColor.rgbReserved=0xFF;

	printf("===========the len:%d=======\n", gs_RoomOSDScroll.mScrolMsg.GetLength());

	RoomScrollInfo(gs_RoomOSDScroll.mScrolMsg.GetString(), gs_RoomOSDScroll.miScrollRange, gs_RoomOSDScroll.miFontSize, iTime, textColor);

#endif
}

#endif

#if USE_MODULE_BCM

static void SleepUIThreadFunc()
{
	usleep(3000);
}

static void SleepGuideThreadFunc()
{
	usleep(1000000);		//休眠1s
}
static LPVOID MonitorUIThreadFunc(LPVOID lpParam)
{
	CBCM_Ctrl *ptr = (CBCM_Ctrl *)lpParam;
	if ( ptr )
		ptr->MinitorUIThreadFunc();

	DbgOutput(DBG_INFO, "===the %s quit===\n", __FUNCTION__);
	return NULL;
}

static void GetColor(const char *buf, RGBQUAD *pOutColor)
{
	if ( !buf || !pOutColor )
		return ;

	char r[3];
	strncpy(r,buf,2);
	r[2] = '\0';
	pOutColor->rgbBlue = (int)(strtoul(r,NULL,16));
	strncpy(r,(buf+2),2);
	r[2] = '\0';
	pOutColor->rgbGreen = (int)(strtoul(r,NULL,16));
	strncpy(r,(buf+4),2);
	r[2] = '\0';
	pOutColor->rgbRed = (int)(strtoul(r,NULL,16));
}

CBCM_Ctrl::CBCM_Ctrl() : miServerSock(-1)
{
	miCurUITrack = Track_R;
	miCurSongTrack = Track_R;
	miVol = gs_SystemSetting.miStartUp_vol;
	miCurBGVideoIdx = 0;
	memset(mPlayingFile, 0, sizeof(mPlayingFile));
	memset(mPreviewFile, 0, sizeof(mPreviewFile));
	miPlayStatus = PLAYSTATUS_STOP;
	mbMinitorGuideStatus = TRUE;
	miInNetMode = FALSE;
	mbOpenMultiChannelFlag	= FALSE;
}

CBCM_Ctrl::~CBCM_Ctrl()
{
	DeInitBCM();
}

void CBCM_Ctrl::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, 0, 0};
	CParentClass::Create("CBCM_Ctrl", pParent,rc);
}

void CBCM_Ctrl::LoadResource()
{
	XMLParser parser;
	if(false == parser.Load(XMLPATH))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", XMLPATH);
		return ;
	}
	char colorbuf[16];
#if WORKMODE == LOCAL_WORKMODE
	gLogoText.Set(parser.GetStrValue("TestFormScrollLogo", "Text", "欢迎使用络星娱乐终端"));
	UINT32 uTextColor = 0xFFFFFFFF;
	LoadScrollText(gLogoText, uTextColor);

	mImgInfo.miPLTID = parser.GetIntValue("StartSysVideoFileName", "Verb", 810);

	XmlLoadRect(&parser, "MainScrollInfo", &(mImgInfo.mMainScrollText.mRc));
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("MainScrollInfo","Color","FFFFFF"), sizeof(colorbuf));
	GetColor(colorbuf, &mImgInfo.mMainScrollText.msColor);
	memset(&mImgInfo.mMainScrollText.msBgColor, 0x00, sizeof(RGBQUAD));
	mImgInfo.mMainScrollText.miSize = parser.GetIntValue("MainScrollInfo", "Size", 45);
	mImgInfo.mMainScrollText.miSpeed = parser.GetIntValue("MainScrollInfo", "Speed", 3);
	mImgInfo.mMainScrollText.miTime = parser.GetIntValue("MainScrollInfo", "Time", 0);
	mImgInfo.mMainScrollText.meType = ScrollType_Main;
	memcpy(&mImgInfo.mMainScrollText.msColor, &uTextColor, sizeof(mImgInfo.mMainScrollText.msColor));
#else
	mImgInfo.miPLTID = parser.GetIntValue("StartSysVideoFileName", "Verb", 810);
	gLogoText.Set(gs_OSDScroll.mScrolMsg.GetString());
	mImgInfo.mMainScrollText.miSize				= gs_OSDScroll.miFontSize;
	mImgInfo.mMainScrollText.miSpeed			= gs_OSDScroll.miScrollSpeed;
	mImgInfo.mMainScrollText.miTime				= 0;
	memset(&mImgInfo.mMainScrollText.msBgColor, 0x00, sizeof(RGBQUAD));
	mImgInfo.mMainScrollText.msColor.rgbBlue 	= gs_OSDScroll.mrColor;
	mImgInfo.mMainScrollText.msColor.rgbGreen	= gs_OSDScroll.mgColor;
	mImgInfo.mMainScrollText.msColor.rgbRed		= gs_OSDScroll.mbColor;
	mImgInfo.mMainScrollText.msColor.rgbReserved=0xFF;
	mImgInfo.mMainScrollText.meType 			= ScrollType_Main;
	memcpy(&(mImgInfo.mMainScrollText.mRc), &gs_OSDScroll.miScrollRange, sizeof(RECT));
#endif

	XmlLoadRect(&parser, "SubScrollInfo", &(mImgInfo.mSubScrollText.mRc));
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("SubScrollInfo","Color","FFFFFF"), sizeof(colorbuf));
	GetColor(colorbuf, &mImgInfo.mSubScrollText.msColor);
	memset(&mImgInfo.mSubScrollText.msBgColor, 0x00, sizeof(RGBQUAD));
	mImgInfo.mSubScrollText.miSize = parser.GetIntValue("SubScrollInfo", "Size", 45);
	mImgInfo.mSubScrollText.miSpeed = parser.GetIntValue("SubScrollInfo", "Speed", 3);
	mImgInfo.mSubScrollText.miTime = parser.GetIntValue("SubScrollInfo", "Time", 0);
	mImgInfo.mSubScrollText.meType = ScrollType_Sub;

	XmlLoadRect(&parser, "SwitchScrollRect", &(mImgInfo.mSwitchInfo.mScrollRect));
	XmlLoadRect(&parser, "SwitchScrollBGINFO", &(mImgInfo.mSwitchInfo.mBGRect));
	XmlLoadRect(&parser, "SwitchSongPicRect", &(mImgInfo.mSwitchInfo.mSongPicRect));
	XmlLoadRect(&parser, "SwitchPlayingRect", &(mImgInfo.mSwitchInfo.mPlayingScrollRect));
	XmlLoadRect(&parser, "SwitchNextRect", &(mImgInfo.mSwitchInfo.mNextScrollRect));
	mImgInfo.mSwitchInfo.mBGPath.Set(parser.GetStrValue("SwitchScrollBGINFO", "Path", "KTV_Data/osdimg/picture.bmp"));

	XmlLoadRect(&parser, "Wartermark", &(mImgInfo.mWartermarkInfo.mRc));
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("Wartermark","Color","FFFFFF"), sizeof(colorbuf));
	GetColor(colorbuf, &mImgInfo.mWartermarkInfo.msColor);
	memset(&mImgInfo.mWartermarkInfo.msBgColor, 0x00, sizeof(RGBQUAD));
	mImgInfo.mWartermarkInfo.miNum = parser.GetIntValue("Wartermark", "num", 7);
	mImgInfo.mWartermarkInfo.miSize = parser.GetIntValue("Wartermark", "size", 26);
	mImgInfo.mWartermarkInfo.mbOpen = FALSE;

	XmlLoadRect(&parser, "VolGBInfo", &(mImgInfo.mVolumeInfo.mBGRect));
	mImgInfo.mVolumeInfo.mBGPath.Set(parser.GetStrValue("VolGBInfo", "Path", "KTV_Data/osdimg/Pole.bmp"));
	XmlLoadRect(&parser, "VolScrollInfo", &(mImgInfo.mVolumeInfo.mScrollRect));
	mImgInfo.mVolumeInfo.mThumbPath.Set(parser.GetStrValue("VolScrollInfo", "Path", "KTV_Data/osdimg/Thumb.bmp"));

	XmlLoadRect(&parser, "AmbienceInfo", &(mImgInfo.mAmbienceInfo.mRcet));
	mImgInfo.mAmbienceInfo.mPath.Set(parser.GetStrValue("AmbienceInfo", "Path", "KTV_Data/osdimg/Ambience.bmp"));
	mImgInfo.mAmbienceInfo.mCheerPath.Set(parser.GetStrValue("AmbienceInfo", "CheerPath", "KTV_Data/osdwav/0.wav"));
	mImgInfo.mAmbienceInfo.mHootPath.Set(parser.GetStrValue("AmbienceInfo", "HootPath", "KTV_Data/osdwav/1.wav"));

	XmlLoadRect(&parser, "CtrlImgInfo", &(mImgInfo.mCtrlImgInfo.mRc));
	mImgInfo.mCtrlImgInfo.mPausePath.Set(parser.GetStrValue("CtrlImgInfo", "PausePath", "KTV_Data/osdimg/Pause.bmp"));
	mImgInfo.mCtrlImgInfo.mResumePath.Set(parser.GetStrValue("CtrlImgInfo", "ResumePath", "KTV_Data/osdimg/Play.bmp"));
	mImgInfo.mCtrlImgInfo.mReplayPath.Set(parser.GetStrValue("CtrlImgInfo", "ReplayPath", "KTV_Data/osdimg/Replay.bmp"));
	mImgInfo.mCtrlImgInfo.mMutePath.Set(parser.GetStrValue("CtrlImgInfo", "MutePath", "KTV_Data/osdimg/Mute.bmp"));
	mImgInfo.mCtrlImgInfo.mOriginalPath.Set(parser.GetStrValue("CtrlImgInfo", "OriginalPath", "KTV_Data/osdimg/Track1.bmp"));
	mImgInfo.mCtrlImgInfo.mAccompanyPath.Set(parser.GetStrValue("CtrlImgInfo", "AccompanyPath", "KTV_Data/osdimg/Track0.bmp"));

	XmlLoadRect(&parser, "LogoInfo", &(mImgInfo.mLogo.mRc));
	mImgInfo.mLogo.mPath.Set(parser.GetStrValue("LogoInfo", "Path", "KTV_Data/osdimg/Logo.bmp"));

	XmlLoadRect(&parser, "MovieInfo", &(mImgInfo.mMovie.mRc));
	mImgInfo.mMovie.mPath.Set(parser.GetStrValue("MovieInfo", "Path", "KTV_Data/osdimg/WaitMovie.bmp"));

	XmlLoadRect(&parser, "GuideInfo", &(mImgInfo.mGuide.mRc));
	mImgInfo.mGuide.mPath.Set(parser.GetStrValue("GuideInfo", "Path", "KTV_Data/osdimg/Guide.bmp"));

	XmlLoadRect(&parser, "RecordInfo", &(mImgInfo.mRecord.mRc));
	mImgInfo.mRecord.mPath.Set(parser.GetStrValue("RecordInfo", "Path", "KTV_Data/osdimg/Record.bmp"));

	XmlLoadRect(&parser, "BarCodeLocation", &(mImgInfo.mBarCodeRect));
	XmlLoadRect(&parser, "SubTitleLocation", &(mImgInfo.mSubTitleRect));

	mImgInfo.mBGVideoPath[BGVideoType_Home].Set(parser.GetStrValue("BGVedioInfo", "HomePath", "KTV_Data/osdmpg/HomeFragment.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_Singer].Set(parser.GetStrValue("BGVedioInfo", "SingerPath", "KTV_Data/osdmpg/SingerFragment.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_Song].Set(parser.GetStrValue("BGVedioInfo", "SongPath", "KTV_Data/osdmpg/SongTypeFragment.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_Rank].Set(parser.GetStrValue("BGVedioInfo", "RankPath", "KTV_Data/osdmpg/RankFragment.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_Movie].Set(parser.GetStrValue("BGVedioInfo", "MoviePath", "KTV_Data/osdmpg/Movie.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_Language].Set(parser.GetStrValue("BGVedioInfo", "LanguagePath", "KTV_Data/osdmpg/LanguageTypeFragment.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_Pinyin].Set(parser.GetStrValue("BGVedioInfo", "PinyinPath", "KTV_Data/osdmpg/Pinyin.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_NewSong].Set(parser.GetStrValue("BGVedioInfo", "NewSongPath", "KTV_Data/osdmpg/NewSongFragment.mpg"));
	mImgInfo.mBGVideoPath[BGVideoType_KGO].Set(parser.GetStrValue("BGVedioInfo", "KGOPath", "KTV_Data/osdmpg/KGOFragment.mpg"));
}

void CBCM_Ctrl::StartShutDownTimer()
{
#if BOARD_TYPE == BCM_BOARD_840
	AddTimer(TimeHandle_ShutDown, 800);
#endif
}

void CBCM_Ctrl::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);

	if ( nTimerID == TimeHandle_SaveSong )
	{
#if USE_NEW_GEN_RANK
		FastSave_AllSongList();
#endif
	}

	if ( nTimerID == TimeHandle_ShutDown )
	{
#if BOARD_TYPE == BCM_BOARD_840
		g_PLYCtrl.Standby();
#endif
	}

	if ( nTimerID == TimeHandle_SRTFile )
	{
		//printf("+++++++++++++++++++\n");
		unsigned long lInterVal = 0;
		char strSubtitle[1024] = {0};
		if ( (g_PLYCtrl.GetPlayState(PLAYER_MAIN) == STATE_PLAY) &&
			 (g_PLYCtrl.GetSubtitle(PLAYER_MAIN, strSubtitle, &lInterVal) == 0) )
		{
			//显示外挂字幕
			//printf("GetSubTitle====%s, InterVal:%lu\n", strSubtitle, lInterVal);
			ShowSubtitle(strSubtitle);
			if ( lInterVal == 0 )
				AddTimer(TimeHandle_SRTFile, 100);
			else
				AddTimer(TimeHandle_SRTFile, lInterVal);
		}

		return ;
	}

	DelTimer(nTimerID);
	RECT rc;

	if ( nTimerID == TimeHandle_SwitchScroll)
	{
		rc.left 	= mImgInfo.mSwitchInfo.mScrollRect.left;
		rc.top  	= mImgInfo.mSwitchInfo.mScrollRect.top;
		rc.right	= mImgInfo.mSwitchInfo.mScrollRect.left + RECTWIDTH(mImgInfo.mSwitchInfo.mBGRect)+30;
		rc.bottom 	= mImgInfo.mSwitchInfo.mScrollRect.bottom;
		g_OSDCtrl.EraseImg(rc);
		//g_OSDCtrl.Update();
	}
	else if ( nTimerID == Timehandle_Volume )
	{
		ShowVolume(FALSE);
	}
	else if ( nTimerID == Timehandle_Ambience )
	{
		ShowAmbience(FALSE);
		ShowCtrlImg(CMD_PAUSE, FALSE);
		UpdateWartermark();
	}
	else if ( nTimerID == TimeHandle_CtrlImg )
	{
		ShowCtrlImg(CMD_PAUSE, FALSE);
	}
}

void CBCM_Ctrl::InitBCM()
{
	//char strFontPath[] = "./KTV_Data/fonts/arialuni.ttf";

	LoadResource();

	g_PLYCtrl.Init(mImgInfo.miPLTID);

	g_OSDCtrl.Init();

	StartMonitorUIState();

	//StartMonitorGuideState();
	mFont.LoadFont(FONTPATH);

	g_bExitBCM = FALSE;
}

void CBCM_Ctrl::DeInitBCM()
{
	if ( !g_bExitBCM )
	{
		g_bExitBCM = TRUE;

		g_OSDCtrl.DeInit();

		//StopMonitorGuideState();

		StopMonitorUIState();

		g_PLYCtrl.DeInit();

		mAudioCompareOSD.AudioCompareDeinit();
	}
}

void CBCM_Ctrl::StartMonitorUIState()
{
	MonitorUIThread.StartThread(MonitorUIThreadFunc, this, "MonitorUIThreadFunc");
}

void CBCM_Ctrl::StopMonitorUIState()
{
	shutdown(miServerSock, SHUT_RDWR);
	CloseSock(miServerSock);
	SleepUIThreadFunc();
	MonitorUIThread.StopThread();
	DbgOutput(DBG_INFO, "MonitorUIThread stopping...\n");
}

void CBCM_Ctrl::StartMonitorGuideState()
{
	//mMonitorGuideThread.StartThread(MonitorGuideThreadFunc, this, "MonitorGuideThreadFunc");
}

void CBCM_Ctrl::StopMonitorGuideState()
{
	SleepGuideThreadFunc();
	mMonitorGuideThread.StopThread();
	DbgOutput(DBG_INFO, "mMonitorGuideThread stopping...\n");
}

void CBCM_Ctrl::MinitorUIThreadFunc()
{
	miServerSock = CreateServer(PLAYERAPI_SOCKET_CLIENTPORT);
	if ( miServerSock < 0 )
	{
		DbgOutput(DBG_ERR, "#Err[%s]: Create Server failed:%d.\n", __FUNCTION__, PLAYERAPI_SOCKET_CLIENTPORT);
		return ;
	}

	while( !g_bExitBCM )
	{
		ProcessCmdMsg();
		SleepUIThreadFunc();
	}

	DbgOutput(DBG_INFO, "the MonitorUIThreadFunc stoped.\n");
	CloseSock(miServerSock);
}

int CBCM_Ctrl::RecvCmdMsg(int &sock, char *msg, int iLen)
{
	if ( sock <= 0 || !msg || iLen <= 0 )
		return -1;

	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	int msgsocket = accept(sock, (struct sockaddr*)&client_addr, &length);
	if ( msgsocket < 0)
	{
		if ( (errno == ECONNABORTED) || (errno == EINTR) )
			return -1;

		// acceptc 出错，重新创建服务sock
		printf("####Err[%s]: Accept failed:%d.\n", __FUNCTION__, errno);
		CloseSock(sock);
		sock = CreateServer(PLAYERAPI_SOCKET_CLIENTPORT);
		if ( sock < 0 )
		{
			DbgOutput(DBG_ERR, "#Err[%s]: Create Server failed:%d.\n", __FUNCTION__, PLAYERAPI_SOCKET_CLIENTPORT);
		}
		return -1;
	}
	memset(msg, 0, iLen);
	if ( (length = RecvMsg(msgsocket, msg, iLen)) <= 0 )
	{
		DbgOutput(DBG_ERR, "#Err[%s]: Recieve Data Failed!\n",  __FUNCTION__);
		CloseSock(msgsocket);
		return -1;
	}
	else
	{
		SendMsg(msgsocket,"0",1);
	}
	CloseSock(msgsocket);

	return 0;
}

int CBCM_Ctrl::AnalyseCmdMsg(char *vCmdMsgInfo, char *vCmdMsg[], int iCmdMaxNum)
{
	if ( !vCmdMsgInfo || !vCmdMsg )
		return -1;

	int vMsgCount = 0;
	int vCmdMessageSize = strlen(vCmdMsgInfo);
	if ( vCmdMessageSize > 0 )
	{
		int vCmdMessageIndex;
		for (vCmdMessageIndex = 0; vCmdMessageIndex < iCmdMaxNum; vCmdMessageIndex ++)
		{
			vCmdMsg[vCmdMessageIndex] = NULL;
		}

		char *p, *q;
		p = vCmdMsgInfo;
		while(p < vCmdMsgInfo + vCmdMessageSize)
		{
			q = strchr(p,',');
			if( q )
			{
				*q=0;
				vCmdMsg[vMsgCount++] = p;
				p = q + 1;
			}
			else
				break;
		}
	}

	return vMsgCount;
}

void CBCM_Ctrl::ProcessCmdMsg()
{
	char vCmdMsgInfo[BUFLEN] ={0};
	char *vCmdMsg[20];
    int vMsgCount = 0;

    if ( RecvCmdMsg(miServerSock, vCmdMsgInfo, BUFLEN) != 0)
    	return ;

	vMsgCount = AnalyseCmdMsg(vCmdMsgInfo, vCmdMsg, 20);
	if ( vMsgCount <= 0 )
		return ;

#if 1
	printf("###Recv Message: ");
	for(int i = 0; i < vMsgCount ; i++ )
	{
		printf("%d -- %s, ", i, vCmdMsg[i]);
	}
	printf("\n");
#endif

    if ( vCmdMsg[0] && strcmp(vCmdMsg[0],CMDLEAD) != 0 )
    	return ;

    if ( vCmdMsg[1] && strcmp(vCmdMsg[1],PLYTYPE) == 0 )
    {
    	if ( !vCmdMsg[2] )
    		return ;

    	switch(vCmdMsg[2][0])
    	{
    	case 'N':
    		if ( strcmp(vCmdMsg[2], PLY_AUTO_PLAYNEXT) == 0 )
    		{
    			miCurSongTrack = vCmdMsg[7] ? (atoi(vCmdMsg[7])) % 2 : miCurSongTrack;
    			miCurUITrack   = vCmdMsg[8] ? atoi(vCmdMsg[8]) : miCurUITrack;
    			miVol		   = vCmdMsg[6] ? atoi(vCmdMsg[6]) : miVol;
#if (WORKMODE == LOCAL_WORKMODE) && (HISI_NET_APP == OFF)
    			PlayNext(vCmdMsg[4], vCmdMsg[9], vCmdMsg[11], TRUE);
#if USE_NEW_GEN_RANK
    			UpdateSongOrderTimes(vCmdMsg[3]);
#endif
#elif WORKMODE == NET_WORKMODE
				char url[1024] = {0};
				snprintf(url, sizeof(url), "http://%s:8966/%s", vCmdMsg[5], vCmdMsg[4]);
				PlayNext(url, vCmdMsg[9], vCmdMsg[11], TRUE);
#else
				char url[1024] = {0};
				snprintf(url, sizeof(url), "http://%s:8966/%s", getServerIpForNetApp(), vCmdMsg[5]);
				PlayNext(url, vCmdMsg[9], vCmdMsg[11], TRUE);
#endif
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_MANUAL_PLAYNEXT) == 0 )
    		{
    			miCurSongTrack = vCmdMsg[7] ? (atoi(vCmdMsg[7])) % 2 : miCurSongTrack;
    			miCurUITrack   = vCmdMsg[8] ? atoi(vCmdMsg[8]) : miCurUITrack;
    			miVol		   = vCmdMsg[6] ? atoi(vCmdMsg[6]) : miVol;
#if WORKMODE == LOCAL_WORKMODE && (HISI_NET_APP == OFF)
    			PlayNext(vCmdMsg[4], vCmdMsg[9], vCmdMsg[11], FALSE);
#if USE_NEW_GEN_RANK
    			UpdateSongOrderTimes(vCmdMsg[3]);
#endif
#elif WORKMODE == NET_WORKMODE
				char url[1024] = {0};
				snprintf(url, sizeof(url), "http://%s:8966/%s", vCmdMsg[5], vCmdMsg[4]);
				PlayNext(url, vCmdMsg[9], vCmdMsg[11], FALSE);
#else
				char url[1024] = {0};
				snprintf(url, sizeof(url), "http://%s:8966/%s", getServerIpForNetApp(), vCmdMsg[5]);
				PlayNext(url, vCmdMsg[9], vCmdMsg[11], TRUE);
#endif
    		}
    		break;
    	case 'R':
    		if ( strcmp(vCmdMsg[2], PLY_MAIN_REPEAT ) == 0)
    		{
    			Replay();
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PIP_RESUME) ==0 )
    		{
    			RePreview();
    		}
    		break;
    	case 'P':
    		if ( strcmp(vCmdMsg[2], PLY_MAIN_PAUSE) == 0 )
    		{
    			Pause();
            }
    		else if ( strcmp(vCmdMsg[2], PLY_MAIN_RESUME) ==0 )
    		{
    			Resume();
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PIP_PAUSE) ==0 )
    		{
    			g_PLYCtrl.Pause(PLAYER_PIP);
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PLAY_PHANTOM) == 0)
    		{
    			RECT rRect = {1045, 945, 184, 121};
    			g_PLYCtrl.StartPhantom(rRect);
    		}
    		else if( strcmp(vCmdMsg[2], PLY_PLAY_MTV) == 0 )
    		{
    			if ( !vCmdMsg[3] || !vCmdMsg[4] || !vCmdMsg[5] || !vCmdMsg[6])
    				DbgOutput(DBG_INFO, "the cmd is wrong, has null.\n");
    			else
    			{
    				mRcMTV.left = atoi(vCmdMsg[3]);
    				mRcMTV.top = atoi(vCmdMsg[4]);
    				mRcMTV.right = mRcMTV.left + atoi(vCmdMsg[5]);
    				mRcMTV.bottom = mRcMTV.top + atoi(vCmdMsg[6]);
    				g_PLYCtrl.StartPhantom(mRcMTV);
    			}
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_FULL_MTV) == 0 )
    		{
    			RECT rRect = {0, 0, 1920, 1080};
    			g_PLYCtrl.StartPhantom(rRect);
    		}
    		break;
    	case 'S':
    		if ( strcmp(vCmdMsg[2], PLY_SWITCH_AUDIO) == 0 )
    		{
    			if ( vCmdMsg[4] )
    				miCurUITrack = atoi( vCmdMsg[4]);
    			SelectAudio( miCurUITrack );
    			ShowCtrlImg(miCurUITrack==1?CMD_ACCOMPANY:CMD_ORIGINAL, TRUE);
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PLAY_SITUATION) == 0 )
    		{
    			if ( vCmdMsg[3] && strlen(vCmdMsg[3]) > 0 )
               		PlaySituation(vCmdMsg[3]);
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PIP_PLAY) == 0 )
    		{
    			if ( !vCmdMsg[4] || !vCmdMsg[5] || !vCmdMsg[6] || !vCmdMsg[7] || !vCmdMsg[8] )
    				DbgOutput(DBG_INFO, "the cmd is wrong, has null.\n");
    			else
    			{
    				mRcPreview.left = atoi(vCmdMsg[5]);
    				mRcPreview.top = atoi(vCmdMsg[6]);
    				mRcPreview.right = mRcPreview.left + atoi(vCmdMsg[7]);
    				mRcPreview.bottom = mRcPreview.top + atoi(vCmdMsg[8]);
#if WORKMODE == LOCAL_WORKMODE && (HISI_NET_APP == OFF)
    				PlayPreview(vCmdMsg[4], mRcPreview);
#elif WORKMODE == NET_WORKMODE
    				char url[1024] = {0};
    				snprintf(url, sizeof(url), "http://%s:8966/%s", vCmdMsg[3], vCmdMsg[4]);
    				PlayPreview(url, mRcPreview);
#else
    				char url[1024] = {0};
    				snprintf(url, sizeof(url), "http://%s:8966/%s", getServerIpForNetApp(), vCmdMsg[3]);
    				PlayPreview(url, mRcPreview);
#endif
    			}
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PIP_STOP) == 0 )
    		{
    			//g_PLYCtrl.Stop(PLAYER_PIP);
    			//WSD_AL_PLAYER_ShowPlayWindow(PLAYER_PIP, 0);
    			ChangeBGVideo(miCurBGVideoIdx);
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_STOP_PHANTOM) == 0 )
    		{
    			g_PLYCtrl.StopPhantom();
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_STOP_MTV) == 0 )
    		{
    			g_PLYCtrl.StopPhantom();
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_MAIN_STOP) == 0 )
    		{
    			g_PLYCtrl.Stop(PLAYER_MAIN);
    			if ( IsTimerSet(TimeHandle_SRTFile) )
    			{
    				DelTimer(TimeHandle_SRTFile);
    				char buf[8] = {0};
    				ShowSubtitle(buf);//主要是清除之前的效果
    				if ( g_pPlayerCtrl )
    					g_pPlayerCtrl->DeInitSubtitle();
    			}
    		}
    		else if ( strcmp(vCmdMsg[2], OTHER_START_BCM) == 0 )
    		{
    			StartBCM();
    		}
    		else if ( strcmp(vCmdMsg[2], OTHER_DECODE_OK) == 0 )
    		{
    			StartPlaySong();
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_SEEKTO) == 0 )
    		{
    			if ( vCmdMsg[3] )
    			{
    				int filetime = atoi(vCmdMsg[3]);
    				if ( filetime >= 0 )
    					g_PLYCtrl.SeekTo(filetime);
    			}
    		}
    		break;
    	case 'V':
    		if ( strcmp(vCmdMsg[2], PLY_MAIN_SETVOL) == 0 )
    		{
    			if ( vCmdMsg[3] != NULL )
    			{
    				 SetVolume(atoi(vCmdMsg[3]));
    				 ShowVolume(TRUE);
    			}
    		}
    		break;
    	case 'F':
    		if ( strcmp(vCmdMsg[2], PLY_SET_FULLMTV) == 0)
    		{
    			//TODO:
    			printf("The ===========%s.\n", PLY_SET_FULLMTV);
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PLAY_FORWARD) == 0 )
    		{
    			g_PLYCtrl.ForWard();
    		}
    		break;
    	case 'C':
    		if ( strcmp(vCmdMsg[2], PLY_PLAY_CHEER) == 0 )
    		{
    			if ( vCmdMsg[3] )
    				PlayCheer(atoi(vCmdMsg[3]));
    		}
    		break;
    	case 'M':
    		if ( strcmp(vCmdMsg[2], PLY_PLAY_MOVIE) == 0 )
    		{
    			if ( vCmdMsg[3] )
    				PlayUSBMovie(vCmdMsg[3]);
    		}
    		else if ( strcmp(vCmdMsg[2], PLY_PLAY_MULTCHANNEL) == 0 )
    		{
    			if ( vCmdMsg[3] )
    			{
    				int bOpen = atoi(vCmdMsg[3]) % 2;
    				printf("-=================the bOpen:%d\n", bOpen);
    				mbOpenMultiChannelFlag = bOpen;
    				//g_PLYCtrl.SetMuitleChannel(bOpen);
    			}
    		}
    		break;
    	case 'B':
    		if ( strcmp(vCmdMsg[2], PLY_PLAY_BACKWARD) == 0 )
    		{
    			g_PLYCtrl.BackWard();
    		}
    		break;
    	case 'U':
    		if ( strcmp(vCmdMsg[2], PLY_MAIN_NETPLAY) == 0 )
    		{
    			if ( vCmdMsg[3] )
    				PlayNetMovie(vCmdMsg[3]);
    		}
    		break;
    	case 'H':
    		if ( strcmp(vCmdMsg[2], PLY_SET_HDMIAUDIO) == 0 )
    		{
    			if ( vCmdMsg[3] )
    			{
    				int bMute = atoi(vCmdMsg[3]) ? 0 : 1;
           			g_PLYCtrl.SetHdmi(bMute);
    			}
    		}
    		break;
////////////////测试吴工的WSD_AL_PLT_SetOuputParam///////////
//    	case 'T':
//    		if ( strcmp(vCmdMsg[2], "TEST_OUTPUT") == 0 )
//    		{
//    			if ( vCmdMsg[3] )
//    			{
//    				int bMain = atoi(vCmdMsg[3]) ? 1 : 0;
//
//    				WSD_OUTPUT_PARAM_T outputParam;
//    				WSD_AL_PLT_GetOuputParam(WSD_PORT_VGA_E, &outputParam);
//    				if ( bMain )
//    				{
//    					outputParam.Layer0Source = WSD_LAYER_SOURCE_PIP_VIDEO;
//    					outputParam.Layer1Source = WSD_LAYER_SOURCE_GUI;
//    					outputParam.Layer2Source = WSD_LAYER_SOURCE_MAIN_VIDEO;
//    				}
//    				else
//    				{
//    					outputParam.Layer0Source = WSD_LAYER_SOURCE_MAIN_VIDEO;
//    					outputParam.Layer1Source = WSD_LAYER_SOURCE_PIP_VIDEO;
//    					outputParam.Layer2Source = WSD_LAYER_SOURCE_GUI;
//
//    				}
//    				WSD_AL_PLT_SetOuputParam(WSD_PORT_VGA_E, &outputParam);
//    			}
//    		}
//    		break;

    	default:
    		break;
    	}
    }
    else if ( vCmdMsg[1] && strcmp(vCmdMsg[1],OSDTYPE) == 0)
    {
    	if ( !vCmdMsg[2] )
    		return ;

    	switch (vCmdMsg[2][0])
    	{
    	case 'S':
    		if ( strcmp(vCmdMsg[2], OSD_SHOW_MARK) == 0 )
    		{
    			if ( vCmdMsg[3] )
    				ShowSongmark(atoi(vCmdMsg[3]));
    		}
    		else if ( strcmp(vCmdMsg[2], OSD_SET_SATURATION) == 0 )
    		{
    			g_pPlayerCtrl->setSaturation(atoi(vCmdMsg[3]));
    		}
    		else if ( strcmp(vCmdMsg[2], OSD_SHOW_GUIDE) == 0 )
    		{
    			mbMinitorGuideStatus = TRUE;
    			ShowGuide(TRUE);
    		}
    		else if ( strcmp(vCmdMsg[2], OSD_STOP_GUIDE) == 0 )
    		{
    			mbMinitorGuideStatus = FALSE;
    			ShowGuide(FALSE);
    		}
    		else if ( strcmp(vCmdMsg[2], OSD_SHOW_RECORD) == 0 )
    		{
    			if ( vCmdMsg[3] )
    			{
    				g_PLYCtrl.StartRecord(vCmdMsg[3], miVol);
    				ShowRecord(TRUE);
    			}
    		}
    		else if(strcmp(vCmdMsg[2], OSD_STOP_RECORD) == 0)
    		{
    			g_PLYCtrl.StopRecord();
    			ShowRecord(FALSE);
    		}
    		else if(strcmp(vCmdMsg[2], OSD_OPEN_WATERMARK) == 0)
    		{
    			mImgInfo.mWartermarkInfo.mbOpen = TRUE;
    			UpdateWartermark();
    		}
    		break;
    	case 'A':
    		if ( strcmp(vCmdMsg[2], OSD_SHOW_AMBIENCE) == 0 )
    		{
    			if ( vCmdMsg[3] )
    			{
    				ShowAmbience(TRUE);
    				if ( strcmp(APP_VERB, "sd500-jundui") != 0 )
    					PlayCheer(atoi(vCmdMsg[3]));
    			}
    		}
    	break;

    	case 'B':
    		if ( strcmp(vCmdMsg[2], OSD_SHOW_BARCODE) == 0 )
    		{
    			if ( vCmdMsg[3] && vCmdMsg[4] )
    			{
    				int bShow = atoi(vCmdMsg[4]);
    				if ( bShow )
    				{
    					g_OSDCtrl.EraseImg(mImgInfo.mBarCodeRect);
    					g_OSDCtrl.ShowPicFile(vCmdMsg[3], mImgInfo.mBarCodeRect);
    				}
    				else
    					g_OSDCtrl.EraseImg(mImgInfo.mBarCodeRect);
    			}
    		}
    		break;

    	case 'P':
    		if ( strcmp(vCmdMsg[2], OSD_PLAY_RECORD) == 0 )
    		{
    			if ( vCmdMsg[3] )
    				g_PLYCtrl.PlayRecord(vCmdMsg[3]);
    		}
    		break;
    	case 'T':
    		if ( strcmp(vCmdMsg[2], OSD_SET_OUTPUTFORMAT) == 0 )
    		{
    			if ( vCmdMsg[3] )
    			{
    				int mode = atoi(vCmdMsg[3]);
    				g_PLYCtrl.SetOutputMode(mode);
    			}
    		}
    		break;
    	case 'U':
    		if ( strcmp(vCmdMsg[2], OSD_CLOSE_WATERMARK) == 0 )
    		{
    			mImgInfo.mWartermarkInfo.mbOpen = FALSE;
    			g_OSDCtrl.EraseImg(mImgInfo.mWartermarkInfo.mRc);
    		}
    		else if( strcmp(vCmdMsg[2], OSD_UPDATE_SCROLL) == 0 )
    		{
    			// OSD_UpdateMsgInfo(vCmdMsg[3],vCmdMsg[4]);
    		}
    		else if ( strcmp(vCmdMsg[2], OSD_UPDATE_WATERMARK) == 0 )
    		{
    			UpdateWartermark();
    		}
    		break;
    	case 'C':
    		if ( strcmp(vCmdMsg[2], OSD_SET_CONTRAST) == 0 )
    		{
    			g_pPlayerCtrl->setContrast(atoi(vCmdMsg[3]));
    		}
    		else if ( strcmp(vCmdMsg[2], OSD_SWITCH_BGVIDEO) == 0 )
    		{
    			if ( vCmdMsg[3] != NULL )
    				ChangeBGVideo(atoi(vCmdMsg[3]));
    		}
    		break;
    	case 'L':
    		if ( strcmp(vCmdMsg[2], OSD_SET_LIGHT) == 0 )
    		{
    			g_pPlayerCtrl->setBrightness(atoi(vCmdMsg[3]));
    		}
    		break;
    	default:
    		break;
    	}
    }
}



int get_file_size(const char *path)  
{  
    unsigned long filesize = -1;  
    FILE *fp;  
    fp = fopen(path, "r");  
    if(fp == NULL)  
        return filesize;  
    fseek(fp, 0L, SEEK_END);  
    filesize = ftell(fp);  
    fclose(fp);  
    return filesize;  
}  

void CBCM_Ctrl::PlayNext(const char *FileName,
				  const char *SongName,
				  const char *PicPath,
				  BOOL bAuto)
{
	if ( !FileName || !SongName || !PicPath )
		return ;

#if WORKMODE == LOCAL_WORKMODE && (HISI_NET_APP == OFF)
	ConvertLink2Path(mPlayingFile, FileName, sizeof(mPlayingFile));
#elif WORKMODE == NET_WORKMODE
	SAFE_STRNCPY(mPlayingFile, FileName, sizeof(mPlayingFile));	// 保存当前播放路径
#else
	SAFE_STRNCPY(mPlayingFile, FileName, sizeof(mPlayingFile));	// 保存当前播放路径
#endif

	BOOL bSync = ( strstr(FileName, "KTV_Data/") != NULL ) ? FALSE : TRUE;

#if WORKMODE == LOCAL_WORKMODE && (HISI_NET_APP == OFF)
	if ( !IsFileExist(mPlayingFile) )
	{
		DbgOutput(DBG_INFO, "the file:%s not exist.\n", mPlayingFile);
    if(gPlayerInfo->GetSelectedSongCount() != 0)
  		BCMP_SendNextRequest();
		return ;
	}
  DbgOutput(DBG_ERR, "the file:%s is exist.\n", mPlayingFile);
	//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_STREAM, mPlayingFile, rc);
	//g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, bSync, bSync, mbOpenMultiChannelFlag);
	int ret = g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, bSync, bSync, mbOpenMultiChannelFlag);
#else
	int ret = g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, bSync, bSync, mbOpenMultiChannelFlag);
	//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_NET, mPlayingFile, rc);
	//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_STREAM, mPlayingFile, rc);
#endif
	if ( ret < 0 )
	{
		BCMP_SendNextRequest();
    int filesize = get_file_size(mPlayingFile);
    
		DbgOutput(DBG_ERR, "=====End to WSD_AL_PLAYER_Play, filesize :%d\n", filesize);
    
    if(filesize < 2048)  //播放失败小与2M的歌曲文件自动删除
    {
      char RemoveFile[64];
      sprintf(RemoveFile, "rm -rf %s", mPlayingFile);
      ret = system(RemoveFile);

    }
	}

	miPlayStatus = PLAYSTATUS_PLAY;
#if NRE_SETVOL_METHOD
	if ( !mbOpenMultiChannelFlag )
	{
		SelectAudio(miCurUITrack);
		SetVolume(miVol);
	}
#endif

	if ( !bAuto )
	{
		ShowCtrlImg(CMD_PAUSE, FALSE);		// 如果取消暂停图标
		//SwicthScrollMsg(SongName, PicPath);
	}

	UpdateWartermark();
	// 更新跑马灯
	UpdateMainScrollInfo();

	if ( IsTimerSet(TimeHandle_SRTFile) )
	{
		DelTimer(TimeHandle_SRTFile);
		char buf[8] = {0};
		ShowSubtitle(buf);//主要是清除之前的效果
		if ( g_pPlayerCtrl )
			g_pPlayerCtrl->DeInitSubtitle();
	}
}

void CBCM_Ctrl::Pause()
{
	g_PLYCtrl.Pause(PLAYER_MAIN);
	miPlayStatus = PLAYSTATUS_PAUSE;
	ShowCtrlImg(CMD_PAUSE, TRUE);
}

void CBCM_Ctrl::Resume()
{
	g_PLYCtrl.Resume(PLAYER_MAIN);
	miPlayStatus = PLAYSTATUS_PLAY;
	ShowCtrlImg(CMD_RESUME, TRUE);
}

void CBCM_Ctrl::Replay()
{
	ShowCtrlImg(CMD_REPLAY, TRUE);
	//RECT rc = {0, 0, 0, 0};
#if WORKMODE == LOCAL_WORKMODE  && (HISI_NET_APP == OFF)
	if ( !IsFileExist(mPlayingFile) )
	{
		DbgOutput(DBG_ERR, "the file:%s not exist.\n", mPlayingFile);
		return ;
	}
  DbgOutput(DBG_ERR, "the file:%s is exist.\n", mPlayingFile);
	//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_STREAM, mPlayingFile, rc);
	//g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, !mbOpenMultiChannelFlag, !mbOpenMultiChannelFlag, mbOpenMultiChannelFlag);
	g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, !mbOpenMultiChannelFlag, !mbOpenMultiChannelFlag, mbOpenMultiChannelFlag);
	//	if ( mbOpenMultiChannelFlag )
//	{
//		g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, FALSE, FALSE, TRUE);
//	}
//	else
//	{
//		g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, TRUE, TRUE, FALSE);
//	}
#else
	g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, mPlayingFile, !mbOpenMultiChannelFlag, !mbOpenMultiChannelFlag, mbOpenMultiChannelFlag);
	//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_NET, mPlayingFile, rc);
	//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_STREAM, mPlayingFile, rc);
#endif

#if NRE_SETVOL_METHOD
	SelectAudio(miCurUITrack);
	//SetVolume(miVol);
#endif

	miPlayStatus = PLAYSTATUS_PLAY;
}

void CBCM_Ctrl::PlayPreview(const char *FileName, RECT rc)
{
	if ( FileName )
	{
#if WORKMODE == LOCAL_WORKMODE  && (HISI_NET_APP == OFF)
		ConvertLink2Path(mPreviewFile, FileName, sizeof(mPreviewFile));
#else
		SAFE_STRNCPY(mPreviewFile, FileName, sizeof(mPlayingFile));	// 保存当前播放路径
#endif
		//g_PLYCtrl.StopBG();
		D_RECT rect;
		rect.Left 	= rc.left;
		rect.Top 	= rc.top;
		rect.Right	= rc.right;
		rect.Bottom	= rc.bottom;
#if WORKMODE == LOCAL_WORKMODE  && (HISI_NET_APP == OFF)
		if ( !IsFileExist(mPreviewFile) )
		{
			DbgOutput(DBG_ERR, "the file:%s not exist.\n", mPreviewFile);
			return ;
		}
		//g_PLYCtrl.Play(PLAYER_PIP, PLYMODE_STREAM, mPreviewFile, rc);
		//WSD_AL_PLAYER_ShowPlayWindow(PLAYER_PIP, 0);
		g_pPlayerCtrl->Preview_Play(PLYMODE_STREAM, mPreviewFile, rect);
#else
		g_pPlayerCtrl->Preview_Play(PLYMODE_NET, mPreviewFile, rect);
		//g_PLYCtrl.Play(PLAYER_PIP, PLYMODE_NET, mPreviewFile, rc);
		//g_PLYCtrl.Play(PLAYER_PIP, PLYMODE_STREAM, mPreviewFile, rc);
#endif
	}
}

void CBCM_Ctrl::RePreview()
{
	//g_PLYCtrl.StopBG();
	D_RECT rect;
	rect.Left 	= mRcPreview.left;
	rect.Top 	= mRcPreview.top;
	rect.Right	= mRcPreview.right;
	rect.Bottom	= mRcPreview.bottom;
#if WORKMODE == LOCAL_WORKMODE  && (HISI_NET_APP == OFF)
	if ( !IsFileExist(mPreviewFile) )
	{
		DbgOutput(DBG_ERR, "the file:%s not exist.\n", mPreviewFile);
		return ;
	}
	//g_PLYCtrl.Play(PLAYER_PIP, PLYMODE_STREAM, mPreviewFile, mRcPreview);

	g_pPlayerCtrl->Preview_Play(PLYMODE_STREAM, mPreviewFile, rect);
#else
	g_pPlayerCtrl->Preview_Play(PLYMODE_NET, mPreviewFile, rect);
	//g_PLYCtrl.Play(PLAYER_PIP, PLYMODE_NET, mPreviewFile, mRcPreview);
	//g_PLYCtrl.Play(PLAYER_PIP, PLYMODE_STREAM, mPreviewFile, mRcPreview);
#endif
}

void CBCM_Ctrl::PlaySituation(const char *FileName)
{
	if ( FileName )
	{
		//RECT rc = {0, 0, 0, 0};
		if ( !IsFileExist(FileName) )
		{
			DbgOutput(DBG_ERR, "the file:%s not exist.\n", FileName);
			return ;
		}
		//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_STREAM, FileName, rc);
		g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, FileName, TRUE, TRUE, FALSE);
		miPlayStatus = PLAYSTATUS_PLAY;
		ShowCtrlImg(CMD_PAUSE, FALSE);
#if NRE_SETVOL_METHOD
		SelectAudio(miCurUITrack);
		SetVolume(miVol);
#endif
	}
}

void CBCM_Ctrl::PlayCheer(int idx)		// 播放气氛
{
	if ( mbOpenMultiChannelFlag )
		return ;
	if ( strcmp(APP_VERB, "sd500-jundui") != 0 )
	{
		if ( idx == 1 )
			g_PLYCtrl.PlayPCM((char *)mImgInfo.mAmbienceInfo.mCheerPath.GetString());
		else if ( idx == 0 )
			g_PLYCtrl.PlayPCM((char *)mImgInfo.mAmbienceInfo.mHootPath.GetString());
	}
	else
	{
		char wavPath[128] = {0};
		snprintf(wavPath, sizeof(wavPath), "KTV_Data/osdwav/%d.wav", idx);
		g_PLYCtrl.PlayPCM(wavPath);
	}
}

void CBCM_Ctrl::PlayUSBMovie(char *FileName)
{
	char cUSBMovie[256] = {0};
	if ( FileName )
	{
#if WORKMODE == LOCAL_WORKMODE
		ConvertLink2Path(cUSBMovie, FileName, sizeof(cUSBMovie));
#elif WORKMODE == NET_WORKMODE
		SAFE_STRNCPY(cUSBMovie, FileName, sizeof(cUSBMovie));
#endif
		if ( !IsFileExist(cUSBMovie) )
		{
			DbgOutput(DBG_ERR, "the file:%s not exist.\n", cUSBMovie);
			return ;
		}
		//g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_STREAM, FileName, rc);
		g_pPlayerCtrl->Player_Play(PLYMODE_FILE, cUSBMovie, !mbOpenMultiChannelFlag, !mbOpenMultiChannelFlag, mbOpenMultiChannelFlag);
//		if ( mbOpenMultiChannelFlag )
//		{
//			g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, FileName, FALSE, FALSE, TRUE);
//		}
//		else
//		{
//			g_pPlayerCtrl->Player_Play(PLYMODE_STREAM, FileName, TRUE, TRUE, FALSE);
//		}
		miPlayStatus = PLAYSTATUS_PLAY;
		ShowCtrlImg(CMD_PAUSE, FALSE);
#if NRE_SETVOL_METHOD
		if ( !mbOpenMultiChannelFlag )
		{
			//非透传模式才设置声道
			SelectAudio(miCurUITrack);
			SetVolume(miVol);
		}

#endif

#if USE_SUBTITLE
		char *p = strrchr(cUSBMovie, '.');
		if ( p != NULL )
		{
			*p = '\0';
			strcat(p, ".srt");

			//printf ("+++++++++++++++the file:%s++++++++++++++\n", FileName);
			if ( IsFileExist(cUSBMovie) )
			{
				if ( g_pPlayerCtrl )
					g_pPlayerCtrl->DeInitSubtitle();
				g_PLYCtrl.SetSubtitleParse(cUSBMovie);
				if ( !IsTimerSet(TimeHandle_SRTFile) )
					AddTimer(TimeHandle_SRTFile, 500);
			}
		}
#endif
	}
}

void CBCM_Ctrl::PlayNetMovie(const char *Url)
{
	if ( !Url )
		return ;

	//RECT rc = {0, 0, 0, 0};
	//theBaseApp->SuspendInput();
	//int ret = g_PLYCtrl.Play(PLAYER_MAIN, PLYMODE_NET, Url, rc);
	printf("%s: (%d) Eva PLYMODE_NET! \n", __PRETTY_FUNCTION__, __LINE__);
	int ret = g_pPlayerCtrl->Player_Play(PLYMODE_NET, Url, FALSE, TRUE, FALSE);
#if NRE_SETVOL_METHOD
	//SelectAudio(miCurUITrack);
	SetVolume(miVol);
#endif
	//theBaseApp->ResumeInput();
	miPlayStatus = PLAYSTATUS_PLAY;
	ShowCtrlImg(CMD_PAUSE, FALSE);
	miInNetMode = TRUE;
	g_OSDCtrl.ShowBitmapFile(mImgInfo.mMovie.mPath.GetString(), mImgInfo.mMovie.mRc);
	if ( IsTimerSet(TimeHandle_SRTFile) )
	{
		DelTimer(TimeHandle_SRTFile);
		char buf[8] = {0};
		ShowSubtitle(buf);//主要是清除之前的效果
		if ( g_pPlayerCtrl )
			g_pPlayerCtrl->DeInitSubtitle();
	}

	if ( ret != 0 )
	{
		// 播放出错了或者无法播放
		if ( theBaseApp )
			theBaseApp->BroadcastMsg(VIDEO_SHOWPIC_MSG, 1, 0);

		if ( miInNetMode == TRUE )
		{
			miInNetMode = FALSE;
			g_OSDCtrl.EraseImg(mImgInfo.mMovie.mRc);
			UpdateWartermark();
		}

	}
	else
	{
		g_OSDCtrl.EraseImg(mImgInfo.mMainScrollText.mRc);
		g_OSDCtrl.StopMainScrollText();
	}
}

void CBCM_Ctrl::SetVolume(int iVol)
{
	miVol = iVol;
	if ( miVol < 0 )
		miVol = 0;
	int iBCM_Vol = iVol;
	if ( iVol < 5 )				// 0-5  --> 0-230
		iBCM_Vol = iVol * 46;
	else if ( iVol < 20 )		// 5-20 --> 230-320
		iBCM_Vol = 230 + (iVol -5) * 6;
//	else if ( iVol < 40 )		// 20-40 --> 290-340
//		iBCM_Vol = 290 + (iVol - 20) * 2.5;
	else if ( iVol <= 100 )		// 20-100 --> 320-400
		iBCM_Vol = 320 + iVol - 20;
#if defined(__arm__)
	g_PLYCtrl.SetVolume(iVol);
#elif defined(__mips__)
	g_PLYCtrl.SetVolume(iBCM_Vol);
#endif
}

void CBCM_Ctrl::SelectAudio(int iAudio)
{
	if ( mbOpenMultiChannelFlag )
		return ;

	int value;
	g_PLYCtrl.GetAllAudioChannel(value);
	printf("TrackCount:%d , UI Track:%d, Song Track:%d\n",value, iAudio, miCurSongTrack);
	if (value > 1)
	{
		if( iAudio == 1 )	// 需要伴唱
		{
			mAudioCompareOSD.PlayMainChannel(FALSE);
			g_PLYCtrl.SetAudioChannel((miCurSongTrack+1)%2);
		}
		else
		{
			mAudioCompareOSD.PlayMainChannel(TRUE);
			g_PLYCtrl.SetAudioChannel(miCurSongTrack);
		}
	}
	else
	{
		int mAudio_Track;
		if ( iAudio == 1 )
		{
#if !defined(__mips__)
			mAudio_Track = (miCurSongTrack==1) ? HS_AL_AUDIO_TRACK_RIGHT_E : HS_AL_AUDIO_TRACK_LEFT_E;
#else
			mAudio_Track = (miCurSongTrack==1) ? WSD_AL_AUDIO_TRACK_RIGHT_E : WSD_AL_AUDIO_TRACK_LEFT_E;
#endif
			g_PLYCtrl.SetAudioTrack(mAudio_Track);
		}
		else
		{
#if !defined(__mips__)
			mAudio_Track = (miCurSongTrack==1) ? HS_AL_AUDIO_TRACK_LEFT_E : HS_AL_AUDIO_TRACK_RIGHT_E;
#else
			mAudio_Track = (miCurSongTrack==1) ? WSD_AL_AUDIO_TRACK_LEFT_E : WSD_AL_AUDIO_TRACK_RIGHT_E;
#endif
			g_PLYCtrl.SetAudioTrack(mAudio_Track);
		}
	}
}

void CBCM_Ctrl::StartAudioCompareInit() 
{
  mAudioCompareOSD.AudioCompareInit();
}

void CBCM_Ctrl::StartBCM()  // 开机视频播放完成，启动
{
#if !defined(__mips__)
	theE3DEngine->setTransparentRect(false);
	gMainCtrlPage->mSharingIndexWnd.StartCheckLogin();
#endif
	g_PLYCtrl.StopPhantom();
	g_PLYCtrl.Stop(PLAYER_MAIN);
	//g_PLYCtrl.RePlayBkgroundView();

#if WORKMODE == LOCAL_WORKMODE
	if ( theBaseApp )
		theBaseApp->ResumeInput();
#else
	if ( theBaseApp && gbIsOpenRoom )
	{
		theBaseApp->ResumeInput();
	}
#endif

	if ( gPlayerInfo )
		gPlayerInfo->CloudFailedDownNext();

	g_OSDCtrl.ShowBitmapFile(mImgInfo.mLogo.mPath.GetString(), mImgInfo.mLogo.mRc);
	UpdateMainScrollInfo();

// 增加显示 二维码功能
	const char *pQRCode = "/tmp/QRCodeWng.png";
	if ( IsFileExist(pQRCode) )
	{
		g_OSDCtrl.EraseImg(mImgInfo.mBarCodeRect);
		g_OSDCtrl.ShowPicFile(pQRCode, mImgInfo.mBarCodeRect);
	}
//

	AddTimer(TimeHandle_SaveSong, 300*1000);
}

void CBCM_Ctrl::StartPlaySong() // 解码完成，开始播放
{
#if !NRE_SETVOL_METHOD
	SelectAudio(miCurUITrack);
	SetVolume(miVol);
#endif

	if ( theBaseApp )
		theBaseApp->BroadcastMsg(VIDEO_SHOWPIC_MSG, 0, 0);

	if ( miInNetMode == TRUE )
	{
		miInNetMode = FALSE;
		g_OSDCtrl.EraseImg(mImgInfo.mMovie.mRc);
		UpdateWartermark();
	}
}

void CBCM_Ctrl::ShowVolume(BOOL bShow)
{
	if ( bShow )
	{
		if ( miVol > 0 )
		{
			if ( miPlayStatus != PLAYSTATUS_PAUSE )
				ShowCtrlImg(CMD_MUTE, FALSE);		//取消静音图标
			RECT rc;
			int pos = (miVol / 100.00) * RECTWIDTH(mImgInfo.mVolumeInfo.mScrollRect);
			rc.left = mImgInfo.mVolumeInfo.mBGRect.left + mImgInfo.mVolumeInfo.mScrollRect.left + pos;
			rc.top  = mImgInfo.mVolumeInfo.mBGRect.top  + mImgInfo.mVolumeInfo.mScrollRect.top;
			rc.right = rc.left;
			rc.bottom = rc.top;
			g_OSD_Lock.Lock();
			//g_OSDCtrl.EraseImg(mImgInfo.mVolumeInfo.mBGRect);
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mVolumeInfo.mBGPath.GetString(), mImgInfo.mVolumeInfo.mBGRect);
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mVolumeInfo.mThumbPath.GetString(), rc);
			g_OSD_Lock.Unlock();

			AddTimer(Timehandle_Volume, DELAYNUM);
		}
		else
		{
			g_OSDCtrl.EraseImg(mImgInfo.mVolumeInfo.mBGRect);
			ShowCtrlImg(CMD_MUTE, TRUE);
		}
	}
	else
	{
		g_OSDCtrl.EraseImg(mImgInfo.mVolumeInfo.mBGRect);
	}
}

void CBCM_Ctrl::ShowAmbience(BOOL bShow)
{
	if ( bShow )
	{
		unsigned char bitCountPerPix;
		Font_Bitmap_Data BmpData;
		BmpData.text_alpha = NULL;

		BmpData.text_alpha = mFont.BMP_GetBmpData(&bitCountPerPix, &BmpData.iWidth, &BmpData.iHeight, mImgInfo.mAmbienceInfo.mPath.GetString());
		if ( BmpData.text_alpha )
		{
			g_OSDCtrl.Show32BitmapImgData(&BmpData, mImgInfo.mAmbienceInfo.mRcet);
			g_OSDCtrl.FreeBitmapData(&BmpData);
			AddTimer(Timehandle_Ambience, DELAYNUM);
		}
	}
	else
	{
		g_OSDCtrl.EraseImg(mImgInfo.mAmbienceInfo.mRcet);
	}
}

void CBCM_Ctrl::ShowCtrlImg(int iCmdType, BOOL bShow)
{
	RECT rc;
	memcpy(&rc, &mImgInfo.mCtrlImgInfo.mRc, sizeof(RECT));
	if ( bShow )
	{
		switch( iCmdType )
		{
		case CMD_PAUSE:
			DelTimer(TimeHandle_CtrlImg);
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mPausePath.GetString(), rc);
			break;

		case CMD_RESUME:
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mResumePath.GetString(), rc);
			AddTimer(TimeHandle_CtrlImg, DELAYNUM);
			break;

		case CMD_REPLAY:
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mReplayPath.GetString(), rc);
			AddTimer(TimeHandle_CtrlImg, DELAYNUM);
			break;

		case CMD_MUTE:
			if ( miPlayStatus != PLAYSTATUS_PAUSE )
			{
				DelTimer(TimeHandle_CtrlImg);
				g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mMutePath.GetString(), rc);
			}
			break;

		case CMD_ACCOMPANY:
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mAccompanyPath.GetString(), rc);
			AddTimer(TimeHandle_CtrlImg, DELAYNUM);
			break;

		case CMD_ORIGINAL:
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mOriginalPath.GetString(), rc);
			AddTimer(TimeHandle_CtrlImg, DELAYNUM);
			break;

		default:
			break;
		}
	}
	else
	{
		DelTimer(TimeHandle_CtrlImg);
		g_OSD_Lock.Lock();
		g_OSDCtrl.EraseImg(rc);
		if ( miPlayStatus == PLAYSTATUS_PAUSE )
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mPausePath.GetString(), rc);
		else if ( miVol == 0 )
			g_OSDCtrl.ShowBitmapFile(mImgInfo.mCtrlImgInfo.mMutePath.GetString(), rc);
		g_OSD_Lock.Unlock();
	}
}

void CBCM_Ctrl::ShowGuide(BOOL bShow)
{
	if ( bShow )
		g_OSDCtrl.ShowBitmapFile(mImgInfo.mGuide.mPath.GetString(), mImgInfo.mGuide.mRc);
	else
		g_OSDCtrl.EraseImg(mImgInfo.mGuide.mRc);
}

void CBCM_Ctrl::ShowSongmark(int iScore)
{

}

void CBCM_Ctrl::ShowRecord(BOOL bShow)
{
	if ( bShow )
		g_OSDCtrl.ShowBitmapFile(mImgInfo.mRecord.mPath.GetString(), mImgInfo.mRecord.mRc);
	else
		g_OSDCtrl.EraseImg(mImgInfo.mRecord.mRc);
}

void CBCM_Ctrl::ChangeBGVideo(int iNum)
{
	//g_PLYCtrl.StopBG();
	miCurBGVideoIdx = iNum;
	int idx = BGVideoType_Home;
	switch ( miCurBGVideoIdx )
	{
	case 0:
#if APP_ID == QIKUN500
			//Main BG Video
		if ( strcmp(APP_VERB, "dml5500") != 0 &&
			 strcmp(APP_VERB, "zhizun51500") != 0 )
			idx = BGVideoType_Home;
		else
		{
			char tmpBgVideoPath[128];
			int iSkinsID = gs_SystemSetting.miSkinsId % 10;
			snprintf(tmpBgVideoPath, sizeof(tmpBgVideoPath), "KTV_Data/ChangSkins/%d.mpg", iSkinsID);
			g_PLYCtrl.PlayBG(tmpBgVideoPath);
			return ;
		}
#elif APP_ID == QIKUN510
		{
			char tmpBgVideoPath[128];
			int iSkinsID = gs_SystemSetting.miSkinsId % 10;
			snprintf(tmpBgVideoPath, sizeof(tmpBgVideoPath), "KTV_Data/ChangSkins/%d.mpg", iSkinsID);
			g_PLYCtrl.PlayBG(tmpBgVideoPath);
			return ;
		}
#else
		//Main
		idx = BGVideoType_Home;
#endif
		break ;
	case 1:
		//singer
		idx = BGVideoType_Singer;
		break;
	case 2:
		//SongType
		idx = BGVideoType_Song;
		break;
	case 3:
		//Rank
		idx = BGVideoType_Rank;
		break;
	case 4:
		//Movie
		idx = BGVideoType_Movie;
		break;
	case 5:
		//Language:
		idx = BGVideoType_Language;
		break;
	case 6:
		//Pinyin:
		idx = BGVideoType_Pinyin;
		break;
	case 7:
		//NewSong
		idx = BGVideoType_NewSong;
		break;
	case 8:
		//KGO
		idx = BGVideoType_KGO;
		break;
	default:
		break;
	}
	g_PLYCtrl.PlayBG((char *)mImgInfo.mBGVideoPath[idx].GetString());
}

void CBCM_Ctrl::WrapMsg(const wchar_t *Msg, wchar_t *pOut)		// 对Msg进行裁减,最后5个汉字
{
	unsigned int iMaxLen = 5;
	if ( pOut && Msg )
	{
		if ( wcslen(Msg) > iMaxLen )
		{
			wcsncat(pOut, Msg, iMaxLen);
			wcscat(pOut, L"...");
		}
		else
			wcscat(pOut, Msg);
	}
}

void CBCM_Ctrl::SwicthScrollMsg(const char *SongName, const char *PicPath)
{
	return ;
	if ( !SongName || !PicPath )
		return ;

	Font_Bitmap_Data sPlaying_Font;
	Font_Bitmap_Data sNext_Font;
	sPlaying_Font.text_alpha = NULL;
	sNext_Font.text_alpha = NULL;

	RGBQUAD sFontColor		= {0xFF, 0xFF, 0xFF};
	RGBQUAD sPlayingBGColor = {0x01, 0x78, 0xff};
	RGBQUAD sNextBGColro 	= {0xff, 0x75, 0x00};

	wchar_t Text[128];
	wmemset(Text, L'\0', 128);
	wcscat(Text, L"在播:");

	CBaseStringW tmp;
	tmp.Set(SongName);

	WrapMsg(tmp.GetString(), Text);
	mFont.SetFontSize(45);
	if ( !GenTextBitmapData(&mFont, Text, sFontColor, sPlayingBGColor, &sPlaying_Font) )
	{
		DbgOutput(DBG_ERR, "GenTextBitmapData:%ls failed.\n", Text);
		return ;
	}

	wcscpy(Text, L"下一首:");
	if ( gPlayerInfo )
	{
		SONGINFO *pSongInfo = gPlayerInfo->GetSelectedSongInfoByIndex(0);
		if ( pSongInfo )
		{
			tmp.Set(pSongInfo->cName);
			WrapMsg(tmp.GetString(), Text);
		}
	}
	if ( !GenTextBitmapData(&mFont, Text, sFontColor, sNextBGColro, &sNext_Font) )
	{
		DbgOutput(DBG_ERR, "GenTextBitmapData:%ls failed.\n", Text);
		if ( sPlaying_Font.text_alpha )
			free(sPlaying_Font.text_alpha);
		return ;
	}

	int StartX = mImgInfo.mSwitchInfo.mScrollRect.left + mImgInfo.mSwitchInfo.mScrollRect.right;
	int EndX   = mImgInfo.mSwitchInfo.mScrollRect.left;
	int Y	   = mImgInfo.mSwitchInfo.mScrollRect.top;
	int h	   = mImgInfo.mSwitchInfo.mScrollRect.bottom - mImgInfo.mSwitchInfo.mScrollRect.top;
	int speed  = 50;

	if ( !sPlaying_Font.text_alpha || !sNext_Font.text_alpha )
	{
		if ( sPlaying_Font.text_alpha ) free(sPlaying_Font.text_alpha);
		if ( sNext_Font.text_alpha )	free(sNext_Font.text_alpha);
		return ;
	}

	for ( int i = StartX; i >= EndX; i -= speed )
	{
		g_OSD_Lock.Lock();
		RECT rc = {0,0,0,0};

		rc.left = i;
		rc.top = Y;
		g_OSDCtrl.ShowBitmapFile(mImgInfo.mSwitchInfo.mBGPath.GetString(), rc);

		if ( IsFileExist(PicPath) )
		{
			rc.left = i + mImgInfo.mSwitchInfo.mSongPicRect.left;
			rc.top = Y + mImgInfo.mSwitchInfo.mSongPicRect.top;
			g_OSDCtrl.ShowPicFile(PicPath, rc);
		}

		rc.left = i + mImgInfo.mSwitchInfo.mPlayingScrollRect.left;
		rc.top = Y + mImgInfo.mSwitchInfo.mPlayingScrollRect.top;
		g_OSDCtrl.Show24BitmapImgData(&sPlaying_Font, rc);

		rc.left = i + mImgInfo.mSwitchInfo.mNextScrollRect.left;
		rc.top = Y + mImgInfo.mSwitchInfo.mNextScrollRect.top;
		g_OSDCtrl.Show24BitmapImgData(&sNext_Font, rc);

		rc.left = i + RECTWIDTH(mImgInfo.mSwitchInfo.mBGRect);
		rc.top  = Y;
		rc.right = 1920;//rc.left + speed + 50;
		rc.bottom = rc.top + h;
		g_OSDCtrl.EraseImg(rc);
		g_OSD_Lock.Unlock();

		usleep(20000);
	}

	if ( sPlaying_Font.text_alpha )
		free(sPlaying_Font.text_alpha);

	if ( sNext_Font.text_alpha )
		free(sNext_Font.text_alpha);

	AddTimer(TimeHandle_SwitchScroll, DELAYNUM);
}

void CBCM_Ctrl::UpdateMainScrollInfo()
{
	CBaseStringW Text;
	GetNewMainScrollInfo(&Text);
	g_OSDCtrl.ShowScrollText(Text.GetString(), &mImgInfo.mMainScrollText);
}

void CBCM_Ctrl::UpdateSubScrollInfo(const wchar_t *Msg)
{
	if ( Msg )
	{
		//printf("======the greeting:%ls\n", Msg);
		g_OSDCtrl.ShowScrollText(Msg, &mImgInfo.mSubScrollText);
	}
}

void CBCM_Ctrl::UpdateWartermark()
{
	if ( IsInMonkeyMode() )  //Monkey状态下始终显示电视水印
	{
		mImgInfo.mWartermarkInfo.mbOpen = TRUE;
	}

	if ( mImgInfo.mWartermarkInfo.mbOpen )
	{
		Font_Bitmap_Data sPlaying_Font;
		sPlaying_Font.text_alpha = NULL;

		RECT rc;
		memcpy(&rc, &mImgInfo.mWartermarkInfo.mRc, sizeof(RECT));
		int dis = RECTHEIGHT(mImgInfo.mWartermarkInfo.mRc) / mImgInfo.mWartermarkInfo.miNum;
		int num = mImgInfo.mWartermarkInfo.miNum;

		if ( !gPlayerInfo )
			return ;

		wchar_t Text[128];
		wmemset(Text, L'\0', 128);
		wcscat(Text, L"在播:");

		SONGINFO *pSongInfo = gPlayerInfo->GetCurPlaySongInfo();
		if ( !pSongInfo && strlen(pSongInfo->cID) < 2 )
			return ;

		CBaseStringW tmp;
		tmp.Set(pSongInfo->cName);

		WrapMsg(tmp.GetString(), Text);
		mFont.SetFontSize(mImgInfo.mWartermarkInfo.miSize);
		if ( !GenTextBitmapData(&mFont, Text, mImgInfo.mWartermarkInfo.msColor,
								mImgInfo.mWartermarkInfo.msBgColor, &sPlaying_Font) )
		{
			DbgOutput(DBG_ERR, "GenTextBitmapData:%ls failed.\n", Text);
			return ;
		}
		g_OSDCtrl.EraseImg(rc);
		g_OSDCtrl.Show24BitmapImgData(&sPlaying_Font, rc);

		num--;
		g_OSDCtrl.FreeBitmapData(&sPlaying_Font);

		for ( int i = 0; i < num; ++i )
		{
			pSongInfo = gPlayerInfo->GetSelectedSongInfoByIndex(i);
			if ( pSongInfo && strlen(pSongInfo->cID) > 2 )
			{
				wmemset(Text, L'\0', 128);
				swprintf(Text, 60, L"%d.", i+1);
				tmp.Set(pSongInfo->cName);
				WrapMsg(tmp.GetString(), Text);
				if ( !GenTextBitmapData(&mFont, Text, mImgInfo.mWartermarkInfo.msColor,
										mImgInfo.mWartermarkInfo.msBgColor, &sPlaying_Font) )
				{
					DbgOutput(DBG_ERR, "GenTextBitmapData:%ls failed.\n", Text);
					break; ;
				}
				rc.top = mImgInfo.mWartermarkInfo.mRc.top + (i+1) * dis;
				g_OSDCtrl.Show24BitmapImgData(&sPlaying_Font, rc);
				g_OSDCtrl.FreeBitmapData(&sPlaying_Font);
			}
		}
		g_OSDCtrl.FreeBitmapData(&sPlaying_Font);
	}
}

void CBCM_Ctrl::GetNewMainScrollInfo(CBaseStringW *pOut)
{
	if ( !pOut )
		return ;

	if ( gPlayerInfo == NULL )
	{
		DbgOutput(DBG_WARN, "#Warning: the gPlayerInfo is not init.\n");
		return ;
	}
	char strTemp[128] = {0};
	if ( theBaseApp && theBaseApp->GetLanguageID() == LanguageID_English )
	{
		strcpy(strTemp, ".Playing:");
	}
	else
	{
		strcpy(strTemp, ".正播:");
	}
	SONGINFO *pSongInfo = NULL;

	pSongInfo = gPlayerInfo->GetCurPlaySongInfo();
	if( (pSongInfo != NULL) && strlen(pSongInfo->cID) >0 )
	{
		if( strlen(pSongInfo->cName) >= sizeof(pSongInfo->cName) )
			pSongInfo->cName[sizeof(pSongInfo->cName)-1] = '\0';
		strcat(strTemp, pSongInfo->cName);

		pSongInfo = gPlayerInfo->GetSelectedSongInfoByIndex(0);
		if(pSongInfo != NULL)
		{
			if( strlen(pSongInfo->cName) >= sizeof(pSongInfo->cName) )
				pSongInfo->cName[sizeof(pSongInfo->cName)-1] = '\0';
			if ( theBaseApp && theBaseApp->GetLanguageID() == LanguageID_English )
			{
				strcat(strTemp, ".Next:");
			}
			else
			{
				strcat(strTemp,".下一首:");
			}
			strcat(strTemp, pSongInfo->cName);
		}
	}
#if UNUSE_SCROLL
	pOut->Format(" ");
#else
	pOut->Format("%s%s", gLogoText.GetString(), strTemp);
#endif
}

void CBCM_Ctrl::ShowSubtitle(const char *pcSubtitle)
{
	static RECT rcShow = {0, 550, 0, 0};

	if ( !pcSubtitle )
		return ;

	if ( strlen(pcSubtitle) == 0 && (rcShow.right-rcShow.left) > 0 )
	{
		g_OSDCtrl.EraseImg(rcShow);
		rcShow.right = rcShow.left;
		return ;
	}

	CBaseStringW tmp;
	tmp.Set(pcSubtitle);
	if ( tmp.GetString() == NULL )
		return ;

	Font_Bitmap_Data sSubtitle_Font;
	sSubtitle_Font.text_alpha 	= NULL;
	RGBQUAD sFontColor			= {0xFF, 0xFF, 0xFF};
	RGBQUAD sBGColor 			= {0x00, 0x00, 0x00};

	mFont.SetFontSize(45);
	if ( !GenTextBitmapData(&mFont, tmp.GetString(), sFontColor, sBGColor, &sSubtitle_Font) )
	{
		DbgOutput(DBG_ERR, "GenTextBitmapData:%ls failed.\n", tmp.GetString());
		return ;
	}

	g_OSD_Lock.Lock();
	rcShow.left = 0;
	rcShow.right = 1920;
	g_OSDCtrl.EraseImg(rcShow);
	int xOffset 	= (1920 - sSubtitle_Font.iWidth) / 2;
	rcShow.left		= (xOffset < 0) ? 0 : xOffset;
	rcShow.right	= sSubtitle_Font.iWidth + rcShow.left;
	rcShow.bottom	= sSubtitle_Font.iHeight + rcShow.top;
	g_OSDCtrl.Show24BitmapImgData(&sSubtitle_Font, rcShow);
	g_OSD_Lock.Unlock();

	if ( sSubtitle_Font.text_alpha )
		free(sSubtitle_Font.text_alpha);
}

void CBCM_Ctrl::SetMainScrollInfo(CBaseStringA &Text, UINT32 color)
{
	if ( Text.GetLength() > 0)
	{
		gLogoText.Set(Text.GetString());
		memcpy(&mImgInfo.mMainScrollText.msColor, &color, sizeof(mImgInfo.mMainScrollText.msColor));
	}
}

void CBCM_Ctrl::AudioCompareStop()
{
	mAudioCompareOSD.StopAudioCompare();
}
void CBCM_Ctrl::AudioComparePause(BOOL bPause)
{
	mAudioCompareOSD.PauseAudioCompare(bPause);
}
void CBCM_Ctrl::AudioCompareStart()
{
	mAudioCompareOSD.StartAudioCompare();
}
void CBCM_Ctrl::AudioCompareCheck(const char* path ,const int track)
{
	mAudioCompareOSD.AudioCompareCheck(path,track);
}
void CBCM_Ctrl::AudioCompareReStart()
{
	mAudioCompareOSD.ReStartAudioCompare();
}


const char *  CBCM_Ctrl::GetMainScrollInfo()
{
	return gLogoText.GetString();
}

void OSD_ShowMtvWatermark()
{
	if ( g_pBCM_Ctrl )
		g_pBCM_Ctrl->UpdateWartermark();
}

void OSD_SetUpdatePlayInfoFlag(int iFlag)
{
	if ( g_pBCM_Ctrl && iFlag )
		g_pBCM_Ctrl->UpdateMainScrollInfo();
}

void OSD_ScrollGreetingMsg(const wchar_t *GreetingMsg, int iTimer)
{
	if ( g_pBCM_Ctrl && GreetingMsg )
	{
		g_pBCM_Ctrl->UpdateSubScrollInfo(GreetingMsg);
	}
}

int GetDuration(int *filetime)
{
	int ret = 0;

	g_GetCurPositionLock.Lock();
	ret = g_PLYCtrl.GetDuration(filetime);
	g_GetCurPositionLock.Unlock();

	return ret;
}

int GetCurPosition(int *filetime)
{
	int ret = 0;

	g_GetCurPositionLock.Lock();
	ret = g_PLYCtrl.GetCurPosition(filetime);
	g_GetCurPositionLock.Unlock();

	return ret;
}

int GetPlayState()
{
	int ret = 0;

	g_GetCurPositionLock.Lock();
	ret = g_PLYCtrl.GetPlayState(PLAYER_MAIN);
	g_GetCurPositionLock.Unlock();

	return ret;
}
#endif
