#include "PlayerInfo.h"
#include "PlayerApi.h"
#include "UserMsgDefine.h"
#include "../GlobalUIClass.h"

#include "../SQL/SearchSQL.h"

#if defined(__mips__) || defined(__arm__)
#include "../OSDControl/BCM_Control.h"
#endif

#include "../DLNA/DlnaApi.h"

#include "E3DCommand.h"
#include "../Record/BaseRecord.h"

CPlayerInfo *gPlayerInfo=NULL;

static int giPubBroadCastIdx = -1;
static BOOL g_bIsPlayMoive = FALSE;
static BOOL g_bIsInPreview = FALSE;		// 是否在预览
static RECT g_rcPreview;
static BOOL g_bIsInPhantom = FALSE;		// 是否在预览
static RECT g_rcPhantom;

#define NETCONFIG_FILE                                  "./KTV_Data/netconfig/network"
void ReadIpInfo(char *dstbuf)
{
        if(NULL == dstbuf)
                return ;
        char cSystemFileName[256];
        char buffer[128] = {0};
        char cIp[32] = {0};

        strcpy(cSystemFileName,NETCONFIG_FILE);

        FILE *fp = fopen(cSystemFileName, "r") ;

        if (fp)
        {
        	if(fgets(buffer,128,fp) != NULL)
        	if(fgets(buffer,128,fp) != NULL)
        	{
        		sscanf(buffer,"IP=%s",cIp);
        	}
        	fclose(fp);
        	//strcat(dstbuf,":8080");
        	sprintf(dstbuf, "%s:8080", cIp);
        }
        else
        {
                DbgOutput(DBG_DEBUG, "open the file %s error.\n", cSystemFileName);
        }
}


