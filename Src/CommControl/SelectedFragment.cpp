#include "SelectedFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../SQL/SearchSQL.h"
#include "../GlobalUIClass.h"

#define BTN_RETURN_ID			0x01
#define BTN_PREVPAGE_ID		0x02
#define BTN_NEXTPAGE_ID		0x03
#define BTN_SONGSTATE_ID		0x04

#define BTN_RUSH_BASE_ID	0x20
#define BTN_DELETE_BASE_ID	0x30

CSelectedFragment::CSelectedFragment()
{
	mpPressedWnd = NULL;
	mFirstShowItem = 0;
	miFirstItemIdx = 0;
	miPageCount = 1;
	miSelectedCount = 0;
	mCurrentTab = selectedTab_Selected;
}

CSelectedFragment::~CSelectedFragment()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CSelectedFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(
		"SubSingerFragment",
		pParent,
		rc,
		WINDOWFLAG_DEFAULT,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

  mPlaySongBtn.Create(this, rcControl, BUTTONTYPE_RADIO, false, PRESSED_SCALE);
	mPlaySongBtn.SetWndID(BTN_SONGSTATE_ID);
	mPlaySongBtn.SetOnClickListener(this);
  
	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	//mTabBKGroundWnd.CreateStatic(this, rcControl);
	//mTabImgWnd.CreateStatic(this, rcControl);
	//mTabImgWnd.SetDrawCircle(TRUE);

	for ( int i = 0; i < itemCount; i++ )
	{
		mSelectedItems[i].itemBKWnd.CreateStatic(this, rcControl);

		mSelectedItems[i].songNameWnd.CreateStatic(&mSelectedItems[i].itemBKWnd, rcControl);
		mSelectedItems[i].songNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
		//mSelectedItems[i].songNameWnd.SetWindowTextA("测试歌曲德尔福");
		mSelectedItems[i].singerWnd.CreateStatic(&mSelectedItems[i].itemBKWnd, rcControl);
		mSelectedItems[i].singerWnd.SetFontDirect(DRAWTEXT_DIRECT_RIGHT|DRAWTEXT_DIRECT_VCENTER);
		//mSelectedItems[i].singerWnd.SetWindowTextA("测试歌星");

		mSelectedItems[i].rushBtn.CreateStatic(&mSelectedItems[i].itemBKWnd, rcControl);
		mSelectedItems[i].rushBtn.SetWndID(BTN_RUSH_BASE_ID+i);
		mSelectedItems[i].rushBtn.SetOnClickListener(this);

		mSelectedItems[i].deleteBtn.CreateStatic(&mSelectedItems[i].itemBKWnd, rcControl);
		mSelectedItems[i].deleteBtn.SetWndID(BTN_DELETE_BASE_ID+i);
		mSelectedItems[i].deleteBtn.SetOnClickListener(this);

		mSelectedItems[i].selecteItemWnd.CreateStatic(&mSelectedItems[i].itemBKWnd, rcControl);
		mSelectedItems[i].selecteItemWnd.SetOnTouchListener(this);
	}

	mPrevPageBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
	mPrevPageBtn.SetWndID(BTN_PREVPAGE_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
	mNextPageBtn.SetWndID(BTN_NEXTPAGE_ID);
	mNextPageBtn.SetOnClickListener(this);

	mPageInfoWnd.CreateStatic(this, rcControl);

	LoadResource();
}

void CSelectedFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SelectedFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	CTexture bkTexture, btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	unsigned long colorSong, colorSinger;
	int sizeSong, sizeSinger;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("rcLocation", "path", "BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "PlaySongBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PlaySongBtnInfo", "path", "SubSingerList/PlaySongBtnInfo"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPlaySongBtn.SetTextures(btnTextures);
	mPlaySongBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PrevPageBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PrevPageBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPrevPageBtn.SetTextures(btnTextures);
	mPrevPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NextPageBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NextPageBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNextPageBtn.SetTextures(btnTextures);
	mNextPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PageInfoWndInfo", &rcControl);
	mPageInfoWnd.MoveWindow(&rcControl);

	//XmlLoadRect(&parser, "TabBKGroundWndInfo", &rcControl);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("TabBKGroundWndInfo", "path", "SubSingerList/SingerImgBK"), sizeof(imgPath));
	//CreateImgTexture(imgPath, &bkTexture);
	//mTabBKGroundWnd.SetBackgroundTexture(&bkTexture);
	//mTabBKGroundWnd.MoveWindow(&rcControl);

	//XmlLoadRect(&parser, "TabImgWndInfo", &rcControl);
	//mTabImgWnd.MoveWindow(&rcControl);

	CTexture btnRushTextures[4], btnDeleteTextures[4];
	RECT rcSelectedBK, rcSong, rcSinger, rcRush, rcDelete, rcSelected;
	int i = 0, yOffset = 50;

	XmlLoadRect(&parser, "SelectedItemBKWndInfo", &rcSelectedBK);
	yOffset = parser.GetIntValue("SelectedItemBKWndInfo", "yOffset", 80);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("SelectedItemBKWndInfo", "path", "SubSingerList/SelectedBK"), sizeof(imgPath));
	//CreateImgTexture(imgPath, &bkTexture);

	XmlLoadRect(&parser, "SongNameWndInfo", &rcSong);
	sizeSong = parser.GetIntValue("SongNameWndInfo", "size", 30);
	strcpy(colorbuf,parser.GetStrValue("SongNameWndInfo", "color", "FFFFFFFF"));
	colorSong = strtoul(colorbuf, NULL,16);

	XmlLoadRect(&parser, "SingerWndInfo", &rcSinger);
	sizeSinger = parser.GetIntValue("SingerWndInfo", "size", 30);
	strcpy(colorbuf,parser.GetStrValue("SingerWndInfo", "color", "FFFFFFFF"));
	colorSinger = strtoul(colorbuf, NULL,16);

	XmlLoadRect(&parser, "RushBtnInfo", &rcRush);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("RushBtnInfo", "path", "SubSingerList/PrevPage"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnRushTextures);

	XmlLoadRect(&parser, "DeleteBtnInfo", &rcDelete);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("DeleteBtnInfo", "path", "SubSingerList/PrevPage"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnDeleteTextures);

	XmlLoadRect(&parser, "SelectedInfo", &rcSelected);

	for ( i = 0; i < itemCount; i++ )
	{
		InflatRect(&rcControl, &rcSelectedBK, 0, i*yOffset);
		//mSelectedItems[i].itemBKWnd.SetBackgroundTexture(&bkTexture);
		mSelectedItems[i].itemBKWnd.MoveWindow(&rcControl);

		mSelectedItems[i].songNameWnd.MoveWindow(&rcSong);
		mSelectedItems[i].songNameWnd.SetFontSize(sizeSong);
		mSelectedItems[i].songNameWnd.SetFontColor(colorSong);

		mSelectedItems[i].singerWnd.MoveWindow(&rcSinger);
		mSelectedItems[i].singerWnd.SetFontSize(sizeSinger);
		mSelectedItems[i].singerWnd.SetFontColor(colorSinger);

		mSelectedItems[i].rushBtn.SetTextures(btnRushTextures);
		mSelectedItems[i].rushBtn.MoveWindow(&rcRush);

		mSelectedItems[i].deleteBtn.SetTextures(btnDeleteTextures);
		mSelectedItems[i].deleteBtn.MoveWindow(&rcDelete);

		mSelectedItems[i].selecteItemWnd.MoveWindow(&rcSelected);
	}
}

void CSelectedFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_RETURN_ID )
	{
		gMainCtrlPage->ReturnToLastFragment();
	}
	else if ( nWndID == BTN_PREVPAGE_ID )
	{
		int curPage = miFirstItemIdx / itemCount;
		if ( curPage > 0 )
		{
			miFirstItemIdx = (curPage-1) * itemCount;
			UpdatePageInfo();
			UpdateItemList();
		}
	}
	else if ( nWndID == BTN_NEXTPAGE_ID )
	{
		int curPage = miFirstItemIdx / itemCount;
		if ( curPage < (miPageCount - 1) )
		{
			miFirstItemIdx = (curPage+1) * itemCount;
			UpdatePageInfo();
			UpdateItemList();
		}
	}
	else if ( nWndID >= BTN_RUSH_BASE_ID && nWndID < (BTN_RUSH_BASE_ID + itemCount) )
	{
		int idx = nWndID - BTN_RUSH_BASE_ID;
		OnRushBtnClick(miFirstItemIdx+idx);
	}
	else if ( nWndID >= BTN_DELETE_BASE_ID && nWndID < (BTN_DELETE_BASE_ID + itemCount) )
	{
		int idx = nWndID - BTN_DELETE_BASE_ID;
		OnDelBtnClick(miFirstItemIdx+idx);
	}
  else if(nWndID == BTN_SONGSTATE_ID)
  {
    if ( mCurrentTab == selectedTab_Selected )
    {
		  gMainCtrlPage->mSelectedFragment.SetSelectedType(selectedTab_Singed);
    }
    else
    {
		  gMainCtrlPage->mSelectedFragment.SetSelectedType(selectedTab_Selected);
    }
  }

}

