#include "HomePageFragment.h"
#include "XMLParser.h"
#include "ImgFileOp.h"
#include "../GlobalUIClass.h"

#define BTN_ITEM_BASE_ID	0x01
#define BTN_PREVABLUM_ID	0x30
#define BTN_NEXTABLUM_ID		0x31
#define ITEM_PRESSED_SCALE	0.9f
#define START_UPDATE_PICTURE_TIME			    0x01		
#define START_UPDATE_PICTURE_TIMEOUT			20000		// 20S

//#define ITEM_PRESSED_SCALE	1


CHomePageFragment::CHomePageFragment()
{
	mpPressedWnd = NULL;
	miXoffset = 0;
	miCurAlbumIdx = 0;
}

CHomePageFragment::~CHomePageFragment()
{
	DbgOutput(DBG_DEBUG, "function %s quit\n", __FUNCTION__);
}

void CHomePageFragment::Create(CBaseWnd *pParent)
{
	RECT rcListCtrl = {50, 100, 1530, 500};
	CParentClass::Create(
		"HomePageFragment",
		pParent,
		rcListCtrl,
		WINDOWFLAG_DEFAULT,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

	mHomeItemBKWnd.CreateStatic(this, rcControl);

	for ( int i = 0; i < selectType_count; ++i )
	{
		mHomePageItem[i].Create(&mHomeItemBKWnd, rcControl, BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
		mHomePageItem[i].SetWndID(BTN_ITEM_BASE_ID + i);
		mHomePageItem[i].SetOnTouchListener(this);
		mHomePageItem[i].SetOnClickListener(this);
	}
	/*
	mPrevAlbumBtn.Create(&mHomePageItem[selectType_album], rcControl,BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
	mPrevAlbumBtn.SetWndID(BTN_PREVABLUM_ID);
  mPrevAlbumBtn.SetOnTouchListener(this);
	mPrevAlbumBtn.SetOnClickListener(this);

	mNextAlbumBtn.Create(&mHomePageItem[selectType_album], rcControl,BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
	mNextAlbumBtn.SetWndID(BTN_NEXTABLUM_ID);
  mNextAlbumBtn.SetOnTouchListener(this);
	mNextAlbumBtn.SetOnClickListener(this);
	*/
	LoadResource();
}

void CHomePageFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;
	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "HomePageFragment.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture btnTextures[4];

	XmlLoadRect(&parser, "ListCtrlLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "HomeItemsBkWndInfo", &mrcOrigItemsBKShow);
	/*
	XmlLoadRect(&parser, "AlbumBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("AlbumBtnInfo", "path", "HomePage/Album"), sizeof(imgPath));
	mHomePageItem[selectType_album].MoveWindow(&rcControl);
	CreateBtnImgTextures(imgPath, mAlbumTextures[0]);
	*/
	XmlLoadRect(&parser, "RankBtnInfo1", &rcControl);
	
	SAFE_STRNCPY(imgPath, parser.GetStrValue("RankBtnInfo1", "path", "HomePage/Rank"), sizeof(imgPath));
	//CreateBtnImgTextures(imgPath, btnTextures);
	CreateBtnImgTextures(imgPath, mAlbumTextures[0]);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("RankBtnInfo2", "path", "HomePage/Rank1"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, mAlbumTextures[1]);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("RankBtnInfo3", "path", "HomePage/Rank2"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, mAlbumTextures[2]);
	
	mHomePageItem[selectType_rank].SetTextures(mAlbumTextures[0]);
	mHomePageItem[selectType_rank].MoveWindow(&rcControl);
	
  XmlLoadRect(&parser, "ShopBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShopBtnInfo", "path", "HomePage/Shop"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_Shop].SetTextures(btnTextures);
	mHomePageItem[selectType_Shop].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "FoodBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("FoodBtnInfo", "path", "HomePage/Food"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_Food].SetTextures(btnTextures);
	mHomePageItem[selectType_Food].MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "SingerBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SingerBtnInfo", "path", "HomePage/Singer"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_singer].SetTextures(btnTextures);
	mHomePageItem[selectType_singer].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SongNameBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SongNameBtnInfo", "path", "HomePage/SongName"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_songName].SetTextures(btnTextures);
	mHomePageItem[selectType_songName].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ClassifyBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ClassifyBtnInfo", "path", "HomePage/classify"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_classify].SetTextures(btnTextures);
	mHomePageItem[selectType_classify].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "NewSongsBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NewSongsBtnInfo", "path", "HomePage/NewSongs"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_NewSongs].MoveWindow(&rcControl);
	mHomePageItem[selectType_NewSongs].SetTextures(btnTextures);

  XmlLoadRect(&parser, "MovieBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("MovieBtnInfo", "path", "HomePage/Movie"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHomePageItem[selectType_movie].MoveWindow(&rcControl);
	mHomePageItem[selectType_movie].SetTextures(btnTextures);

  
	//mrcOrigItemsBKShow.right = rcControl.right;
	miXoffset = 0;
	mHomeItemBKWnd.MoveWindow(&mrcOrigItemsBKShow);

  #if 0
	XmlLoadRect(&parser, "PrevAlbumBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PrevAlbumBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPrevAlbumBtn.SetTextures(btnTextures);
	mPrevAlbumBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NextAlbumBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NextAlbumBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNextAlbumBtn.SetTextures(btnTextures);
	mNextAlbumBtn.MoveWindow(&rcControl);
  #endif

	//SetAlbumPicByIdx(miCurAlbumIdx);
  
  	AddTimer(START_UPDATE_PICTURE_TIME, START_UPDATE_PICTURE_TIMEOUT);
}

void CHomePageFragment::OnDestroy()
{
	CParentClass::OnDestroy();
}

void CHomePageFragment::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
		mpPressedWnd = NULL;
}

void CHomePageFragment::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	miXoffset = mHomeItemBKWnd.mRectRelativeToParent.left;

	pWnd->SetCapture();
}