int FindSongInfoByID(const void *pItem, const void *pData)
{
	if ( pItem == NULL || pData == NULL )
		return 0;

	SONGINFO *pSongInfo = (SONGINFO *)pItem ;
	char *cID = (char *)pData ;

	if ( strcmp(pSongInfo->cID, cID) == 0 )
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

//static char *FindSingerPicByName(const char *strName)
//{
//	char *temp = NULL;
//
//	if(NULL == strName)
//		return NULL;
//
//	int count = gAllSingerList.GetCount();
//	for(int i=0; i<count; i++)
//	{
//		SINGERINFO *pSinger = (SINGERINFO *)gAllSingerList.GetAt(i);
//		if(!pSinger)
//			continue;
//		if(strstr(strName, pSinger->cName) != NULL)
//		{
//			//temp = pSinger->cPicPath;
//			break;
//		}
//	}
//	return temp;
//}

int GetRandomNum()
{
	srand((unsigned)time(NULL));
	return rand()%5;
}

static void PlayBarNumForSongInfo(SONGINFO *pSong)
{
#if HISI_NET_APP
	if ( getFireStaus() )			// 火警状态中,不能切歌
		return ;
#endif
	if ( !pSong )
		return;
	// 需要播放台号
	char cAppFolder[MAX_PATH] = {0};
	GetProgramPath(cAppFolder, MAX_PATH);
	char cBarNumSongFile[MAX_PATH] = {0};
	int num = 0;

	if ( sscanf(pSong->cName, "(%d)", &num) == 1 )
	{
		sprintf(cBarNumSongFile, "%sKTV_Data/barnumsong/%02d.mpg", cAppFolder, num);
		if ( !IsFileExist(cBarNumSongFile) )
			sprintf(cBarNumSongFile, "%sKTV_Data/barnumsong/%02d.mpg", cAppFolder, 0);
	}
	else
		sprintf(cBarNumSongFile, "%sKTV_Data/barnumsong/%02d.mpg", cAppFolder, 0);
	if ( !IsFileExist(cBarNumSongFile) )
	{
		gPlayerInfo->PlayNext(FALSE);
		return;
	}
	char *singer = FastSearch_GetSingerByID(pSong->singerID) ;
	PlayerApi_Play(
		pSong->cID,
		cBarNumSongFile,
		cBarNumSongFile,
		100,
		1,
		1,
		0,
		pSong->cName,
		singer ? singer : "",
		""/*pSong->cPicPath*/);
	//	PlayerApi_PlayMovie(
//		cBarNumSongFile,
//		cBarNumSongFile,
//		100,
//		0,
//		0);
}

CPlayerInfo::CPlayerInfo()
{
	mVolume = gs_SystemSetting.miStartUp_vol;
	mOriginal = !(gs_SystemSetting.miStartUp_orignal);
	mPlayStatus = -1;
	memset(&mPlayingSong, 0, sizeof(SONGINFO));
	memset(&mPlayingPublicSong,0,sizeof(mPlayingPublicSong));

	//ReadIpInfo(mIp);
	memset(mIp, 0, sizeof(mIp));

	mSituationMode = FALSE;

	mSongMarkMode = FALSE;
	mPadChangeState = FALSE;

	FastSearch_LoadSelectedList(&mSelectedList);		//add by yq,2012-12-20 for load the selected song list

	char cLocalPath[256];
	char cAppVer[20];
	char cDbVer[10];
#if defined(__arm__)
	sprintf(cLocalPath, "/stb/config/media/songs/");
#else
	sprintf(cLocalPath,"/media/songs/");
#endif
	sprintf(cAppVer,"510");
	sprintf(cDbVer,"0");

	mCloudSongs.Init((char*)cLocalPath, (char*)cAppVer, (char*)cDbVer);

	mCurPlayBgVideoIdx = -1;
}

CPlayerInfo::~CPlayerInfo()
{
	gPlayerInfo = NULL;
	FastSearch_SaveSelectedList(&mSelectedList);		//add by yq,2012-12-20 for save the selected song list

	mSongInfoListLock.Lock();
	DelNormalList(&mSelectedList, SONGINFO);
	DelNormalList(&mSingedList, SONGINFO);
	mSongInfoListLock.Unlock();
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

char* CPlayerInfo::GetIp()
{
	ReadIpInfo(mIp);
	return mIp;
}

void CPlayerInfo::Play(
	SONGINFO *pSongInfo,
	BOOL bForceSwitch)
{
#if HISI_NET_APP
	if ( getFireStaus() )			// 火警状态中,不能切歌
		return ;
#endif
//	printf("the CPlayerInfo:%X\n", (unsigned int)this);
	if ( !pSongInfo )
		return ;

#if ( BAR_MODE == 1 )
	if ( (gs_SystemSetting.miBarOnOff == 1)
			&& pSongInfo->bChos )
	{
		PlayBarNumForSongInfo(pSongInfo);
		pSongInfo->bChos = false;
		return;
	}
#endif
	char *pSinger = FastSearch_GetSingerByID(pSongInfo->singerID);
	char strSingerPic[MAX_PATH] = {0};
	//char *pSingerPic = FindSingerPicByName(pSongInfo->cSinger);
	if( pSinger != NULL )
	{
		char strSingerPicShort[32] = {0};
		//int idx;
		//if ( sscanf(pSingerPic,"%d.jpg",&idx) == 1 )
		{
			sprintf(strSingerPicShort,"%s.jpg", pSinger);
			const char *cSingerPath = theBaseApp->GetSingerTextureFolder();
			CombinePathName(strSingerPic, cSingerPath, strSingerPicShort);
		}
	}

	// 对 歌星 歌曲中出现',',做处理，避免发送过去，解析出错
	char strName[50+1] = {0};
	//char strSinger[20+1] ={0};
	SAFE_STRNCPY(strName, pSongInfo->cName, sizeof(strName));
	//SAFE_STRNCPY(strSinger, pSongInfo->cSinger, sizeof(strSinger));
	char *p = NULL;
	while( (p = strchr(strName, ',')) != NULL )
		*p = '&';		// ','替换'&'
	//while( (p = strchr(strSinger, ',')) != NULL )
	//	*p = '&';		// ','替换'&'
	// end ‘，’处理

	mPlayStatus = 1;
	
	PlayerApi_Play(
		pSongInfo->cID,
		pSongInfo->cServerName,
		pSongInfo->cFilePath,
		mVolume,
		pSongInfo->wTrack,
		mOriginal,
		bForceSwitch ? 1 : 0,
		strName,
		pSinger ? pSinger : "",
		strSingerPic);

}

void CPlayerInfo::PlayNext(
	BOOL bForceSwitch)
{
	if(g_bIsPlayMoive)
		theBaseApp->BroadcastMsg(NOTETOCLOSEMOVIE_MSG, 0, 0);

	if ( !IsInMonkeyMode() )  //Monkey状态下进入情景一样切歌
	{
		if(mSituationMode)
			return ;  //忽略手动切歌操作
	}

	if ( strlen(mPlayingSong.cID) > 0 )
	{
		gMainCtrlPage->mSharingIndexWnd.DecreaseLeftCount(payType_count);
		CBaseRecord::Stop_Record(mPlayingSong.cName);
		g_pBCM_Ctrl->AudioCompareStop();
	}

	char strPrevSongID[8+1] ={0};

	SONGINFO *pSongInfo = (SONGINFO *)GetSelectedSongInfoByIndex(0);
	if (pSongInfo)
	{
#if ( BAR_MODE == 1 )
		if ( (gs_SystemSetting.miBarOnOff == 1)
				&& (pSongInfo->bChos) )
		{
			pSongInfo->bChos = false;
			PlayBarNumForSongInfo(pSongInfo);
	//		ReportSongStateChanged(strPrevSongID);
			return;
		}

		if ( strlen(mPlayingSong.cID) > 0)
		{
			//printf("==================add song to singed list:%s\n", mPlayingSong.cName);
			AddSongToSingedList(&mPlayingSong, 0);
			strcpy(strPrevSongID, mPlayingSong.cID);
		}

		memcpy(&mPlayingSong, pSongInfo, sizeof(SONGINFO));
		giPubBroadCastIdx = -1;
#endif
	}
	else
	{
		if ( strlen(mPlayingSong.cID) > 0)
		{
			//printf("==================add song to singed list:%s\n", mPlayingSong.cName);
			AddSongToSingedList(&mPlayingSong, 0);
			strcpy(strPrevSongID, mPlayingSong.cID);
		}
		memset(&mPlayingSong, 0, sizeof(SONGINFO));
		giPubBroadCastIdx++;
	}

	BOOL temp = mPadChangeState;
	if(!temp)
		mPadChangeState = TRUE;
	DelSongFromSelectedList(0);
	if(!temp)
		mPadChangeState = FALSE;

	ReportSongStateChanged(strPrevSongID);
//	ReportListStateChanged();

	//add by yq,2012-12-13
	if(gs_SystemSetting.miStartUp_stwichOrignal == 1)
	{
		mOriginal = 0;
	}
	else if(gs_SystemSetting.miStartUp_stwichOrignal == 0)
	{
		mOriginal = 1;
	}
	ReportOriginalUIStateChanged();
	//end add by yq,2012-12-13

	if(giPubBroadCastIdx >= 0)
	{
		if(giPubBroadCastIdx >= FastSearch_GetPubBroadCastSongCount())
		{
			giPubBroadCastIdx = 0;
		}
		pSongInfo = FastSearch_GetPubBroadCastSongByIdx(giPubBroadCastIdx);
		if(pSongInfo != NULL)
		{
			//memcpy(&mPlayingSong, pSongInfo, sizeof(SONGINFO));
			memcpy(&mPlayingPublicSong, pSongInfo, sizeof(SONGINFO));
			mPlayingPublicSong.bChos = false;
			Play(&mPlayingPublicSong, bForceSwitch);
			ReportUIStateChangedToPad((int)SYNCTYPE_PUBLICSONG);
		}else
		{
			memset(&mPlayingPublicSong,0,sizeof(mPlayingPublicSong));
		}
		
	}
	else
	{
		/*
		memset(&mPlayingPublicSong,0,sizeof(mPlayingPublicSong));
		/////mPlayingSong  have path and track 
		///start dump main track 
#ifdef  DUMP_AUDIO_PCM
		g_pBCM_Ctrl->AudioCompareCheck(mPlayingSong.cFilePath,mPlayingSong.wTrack);
#else
		g_pBCM_Ctrl->AudioCompareStart();
#endif
		Play(&mPlayingSong, bForceSwitch);

		if(!mPadChangeState)
			ReportUIStateChangedToPad((int)SYNCTYPE_SWITCH);

		CBaseRecord::Start_Record();
		*/
	}

	ReportListStateChanged();

	ReportSongStateChanged(mPlayingSong.cID);
	UpdateScrollMsg();
	mPlayStatus = 1;
	gMainCtrlPage->SetPalyState(FALSE);
}

void CPlayerInfo::Pause()
{
	PlayerApi_Pause();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_PAUSE);
	mPlayStatus = 0;

	CBaseRecord::Pause_Record();
	g_pBCM_Ctrl->AudioComparePause(0);
}

void CPlayerInfo::Resume()
{
	PlayerApi_Resume();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_RESUME);
	mPlayStatus = 1;

	CBaseRecord::Resume_Record();
	g_pBCM_Ctrl->AudioComparePause(1);
}