void CSelectedFragment::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	switch(uType)
	{
	case PLAYERINFO_SONGSTATE_CHANGE:
	case PLAYERINFO_LISTSTATE_CHANGE:
		SetSelectedType(mCurrentTab);
		break;

	default:
		break;
	}
}

void CSelectedFragment::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	mFirstShowItem = miFirstItemIdx;

	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	pWnd->SetCapture();
}

void CSelectedFragment::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CSelectedFragment::OnTouchMove(CBaseWnd *pWnd, POINT pt)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

	POINT ptDrag;
	pWnd->ConvertWndPointToScreenPoint(&ptDrag, pt);
	ConvertScreenPointToWndPoint(&ptDrag, ptDrag);

	int yDragLen = ptDrag.y-mPtPressedScreen.y;
	int nMoveItems = yDragLen / RECTHEIGHT(mSelectedItems[0].itemBKWnd.mWndRect);

	miFirstItemIdx = mFirstShowItem - nMoveItems;
	if ( miFirstItemIdx < 0 )
	{
		miFirstItemIdx = 0;
		return ;
	}
	else if ( miFirstItemIdx >= miSelectedCount )
	{
		miFirstItemIdx = miSelectedCount;
		return;
	}

	UpdateItemList();
	UpdatePageInfo();
}

void CSelectedFragment::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CSelectedFragment::SetSelectedType(SELECTED_TAB_TYPE_S type)
{
	RefreshPlayingSinger();

	mCurrentTab = type;

	miFirstItemIdx = 0;
	if ( type == selectedTab_Selected )
	{
		miSelectedCount = gPlayerInfo->GetSelectedSongCount();
    mPlaySongBtn.SetRadioBtnPressed(FALSE);
	}
	else if ( type == selectedTab_Singed )
		miSelectedCount = gPlayerInfo->GetSingedSongCount();
	miPageCount = (miSelectedCount+itemCount-1) / itemCount;

	UpdateItemList();
	UpdatePageInfo();
}

void CSelectedFragment::UpdatePageInfo(void)
{
	char tmpPage[128] = {0};

	snprintf(tmpPage, sizeof(tmpPage), "%d/%d", miFirstItemIdx/itemCount+1, miPageCount);
	mPageInfoWnd.SetWindowTextA(tmpPage);
}

