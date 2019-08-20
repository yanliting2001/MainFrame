#include "DownloadListFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "CommApi.h"
#include "CSGetMacAddr.h"
#include "../SQL/FastSearch.h"
#include "InternetCtrl/NetCtrlApi.h"
#include "../PlayerApi/PlayerInfo.h"
#include "../CommControl/MainCtrlPage.h"

extern CMainCtrlPage *gMainCtrlPage;

#define BTN_RETURN_ID			0x01
#define BTN_SHOW_CLOUDSONG_ID		0x02
#define BTN_PREVPAGE_ID		0x03
#define BTN_NEXTPAGE_ID		0x04
#define BTN_BASE_ITEM_ID	0x10

#define START_UPDATE_PROCESS_TIME_ID		0x01
#define START_UPDATE_PROCESS_TIME			2000		// 2S

void CSystemInfoWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, 100, 100};

	CParentClass::Create("SystemInfoWnd", pParent,rc);

	RECT rcControl = {0, 0, 100, 100};
	mVodIDWnd.CreateStatic(this, rcControl);
	mVodIDWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mIPAddressWnd.CreateStatic(this, rcControl);
	mIPAddressWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mLocalSongCntWnd.CreateStatic(this, rcControl);
	mLocalSongCntWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mCloudSongCntWnd.CreateStatic(this, rcControl);
	mCloudSongCntWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mLeftMemoryWnd.CreateStatic(this, rcControl);
	mLeftMemoryWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mCanBeAddCntWnd.CreateStatic(this, rcControl);
	mCanBeAddCntWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mShowCloudSongBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mShowCloudSongBtn.SetWndID(BTN_SHOW_CLOUDSONG_ID);
	mShowCloudSongBtn.SetOnClickListener(this);

	LoadResource();
}

void CSystemInfoWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "DownloadListFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml

	RECT rcShow;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];
	char colorbuf[16];
	unsigned long color;
	int size;

	XmlLoadRect(&parser, "RightBKGroundWndInfo", &rcShow);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("RightBKGroundWndInfo", "path", "DLNA/BkGround"), sizeof(imgPath));
	//CreateImgTexture(imgPath, &bkTexture);
	//SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "VodIDWndInfo", &rcControl);
	size = parser.GetIntValue("VodIDWndInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("VodIDWndInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mVodIDWnd.SetFontSize(size);
	mVodIDWnd.SetFontColor(color);
	mVodIDWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "IPAddressWndInfo", &rcControl);
	mIPAddressWnd.SetFontSize(size);
	mIPAddressWnd.SetFontColor(color);
	mIPAddressWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LocalSongCntWndInfo", &rcControl);
	mLocalSongCntWnd.SetFontSize(size);
	mLocalSongCntWnd.SetFontColor(color);
	mLocalSongCntWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CloudSongCntWndInfo", &rcControl);
	mCloudSongCntWnd.SetFontSize(size);
	mCloudSongCntWnd.SetFontColor(color);
	mCloudSongCntWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LeftMemoryWndInfo", &rcControl);
	mLeftMemoryWnd.SetFontSize(size);
	mLeftMemoryWnd.SetFontColor(color);
	mLeftMemoryWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CanBeAddCntWndInfo", &rcControl);
	mCanBeAddCntWnd.SetFontSize(size);
	mCanBeAddCntWnd.SetFontColor(color);
	mCanBeAddCntWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ShowCloudSongBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowCloudSongBtnInfo", "path", "Public/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mShowCloudSongBtn.SetTextures(btnTextures);
	mShowCloudSongBtn.MoveWindow(&rcControl);

	RefreshSystemInfo();
}

void CSystemInfoWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
}

void CSystemInfoWnd::RefreshSystemInfo(void)
{
	char tmp[32] = {0};
	mVodIDWnd.SetWindowTextA(g_sIPInfo.cMac);
	mIPAddressWnd.SetWindowTextA(g_sIPInfo.cIpAddr);

	sprintf(tmp, "%d", FastSearch_GetLocalSongCount());
	mLocalSongCntWnd.SetWindowTextA(tmp);

	sprintf(tmp, "%d", FastSearch_GetCloudSongCount());
	mCloudSongCntWnd.SetWindowTextA(tmp);

	DiskInfoDetail sDiskInfoDetail;
	CSDetectDiskFreeSpace(g_cMainSongParentPath, &sDiskInfoDetail);
	sprintf(tmp, "%0.1f G", sDiskInfoDetail.uFreeSize/1024.0f);
	mLeftMemoryWnd.SetWindowTextA(tmp);

	sprintf(tmp, "%lld", sDiskInfoDetail.uFreeSize / 100);
	mCanBeAddCntWnd.SetWindowTextA(tmp);
}

void CDownloadListFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(	"DownloadListFragment", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};
	mDownloadListLabel.CreateStatic(this, rcControl);
	mDownloadListLabel.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mLeftBKGroundWnd.CreateStatic(this, rcControl);
	mSystemInfoWnd.Create(this);

	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].itemBKGroundWnd.CreateStatic(&mLeftBKGroundWnd, rcControl);

		mItemList[i].idxWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].songNameWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].songNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
		mItemList[i].singerWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].singerWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
		mItemList[i].precentWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].downloadIcoWnd.Create(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].deleteBtn.Create(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].deleteBtn.SetWndID(BTN_BASE_ITEM_ID + i);
		mItemList[i].deleteBtn.SetOnClickListener(this);
		mItemList[i].fengexianWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
	}

	mPrevPageBtn.Create(this, rcControl);
	mPrevPageBtn.SetWndID(BTN_PREVPAGE_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcControl);
	mNextPageBtn.SetWndID(BTN_NEXTPAGE_ID);
	mNextPageBtn.SetOnClickListener(this);

	mPageWnd.CreateStatic(this, rcControl);

	LoadResource();
}

void CDownloadListFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "DownloadListFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcBase ;
	CTexture bkTexture, btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	unsigned long color;
	int size, size1, yOffset = 0;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("rcLocation", "path", "DownloadList/BKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SearchPromptInfo", &rcControl);
	size = parser.GetIntValue("SearchPromptInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("SearchPromptInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mDownloadListLabel.SetFontSize(size);
	mDownloadListLabel.SetFontColor(color);
	//mDownloadListLabel.SetWindowTextA(parser.GetStrValue("SearchPromptInfo", "text", "主题点歌"));
	mDownloadListLabel.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LeftBKGroundWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LeftBKGroundWndInfo", "path", "DownloadList/LeftBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mLeftBKGroundWnd.SetBackgroundTexture(&bkTexture);
	mLeftBKGroundWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "BaseItemInfo", &rcBase);
	size = parser.GetIntValue("BaseItemInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("BaseItemInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	yOffset = parser.GetIntValue("BaseItemInfo", "yOffset", 20);

	for ( int i = 0; i < item_count; i++ )
	{
		InflatRect(&rcControl, &rcBase, 0, i*yOffset);
		mItemList[i].itemBKGroundWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "IdxWndInfo", &rcControl);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].idxWnd.SetFontSize(size);
		mItemList[i].idxWnd.SetFontColor(color);
		mItemList[i].idxWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SongNameWndInfo", &rcControl);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].songNameWnd.SetFontSize(size);
		mItemList[i].songNameWnd.SetFontColor(color);
		mItemList[i].songNameWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SingerWndInfo", &rcControl);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].singerWnd.SetFontSize(size);
		mItemList[i].singerWnd.SetFontColor(color);
		mItemList[i].singerWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PrecentWndInfo", &rcControl);
	size1 = parser.GetIntValue("PrecentWndInfo", "size", 20);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].precentWnd.SetFontSize(size1);
		mItemList[i].precentWnd.SetFontColor(color);
		mItemList[i].precentWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "DownloadIcoWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("DownloadIcoWndInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].downloadIcoWnd.SetTextures(btnTextures);
		mItemList[i].downloadIcoWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "DeleteBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("DeleteBtnInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].deleteBtn.SetTextures(btnTextures);
		mItemList[i].deleteBtn.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "FengexianWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("FengexianWndInfo", "path", "DLNA/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].fengexianWnd.SetBackgroundTexture(&bkTexture);
		mItemList[i].fengexianWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PrevPageBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PrevPageBtnInfo", "path", "Public/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPrevPageBtn.SetTextures(btnTextures);
	mPrevPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NextPageBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NextPageBtnInfo", "path", "Public/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNextPageBtn.SetTextures(btnTextures);
	mNextPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PageWndInfo", &rcControl);
	mPageWnd.SetFontSize(size);
	mPageWnd.SetFontColor(color);
	mPageWnd.MoveWindow(&rcControl);
}

void CDownloadListFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;
	int nWndID = pWnd->GetWndID();

	if ( nWndID == BTN_RETURN_ID )
		gMainCtrlPage->ReturnToLastFragment();
	else if ( nWndID == BTN_PREVPAGE_ID )
	{
		if ( miCurPage > 0 )
		{
			miCurPage--;
			UpdateItemList();
		}
	}
	else if ( nWndID == BTN_NEXTPAGE_ID )
	{
		if ( miCurPage < (miAllPage - 1) )
		{
			miCurPage++;
			UpdateItemList();
		}
	}
	else if ( nWndID >= BTN_BASE_ITEM_ID && (nWndID < (BTN_BASE_ITEM_ID + item_count)) )
	{
		int idx = nWndID - BTN_BASE_ITEM_ID + miCurPage * item_count - 1;
		if ( idx >= 0 )
			gPlayerInfo->DelSongFromCloudList(idx);
		else
			DbgOutput(DBG_WARN, "delete download item idx:%d, miCurPage:%d failed\n", nWndID-BTN_BASE_ITEM_ID, miCurPage);
	}

}

