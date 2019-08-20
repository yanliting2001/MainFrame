#include "UDiskFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "CommApi.h"
#include "../PlayerApi/PlayerInfo.h"
#include "../CommControl/MainCtrlPage.h"

extern CMainCtrlPage *gMainCtrlPage;

#define BTN_RETURN_ID			0x01
#define BTN_SHOW_CLOUDSONG_ID		0x02
#define BTN_PREVPAGE_ID		0x03
#define BTN_NEXTPAGE_ID		0x04
#define BTN_ADD_FILE_ID		0x05
#define BTN_ADD_ALL_FILE_ID	0x06
#define BTN_BASE_PLAY_ID	0x10
#define BTN_BASE_ADD_ID		0x20

#define COPY_PROMPT_TIMER_ID			0x01
#define COPY_PROMPT_DELAY_TIME		2000	// 2S

void CUDiskFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(	"UDiskFragment", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};
	mDownloadListLabel.CreateStatic(this, rcControl);
	mDownloadListLabel.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mLeftBKGroundWnd.CreateStatic(this, rcControl);
	mUDiskFileBKGroundWnd.CreateStatic(this, rcControl);

	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].itemBKGroundWnd.CreateStatic(&mUDiskFileBKGroundWnd, rcControl);

		mItemList[i].idxWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].songNameWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].songNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
		mItemList[i].singerWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].singerWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

		mItemList[i].playBtn.Create(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].playBtn.SetWndID(BTN_BASE_PLAY_ID + i);
		mItemList[i].playBtn.SetOnClickListener(this);

		mItemList[i].addBtn.Create(&mItemList[i].itemBKGroundWnd, rcControl);
		mItemList[i].addBtn.SetWndID(BTN_BASE_ADD_ID + i);
		mItemList[i].addBtn.SetOnClickListener(this);

		mItemList[i].fengexianWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
	}

	mPrevPageBtn.Create(this, rcControl);
	mPrevPageBtn.SetWndID(BTN_PREVPAGE_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcControl);
	mNextPageBtn.SetWndID(BTN_NEXTPAGE_ID);
	mNextPageBtn.SetOnClickListener(this);

	mPageWnd.CreateStatic(this, rcControl);

	mAddFileBtn.Create(this, rcControl);
	mAddFileBtn.SetWndID(BTN_ADD_FILE_ID);
	mAddFileBtn.SetOnClickListener(this);

	mAddAllFileBtn.Create(this, rcControl);
	mAddAllFileBtn.SetWndID(BTN_ADD_ALL_FILE_ID);
	mAddAllFileBtn.SetOnClickListener(this);

	mAddUDiskFileWnd.Create(this);

	LoadResource();
}

void CUDiskFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "UDisk/UDiskFragment.xml");
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
	int size, yOffset = 0;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
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
	mDownloadListLabel.MoveWindow(&rcControl);
	for ( int i = 0; i < page_count; i++ )
	{
		char field[8] = {0};
		sprintf(field, "text%d", i);
		SAFE_STRNCPY(mPromptText[i], parser.GetStrValue("SearchPromptInfo", field, "FFFFFFFF") , sizeof(mPromptText[i]));
	}

	XmlLoadRect(&parser, "LeftBKGroundWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LeftBKGroundWndInfo", "path", "DLNA/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mLeftBKGroundWnd.SetBackgroundTexture(&bkTexture);
	mLeftBKGroundWnd.MoveWindow(&rcControl);
	mUDiskFileBKGroundWnd.MoveWindow(&rcControl);
	mAddUDiskFileWnd.MoveWindow(&rcControl);

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

	XmlLoadRect(&parser, "DownloadIcoWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("DownloadIcoWndInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].playBtn.SetTextures(btnTextures);
		mItemList[i].playBtn.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "DeleteBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("DeleteBtnInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].addBtn.SetTextures(btnTextures);
		mItemList[i].addBtn.MoveWindow(&rcControl);
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

	XmlLoadRect(&parser, "AddFileBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("AddFileBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mAddFileBtn.SetTextures(btnTextures);
	mAddFileBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "AddAllFileBtnInfo", &rcControl);
	mAddAllFileBtn.SetFontSize(size);
	mAddAllFileBtn.SetFontColor(color);
	mAddAllFileBtn.SetWindowTextA(parser.GetStrValue("AddAllFileBtnInfo", "text", "全部添加"));
	mAddAllFileBtn.MoveWindow(&rcControl);

	mUDiskFileBKGroundWnd.SetFontSize(size);
	mUDiskFileBKGroundWnd.SetFontColor(0xFFFF0000);
}

void CUDiskFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;
	int nWndID = pWnd->GetWndID();

	if ( nWndID == BTN_RETURN_ID )
	{
		if ( mPageType == page_uDisk )
			gMainCtrlPage->ReturnToLastFragment();
		else if ( mPageType == page_item )
		{
			mPageType = page_uDisk;
			ShowPageType();
		}
	}
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
	else if ( nWndID ==BTN_ADD_FILE_ID )
	{
		gMainCtrlPage->StartWaiting();
		BOOL bAddOK = AddUDiskFileToVod(mAddUDiskFileWnd.GetAddUDiskFileInfo());
		mAddUDiskFileWnd.SetAddResultPrompt(bAddOK);
		gMainCtrlPage->StopWaiting();

		//AddTimer(COPY_PROMPT_TIMER_ID, COPY_PROMPT_DELAY_TIME);
	}
	else if ( nWndID == BTN_ADD_ALL_FILE_ID )
	{
		gMainCtrlPage->StartWaiting();

		int cnt = mUDiskFileLists.GetCount();
		int successCnt = 0, faileCnt = 0;

		for ( int i = 0; i < cnt; i++ )
		{
			UDISK_FILE_INFO_S *pFile = (UDISK_FILE_INFO_S *) mUDiskFileLists.GetAt(i);
			if ( pFile )
			{
				BOOL bAddOK = AddUDiskFileToVod(pFile , FALSE);
				if ( bAddOK )
					successCnt++;
				else
					faileCnt++;
			}
		}
		char msg[128] = {0};
		snprintf(msg, sizeof(msg), "添加成功:%d, 失败:%d", successCnt, faileCnt);
		mUDiskFileBKGroundWnd.SetWindowTextA(msg);

		gMainCtrlPage->StopWaiting();

		AddTimer(COPY_PROMPT_TIMER_ID, COPY_PROMPT_DELAY_TIME);
	}
	else if ( nWndID >= BTN_BASE_PLAY_ID && (nWndID < (BTN_BASE_PLAY_ID + item_count)) )
	{
		int idx = nWndID - BTN_BASE_PLAY_ID + miCurPage * item_count;
		UDISK_FILE_INFO_S *pFileInfo = (UDISK_FILE_INFO_S *)mUDiskFileLists.GetAt(idx);
		if ( pFileInfo )
			gPlayerInfo->Play_UDiskSong(pFileInfo->cFilePath, pFileInfo->cFilePath);
		else
			DbgOutput(DBG_WARN, "play udisk file item idx:%d, miCurPage:%d failed\n", nWndID-BTN_BASE_PLAY_ID, miCurPage);
	}
	else if ( nWndID >= BTN_BASE_ADD_ID && (nWndID < (BTN_BASE_ADD_ID + item_count)) )
	{
		int idx = nWndID - BTN_BASE_ADD_ID + miCurPage * item_count;
		UDISK_FILE_INFO_S *pFileInfo = (UDISK_FILE_INFO_S *)mUDiskFileLists.GetAt(idx);
		if ( pFileInfo )
		{
			mAddUDiskFileWnd.SetAddUDiskFileInfo(pFileInfo);
			mPageType = page_item;
			ShowPageType();
		}
		else
			DbgOutput(DBG_WARN, "add udisk file item idx:%d, miCurPage:%d failed\n", nWndID-BTN_BASE_PLAY_ID, miCurPage);
	}
}

