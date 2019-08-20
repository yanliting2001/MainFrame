#include "FavoritSubBoxWnd.h"
#include "XMLParser.h"

#include "UserMsgDefine.h"
#include "../CommApi/GetResParentPath.h"

#include "../PlayerApi/PlayerInfo.h"
#include "../SQL/FastSearch.h"
#include "../GlobalUIClass.h"


#define BUTTON_ID_INSERTBASE	0x01
#define BUTTON_ID_RUSHBASE		0x20
#define BUTTON_ID_DELBASE		0x30
#define BUTTON_ID_PREV			0x41
#define BUTTON_ID_NEXT			0x42
#define BUTTON_ID_LOGINEXIT		0x43
#define BUTTON_ID_CLOSE		0x02

CFavoritSubBoxWnd::CFavoritSubBoxWnd()
{
	mCurPage = 0;
	mPageCount = 0;
	mSelectColor = 0xFFFFFFFF;
}

CFavoritSubBoxWnd::~CFavoritSubBoxWnd()
{
	FreeFavoritList();
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CFavoritSubBoxWnd::Create(CBaseWnd *pParent)
{

  RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 30);
	CParentClass::Create(
		"CFavoritSubBoxWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);
  
	RECT rcControl;
	int i;
	SetRectXY(&rcControl, 64, 147, 206, 40);
 
  mTitleIcoWnd.CreateStatic(this, rcControl);
  mColseBtn.Create(this, rcControl);
	mColseBtn.SetWndID(BUTTON_ID_CLOSE);
	mColseBtn.SetOnClickListener(this);
  
	for(i = 0; i < MYDOCSUBBOXWND_COUNT; i++)
	{
		mMyDocItem[i].SongName.CreateStatic(this, rcControl);
		mMyDocItem[i].SongName.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

		mMyDocItem[i].Insert.Create(this, rcControl);
		mMyDocItem[i].Insert.SetWndID(BUTTON_ID_INSERTBASE + i);
		mMyDocItem[i].Insert.SetOnClickListener(this);

		mMyDocItem[i].Rush.Create(this, rcControl);
		mMyDocItem[i].Rush.SetWndID(BUTTON_ID_RUSHBASE + i);
		mMyDocItem[i].Rush.SetOnClickListener(this);

		mMyDocItem[i].Del.Create(this, rcControl);
		mMyDocItem[i].Del.SetWndID(BUTTON_ID_DELBASE + i);
		mMyDocItem[i].Del.SetOnClickListener(this);

		mMyDocItem[i].Tiao.CreateStatic(this, rcControl);
	}

	mPageInfoWnd.CreateStatic(this, rcControl);

	SetRectXY(&rcControl, 145, 368, 60, 48);
	mPrevPageBtn.Create(this, rcControl);
	mPrevPageBtn.SetWndID(BUTTON_ID_PREV);
	mPrevPageBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 145, 368, 60, 48);
	mNextPageBtn.Create(this, rcControl);
	mNextPageBtn.SetWndID(BUTTON_ID_NEXT);
	mNextPageBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 145, 368, 60, 48);
	mLoginExit.Create(this, rcControl);
	mLoginExit.SetWndID(BUTTON_ID_LOGINEXIT);
	mLoginExit.SetOnClickListener(this);

	LoadResource();
}

void CFavoritSubBoxWnd::LoadResource()
{
#if APP_ID == QIKUN500
	LoadResource_v500();
#elif APP_ID == QIKUN510
	LoadResource_v510();
#elif APP_ID == QIKUN501
	LoadResource_v501();
#elif APP_ID == QIKUN520
	LoadResource_v520();
#endif
}