void CPlayerInfo::Replay()
{
	if(mSituationMode)
		return ;  //忽略手动切歌操作
	PlayerApi_Replay();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_REPLAY);
	mPlayStatus = 1;

	CBaseRecord::Restart_Record();
	g_pBCM_Ctrl->AudioCompareReStart();
}
void CPlayerInfo::SeekTo(int msec)
{
	PlayApi_SeekTo(msec);
}

void CPlayerInfo::SetOriginal(
	int nOriginal)
{
	mOriginal = nOriginal;

	PlayerApi_SetOriginal(0, nOriginal);
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_CHANNEL);
}

void CPlayerInfo::SetVolume(
	int nVolume)
{
	mVolume = nVolume;

	PlayerApi_SetVolume(nVolume);
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_VOLUME);
}
int CPlayerInfo::GetVolume()
{
	return mVolume;
}
int CPlayerInfo::GetPlayState()
{
	return mPlayStatus;
}

void CPlayerInfo::StartPhantom()
{
	PlayerApi_StartPhantom();
}

void CPlayerInfo::StopPhantom()
{
	PlayerApi_StopPhantom();
}

void CPlayerInfo::StartPreview(
	const char *cServerName,
	const char *cFileName,
	int x,
	int y,
	int w,
	int h)
{
	g_bIsInPreview = TRUE;
	g_rcPreview.left = x;
	g_rcPreview.top = y;
	g_rcPreview.right = x + w;
	g_rcPreview.bottom = y + h;
	theE3DEngine->setTransparentRect(true, x, y, w, h);
	PlayerApi_StartPreview(cServerName, cFileName, x, y, w, h);
}

void CPlayerInfo::PausePreview()
{
	PlayerApi_PausePreview();
}

void CPlayerInfo::StopPreview()
{
	g_bIsInPreview = FALSE;
	theE3DEngine->setTransparentRect(false);
	PlayerApi_StopPreview();
}

void CPlayerInfo::ResumePreview()
{
	PlayerApi_ResumePreview();
}

void CPlayerInfo::PlayMTV(
	int x,
	int y,
	int w,
	int h)
{
	if (strlen(mPlayingSong.cID) || strlen(mPlayingPublicSong.cID))
	{
		g_bIsInPhantom = TRUE;
		g_rcPhantom.left = x;
		g_rcPhantom.top = y;
		g_rcPhantom.right = x + w;
		g_rcPhantom.bottom = y + h;
		theE3DEngine->setTransparentRect(true, x, y, w, h);
		PlayerApi_PlayMTV(x, y, w, h);
	}
}

void CPlayerInfo::PlayMTVFullScreen(
	int bFullScreen)
{
	if (strlen(mPlayingSong.cID))
	{
		PlayerApi_PlayMTVFullScreen(bFullScreen);
	}
}

void CPlayerInfo::StopMTV()
{
	g_bIsInPhantom = FALSE;
	theE3DEngine->setTransparentRect(false);
	PlayerApi_StopMTV();
}

void CPlayerInfo::StartSituation(
	const char *cServerName,
	const char *cFileName)
{
#if HISI_NET_APP
	if ( getFireStaus() )			// 火警状态中,不能切歌
		return ;
#endif
	mSituationMode = TRUE;
	PlayerApi_Situation(
		cServerName,
		cFileName,
		mVolume,
		0,
		mOriginal);
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_SITUATION);
}

void CPlayerInfo::StopSituation()
{
	if(mSituationMode)
	{
		mSituationMode = FALSE;
		PlayNext(TRUE);
	}
}

void CPlayerInfo::Cheer(
	int nType)
{
	PlayerApi_Cheer(nType);
}

void CPlayerInfo::Ambience(int nFaceIndex)
{
	PlayerApi_Ambience(nFaceIndex);
}

void CPlayerInfo::Attunement(
	int nType)
{
	PlayerApi_Attunement(nType);
}

void CPlayerInfo::StartSongMark()
{
	mSongMarkMode = TRUE;
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_START_SCORE);
}

