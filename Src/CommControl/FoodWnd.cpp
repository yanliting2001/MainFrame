#include "FoodWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "InternetCtrl/NetCtrlApi.h"


#define BTN_DLNA_COUNT	  10

#define BTN_PUSHINFO_ID			0x01
#define BTN_PUSHITEM_ID		0x02
#define BTN_CLOSE_ID		0x30
#define ITEM_PRESSED_SCALE	0.9f
#define BTN_ITEM_BASE_ID	0x01
#define FOODS_PATH		"./KTV_Data/food_textures"


void CFoodWnd::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+18);
	CParentClass::Create(
		"FoodWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	RECT rcControl = {50, 36, 100, 100};
  
	//mPushInfo.CreateStatic(this, rcControl);

  mHomeItemBKWnd.CreateStatic(this, rcControl);

  //mPushItem.CreateStatic(this, rcControl);
  	getFilesFromdir(&mFoodFileLists,FOODS_PATH,0);
	int count = mFoodFileLists.GetCount();
  	for ( int i = 0; i < count; ++i )
	{
		mFoodItems[i].Create(&mHomeItemBKWnd, rcControl, BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
		mFoodItems[i].SetWndID(BTN_ITEM_BASE_ID + i);
		mFoodItems[i].SetOnTouchListener(this);
		mFoodItems[i].SetOnClickListener(this);
	}

  mClose.Create(this, rcControl);
  mClose.SetWndID(BTN_CLOSE_ID);
  mClose.SetOnClickListener(this);

	LoadResource();
}

void CFoodWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "FoodWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl;
	CTexture bkTexture, btnTextures[4];
	char str[64] = {0};
	char colorbuf[16];
	unsigned long color;
	int size;
  char imgPath[64] = {0};
  RECT rcShow, rcHide;
  
  XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	XmlLoadRect(&parser, "HideWindowInfo", &rcHide);
	
	/*
  SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
  	CParentClass::MoveWindow(rcShow, rcHide);
	*/
	
	int count = mFoodFileLists.GetCount();
  	for ( int i = 0; i < count; ++i )	
	{
		FOOD_FILE_INFO_S *pFile;
		SetRectXY(&rcControl, 1000*i, 0, 1000, RECTHEIGHT(mWndRect));
		CTexture texture;
		pFile = (FOOD_FILE_INFO_S*)mFoodFileLists.GetAt(i);
		texture.CreateFromImgFile(pFile->cFilePath);
		mFoodItems[i].SetTextures(&texture);
		mFoodItems[i].MoveWindow(&rcControl);
	}


  	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "LeftPier/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mClose.SetTextures(btnTextures);
	mClose.MoveWindow(&rcControl);

  
}

void CFoodWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
  
	switch (nWndID)
	{
  SlideShowWindow(FALSE);

	case BTN_CLOSE_ID:
    SlideShowWindow(FALSE);
		break;

	default:
    SlideShowWindow(FALSE);
		break;
	}

}

void CFoodWnd::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	miXoffset = mHomeItemBKWnd.mRectRelativeToParent.left;

	pWnd->SetCapture();
}

void CFoodWnd::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, 	UINT64 uUsedTime)
{
	if ( pWnd != mpPressedWnd )
		return;

	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CFoodWnd::OnTouchMove(CBaseWnd *pWnd, POINT pt)
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

void CFoodWnd::OnTouchDrag(CBaseWnd *pWnd, int xDragLen, int yDragLen)
{
}

void CFoodWnd::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CFoodWnd::getFilesFromdir(CPtrCtrl *list, const char *path, int depth)
{
	if ( !list || !path )  return ;

	DIR *pDir = NULL;
	struct dirent *file = NULL;
  
	if ( (pDir = opendir(path)) == NULL )
	{
		DbgOutput(DBG_INFO,"open dirent:%s failed\n", path);
		return;
	}

	while( (file = readdir(pDir)) != NULL )
	{
		if ( strncmp(file->d_name,".",1) == 0 )
			continue;
		else if ( file->d_type == DT_DIR )
		{
			if ( depth >= 1 )
				continue;
			char tmpPath[256] = {0};
			snprintf(tmpPath, sizeof(tmpPath), "%s/%s", path, file->d_name);
			getFilesFromdir(list, tmpPath, 1);
			continue;
		}

		if( (strcasestr(file->d_name,".jpg") == NULL) 	&&
			(strcasestr(file->d_name,".png") == NULL) )
		{
			continue;
		}

		printf("==the file name:%s\n", file->d_name);

		FOOD_FILE_INFO_S *pNewFile = new FOOD_FILE_INFO_S;
		if ( !pNewFile )
		{
			DbgOutput(DBG_INFO, "function %s:%u failed\n", __FUNCTION__, __LINE__);
			break ;
		}

		memset(pNewFile, 0x0, sizeof(FOOD_FILE_INFO_S));

		snprintf(pNewFile->cFileName, sizeof(pNewFile->cFileName), "%s", file->d_name);
		snprintf(pNewFile->cFilePath, sizeof(pNewFile->cFilePath), "%s/%s", path, file->d_name);
		list->AddData(pNewFile);
	}
	closedir(pDir);
}


