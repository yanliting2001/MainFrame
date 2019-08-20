#include "AlbumSongFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../SQL/SearchSQL.h"
#include "../GlobalUIClass.h"

#define BTN_ITEM_BASE_ID		0x02

#define BTN_ITEM_SHADOW_ID		0x0C
#define BTN_ITEM_RETURN_ID		0x0D


#define ITEM_PRESSED_SCALE	0.9f

CAlbumSongFragment::CAlbumSongFragment()
{
	mpPressedWnd = NULL;
	miXoffset = 0;
}

CAlbumSongFragment::~CAlbumSongFragment()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CAlbumSongFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(	"RankSongFragment", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

	mItemListBKWnd.CreateStatic(this, rcControl);

	for ( int i = 0; i < albumType_count; ++i )
	{
		mItemsBtn[i].Create(&mItemListBKWnd, rcControl, BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
		mItemsBtn[i].SetWndID(BTN_ITEM_BASE_ID + i);
		mItemsBtn[i].SetOnTouchListener(this);
		mItemsBtn[i].SetOnClickListener(this);
	}
  
  mReturnBtn.Create(&mItemListBKWnd, rcControl, BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
  mReturnBtn.SetWndID(BTN_ITEM_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);
  
	LoadResource();
}

void CAlbumSongFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "AlbumSongFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl ;
	CTexture btnTextures[4];
	char imgPath[64] = {0};

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ItemsBkWndInfo", &mrcOrigItemsBKShow);

	XmlLoadRect(&parser, "zhongGXGeSBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("zhongGXGeSBtnInfo", "path", "LangType/Minnan"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_zhongGXGeS].SetTextures(btnTextures);
	mItemsBtn[albumType_zhongGXGeS].MoveWindow(&rcControl);
	mAlbumsID[albumType_zhongGXGeS].id = parser.GetIntValue("zhongGXGeSBtnInfo", "typeID", 20);
	SAFE_STRNCPY(mAlbumsID[albumType_zhongGXGeS].typeName, parser.GetStrValue("zhongGXGeSBtnInfo", "typeName", "中国好声音"), sizeof(mAlbumsID[albumType_zhongGXGeS].typeName));

	XmlLoadRect(&parser, "geMingBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("geMingBtnInfo", "path", "LangType/JapanKorea"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_geMing].SetTextures(btnTextures);
	mItemsBtn[albumType_geMing].MoveWindow(&rcControl);
	mAlbumsID[albumType_geMing].id = parser.GetIntValue("geMingBtnInfo", "typeID", 22);
	SAFE_STRNCPY(mAlbumsID[albumType_geMing].typeName, parser.GetStrValue("geMingBtnInfo", "typeName", "革命"), sizeof(mAlbumsID[albumType_geMing].typeName));

	XmlLoadRect(&parser, "xiQingBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("xiQingBtnInfo", "path", "LangType/AllLang"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_xiQing].SetTextures(btnTextures);
	mItemsBtn[albumType_xiQing].MoveWindow(&rcControl);
	mAlbumsID[albumType_xiQing].id = parser.GetIntValue("xiQingBtnInfo", "typeID", 0);
	SAFE_STRNCPY(mAlbumsID[albumType_xiQing].typeName, parser.GetStrValue("xiQingBtnInfo", "typeName", "喜庆"), sizeof(mAlbumsID[albumType_xiQing].typeName));

	XmlLoadRect(&parser, "wuQuBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("wuQuBtnInfo", "path", "LangType/Other"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_wuQu].SetTextures(btnTextures);
	mItemsBtn[albumType_wuQu].MoveWindow(&rcControl);
	mAlbumsID[albumType_wuQu].id = parser.GetIntValue("wuQuBtnInfo", "typeID", 24);
	SAFE_STRNCPY(mAlbumsID[albumType_wuQu].typeName, parser.GetStrValue("wuQuBtnInfo", "typeName", "舞曲"), sizeof(mAlbumsID[albumType_wuQu].typeName));

	XmlLoadRect(&parser, "woShiGeShouBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("woShiGeShouBtnInfo", "path", "LangType/Yueyu"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_woShiGeShou].SetTextures(btnTextures);
	mItemsBtn[albumType_woShiGeShou].MoveWindow(&rcControl);
	mAlbumsID[albumType_woShiGeShou].id = parser.GetIntValue("woShiGeShouBtnInfo", "typeID", 19);
	SAFE_STRNCPY(mAlbumsID[albumType_woShiGeShou].typeName, parser.GetStrValue("woShiGeShouBtnInfo", "typeName", "我是歌手"), sizeof(mAlbumsID[albumType_woShiGeShou].typeName));

	XmlLoadRect(&parser, "erTongBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("erTongBtnInfo", "path", "LangType/English"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_erTong].SetTextures(btnTextures);
	mItemsBtn[albumType_erTong].MoveWindow(&rcControl);
	mAlbumsID[albumType_erTong].id = parser.GetIntValue("erTongBtnInfo", "typeID", 21);
	SAFE_STRNCPY(mAlbumsID[albumType_erTong].typeName, parser.GetStrValue("erTongBtnInfo", "typeName", "儿歌"), sizeof(mAlbumsID[albumType_erTong].typeName));

	XmlLoadRect(&parser, "guangChangBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("guangChangBtnInfo", "path", "LangType/AllLang"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_guangChang].SetTextures(btnTextures);
	mItemsBtn[albumType_guangChang].MoveWindow(&rcControl);
	mAlbumsID[albumType_guangChang].id = parser.GetIntValue("guangChangBtnInfo", "typeID", 0);
	SAFE_STRNCPY(mAlbumsID[albumType_guangChang].typeName, parser.GetStrValue("guangChangBtnInfo", "typeName", "广场"), sizeof(mAlbumsID[albumType_guangChang].typeName));

	XmlLoadRect(&parser, "xiQuBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("xiQuBtnInfo", "path", "LangType/Yueyu"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_xiQu].SetTextures(btnTextures);
	mItemsBtn[albumType_xiQu].MoveWindow(&rcControl);
	mAlbumsID[albumType_xiQu].id = parser.GetIntValue("xiQuBtnInfo", "typeID", 19);
	SAFE_STRNCPY(mAlbumsID[albumType_xiQu].typeName, parser.GetStrValue("xiQuBtnInfo", "typeName", "戏曲"), sizeof(mAlbumsID[albumType_xiQu].typeName));

	XmlLoadRect(&parser, "mengMianCJBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("mengMianCJBtnInfo", "path", "LangType/AllLang"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_mengMianCJ].SetTextures(btnTextures);
	mItemsBtn[albumType_mengMianCJ].MoveWindow(&rcControl);
	mAlbumsID[albumType_mengMianCJ].id = parser.GetIntValue("mengMianCJBtnInfo", "typeID", 0);
	SAFE_STRNCPY(mAlbumsID[albumType_mengMianCJ].typeName, parser.GetStrValue("mengMianCJBtnInfo", "typeName", "蒙面歌王"), sizeof(mAlbumsID[albumType_mengMianCJ].typeName));

	XmlLoadRect(&parser, "heChangBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("heChangBtnInfo", "path", "LangType/Minnan"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_heChang].SetTextures(btnTextures);
	mItemsBtn[albumType_heChang].MoveWindow(&rcControl);
	mAlbumsID[albumType_heChang].id = parser.GetIntValue("heChangBtnInfo", "typeID", 20);
	SAFE_STRNCPY(mAlbumsID[albumType_heChang].typeName, parser.GetStrValue("heChangBtnInfo", "typeName", "合唱"), sizeof(mAlbumsID[albumType_heChang].typeName));

	XmlLoadRect(&parser, "caoYuanBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("caoYuanBtnInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_caoYuan].SetTextures(btnTextures);
	mItemsBtn[albumType_caoYuan].MoveWindow(&rcControl);
	mAlbumsID[albumType_caoYuan].id = parser.GetIntValue("caoYuanBtnInfo", "typeID", 18);
	SAFE_STRNCPY(mAlbumsID[albumType_caoYuan].typeName, parser.GetStrValue("caoYuanBtnInfo", "typeName", "草原"), sizeof(mAlbumsID[albumType_caoYuan].typeName));

	XmlLoadRect(&parser, "minGeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("minGeBtnInfo", "path", "LangType/Yueyu"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[albumType_minGe].SetTextures(btnTextures);
	mItemsBtn[albumType_minGe].MoveWindow(&rcControl);
	mAlbumsID[albumType_minGe].id = parser.GetIntValue("minGeBtnInfo", "typeID", 19);
	SAFE_STRNCPY(mAlbumsID[albumType_minGe].typeName, parser.GetStrValue("minGeBtnInfo", "typeName", "民歌"), sizeof(mAlbumsID[albumType_minGe].typeName));

  XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "LangType/Return"), sizeof(imgPath));
  CreateBtnImgTextures(imgPath, btnTextures);
  mReturnBtn.SetTextures(btnTextures);
  mReturnBtn.MoveWindow(&rcControl);

  mrcOrigItemsBKShow.right = rcControl.right;
	miXoffset = 0;
	mItemListBKWnd.MoveWindow(&mrcOrigItemsBKShow);
}

void CAlbumSongFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_ITEM_RETURN_ID )
	{
		gMainCtrlPage->ReturnToLastFragment();
	}
	else if ( nWndID >= BTN_ITEM_BASE_ID && nWndID < (BTN_ITEM_BASE_ID + albumType_count) )
	{
		int idx = nWndID - BTN_ITEM_BASE_ID;
		static SEARCHITEM search;
		search.eType = SearchBySongType;
		search.nDepth = mAlbumsID[idx].id;
		search.pParentItem = NULL;
		SAFE_STRNCPY(search.cItemName, mAlbumsID[idx].typeName, sizeof(search.cItemName));
		gSongListFragment->SetSearchBarItem(&search, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
	}
}

void CAlbumSongFragment::ShowChidlSong()
{

	static SEARCHITEM search;
	search.eType = SearchBySongType;
	search.nDepth = mAlbumsID[albumType_erTong].id;
	search.pParentItem = NULL;
	SAFE_STRNCPY(search.cItemName, mAlbumsID[albumType_erTong].typeName, sizeof(search.cItemName));
	gSongListFragment->SetSearchBarItem(&search, 0);
	gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
}

void CAlbumSongFragment::ShowBirthdaySong()
{

	static SEARCHITEM search;
	search.eType = SearchBySongType;
	search.nDepth = mAlbumsID[albumType_erTong].id;
	search.pParentItem = NULL;
	SAFE_STRNCPY(search.cItemName, mAlbumsID[albumType_xiQing].typeName, sizeof(search.cItemName));
	gSongListFragment->SetSearchBarItem(&search, 0);
	gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
}


void CAlbumSongFragment::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	miXoffset = mItemListBKWnd.mRectRelativeToParent.left;

	pWnd->SetCapture();
}

void CAlbumSongFragment::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CAlbumSongFragment::OnTouchMove(CBaseWnd *pWnd, POINT pt)
{
	if (pWnd != mpPressedWnd)
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

	mItemListBKWnd.MoveWindow(&rcCurBK);
}

void CAlbumSongFragment::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