void CPlayerInfo::StopSongMark()
{
	mSongMarkMode = FALSE;
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_STOP_SCORE);
}

void CPlayerInfo::SongMark(
	int nType)
{
	PlayerApi_SongMark(nType);
}

int CPlayerInfo::GetSelectedSongCount()
{
	mSongInfoListLock.Lock();
	int nCount = mSelectedList.GetCount();
	mSongInfoListLock.Unlock();

	return nCount;
}

int CPlayerInfo::GetSingedSongCount()
{
	mSongInfoListLock.Lock();
	int nCount = mSingedList.GetCount();
	mSongInfoListLock.Unlock();

	return nCount;
}

SONGINFO *CPlayerInfo::GetCurPlaySongInfo()
{
	if(strlen(mPlayingSong.cID) > 0)
		return &mPlayingSong;
	else
		return &mPlayingPublicSong;
}

SONGINFO *CPlayerInfo::GetSelectedSongInfoByIndex(
	int nIndex)
{
	mSongInfoListLock.Lock();
	SONGINFO *pSongInfo = (SONGINFO *)mSelectedList.GetAt(nIndex);
	mSongInfoListLock.Unlock();

	return pSongInfo;
}

SONGINFO *CPlayerInfo::GetSingedSongInfoByIndex(
	int nIndex)
{
	mSongInfoListLock.Lock();
	SONGINFO *pSongInfo = (SONGINFO *)mSingedList.GetAt(nIndex);
	mSongInfoListLock.Unlock();

	return pSongInfo;
}

int CPlayerInfo::GetSelectedSongIndexByID(
	const char *cSongID)
{
	mSongInfoListLock.Lock();
	int nIndex = mSelectedList.FindFirst(cSongID, FindSongInfoByID);
	mSongInfoListLock.Unlock();

	return nIndex;
}

int CPlayerInfo::GetSingedSongIndexByID(
	const char *cSongID)
{
	mSongInfoListLock.Lock();
	int nIndex = mSingedList.FindFirst(cSongID, FindSongInfoByID);
	mSongInfoListLock.Unlock();

	return nIndex;
}

BOOL CPlayerInfo::IsSongInSelectedList(
	const char* cSongID)
{
	if (!cSongID)
	{
		return FALSE;
	}

	if (strcmp(cSongID, mPlayingSong.cID) == 0)
	{
		return TRUE;
	}

	mSongInfoListLock.Lock();
	int nIndex = mSelectedList.FindFirst(cSongID, FindSongInfoByID);
	mSongInfoListLock.Unlock();
	if (nIndex != -1)
	{
		// already exist
		return TRUE;
	}

	//return mCloudSongs.IsSongInCloudList(cSongID);		// 把 云加歌下载的歌曲也算上
	return FALSE;
}

BOOL CPlayerInfo::IsSongInSingedList(
	const char* cSongID)
{
	if (!cSongID)
	{
		return FALSE;
	}

	mSongInfoListLock.Lock();
	int nIndex = mSingedList.FindFirst(cSongID, FindSongInfoByID);
	mSongInfoListLock.Unlock();
	if (nIndex != -1)
	{
		// already exist
		return TRUE;
	}

	return FALSE;
}

BOOL CPlayerInfo::AddSongToSelectedList(
	const SONGINFO *pSongInfo,
	int nIndex,
	BOOL bForceAdd)
{
	if ( !pSongInfo )
		return FALSE;

#if !USE_FREE_MODE
	if ( CClientSharing::mUserInfo.t_count==0 && CClientSharing::mUserInfo.n_count==0 )
	{
		//gMainCtrlPage->mLeftPierPanel.ShowRepayVisible(TRUE);
		return FALSE;
	}
#endif
// 如果是云加歌模式，则进入云加歌列表
	if ( !pSongInfo->bExist && pSongInfo->bCloud )
	{
    //gMainCtrlPage->SetPromptMsgVisible();
    gMainCtrlPage->mPromptMsgWnd.SetPromptMsg(PromptType_TimeClose, 3000);
		return AddSongToCloudList(pSongInfo, nIndex);
	}
// end 云加歌
#if ( BAR_MODE == 1 )
	if( !bForceAdd && gs_SystemSetting.miBarOnOff == 1 )
	{
		SONGINFO BarSongInfo;
		memcpy(&BarSongInfo, pSongInfo, sizeof(SONGINFO));


		// 取消已点中的报台号
		// 修正在已点列表中点歌，台号累加的问题
		int num = 0;
		char name[50+1] = {0};
		if ( sscanf(pSongInfo->cName, "(%d)%s", &num, name) == 2 )
		{
			SAFE_STRNCPY(BarSongInfo.cName, name, sizeof(BarSongInfo.cName));
		}


		//gMainCtrlPage->SetBarNumForSelectedSong(&BarSongInfo);
		return FALSE;
	}
#endif

	if (!pSongInfo || !strlen(pSongInfo->cID))
	{
		return FALSE;
	}

	if (IsSongInSelectedList(pSongInfo->cID))
	{
		return FALSE;
	}

	if (GetSelectedSongCount() >= SONGITEM_MAX)
	{
		return FALSE;
	}

	if (strlen(mPlayingSong.cID) == 0)
	{
		if( mSituationMode )
			return FALSE;

		memcpy(&mPlayingSong, pSongInfo, sizeof(SONGINFO));
		mPlayingSong.bChos = false;
		Play(&mPlayingSong, FALSE);

		ReportUIStateChangedToPad((int)SYNCTYPE_SWITCH);
		CBaseRecord::Start_Record();
#ifdef  DUMP_AUDIO_PCM
		g_pBCM_Ctrl->AudioCompareCheck(mPlayingSong.cFilePath,mPlayingSong.wTrack);
#else
		adsfhasdf
		g_pBCM_Ctrl->AudioCompareStart();
#endif
	}
	else
	{
		SONGINFO *pNewSongInfo = new SONGINFO;
		if (!pNewSongInfo)
		{
			return FALSE;
		}

		memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));
		pNewSongInfo->bPlaying = FALSE;