#if APP_ID == QIKUN500
void CFavoritSubBoxWnd::LoadResource_v500()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cParentName[MAX_PATH] = {0};
	char cFileName[MAX_PATH];
  RECT rcShow;
	RECT rcHide;
	CTexture bkTexture;
	char imgPath[64] = {0};
  CTexture btnTextures[4];
  
	GetXmlBasePath(cParentName, sizeof(cParentName));
	CombinePathName(cFileName, cParentName, "TreasureBox/MyDoc.xml");

	if( !parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

  XmlLoadRect(&parser, "ShowWindowLocation", &rcShow);
	XmlLoadRect(&parser, "HideWindowLocation", &rcHide);
	CParentClass::MoveWindow(rcShow, rcHide);

	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowLocation", "path", "Box/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
  
	// read from xml
	RECT rcControl;
	RECT rcSongName,rcInsert,rcRush,rcDel,rcTiao;
	int i,yOffset= 0, size;
  
	XmlLoadRect(&parser, "MyDocItemLocation", &rcSongName);
  size = parser.GetIntValue("MyDocItemLocation", "size", 16);
	XmlLoadRect(&parser, "InsertLocation", &rcInsert);
	XmlLoadRect(&parser, "RushLocation", &rcRush);
	XmlLoadRect(&parser, "DelLocation", &rcDel);
	XmlLoadRect(&parser, "TiaoLocation", &rcTiao);
	yOffset = parser.GetIntValue("MyDocItemLocation","yOffset",50);
	for(i = 0; i < MYDOCSUBBOXWND_COUNT; i++)
	{
		InflatRect(&rcControl, &rcSongName, 0, i*yOffset);
	  mMyDocItem[i].SongName.SetFontSize(size);
		mMyDocItem[i].SongName.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcInsert, 0, i*yOffset);
		mMyDocItem[i].Insert.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcRush, 0, i*yOffset);
		mMyDocItem[i].Rush.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcDel, 0, i*yOffset);
		mMyDocItem[i].Del.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcTiao, 0, i*yOffset);
		mMyDocItem[i].Tiao.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PageInfoWndLocation", &rcControl);
	mPageInfoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PrevPageBtnLocation", &rcControl);
	mPrevPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NextPageBtnLocation", &rcControl);
	mNextPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LoginExitLocation", &rcControl);
	mLoginExit.MoveWindow(&rcControl);

  //XmlLoadRect(&parser, "IcoWndInfo", &rcControl);
	//mTitleIcoWnd.MoveWindow(&rcControl);
  //SAFE_STRNCPY(imgPath, parser.GetStrValue("IcoWndInfo","Path","Box/ico10"), sizeof(imgPath));
  //CreateImgTexture(imgPath, &bkTexture);
	//mTitleIcoWnd.SetBackgroundTexture(&bkTexture);
  
	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "Box/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mColseBtn.MoveWindow(&rcControl);
	mColseBtn.SetTextures(btnTextures);


	strcpy(imgPath, parser.GetStrValue("PageInfoWndImg","path","MyFavorite/LoginID"));
	CreateImgTexture(imgPath, &bkTexture);
	mPageInfoWnd.SetBackgroundTexture(&bkTexture);

	CTexture btnInsert[4],btnRush[4],btnDel[4];

	strcpy(imgPath, parser.GetStrValue("InsertImg","path","MyFavorite"));
	CreateBtnImgTextures(imgPath, btnInsert);

	strcpy(imgPath, parser.GetStrValue("RushImg","path","MyFavorite"));
	CreateBtnImgTextures(imgPath, btnRush);

	strcpy(imgPath, parser.GetStrValue("DelImg","path","MyFavorite"));
	CreateBtnImgTextures(imgPath, btnDel);

	for ( i = 0 ; i < MYDOCSUBBOXWND_COUNT; i++ )
	{
		mMyDocItem[i].Insert.SetTextures(btnInsert);
		mMyDocItem[i].Rush.SetTextures(btnRush);
		mMyDocItem[i].Del.SetTextures(btnDel);
	}

	strcpy(imgPath, parser.GetStrValue("PrevPageBtnImg","path","MyFavorite/Back"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPrevPageBtn.SetTextures(btnTextures);

	strcpy(imgPath, parser.GetStrValue("NextPageBtnImg","path","MyFavorite/Save"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNextPageBtn.SetTextures(btnTextures);

	strcpy(imgPath, parser.GetStrValue("LoginExitImg","path","MyFavorite/Save"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mLoginExit.SetTextures(btnTextures);

	char colorbuf[16];
	strcpy(colorbuf,parser.GetStrValue("MyDocItemLocation","color","FF0000FF"));
	mSelectColor = strtoul(colorbuf,NULL,16);
}
#elif APP_ID == QIKUN510
void CFavoritSubBoxWnd::LoadResource_v510()
{

}
#elif APP_ID == QIKUN501
void CFavoritSubBoxWnd::LoadResource_v501()
{

}
#elif APP_ID == QIKUN520
void CFavoritSubBoxWnd::LoadResource_v520()
{

}
#endif

void CFavoritSubBoxWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )
		return ;

	int nWndID = pWnd->GetWndID();
	if ( nWndID == BUTTON_ID_PREV )
	{
		if ( mCurPage > 0 )
			mCurPage --;
		else if (mPageCount > 0)
			mCurPage = mPageCount-1;
		UpdateFavoritItem();
	}
	else if ( nWndID == BUTTON_ID_NEXT )
	{
		if ( mCurPage < (mPageCount-1) )
			mCurPage ++;
		else
			mCurPage = 0;
		UpdateFavoritItem();
	}
	else if ( nWndID == BUTTON_ID_LOGINEXIT )
	{
		//ReportMsg(FAVORIT_EXIT, 0, 0);
    gMainCtrlPage->UserLoginExit();
    SlideShowWindow(FALSE);
	}
  else if(nWndID == BUTTON_ID_CLOSE)
  {
    SlideShowWindow(FALSE);
  }
	else if ( nWndID >= BUTTON_ID_INSERTBASE &&
		nWndID <= BUTTON_ID_INSERTBASE + MYDOCSUBBOXWND_COUNT )
	{
		SONGINFO *pSong = (SONGINFO *)pWnd->GetWindowData();
		if ( gPlayerInfo->AddSongToSelectedList(pSong, -1) )
		{
			UpdateFavoritItem();
		}
	}
	else if ( nWndID >= BUTTON_ID_RUSHBASE &&
		nWndID <= BUTTON_ID_RUSHBASE + MYDOCSUBBOXWND_COUNT)
	{
		SONGINFO *pSong = (SONGINFO *)pWnd->GetWindowData();
		if (gPlayerInfo->IsSongInSelectedList(pSong->cID))
			gPlayerInfo->MoveSelectedSongToFirst(pSong->cID);
		else
			gPlayerInfo->AddSongToSelectedList(pSong, 0);
		UpdateFavoritItem();
	}
	else if ( nWndID >= BUTTON_ID_DELBASE &&
		nWndID <= BUTTON_ID_DELBASE+MYDOCSUBBOXWND_COUNT)
	{
		SONGINFO *pSong = (SONGINFO *)pWnd->GetWindowData();
		FastSearch_DelSongFromFavorit(pSong->cID, pSong->cID);
		InitFavoritList();
		UpdateFavoritItem();
	}
  
}

void CFavoritSubBoxWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if(bVisible)
	{
		InitFavoritList();
		mCurPage = 0;
		UpdateFavoritItem();
	}
	else
	{
		FreeFavoritList();
	}
}

void CFavoritSubBoxWnd::SetPageInfo()
{
	char pageInfo[64];

	sprintf(pageInfo,"%d/%d",mCurPage+1,mPageCount);
	mPageInfoWnd.SetWindowTextA(pageInfo);
}

void CFavoritSubBoxWnd::UpdateFavoritItem()
{
	int nFirstIndex = mCurPage * MYDOCSUBBOXWND_COUNT;
	int i;
	CBaseStringW sSongInfo;
	for ( i = 0 ; i < MYDOCSUBBOXWND_COUNT ; i++ )
	{
		SONGINFO *pSong = (SONGINFO *)mFavoritList.GetAt(i+nFirstIndex);
		if (pSong)
		{
			mMyDocItem[i].SongName.SetWindowData((UINT64)pSong);
			mMyDocItem[i].Insert.SetWindowData((UINT64)pSong);
			mMyDocItem[i].Rush.SetWindowData((UINT64)pSong);
			mMyDocItem[i].Del.SetWindowData((UINT64)pSong);
			sSongInfo.Format("%d.%s",i+nFirstIndex+1,pSong->cName);
			mMyDocItem[i].SongName.SetWindowTextW(sSongInfo.GetString());

			if (gPlayerInfo->IsSongInSelectedList(pSong->cID))
			{
				mMyDocItem[i].SongName.SetFontColor(mSelectColor);
			}
			else
			{
				mMyDocItem[i].SongName.SetFontColor(0xFFFFFFFF);
			}

		}
		else
		{
			mMyDocItem[i].SongName.SetWindowData(0);
			mMyDocItem[i].Insert.SetWindowData(0);
			mMyDocItem[i].Rush.SetWindowData(0);
			mMyDocItem[i].Del.SetWindowData(0);
			mMyDocItem[i].SongName.SetWindowTextA(NULL);
		}
	}

	SetPageInfo();
}

void CFavoritSubBoxWnd::AddFavoritItem(SONGINFO *pSingerInfo)
{
	SONGINFO *pNewSongInfo = new SONGINFO;
	if (!pNewSongInfo)
	{
		return;
	}
	memset(pNewSongInfo, 0, sizeof(SONGINFO));
	memcpy(pNewSongInfo, pSingerInfo, sizeof(SONGINFO));
	//pNewSongInfo->nLyricsLen = 0;
	//pNewSongInfo->pLyrics = 0;

	mFavoritList.AddData(pNewSongInfo);
	mPageCount = (mFavoritList.GetCount()+MYDOCSUBBOXWND_COUNT-1) / MYDOCSUBBOXWND_COUNT;
}

void CFavoritSubBoxWnd::InitFavoritList()
{
	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};

	FreeFavoritList();

	CPtrArrayCtrl sSongList;
	FastSearch_SearchSongListByFavorit(
		&searchinfo,
		&sSongList);

	int nSongCount = sSongList.GetCount();
	int i;

	for ( i = 0 ; i < nSongCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)sSongList.GetAt(i);
		if(pSongInfo != NULL)
			AddFavoritItem(pSongInfo);
	}
}

void CFavoritSubBoxWnd::FreeFavoritList()
{
	DelNormalList(&mFavoritList, SONGINFO);
}