void CSelectedFragment::UpdateItemList()
{
	SONGINFO *pSongInfo = NULL;

	if ( mCurrentTab == selectedTab_Singed )
	{
		for ( int i = 0; i < itemCount; i++ )
		{
			pSongInfo =  gPlayerInfo->GetSingedSongInfoByIndex(i + miFirstItemIdx);
			SetSongInfo(i, pSongInfo);
		}
	}
	else if ( mCurrentTab == selectedTab_Selected )
	{
		for ( int i = 0 ; i < itemCount; i++ )
		{
			pSongInfo =  gPlayerInfo->GetSelectedSongInfoByIndex(i +miFirstItemIdx);
			SetSongInfo(i, pSongInfo);
		}
	}
}

void CSelectedFragment::SetSongInfo(int nIndex, const SONGINFO *pSongInfo)
{
	CBaseStringA sSongInfo;
	int nCuridx = nIndex % itemCount;

	if ( pSongInfo )
	{
		mSelectedItems[nCuridx].itemBKWnd.SetWindowVisible(TRUE);
		sSongInfo.Format("%s", pSongInfo->cName);
		mSelectedItems[nCuridx].songNameWnd.SetWindowTextA(sSongInfo.GetString());
		char *singer = FastSearch_GetSingerByID(pSongInfo->singerID);
		mSelectedItems[nCuridx].singerWnd.SetWindowTextA(singer ? singer : "");
		mSelectedItems[nCuridx].deleteBtn.SetWindowVisible(mCurrentTab == selectedTab_Selected ? TRUE : FALSE);
	}
	else
	{
		mSelectedItems[nCuridx].itemBKWnd.SetWindowVisible(FALSE);
	}
}

void CSelectedFragment::OnRushBtnClick(int nIndex)
{
	if ( mCurrentTab == selectedTab_Selected )
	{
		SONGINFO *pSongInfo = gPlayerInfo->GetSelectedSongInfoByIndex(nIndex);
		if ( pSongInfo )
			gPlayerInfo->MoveSelectedSongToFirst(nIndex);
	}
	else if ( mCurrentTab == selectedTab_Singed )
	{
		SONGINFO *pSongInfo = gPlayerInfo->GetSingedSongInfoByIndex(nIndex);
		if ( pSongInfo )
		{
			gPlayerInfo->MoveSingedListItem(nIndex, -1);
    
      gPlayerInfo->AddSongToSelectedList(pSongInfo, 0);
      if (gPlayerInfo->IsSongInSelectedList(pSongInfo->cID))
  		{
  			gPlayerInfo->MoveSelectedSongToFirst(pSongInfo->cID);
  		}
		}
	}
	UpdateItemList();
}

void CSelectedFragment::OnDelBtnClick(int nIndex)
{
	if ( mCurrentTab == selectedTab_Selected )
	{
		gPlayerInfo->DelSongFromSelectedList(nIndex);
	}
	else if ( mCurrentTab == selectedTab_Singed )
	{
		gPlayerInfo->DelSongFromSingedList(nIndex);
	}
	UpdateItemList();
}

void CSelectedFragment::RefreshPlayingSinger(void)
{
	const char *cSingerPath = theBaseApp->GetSingerTextureFolder();
	char cTemp[MAX_PATH];
	memset(cTemp,0,MAX_PATH);
	char cImgPath[MAX_PATH];

	SONGINFO *pSongInfo = gPlayerInfo->GetPlayingSongInfo();
	if ( !pSongInfo ) return ;

	SINGERINFO *pSinger = FastSearch_GetSingerInfoByID(&pSongInfo->singerID);
	if ( !pSinger ) return ;

	sprintf(cImgPath,"%d.jpg",pSinger->id);
	CombinePathName(cTemp, cSingerPath, cImgPath);
	if ( cTemp && strlen(cTemp) > 0 )
	{
		if ( !IsFileExist(cTemp) )
		{
			sprintf(cTemp, "%s/0.png", cSingerPath);
		}

		CTexture texture;
		texture.CreateFromImgFile(cTemp);
		mTabImgWnd.SetBackgroundTexture(&texture);
	}
}