#if ( BAR_MODE == 1 )
		pNewSongInfo->bChos = false;
#endif

		mSongInfoListLock.Lock();
		nIndex = mSelectedList.AddData(pNewSongInfo, nIndex);
		mSongInfoListLock.Unlock();
	}

	//Sql_UpdateSongOrderTimes(pSongInfo);	//反应慢			//add by yq for 增加点击率

	//DelSongFromSingedList(pSongInfo->cID);
	ReportSongStateChanged(pSongInfo->cID);
	ReportListStateChanged();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_SONG);

//	FastSearch_SaveSelectedList(&mSelectedList); // 在SelectedList.cpp做了定时器保存	//add by yq,2013-01-21

#if defined(__arm__) || defined(__mips__)
	PlayApi_UpdateWatermark();
	//OSD_ShowMtvWatermark();
	UpdateScrollMsg();
#endif
	return TRUE;
}

BOOL CPlayerInfo::AddSongToSingedList(
	const SONGINFO *pSongInfo,
	int nIndex)
{
	if (!pSongInfo || !strlen(pSongInfo->cID))
	{
		return FALSE;
	}

	if (IsSongInSingedList(pSongInfo->cID))
	{
	//	return FALSE;
	}

	if (GetSelectedSongCount() >= SONGITEM_MAX)
	{
		DelSongFromSingedList(SONGITEM_MAX-1);
	}

	SONGINFO *pNewSongInfo = new SONGINFO;
	if (!pNewSongInfo)
	{
		return FALSE;
	}

	memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));
	pNewSongInfo->bPlaying = FALSE;

	mSongInfoListLock.Lock();
	mSingedList.AddData(pNewSongInfo, nIndex);
	mSongInfoListLock.Unlock();

	ReportListStateChanged();

	return TRUE;
}

void CPlayerInfo::DelSongFromSelectedList(
	const char *cSongID)
{

	if (strcmp(cSongID, mPlayingSong.cID) == 0)
	{
		PlayNext(TRUE);
	}
	else
	{
		int nIndex = GetSelectedSongIndexByID(cSongID);
		DelSongFromSelectedList(nIndex);
	}

}

void CPlayerInfo::DelSongFromSelectedList(
	int nIndex)
{
	mSongInfoListLock.Lock();
	SONGINFO *pSongInfo = (SONGINFO *)mSelectedList.DeleteAt(nIndex);
	mSongInfoListLock.Unlock();

	if (pSongInfo)
	{
		ReportSongStateChanged(pSongInfo->cID);
		ReportListStateChanged();

		delete pSongInfo;
		if(!mPadChangeState)
			ReportUIStateChangedToPad((int)SYNCTYPE_SONG);
#if defined(__arm__) || defined(__mips__)
	PlayApi_UpdateWatermark();
	//OSD_ShowMtvWatermark();
	UpdateScrollMsg();
#endif
	}

}

void CPlayerInfo::DelSongFromSingedList(
	const char *cSongID)
{
	int nIndex = GetSingedSongIndexByID(cSongID);
	DelSongFromSingedList(nIndex);
}

void CPlayerInfo::DelSongFromSingedList(
	int nIndex)
{
	mSongInfoListLock.Lock();
	SONGINFO *pSongInfo = (SONGINFO *)mSingedList.DeleteAt(nIndex);
	mSongInfoListLock.Unlock();

	if (pSongInfo)
	{
		ReportListStateChanged();

		delete pSongInfo;

		if(!mPadChangeState)
			ReportUIStateChangedToPad((int)SYNCTYPE_SINGED);
	}
}

void CPlayerInfo::MoveSelectedSongToFirst(
	const char *cSongID)
{
	int nIndex = GetSelectedSongIndexByID(cSongID);
	MoveSelectedSongToFirst(nIndex);
}

void CPlayerInfo::MoveSelectedSongToFirst(
	int nIndex)
{
	mSongInfoListLock.Lock();
	mSelectedList.MoveData(nIndex, 1, -1);
	mSongInfoListLock.Unlock();

	ReportListStateChanged();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_SONG);

#if defined(__arm__) || defined(__mips__)
	PlayApi_UpdateWatermark();
	//OSD_ShowMtvWatermark();
	UpdateScrollMsg();
#endif
}

void CPlayerInfo::MoveSelectedListItem(
	int nItemFrom,
	int nItemTo)
{
	mSongInfoListLock.Lock();
	mSelectedList.MoveData(nItemFrom, 1, nItemTo);
	mSongInfoListLock.Unlock();

	ReportListStateChanged();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_SONG);

#if defined(__arm__) || defined(__mips__)
	PlayApi_UpdateWatermark();
	//OSD_ShowMtvWatermark();
	UpdateScrollMsg();
#endif
}

void CPlayerInfo::MoveSingedListItem(
	int nItemFrom,
	int nItemTo)
{
	mSongInfoListLock.Lock();
	mSingedList.MoveData(nItemFrom, 1, nItemTo);
	mSongInfoListLock.Unlock();

	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_SINGED);

	ReportListStateChanged();
}