void CHomePageFragment::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, 	UINT64 uUsedTime)
{
	if ( pWnd != mpPressedWnd )
		return;

	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CHomePageFragment::OnTouchMove(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd != mpPressedWnd )
	{
		return;
	}

	if ( RECTWIDTH(mrcOrigItemsBKShow) <= RECTWIDTH(mRectRelativeToParent) )
		return ;

	POINT ptDrag;
	pWnd->ConvertWndPointToScreenPoint(&ptDrag, pt);
	ConvertScreenPointToWndPoint(&ptDrag, ptDrag);

	int xDragLen = ptDrag.x-mPtPressedScreen.x;
	int dstLeft = miXoffset + xDragLen;
	RECT rcCurBK;
	rcCurBK.left = mrcOrigItemsBKShow.left + dstLeft;
	rcCurBK.right = rcCurBK.left + RECTWIDTH(mrcOrigItemsBKShow);
	rcCurBK.top = mrcOrigItemsBKShow.top;
	rcCurBK.bottom = mrcOrigItemsBKShow.bottom;

	if ( rcCurBK.left > mrcOrigItemsBKShow.left )
	{
		rcCurBK.left = mrcOrigItemsBKShow.left;
		rcCurBK.right = mrcOrigItemsBKShow.right;
	}
	else if ( rcCurBK.right <= RECTWIDTH(mRectRelativeToParent) )
	{
		rcCurBK.right = RECTWIDTH(mRectRelativeToParent);
		rcCurBK.left = rcCurBK.right - RECTWIDTH(mrcOrigItemsBKShow);
	}

	mHomeItemBKWnd.MoveWindow(&rcCurBK);
}

void CHomePageFragment::OnTouchDrag(CBaseWnd *pWnd, int xDragLen, int yDragLen)
{
}

void CHomePageFragment::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
	ReleaseCapture();

	mpPressedWnd = NULL;
}
void CHomePageFragment::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
	if ( nTimerID == START_UPDATE_PICTURE_TIME )
	{
    if(miCurAlbumIdx < (album_count-1))
    {
			miCurAlbumIdx++;
    }
    else
    {
      miCurAlbumIdx = 0;
    }
    DbgOutput(DBG_ERR, "OnTimer  <%d>!!\n", miCurAlbumIdx);
    
		SetAlbumPicByIdx(miCurAlbumIdx);
	}
}

void CHomePageFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_PREVABLUM_ID )
	{
		if ( miCurAlbumIdx > 0 )
		{
			miCurAlbumIdx--;
			SetAlbumPicByIdx(miCurAlbumIdx);
		}
	}
	else if ( nWndID == BTN_NEXTABLUM_ID )
	{
		if ( miCurAlbumIdx < (album_count-1) )
		{
			miCurAlbumIdx++;
			SetAlbumPicByIdx(miCurAlbumIdx);
		}
	}
	else if ( nWndID >= BTN_ITEM_BASE_ID && (nWndID < BTN_ITEM_BASE_ID+selectType_count) )
	{
		int nClickID = nWndID - BTN_ITEM_BASE_ID;
		OnClickItem((HOMEPAGESELECTTYPE_E) nClickID);
	}
}

void CHomePageFragment::OnClickItem(HOMEPAGESELECTTYPE_E type)
{
	switch ( type )
	{
    
  case selectType_Shop:
    DbgOutput(DBG_ERR, "selectType_Food\n");
    gMainCtrlPage->mShopWnd.SlideShowWindow(TRUE);
		break;
    
  case selectType_Food:
    DbgOutput(DBG_ERR, "selectType_Shop\n");
    gMainCtrlPage->mFoodWnd.SlideShowWindow(TRUE);
  
		break;
    
	case selectType_singer:
		gSingerListFragment->SetSearchBarItem(&gSearchSingerType, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SingerList);
		break;
	case selectType_songName:
		gSongListFragment->SetSearchBarItem(&gSearchLanguageType, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
		break;
	
	case selectType_classify:
		gMainCtrlPage->SetCurrentFragment(Fragment_AlbumSong);
		break;
  	/*
	case selectType_Grade:
   {
  		static SEARCHITEM search;
  		search.eType = SearchBYGradeSong;
  		search.nDepth = 0;
  		search.pParentItem = NULL;
  		SAFE_STRNCPY(search.cItemName, "评分", sizeof(search.cItemName));
  		gSongListFragment->SetSearchBarItem(&search, 0);
  		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
  	}
  break;
  */
	case selectType_rank:
	{
		static SEARCHITEM search;
		search.eType = SearchByRank;
		search.nDepth = 0;
		search.pParentItem = NULL;
		SAFE_STRNCPY(search.cItemName, "热榜", sizeof(search.cItemName));
		gSongListFragment->SetSearchBarItem(&search, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
	}
		break;
  /*
  case selectType_Child:
   if ( gMainCtrlPage )
    gMainCtrlPage->mAlbumSongFragment.ShowChidlSong();
    break;
   
  case selectType_Birthday:
   if ( gMainCtrlPage )
    gMainCtrlPage->mAlbumSongFragment.ShowBirthdaySong();
    break;
   */ 
  case selectType_NewSongs:
    {
    static SEARCHITEM search;
		search.eType = SearchByNewSong;
		search.nDepth = 0;
		search.pParentItem = NULL;
		SAFE_STRNCPY(search.cItemName, "新歌", sizeof(search.cItemName));
		gSongListFragment->SetSearchBarItem(&search, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
    }
    break;
    case selectType_movie:
    {
		static SEARCHITEM search;
		search.eType = SearchBySongType;
		search.nDepth = 248;
		search.pParentItem = NULL;
		SAFE_STRNCPY(search.cItemName, "影视", sizeof(search.cItemName));
		gSongListFragment->SetSearchBarItem(&search, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
    }
		break;
	/*
	case selectType_UDisk:
   if ( gMainCtrlPage )
		gMainCtrlPage->SetCurrentFragment(Fragment_UDisk);
    break;
   
	case selectType_MySongs:
    gMainCtrlPage->AddToFavorit(NULL);
    break;
  
	case selectType_Cloud:
    if ( gMainCtrlPage->GetCurFragmentType() != Fragment_Download )
				gMainCtrlPage->SetCurrentFragment(Fragment_Download);
    break;
    */
	default:
		break;
	}
}

void CHomePageFragment::SetAlbumPicByIdx(int idx)
{
	if ( idx >= 0 && idx < album_count )
		mHomePageItem[selectType_rank].SetTextures(mAlbumTextures[idx]);
}
