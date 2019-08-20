#include "LangSongFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../SQL/SearchSQL.h"
#include "../GlobalUIClass.h"

//#define BTN_RETURN_ID			0x01
#define BTN_ITEM_BASE_ID		0x02

#define ITEM_PRESSED_SCALE	0.9f

CLangSongFragment::CLangSongFragment()
{
	mpPressedWnd = NULL;
	miXoffset = 0;
}

CLangSongFragment::~CLangSongFragment()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CLangSongFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(	"RankSongFragment", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

	mItemListBKWnd.CreateStatic(this, rcControl);

	for ( int i = 0; i < langType_count; ++i )
	{
		mItemsBtn[i].Create(&mItemListBKWnd, rcControl, BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
		mItemsBtn[i].SetWndID(BTN_ITEM_BASE_ID + i);
		mItemsBtn[i].SetOnTouchListener(this);
		mItemsBtn[i].SetOnClickListener(this);
	}

//	mReturnBtn.Create(this, rcControl);
//	mReturnBtn.SetWndID(BTN_RETURN_ID);
//	mReturnBtn.SetOnClickListener(this);
//
//	mSearchPrompt.CreateStatic(this, rcControl);
//	mSearchPrompt.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	LoadResource();
}

void CLangSongFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "LangSongFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcAllControl ;
	CTexture bkTexture, btnTextures[4], allBtnTextures[4];
	char imgPath[64] = {0};
//	char colorbuf[16];
//	unsigned long color;
//	int size;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

//	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
//	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
//	CreateBtnImgTextures(imgPath, btnTextures);
//	mReturnBtn.SetTextures(btnTextures);
//	mReturnBtn.MoveWindow(&rcControl);
//
//	XmlLoadRect(&parser, "SearchPromptInfo", &rcControl);
//	size = parser.GetIntValue("SearchPromptInfo", "size", 20);
//	SAFE_STRNCPY(colorbuf, parser.GetStrValue("SearchPromptInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
//	color = strtoul(colorbuf,NULL,16);
//	mSearchPrompt.SetFontSize(size);
//	mSearchPrompt.SetFontColor(color);
//	mSearchPrompt.SetWindowTextA(parser.GetStrValue("SearchPromptInfo", "text", "语种点歌"));
//	mSearchPrompt.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ItemsBkWndInfo", &mrcOrigItemsBKShow);

	XmlLoadRect(&parser, "chineseLangBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("chineseLangBtnInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_chinese].SetTextures(btnTextures);
	mItemsBtn[langType_chinese].MoveWindow(&rcControl);
	mTypesID[langType_chinese].id = parser.GetIntValue("chineseLangBtnInfo", "typeID", 18);
	SAFE_STRNCPY(mTypesID[langType_chinese].typeName, parser.GetStrValue("chineseLangBtnInfo", "typeName", "国语"), sizeof(mTypesID[langType_chinese].typeName));

	XmlLoadRect(&parser, "yeuyuLangBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("yeuyuLangBtnInfo", "path", "LangType/Yueyu"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_yueyu].SetTextures(btnTextures);
	mItemsBtn[langType_yueyu].MoveWindow(&rcControl);
	mTypesID[langType_yueyu].id = parser.GetIntValue("yeuyuLangBtnInfo", "typeID", 19);
	SAFE_STRNCPY(mTypesID[langType_yueyu].typeName, parser.GetStrValue("yeuyuLangBtnInfo", "typeName", "粤语"), sizeof(mTypesID[langType_yueyu].typeName));

	XmlLoadRect(&parser, "englishTypeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("englishTypeBtnInfo", "path", "LangType/English"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_englisth].SetTextures(btnTextures);
	mItemsBtn[langType_englisth].MoveWindow(&rcControl);
	mTypesID[langType_englisth].id = parser.GetIntValue("englishTypeBtnInfo", "typeID", 21);
	SAFE_STRNCPY(mTypesID[langType_englisth].typeName, parser.GetStrValue("englishTypeBtnInfo", "typeName", "英语"), sizeof(mTypesID[langType_englisth].typeName));

	XmlLoadRect(&parser, "minnanLangBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("minnanLangBtnInfo", "path", "LangType/Minnan"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_minnan].SetTextures(btnTextures);
	mItemsBtn[langType_minnan].MoveWindow(&rcControl);
	mTypesID[langType_minnan].id = parser.GetIntValue("minnanLangBtnInfo", "typeID", 20);
	SAFE_STRNCPY(mTypesID[langType_minnan].typeName, parser.GetStrValue("minnanLangBtnInfo", "typeName", "闽南"), sizeof(mTypesID[langType_minnan].typeName));

	XmlLoadRect(&parser, "koreaTypeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("koreaTypeBtnInfo", "path", "LangType/JapanKorea"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_korea].SetTextures(btnTextures);
	mItemsBtn[langType_korea].MoveWindow(&rcControl);
	mTypesID[langType_korea].id = parser.GetIntValue("koreaTypeBtnInfo", "typeID", 22);
	SAFE_STRNCPY(mTypesID[langType_korea].typeName, parser.GetStrValue("koreaTypeBtnInfo", "typeName", "韩语"), sizeof(mTypesID[langType_korea].typeName));

	XmlLoadRect(&parser, "japanTypeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("japanTypeBtnInfo", "path", "LangType/AllLang"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_japan].SetTextures(btnTextures);
	mItemsBtn[langType_japan].MoveWindow(&rcControl);
	mTypesID[langType_japan].id = parser.GetIntValue("japanTypeBtnInfo", "typeID", 0);
	SAFE_STRNCPY(mTypesID[langType_japan].typeName, parser.GetStrValue("japanTypeBtnInfo", "typeName", "日语"), sizeof(mTypesID[langType_japan].typeName));

  XmlLoadRect(&parser, "otherLangBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("otherLangBtnInfo", "path", "LangType/Other"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[langType_other].SetTextures(btnTextures);
	mItemsBtn[langType_other].MoveWindow(&rcControl);
	mTypesID[langType_other].id = parser.GetIntValue("otherLangBtnInfo", "typeID", 24);
	SAFE_STRNCPY(mTypesID[langType_other].typeName, parser.GetStrValue("otherLangBtnInfo", "typeName", "其他"), sizeof(mTypesID[langType_other].typeName));

	mrcOrigItemsBKShow.right = rcControl.right;
	miXoffset = 0;
	mItemListBKWnd.MoveWindow(&mrcOrigItemsBKShow);
}

void CLangSongFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	/*if ( nWndID == BTN_RETURN_ID )
	{
		gMainCtrlPage->ReturnToLastFragment();
	}
	else */if ( nWndID >= BTN_ITEM_BASE_ID && nWndID < (BTN_ITEM_BASE_ID + langType_count) )
	{
		int idx = nWndID - BTN_ITEM_BASE_ID;
		static SEARCHITEM search;
		search.eType = SearchByLanguageType;
		search.nDepth = mTypesID[idx].id;
		search.pParentItem = NULL;
		SAFE_STRNCPY(search.cItemName, mTypesID[idx].typeName, sizeof(search.cItemName));
		gSongListFragment->SetSearchBarItem(&search, 0);
		gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
	}
}

void CLangSongFragment::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	miXoffset = mItemListBKWnd.mRectRelativeToParent.left;

	pWnd->SetCapture();
}

void CLangSongFragment::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CLangSongFragment::OnTouchMove(CBaseWnd *pWnd, POINT pt)
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

void CLangSongFragment::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