void CPlayerInfo::ReportSongStateChanged(
	const char* cSongID)
{
	UINT64 wParam = cSongID[0] |
		(cSongID[1] << 8) |
		(cSongID[2] << 16) |
		(cSongID[3] << 24);
	UINT64 lParam = cSongID[4] |
		(cSongID[5] << 8) |
		(cSongID[6] << 16) |
		(cSongID[7] << 24);

	theBaseApp->BroadcastMsg(PLAYERINFO_SONGSTATE_CHANGE, wParam, lParam);

//	SONGINFO *pSongInfo = GetSelectedSongInfoByIndex(0);
	//PlayerApi_UpdateMsgInfo(((strcmp(mPlayingSong.cID, "")==0)?("NULL"):(mPlayingSong.cName)),((pSongInfo==NULL)?("NULL"):(pSongInfo->cName)));

}

void CPlayerInfo::ReportListStateChanged()
{
	theBaseApp->BroadcastMsg(PLAYERINFO_LISTSTATE_CHANGE, 0, 0);
}

void CPlayerInfo::ReportOriginalUIStateChanged()
{
	theBaseApp->BroadcastMsg(PLAYERINFO_ORIGINAL_CHANGE, 0, 0);
}

//add by yq,2012-12-13
void CPlayerInfo::SetTVLightness(int iLightness)
{
	PlayerApi_SetTVLightness(iLightness);
}
void CPlayerInfo::SetTVContrast(int iContrast)
{
	PlayerApi_SetTVContrast(iContrast);
}
void CPlayerInfo::SetTVSaturation(int iStaturation)
{
	PlayerApi_SetTVSaturation(iStaturation);
}

void CPlayerInfo::ChangeBGVideo(int iBGVideoIndex)
{
	if ( mCurPlayBgVideoIdx != iBGVideoIndex )
	{
		mCurPlayBgVideoIdx = iBGVideoIndex;
		PlayerApi_ChangeBGVideo(iBGVideoIndex);
	}
}

int CPlayerInfo::GetCurBGVideo(void)
{
	return mCurPlayBgVideoIdx;
}
//end add by yq,2012-12-13

void CPlayerInfo::PlayStart()
{

//	SONGINFO *pSongInfo = (SONGINFO *)GetSelectedSongInfoByIndex(0);
//	if (pSongInfo)
//	{
//		memcpy(&mPlayingSong, pSongInfo, sizeof(SONGINFO));
//		DelSongFromSelectedList(0);
//		ReportListStateChanged();
//	}
//	else
//	{
//		memset(&mPlayingSong, 0, sizeof(SONGINFO));
//		giPubBroadCastIdx++;
//	}
//
//	if(gs_SystemSetting.miStartUp_orignal == 1)
//	{
//		mOriginal = 0;
//	}
//	else if(gs_SystemSetting.miStartUp_orignal == 0)
//	{
//		mOriginal = 1;
//	}
//
//	if(giPubBroadCastIdx >= 0)
//	{
//		pSongInfo = FastSearch_GetPubBroadCastSongByIdx(giPubBroadCastIdx);
//		if(pSongInfo != NULL)
//		{
//			memcpy(&mPlayingPublicSong, pSongInfo, sizeof(SONGINFO));
//			Play(pSongInfo, FALSE);
//			ReportSongStateChanged(mPlayingPublicSong.cID);
//		}
//	}
//	else
//	{
//		Play(&mPlayingSong, FALSE);
//		ReportSongStateChanged(mPlayingSong.cID);
//	}
//	mPlayStatus = 1;
}

void CPlayerInfo::ReportUIStateChangedToPad(int iType)
{
	SendSyncInfoToFromUI(iType);

#if 0 && !defined(ANDROID) && defined(__mips__)
	switch(iType)
	{
	case SYNCTYPE_SWITCH:
		//SendSyncToPadFromUI(0,SYNCTYPE_SWITCH);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SWITCH_SONG);
		break;
	case SYNCTYPE_REPLAY:
		//SendSyncToPadFromUI(0,SYNCTYPE_REPLAY);
		dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_START);
		break;
	case SYNCTYPE_SONG:
		//SendSyncToPadFromUI(0,SYNCTYPE_SONG);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SELECT_SONG);
		break;
	case SYNCTYPE_PAUSE:
		//SendSyncToPadFromUI(0,SYNCTYPE_PAUSE);
		dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_PAUSE);
		break;
	case SYNCTYPE_RESUME:
		//SendSyncToPadFromUI(0,SYNCTYPE_RESUME);
		dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_RESUME);
		break;
	case SYNCTYPE_VOLUME:
		//SendSyncToPadFromUI(mVolume,SYNCTYPE_VOLUME);
		dmr_process_uievent(DMR_EVENT_UICHANGE_VOLUME);
		break;
	case SYNCTYPE_CHANNEL:
		//SendSyncToPadFromUI(mOriginal,SYNCTYPE_CHANNEL);
		dmr_process_uievent(DMR_EVENT_UICHANGE_AUDIO_TRACK);
		break;
	case SYNCTYPE_START_SCORE:
		//SendSyncToPadFromUI(0,SYNCTYPE_START_SCORE);
		dmr_process_uievent(DMR_EVENT_UICHANGE_START_SCORE);
		break;
	case SYNCTYPE_STOP_SCORE:
		//SendSyncToPadFromUI(0,SYNCTYPE_STOP_SCORE);
		dmr_process_uievent(DMR_EVENT_UICHANGE_STOP_SCORE);
		break;
	case SYNCTYPE_SITUATION:
		//SendSyncToPadFromUI(0,SYNCTYPE_SITUATION);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SITUATION);
		break;
	case SYNCTYPE_PUBLICSONG:
		//SendSyncToPadFromUI(0,SYNCTYPE_PUBLICSONG);
		break;
	case SYNCTYPE_PLAYUDISK:
		//SendSyncToPadFromUI(0,SYNCTYPE_PLAYUDISK);
		break;
	case SYNCTYPE_SINGED:
		//SendSyncToPadFromUI(0,SYNCTYPE_SINGED);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SWITCH_SONG);
		break;
	default:
		break;
	}