void CDownloadListFragment::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
	if ( !IsWindowVisible() )
		return ;

	if ( nTimerID == START_UPDATE_PROCESS_TIME_ID )
	{
		int iProcess = gPlayerInfo->GetDownProgress();
		SetDownProcess(iProcess);
	}
}

void CDownloadListFragment::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	switch(uType)
	{
	case PLAYERINFO_SONGSTATE_CHANGE:
	case PLAYERINFO_LISTSTATE_CHANGE:
		InitDownloadList();
		break;

	case DOWNINFO_LISTSTATE_CHANGE:	// 开始下载了
		SetDownProcess(0);
		StartUpdateProcess(TRUE);
		InitDownloadList();
		break;

	case DOWNINFO_DOWNLOAD_OK:			// 下载成功
	  if(gPlayerInfo->GetDownProgress() == 0 || gPlayerInfo->GetDownProgress() < 100) //当下载进度0或没有下完
	  {
      gPlayerInfo->ClearDowningSongInfo();
      
      DbgOutput(DBG_ERR,"DOWNINFO_DOWNLOAD_FAILURE  LINE %d GetDownProgress = %d\n", __LINE__, gPlayerInfo->GetDownProgress());
    }
    else
    {
		  gPlayerInfo->CloudDownNext();
    }
    
		break;

	case DOWNINFO_DOWNLOAD_FAILURE:	// 下载失败

  
		if ( wParam == 1 )
		{
			// 提示空间不足
			//mPlayInfoWnd.SetNoSpaceStatus(TRUE);
			gPlayerInfo->ClearDowningSongInfo();
		}
		else if ( !g_bIsNetConn )
		{
			gPlayerInfo->ClearDowningSongInfo();
			//mPlayInfoWnd.RefreshPlayInfo();
		}
		else
		{
			gPlayerInfo->CloudFailedDownNext();
		}
		break;
	}
}

void CDownloadListFragment::InitDownloadList(void)
{
	miCurPage = 0;
	int cnt = gPlayerInfo->GetCurDowningInfo() ? 1 : 0;
	miAllPage = (cnt + gPlayerInfo->GetCloudSongCount() + item_count - 1) / item_count;
	if ( miAllPage == 0 )
		miAllPage = 1;

	UpdateItemList();
}

void CDownloadListFragment::UpdateItemList(void)
{
	char tmp[32] = {0};
	int idx = miCurPage * item_count;
	SONGINFO *pDownSong = NULL;
	// 首页
	int i = 0;
	if ( miCurPage == 0 )
	{
		pDownSong = gPlayerInfo->GetCurDowningInfo();
		mItemList[i].precentWnd.SetWindowVisible(pDownSong ? TRUE : FALSE);
		mItemList[i].deleteBtn.SetWindowVisible(FALSE);
		SetSongInfo(0, pDownSong);
		i = 1;
	}
	else
	{
		mItemList[i].precentWnd.SetWindowVisible( FALSE);
		mItemList[i].deleteBtn.SetWindowVisible(TRUE);
		i = 0;
	}
	for ( ; i < item_count; i++ )
	{
		pDownSong =  gPlayerInfo->GetCloudSongInfoByIndex(i + idx - 1);
		SetSongInfo(i, pDownSong);
	}

	snprintf(tmp, sizeof(tmp), "%d/%d", miCurPage+1, miAllPage);
	mPageWnd.SetWindowTextA(tmp);
}

void CDownloadListFragment::SetSongInfo(int nIndex, const SONGINFO *pSongInfo)
{
	CBaseStringA tmp;
	int nCuridx = nIndex % item_count;

	if ( nCuridx >= 0 && nCuridx < item_count )
	{
		if ( pSongInfo )
		{
			mItemList[nCuridx].itemBKGroundWnd.SetWindowVisible(TRUE);

			tmp.Format("%02d |", nIndex + 1 + miCurPage * item_count);
			mItemList[nCuridx].idxWnd.SetWindowTextA(tmp.GetString());

			mItemList[nCuridx].songNameWnd.SetWindowTextA(pSongInfo->cName);

			char *singer = FastSearch_GetSingerByID(pSongInfo->singerID);
			mItemList[nCuridx].singerWnd.SetWindowTextA(singer ? singer : "无此歌星");
		}
		else
		{
			mItemList[nCuridx].itemBKGroundWnd.SetWindowVisible(FALSE);
		}
	}
}

void CDownloadListFragment::SetDownProcess(int process)
{
	char tmp[32] = {0};
	sprintf(tmp, "%02d%%", process);
	mItemList[0].precentWnd.SetWindowTextA(tmp);
}

void CDownloadListFragment::StartUpdateProcess(BOOL bStart)
{
	if ( bStart )
		AddTimer(START_UPDATE_PROCESS_TIME_ID, START_UPDATE_PROCESS_TIME);
	else
		DelTimer(START_UPDATE_PROCESS_TIME_ID);
}