void CUDiskFragment::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	gMainCtrlPage->mSearchInputWnd.SetInputWndVisible(bVisible);
}

void CUDiskFragment::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);
}

void CUDiskFragment::InitUDiskFileList(void)
{
	ClearFileList();

  //////umount mount
//  system("mount |  grep sdb1 ");
  system("umount /stb/usb/sdb1;mount -o iocharset=utf8  /dev/sdb1 /stb/usb/sdb1");
//  system("mount |  grep sdb1 ");
	FindFileByPath(&mUDiskFileLists, "/stb/usb/sdb1", 0);

	mPageType = page_uDisk;
	ShowPageType();

	miCurPage = 0;
	miAllPage = (mUDiskFileLists.GetCount() + item_count - 1) / item_count;
	if ( miAllPage == 0 )
		miAllPage = 1;
  
	UpdateItemList();
}

void CUDiskFragment::UpdateItemList(void)
{
	char tmp[32] = {0};
	int idx = miCurPage * item_count;
	UDISK_FILE_INFO_S *pFile = NULL;

	for ( int i = 0; i < item_count; i++ )
	{
		pFile =  (UDISK_FILE_INFO_S *)mUDiskFileLists.GetAt(i + idx);
		SetUDiskFileInfo(i, pFile);
	}

	snprintf(tmp, sizeof(tmp), "%d/%d", miCurPage+1, miAllPage);
	mPageWnd.SetWindowTextA(tmp);
}

void CUDiskFragment::SetUDiskFileInfo(int nIndex, const UDISK_FILE_INFO_S *pFileInfo)
{
	CBaseStringA tmp;
	int nCuridx = nIndex % item_count;

	if ( nCuridx >= 0 && nCuridx < item_count )
	{
		if ( pFileInfo )
		{
			mItemList[nCuridx].itemBKGroundWnd.SetWindowVisible(TRUE);

			tmp.Format("%02d |", nIndex + 1 + miCurPage * item_count);
			mItemList[nCuridx].idxWnd.SetWindowTextA(tmp.GetString());

			mItemList[nCuridx].songNameWnd.SetWindowTextA(pFileInfo->cSongName);
			mItemList[nCuridx].singerWnd.SetWindowTextA(pFileInfo->cSinger);
		}
		else
		{
			mItemList[nCuridx].itemBKGroundWnd.SetWindowVisible(FALSE);
		}
	}
}

void CUDiskFragment::ShowPageType(void)
{
	mDownloadListLabel.SetWindowTextA(mPromptText[mPageType]);
	switch ( mPageType )
	{
	case page_uDisk:
		mUDiskFileBKGroundWnd.SetWindowVisible(TRUE);
		mPrevPageBtn.SetWindowVisible(TRUE);
		mNextPageBtn.SetWindowVisible(TRUE);
		mPageWnd.SetWindowVisible(TRUE);
		mAddUDiskFileWnd.SetWindowVisible(FALSE);
		mAddFileBtn.SetWindowVisible(FALSE);
		mAddAllFileBtn.SetWindowVisible(TRUE);
		break;

	case page_item:
		mUDiskFileBKGroundWnd.SetWindowVisible(FALSE);
		mPrevPageBtn.SetWindowVisible(FALSE);
		mNextPageBtn.SetWindowVisible(FALSE);
		mPageWnd.SetWindowVisible(FALSE);
		mAddUDiskFileWnd.SetWindowVisible(TRUE);
		mAddFileBtn.SetWindowVisible(TRUE);
		mAddAllFileBtn.SetWindowVisible(FALSE);
		break;

	default:
		break;
	}
}

void CUDiskFragment::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
	if ( nTimerID == COPY_PROMPT_TIMER_ID )
	{
		DelTimer(nTimerID);
		if ( mPageType == page_uDisk )
		{
			mUDiskFileBKGroundWnd.SetWindowTextA(NULL);
		}
	}
}