#endif

}

SONGINFO *CPlayerInfo::GetPlayingSongInfo()
{
	if (strlen(mPlayingSong.cID) == 0)
	{
		return NULL;
	}
	return &mPlayingSong;
}

void CPlayerInfo::SetPadChangeState(BOOL bFlag)
{
	mPadChangeState = bFlag;
}

int CPlayerInfo::GetOriginalState()
{
	return mOriginal;
}

void CPlayerInfo::SetTvPutOutMode(int mode)
{
	PlayApi_SetTvPutOutMode(mode);
}

void CPlayerInfo::SetHdmiAudio(int bOpen)
{
	PlayApi_SetHdmiAudio(bOpen);
}

void CPlayerInfo::StartGuide()
{
	PlayApi_StartGuide();
}

void CPlayerInfo::StopGuide()
{
	PlayApi_StopGuide();
}

void CPlayerInfo::Play_UDiskSong(
	const char *cServerName,
	const char *cFileName)
{
#if HISI_NET_APP
	if ( getFireStaus() )			// 火警状态中,不能切歌
		return ;
#endif
	if((NULL == cServerName) || (NULL == cFileName))
		return ;

	if( mSituationMode )
		return ;

	PlayerApi_PlayMovie(
		cServerName,
		cFileName,
		mVolume,
		0,
		mOriginal);
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_PLAYUDISK);
}

void CPlayerInfo::PreviewMovie(int x, int y, int w, int h)
{
	g_bIsInPhantom = TRUE;
	g_bIsPlayMoive = TRUE;
	theE3DEngine->setTransparentRect(true, x, y, w, h);
	PlayerApi_PlayMTV(x, y, w, h);
}

void CPlayerInfo::StopPreviewMoview()
{
	g_bIsInPhantom = FALSE;
	g_bIsPlayMoive = FALSE;
	theE3DEngine->setTransparentRect(false);
	PlayerApi_StopMTV();
}

void CPlayerInfo::PauseMovie()
{
	PlayerApi_Pause();
}

void CPlayerInfo::ResumeMovie()
{
	PlayerApi_Resume();
}

void CPlayerInfo::StartForwardMovie()
{
	PlayApi_ForwardMovie();
}

void CPlayerInfo::StartBackwardMovie()
{
	PlayApi_BackwardMovie();
}

void CPlayerInfo::StartRecord(const char *recordName)
{
	if(NULL == recordName)
		return;
	PlayApi_StartRecord(recordName);
}

void CPlayerInfo::StopRecord()
{
	PlayApi_StopRecord();
}

void CPlayerInfo::PlayRecord(const char *filename)
{
	if(NULL == filename)
		return;
	PlayApi_PlayRecord(filename,
			mVolume,
			mOriginal);
}

//电视水印
void CPlayerInfo::ShowWatermark()
{
	PlayApi_ShowWatermark();
}

void CPlayerInfo::UnShowWatermark()
{
	PlayApi_UnShowWatermark();
}

void CPlayerInfo::PlayUrlStream(const char *url)
{
	if(url == NULL)
		return ;

	if( mSituationMode )
		return ;

	PlayApi_PlayUrlStream(url);
}

//更新跑马灯的信息（正播，和下一首）
void CPlayerInfo::UpdateScrollMsg()
{
#if defined(__mips__) || defined(__arm__)
	int iNeedUpdate = 0;
	static char strPlayID[8+1] = {0};
	static char strNextPlayID[8+1] = {0};

	SONGINFO *pPlayingSongInfo = GetCurPlaySongInfo();
	SONGINFO *pSongInfo = GetSelectedSongInfoByIndex(0);
	if(strcmp(strPlayID, pPlayingSongInfo->cID) != 0)			//需要更新
		iNeedUpdate = 1;
	else if(pSongInfo && strcmp(strNextPlayID, pSongInfo->cID)!=0)
		iNeedUpdate = 1;

	strcpy(strPlayID, pPlayingSongInfo->cID);

	if(pSongInfo)
		strcpy(strNextPlayID, pSongInfo->cID);

	OSD_SetUpdatePlayInfoFlag(iNeedUpdate);
#endif
}

void CPlayerInfo::StopPlay(void)
{
	PlayApi_StopPlay();
}

BOOL CPlayerInfo::AddSongToCloudList(
	const SONGINFO *pSongInfo,
	int nIndex)
{
	mCloudSongs.AddSongToCloudList(pSongInfo,nIndex);
	ReportSongStateChanged(pSongInfo->cID);
	return TRUE;
}

BOOL CPlayerInfo::IsSongInCloudList(
	const char* cSongID)
{
	return mCloudSongs.IsSongInCloudList(cSongID);
}

int CPlayerInfo::GetCloudSongCount()
{
	return mCloudSongs.GetCloudSongCount();
}

SONGINFO *CPlayerInfo::GetCloudSongInfoByIndex(
	int nIndex)
{
	return mCloudSongs.GetCloudSongInfoByIndex(nIndex);
}

void CPlayerInfo::MoveCloudListItem(
		int nItemFrom,
		int nItemTo)
{
	mCloudSongs.MoveCloudListItem(nItemFrom, nItemTo);
	ReportListStateChanged();
}

SONGINFO *CPlayerInfo::GetCurDowningInfo()
{
	return mCloudSongs.GetCurDowningInfo();
}

void CPlayerInfo::ReportDowningStateChanged()
{
//	mCloudSongs.ReportDowningStateChanged();
	theBaseApp->BroadcastMsg(DOWNINFO_LISTSTATE_CHANGE, 0, 0);
}

