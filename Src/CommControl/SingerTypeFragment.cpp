#include "SingerTypeFragment.h"
#include "XMLParser.h"
#include "ImgFileOp.h"
#include "../GlobalUIClass.h"

//#define BTN_RETURN_ID			0x01
#define BTN_ITEM_BASE_ID	0x10

#define ITEM_PRESSED_SCALE	0.9f

CSingerTypeFragment::CSingerTypeFragment()
{
	mpPressedWnd = NULL;
	miXoffset = 0;
}

CSingerTypeFragment::~CSingerTypeFragment()
{
	DbgOutput(DBG_DEBUG, "function %s quit\n", __FUNCTION__);
}

void CSingerTypeFragment::Create(CBaseWnd *pParent)
{
	RECT rcListCtrl = {50, 100, 1230, 500};
	CParentClass::Create(
		"SingerTypeFragment",
		pParent,
		rcListCtrl,
		WINDOWFLAG_DEFAULT,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

	mItemListBKWnd.CreateStatic(this, rcControl);

	for ( int i = 0; i < singerType_count; ++i )
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

void CSingerTypeFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;
	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SingerTypeFragment.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture btnTextures[4];
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
//	mSearchPrompt.SetWindowTextA(parser.GetStrValue("SearchPromptInfo", "text", "歌星点歌"));
//	mSearchPrompt.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ItemsBkWndInfo", &mrcOrigItemsBKShow);

	XmlLoadRect(&parser, "dlMaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("dlMaleBtnInfo", "path", "SingerType/Type0"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_dlMale].SetTextures(btnTextures);
	mItemsBtn[singerType_dlMale].MoveWindow(&rcControl);
	mTypesID[singerType_dlMale].id = parser.GetIntValue("dlMaleBtnInfo", "typeID", 2);
	SAFE_STRNCPY(mTypesID[singerType_dlMale].typeName, parser.GetStrValue("dlMaleBtnInfo", "typeName", "大陆男星"), sizeof(mTypesID[singerType_dlMale].typeName));

	XmlLoadRect(&parser, "dlFemaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("dlFemaleBtnInfo", "path", "SingerType/Type1"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_dlFemale].SetTextures(btnTextures);
	mItemsBtn[singerType_dlFemale].MoveWindow(&rcControl);
	mTypesID[singerType_dlFemale].id = parser.GetIntValue("dlFemaleBtnInfo", "typeID", 3);
	SAFE_STRNCPY(mTypesID[singerType_dlFemale].typeName, parser.GetStrValue("dlFemaleBtnInfo", "typeName", "大陆女星"), sizeof(mTypesID[singerType_dlFemale].typeName));

	XmlLoadRect(&parser, "gwMaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("gwMaleBtnInfo", "path", "SingerType/Type4"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_gwMale].SetTextures(btnTextures);
	mItemsBtn[singerType_gwMale].MoveWindow(&rcControl);
	mTypesID[singerType_gwMale].id = parser.GetIntValue("gwMaleBtnInfo", "typeID", 6);
	SAFE_STRNCPY(mTypesID[singerType_gwMale].typeName, parser.GetStrValue("gwMaleBtnInfo", "typeName", "国外男星"), sizeof(mTypesID[singerType_gwMale].typeName));

	XmlLoadRect(&parser, "gtMaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("gtMaleBtnInfo", "path", "SingerType/Type2"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_gtMale].SetTextures(btnTextures);
	mItemsBtn[singerType_gtMale].MoveWindow(&rcControl);
	mTypesID[singerType_gtMale].id = parser.GetIntValue("gtMaleBtnInfo", "typeID", 4);
	SAFE_STRNCPY(mTypesID[singerType_gtMale].typeName, parser.GetStrValue("gtMaleBtnInfo", "typeName", "港台男星"), sizeof(mTypesID[singerType_gtMale].typeName));

	XmlLoadRect(&parser, "gtFemaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("gtFemaleBtnInfo", "path", "SingerType/Type3"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_gtFemale].SetTextures(btnTextures);
	mItemsBtn[singerType_gtFemale].MoveWindow(&rcControl);
	mTypesID[singerType_gtFemale].id = parser.GetIntValue("gtFemaleBtnInfo", "typeID", 5);
	SAFE_STRNCPY(mTypesID[singerType_gtFemale].typeName, parser.GetStrValue("gtFemaleBtnInfo", "typeName", "港台女星"), sizeof(mTypesID[singerType_gtFemale].typeName));

	XmlLoadRect(&parser, "gwFemaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("gwFemaleBtnInfo", "path", "SingerType/Type5"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_gwFemale].SetTextures(btnTextures);
	mItemsBtn[singerType_gwFemale].MoveWindow(&rcControl);
	mTypesID[singerType_gwFemale].id = parser.GetIntValue("gwFemaleBtnInfo", "typeID", 7);
	SAFE_STRNCPY(mTypesID[singerType_gwFemale].typeName, parser.GetStrValue("gwFemaleBtnInfo", "typeName", "国外女星"), sizeof(mTypesID[singerType_gwFemale].typeName));

	XmlLoadRect(&parser, "RHMaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("RHMaleBtnInfo", "path", "SingerType/Type7"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_RHMale].SetTextures(btnTextures);
	mItemsBtn[singerType_RHMale].MoveWindow(&rcControl);
	mTypesID[singerType_RHMale].id = parser.GetIntValue("RHMaleBtnInfo", "typeID", 0);
	SAFE_STRNCPY(mTypesID[singerType_RHMale].typeName, parser.GetStrValue("RHMaleBtnInfo", "typeName", "日韩男星"), sizeof(mTypesID[singerType_RHMale].typeName));

	XmlLoadRect(&parser, "RHFemaleBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("RHFemaleBtnInfo", "path", "SingerType/Type7"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_RHFemale].SetTextures(btnTextures);
	mItemsBtn[singerType_RHFemale].MoveWindow(&rcControl);
	mTypesID[singerType_RHFemale].id = parser.GetIntValue("RHFemaleBtnInfo", "typeID", 0);
	SAFE_STRNCPY(mTypesID[singerType_RHFemale].typeName, parser.GetStrValue("RHFemaleBtnInfo", "typeName", "全部歌星"), sizeof(mTypesID[singerType_RHFemale].typeName));

	XmlLoadRect(&parser, "ldGroupBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ldGroupBtnInfo", "path", "SingerType/Type6"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[singerType_ldGroup].SetTextures(btnTextures);
	mItemsBtn[singerType_ldGroup].MoveWindow(&rcControl);
	mTypesID[singerType_ldGroup].id = parser.GetIntValue("ldGroupBtnInfo", "typeID", 8);
	SAFE_STRNCPY(mTypesID[singerType_ldGroup].typeName, parser.GetStrValue("ldGroupBtnInfo", "typeName", "乐对组合"), sizeof(mTypesID[singerType_ldGroup].typeName));

	mrcOrigItemsBKShow.right = rcControl.right;
	miXoffset = 0;
	mItemListBKWnd.MoveWindow(&mrcOrigItemsBKShow);
}

void CSingerTypeFragment::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
		mpPressedWnd = NULL;
}

void CSingerTypeFragment::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	miXoffset = mItemListBKWnd.mRectRelativeToParent.left;

	pWnd->SetCapture();
}

void CSingerTypeFragment::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, 	UINT64 uUsedTime)
{
	if ( pWnd != mpPressedWnd )
		return;

	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CSingerTypeFragment::OnTouchMove(CBaseWnd *pWnd, POINT pt)
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

	mItemListBKWnd.MoveWindow(&rcCurBK);
}

void CSingerTypeFragment::OnTouchDrag(CBaseWnd *pWnd, int xDragLen, int yDragLen)
{
}

void CSingerTypeFragment::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CSingerTypeFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	/*if ( nWndID == BTN_RETURN_ID )
	{
		gMainCtrlPage->ReturnToLastFragment();
	}
	else */if ( nWndID >= BTN_ITEM_BASE_ID && nWndID < (BTN_ITEM_BASE_ID+singerType_count) )
	{
		int nClickID = nWndID - BTN_ITEM_BASE_ID;
		gSingerListFragment->SetSearchBarItem(mTypesID[nClickID].typeName);
		gMainCtrlPage->SetCurrentFragment(Fragment_SingerList);
	}
}