void CPlayerInfo::CloudDownNext()
{
	SONGINFO *pSong = GetCurDowningInfo();
	FastModify_CloudSongExist(pSong);

	if( gs_SystemSetting.miBarOnOff != 1 )
		AddSongToSelectedList(pSong, -1, TRUE);
	else
		AddSongToSelectedList(pSong, -1, FALSE);

	if ( pSong )
		ReportSongStateChanged(pSong->cID);
	mCloudSongs.CloudDownNext();
	ReportListStateChanged();
}

void CPlayerInfo::CloudFailedDownNext()
{
	mCloudSongs.CloudDownNext();
	ReportListStateChanged();
}

void CPlayerInfo::ClearDowningSongInfo()
{
	SONGINFO DowningSong;
	SONGINFO *pSong = GetCurDowningInfo();
	if ( pSong != NULL )
		memcpy(&DowningSong, pSong, sizeof(SONGINFO));
	else
		memset(&DowningSong, 0, sizeof(SONGINFO));

	mCloudSongs.ClearDowningSongInfo();
	ReportSongStateChanged(DowningSong.cID);
	//ReportListStateChanged();
}

void CPlayerInfo::DelSongFromCloudList(
	int nIndex)
{
	char strSongID[8+1] = {0};
	SONGINFO *pSongInfo = mCloudSongs.GetCloudSongInfoByIndex(nIndex);
	if ( pSongInfo )
	{
		strcpy(strSongID, pSongInfo->cID);
	}
	mCloudSongs.DelSongFromCloudList(nIndex);

	ReportSongStateChanged(strSongID);
	ReportListStateChanged();
}

void CPlayerInfo::DelSongFromCloudListByID(const char *cSongID)
{
	if ( !cSongID )
		return ;

	int nIndex = mCloudSongs.GetCloudSongIndexByID(cSongID);

	mCloudSongs.DelSongFromCloudList(nIndex);

	ReportSongStateChanged(cSongID);
	ReportListStateChanged();
}

int CPlayerInfo::GetDownProgress()
{
	return mCloudSongs.GetDownProgress();
}

void CPlayerInfo::MoveCloudSongToFirst(
	int nIndex)
{
	mCloudSongs.MoveCloudSongToFirst(nIndex);
	ReportListStateChanged();
}

void CPlayerInfo::MoveCloudSongToFirst(
	const char *cSongID)
{
	int nIndex = mCloudSongs.GetCloudSongIndexByID(cSongID);
	MoveCloudSongToFirst(nIndex);
}

void CPlayerInfo::SaveSelectSongList()		// 保存已点列表
{
	FastSearch_SaveSelectedList(&mSelectedList);
}

void CPlayerInfo::SaveCloudSongList()
{
	mCloudSongs.SaveCloudSongList();
}

void CPlayerInfo::ClearSelectedSongList(void)
{
	DelNormalList(&mSelectedList, SONGINFO);
	DelNormalList(&mSingedList, SONGINFO);
}

// 点歌有吧台号的歌曲
void CPlayerInfo::AddBarNumSongToSelectedList(
		const SONGINFO *pSongInfo,
		int nIndex)
{
	if (!pSongInfo || !strlen(pSongInfo->cID))
		return ;

	if (GetSelectedSongCount() >= SONGITEM_MAX)
		return ;

//	if (strlen(mPlayingSong.cID) == 0)
//	{
//		memcpy(&mPlayingSong, pSongInfo, sizeof(SONGINFO));
//		Play(&mPlayingSong, FALSE);
//		ReportUIStateChangedToPad((int)SYNCTYPE_SWITCH);
//	}
//	else
	{
		SONGINFO *pNewSongInfo = new SONGINFO;
		if (!pNewSongInfo)
		{
			return ;
		}

		memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));
		pNewSongInfo->bPlaying = FALSE;
		mSongInfoListLock.Lock();
		nIndex = mSelectedList.AddData(pNewSongInfo, nIndex);
		mSongInfoListLock.Unlock();
	}

	//DelSongFromSingedList(pSongInfo->cID);
	ReportSongStateChanged(pSongInfo->cID);
	ReportListStateChanged();
	if(!mPadChangeState)
		ReportUIStateChangedToPad((int)SYNCTYPE_SONG);

#if defined(__mips__) || defined(__arm__)
	PlayApi_UpdateWatermark();
	//OSD_ShowMtvWatermark();
	UpdateScrollMsg();
#endif

	if (strlen(mPlayingSong.cID) == 0)
	{
		PlayNext(FALSE);
//		memcpy(&mPlayingSong, pSongInfo, sizeof(SONGINFO));
//		Play(&mPlayingSong, FALSE);
//		ReportUIStateChangedToPad((int)SYNCTYPE_SWITCH);
	}

	return ;
}

static BOOL g_bInPreview = FALSE;
static BOOL g_bInPhantom = FALSE;

void CPlayerInfo::InScreenSave()
{
	if ( g_bIsInPreview )
	{
		g_bInPreview = TRUE;
		StopPreview();
	}
	else
	{
		g_bInPreview = FALSE;
	}

	if ( g_bIsInPhantom )
	{
		g_bInPhantom = TRUE;
		StopMTV();
	}
	else
	{
		g_bInPhantom = FALSE;
	}
}

void CPlayerInfo::OutScreenSave()
{
	if ( g_bInPreview )
	{
		;
	}
	if ( g_bInPhantom )
	{
		//g_bIsInPhantom = TRUE;
		//PlayerApi_PlayMTV(g_rcPhantom.left, g_rcPhantom.top, g_rcPhantom.right-g_rcPhantom.left, g_rcPhantom.bottom - g_rcPhantom.top);
	}
}

CSongLibrary *CPlayerInfo::GetCloudSongHandle()
{
	return &mCloudSongs;
}
