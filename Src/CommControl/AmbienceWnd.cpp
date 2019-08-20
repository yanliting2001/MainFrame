#include "AmbienceWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "ImgFileOp.h"
#include "../GlobalUIClass.h"

#define COLORBTN_CLICKID_BASE		1
#define GREETINGBTN_CLICKID_BASE	11
#define FACEBTN_CLICKID_BASE		21
#define CLEARBTN_CLICKID			70
#define SENDBTN_CLICKID				71

#define RETURNBTN_CLICKID			72
#define CHEERBTN_CLICKID			73
#define HOOTINGBTN_CLICKID			74

CAmbienceWnd::CAmbienceWnd()
{
	mLocusColor[0] = 0xFFD1C5BE;
	mLocusColor[1] = 0xFF33AEFF;
	mLocusColor[2] = 0xFFD22333;
	mLocusColor[3] = 0xFFD64199;
	mLocusColor[4] = 0xFFFACC26;
	mLocusColor[5] = 0xFF9BCA33;

#ifdef ENABLE_SAVE_LOCUS
	mLocusBufferStride = 0;
	mpLocusBuffer = NULL;
#endif

	mpLocusPoint = NULL;
	mpLocusCount = 0;
	mLocusIndex = 0;
}

CAmbienceWnd::~CAmbienceWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);

	if (mpLocusPoint != NULL)
	{
		DbgOutput(DBG_WARN, "mpLocusPoint NOT delete on destroy\n");
	}

#ifdef ENABLE_SAVE_LOCUS
	SAFE_DELETEARRAY(mpLocusBuffer);
#endif
}

void CAmbienceWnd::Create(
	CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 100, 713, 445);
	SetRectXY(&rcHide, 300, -445, 713, 445);

	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 30);
	CParentClass::Create(
		"Ambience",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	RECT rcControl;
	int i;

	// color button
	for ( i = 0 ; i < 6 ; i++ )
	{
		SetRectXY(&rcControl, 40, 35+i*(34+1), 34, 34);
		mColorBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mColorBtn[i].SetWndID(COLORBTN_CLICKID_BASE+i);
		mColorBtn[i].SetOnClickListener(this);
		mColorBtnGroup.AddToGroup(&(mColorBtn[i]));
	}
	mColorBtnGroup.SetSelectButton(0);

	mLocusWnd.CreateStatic(this,rcControl);
	mLocusWnd.SetOnTouchListener(this);

	// face
	for ( i = 0 ; i < FACEBTN_NUM ; i++ )
	{
		int nCow = i % 3;
		int nRow = i / 3;
		SetRectXY(&rcControl, 35+nCow*(64+2), 254+nRow*(78+2), 64, 78);
		mFaceBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mFaceBtn[i].SetWndID(FACEBTN_CLICKID_BASE+i);
		mFaceBtn[i].SetOnClickListener(this);
		mFaceBtnGroup.AddToGroup(&(mFaceBtn[i]));
	}
	mFaceBtnGroup.SetSelectButton(20);

//	SetRectXY(&rcControl, 620, 38, 65, 53);
//	mCheerBtn.Create(this, rcControl);
//	mCheerBtn.SetOnClickListener(this);
//	mCheerBtn.SetWndID(CHEERBTN_CLICKID);
//
//	SetRectXY(&rcControl, 620, 120, 65, 53);
//	mHootingBtn.Create(this, rcControl);
//	mHootingBtn.SetWndID(HOOTINGBTN_CLICKID);
//	mHootingBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 620, 38, 65, 53);
	mClearBtn.Create(this, rcControl);
	mClearBtn.SetOnClickListener(this);
	mClearBtn.SetWndID(CLEARBTN_CLICKID);

	SetRectXY(&rcControl, 620, 370, 65, 53);
	mSendBtn.Create(this, rcControl);
	mSendBtn.SetWndID(SENDBTN_CLICKID);
	mSendBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 620, 370, 65, 53);
	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(RETURNBTN_CLICKID);
	mReturnBtn.SetOnClickListener(this);

	LoadResource();
}

void CAmbienceWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "AmbienceWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcShow;
	RECT rcHide;
  char imgPath[64] = {0};
	CTexture bkTexture;
  
	XmlLoadRect(&parser, "ShowWindowLocation", &rcShow);
	XmlLoadRect(&parser, "HideWindowLocation", &rcHide);
  strcpy(imgPath, parser.GetStrValue("BkGroundImg","path","Ambience/BkGround"));
	CreateImgTexture(imgPath, &bkTexture);
	CParentClass::MoveWindow(rcShow, rcHide);
	SetBackgroundTexture(&bkTexture);
  
	RECT rcControl;

	int i, xOffset, yOffset;
	RECT rcItem;

	// color button
	XmlLoadRect(&parser, "ColorBtnLocation", &rcItem);
	xOffset = parser.GetIntValue("ColorBtnLocation","xOffset", 35);
    yOffset = parser.GetIntValue("ColorBtnLocation","yOffset", 35);
	for ( i = 0 ; i < 6 ; i++ )
	{
		InflatRect(&rcControl, &rcItem, i*xOffset, i*yOffset);
		mColorBtn[i].MoveWindow(&rcControl);
	}

	// line box
	XmlLoadRect(&parser, "LineBoxLocation", &rcControl);
	mLocusWnd.MoveWindow(&rcControl);
	mLocusWnd.SetLineBuf();

	SAFE_DELETEARRAY(mpLocusPoint);
	int nLocusWidth = RECTWIDTH(rcControl);
	int nLocusHeight = RECTHEIGHT(rcControl);
	mpLocusCount = nLocusWidth*nLocusHeight;
	mpLocusPoint = new short[mpLocusCount];

#ifdef ENABLE_SAVE_LOCUS
	SAFE_DELETEARRAY(mpLocusBuffer);
	mLocusBufferStride = BMP_LINE_WIDTH(RECTWIDTH(rcControl), 32) ;
	mpLocusBuffer = new BYTE[mLocusBufferStride * RECTHEIGHT(rcControl)];
	memset(mpLocusBuffer, 0xFF, mLocusBufferStride * RECTHEIGHT(rcControl));
#endif


	// face
	XmlLoadRect(&parser, "FaceLocation", &rcItem);
	xOffset = parser.GetIntValue("FaceLocation","xOffset", 66);
	yOffset = parser.GetIntValue("FaceLocation","yOffset", 80);
	for ( i = 0 ; i < FACEBTN_NUM -1 ; i++ )
	{
		int nCow = i % 3;
		int nRow = i / 3;
		InflatRect(&rcControl, &rcItem, nCow*xOffset, nRow*yOffset);
		mFaceBtn[i].MoveWindow(&rcControl);
	}
	RECT rcItemInvalid = {0,0,0,0,};;
	mFaceBtn[FACEBTN_NUM-1].MoveWindow(&rcItemInvalid); // 最后一个无效

//	XmlLoadRect(&parser, "CheerBtnLocation", &rcControl);
//	mCheerBtn.MoveWindow(&rcControl);
//
//	XmlLoadRect(&parser, "HootingBtnLocation", &rcControl);
//	mHootingBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ClearBtnLocation", &rcControl);
	mClearBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SendBtnLocation", &rcControl);
	mSendBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnLocation", &rcControl);
	mReturnBtn.MoveWindow(&rcControl);

	

	CTexture btnTextures[4];

	// color button
	strcpy(imgPath, parser.GetStrValue("FontColorImg","path","Ambience/FontColor"));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( i = 0 ; i < 6 ; i++ )
	{
		mColorBtn[i].SetTextures(btnTextures);
	}

	// Greeting

	// face
	strcpy(imgPath, parser.GetStrValue("FaceImg","path","Ambience/Face"));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( i = 0 ; i < FACEBTN_NUM ; i++ )
	{
		mFaceBtn[i].SetTextures(btnTextures);
	}

//	strcpy(imgPath, parser.GetStrValue("CheerBtnImg","path","Ambience/FontColor"));
//	CreateBtnImgTextures(imgPath, btnTextures);
//	mCheerBtn.SetTextures(btnTextures);
//
//	strcpy(imgPath, parser.GetStrValue("HootingBtnImg","path","Ambience/FontColor"));
//	CreateBtnImgTextures(imgPath, btnTextures);
//	mHootingBtn.SetTextures(btnTextures);

	strcpy(imgPath, parser.GetStrValue("ClearBtnImg","path","Ambience/FontColor"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mClearBtn.SetTextures(btnTextures);

	strcpy(imgPath, parser.GetStrValue("SendBtnImg","path","Ambience/FontColor"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSendBtn.SetTextures(btnTextures);

	strcpy(imgPath, parser.GetStrValue("ReturnBtnImg","path","Ambience/FontColor"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);

	char colorbuf[16];
	strcpy(colorbuf,parser.GetStrValue("ColorList","color0","FFD1C5BE"));
	mLocusColor[0] = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("ColorList","color1","FF33AEFF"));
	mLocusColor[1] = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("ColorList","color2","FFD22333"));
	mLocusColor[2] = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("ColorList","color3","FFD64199"));
	mLocusColor[3] = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("ColorList","color4","FFFACC26"));
	mLocusColor[4] = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("ColorList","color5","FF9BCA33"));
	mLocusColor[5] = strtoul(colorbuf,NULL,16);

	mLocusWnd.SetLineColor(mLocusColor[0]);
	mColorBtnGroup.SetSelectButton(0);
}

void CAmbienceWnd::OnDestroy()
{
	CParentClass::OnDestroy();

	SAFE_DELETEARRAY(mpLocusPoint);
	mpLocusCount = 0;
}

void CAmbienceWnd::OnWindowVisible(
	BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if(!bVisible)
	{
		RECT rcControl;
		mLocusWnd.GetWndRect(&rcControl);
		memset(mpLocusBuffer, 0xFF, mLocusBufferStride * RECTHEIGHT(rcControl));
		mLocusWnd.Clear();

		//if(gMainCtrlPage->GetCurFragmentType() == Fragment_HomePage)
			//gMainCtrlPage->SetPhantomVisible(TRUE);
	}
	else
		mFaceBtnGroup.SetSelectButton(FACEBTN_NUM-1);
}

void CAmbienceWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();

	if(nWndID == RETURNBTN_CLICKID)
	{
		SlideShowWindow(FALSE);
	}
	else if (nWndID == HOOTINGBTN_CLICKID)
	{
		gPlayerInfo->Cheer(1);
	}
	else if (nWndID == CHEERBTN_CLICKID)
	{
		gPlayerInfo->Cheer(0);
	}

	else if (nWndID == SENDBTN_CLICKID)
	{
#ifdef ENABLE_SAVE_LOCUS
		static UINT64 uTimeOld = GetTickCount() - 1200;
		UINT64 uTimeNew = GetTickCount();
		if((uTimeNew-uTimeOld) < 1000)
		{
			return ;
		}
		uTimeOld = uTimeNew;

		SaveDrawBoxToBmpFile();
		int nFaceIndex = mFaceBtnGroup.GetSelectIndex();
		if(nFaceIndex == 20)
			gPlayerInfo->Ambience(2);
		else if(nFaceIndex < 10)
			gPlayerInfo->Ambience(0);
		else
			gPlayerInfo->Ambience(1);
		mLocusWnd.Clear();
		RECT rcControl;
		mLocusWnd.GetWndRect(&rcControl);
		memset(mpLocusBuffer, 0xFF, mLocusBufferStride * RECTHEIGHT(rcControl));
#endif
	}
	else if (nWndID == CLEARBTN_CLICKID)
	{
		mLocusWnd.Clear();
		RECT rcControl;
		mLocusWnd.GetWndRect(&rcControl);
		memset(mpLocusBuffer, 0xFF, mLocusBufferStride * RECTHEIGHT(rcControl));
		mFaceBtnGroup.SetSelectButton(FACEBTN_NUM-1);
	}
	else if (nWndID >= FACEBTN_CLICKID_BASE)
	{
		int nIndex = nWndID-FACEBTN_CLICKID_BASE;

		mFaceBtnGroup.SetSelectButton(nIndex);
	}
	else if (nWndID >= GREETINGBTN_CLICKID_BASE)
	{
	//	int nIndex = nWndID-GREETINGBTN_CLICKID_BASE;

	//	mGreetingBtnGroup.SetSelectButton(nIndex);
	}
	else if (nWndID >= COLORBTN_CLICKID_BASE)
	{
		int nIndex = nWndID-COLORBTN_CLICKID_BASE;

		mColorBtnGroup.SetSelectButton(nIndex);
		mLocusWnd.SetLineColor(mLocusColor[nIndex]);
	}
}

void CAmbienceWnd::OnTouchDown(
	CBaseWnd *pWnd,
	POINT pt)
{
	if (pWnd != &mLocusWnd)
	{
		return;
	}

#ifdef ENABLE_SAVE_LOCUS
	mPtLastLocus = pt;
	SetLocusToBuffer(pt);
#endif

	mLocusWnd.LineStart(pt);
}

void CAmbienceWnd::OnTouchUp(
	CBaseWnd *pWnd,
	POINT pt,
	int xDistance,
	int yDistance)
{
	if (pWnd != this)
	{
		return;
	}

	DbgOutput(DBG_WARN, "OnTouchUp\n");
	if (mLocusIndex <= mpLocusCount-4)
	{
		mpLocusPoint[mLocusIndex++] = -1;
		mpLocusPoint[mLocusIndex++] = 0;
	}
	else
	{
		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", mLocusIndex);
	}
}

void CAmbienceWnd::OnTouchMove(
	CBaseWnd *pWnd,
	POINT pt)
{
	if (pWnd != &mLocusWnd)
	{
		return;
	}

#ifdef ENABLE_SAVE_LOCUS
	SetLocusToBuffer(pt);
	mPtLastLocus = pt;
#endif

	mLocusWnd.LineTo(pt);
}

void CAmbienceWnd::AddPointToLocus(
	POINT pt)
{
//	DbgOutput(DBG_WARN, "AddPointToLocus\n");
	int nLocusWidth = RECTWIDTH(mWndRect);
	int nLocusHeight = RECTHEIGHT(mWndRect);

	if (mLocusIndex >= nLocusWidth*nLocusHeight-3)
	{
		return;
	}

	if (mLocusIndex <= mpLocusCount-4)
	{
		mpLocusPoint[mLocusIndex++] = pt.x;
		mpLocusPoint[mLocusIndex++] = pt.y;
		DbgOutput(DBG_DEBUG, "pt.x %d, pt.y %d\n", pt.x, pt.y);
	}
	else
	{
		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", mLocusIndex);
	}
}

#ifdef ENABLE_SAVE_LOCUS
void CAmbienceWnd::SetLocusToBuffer(
	POINT pt)
{
	UINT32 uColor = mLocusColor[mColorBtnGroup.GetSelectIndex()];

		BufferDrawLine_32(
		mPtLastLocus.x,
		mPtLastLocus.y,
		pt.x,
		pt.y,
		mpLocusBuffer,
		RECTWIDTH(mLocusWnd.mWndRect),
		RECTHEIGHT(mLocusWnd.mWndRect),
		mLocusBufferStride,
		uColor,3);
}

void MoveText32Buffer(
	BYTE *pDstBuffer,
	BYTE *pSrcBuffer,
	int nWidth,
	int nHeight,
	int nStride,
	int nMoveLength)
{
	if (nWidth < nMoveLength)
	{
		return;
	}

	BYTE *pDstLine = pDstBuffer;
	BYTE *pSrcLine = pSrcBuffer;

	int i, j;
	for ( i = 0 ; i < nHeight ; i++ )
	{
		// 拷贝剩余的点
		memcpy(pDstLine, pSrcLine+nMoveLength*4, (nWidth-nMoveLength)*4);

		// 补足尾部的点，透明�?		
		UINT32 *pTail = (UINT32 *)(pDstLine + (nWidth-nMoveLength)*4);
		for ( j = 0 ; j < nMoveLength ; j++ )
		{
			*(pTail + j) = 0x00FFFFFF;
		}

		pDstLine += nStride;
		pSrcLine += nStride;
	}
}

void CAmbienceWnd::SaveDrawBoxToBmpFile()
{
	if ( !mpLocusBuffer )
	{
		DbgOutput(DBG_ERR, "mpLocusBuffer is null.\n");
		return ;
	}
	// 先分配一块和手写板一样大小的内存作为底图，设置为黑色，半透明
	int nWidth = RECTWIDTH(mLocusWnd.mWndRect);
	int nHeight = RECTHEIGHT(mLocusWnd.mWndRect);
	int nBoardStride = BMP_LINE_WIDTH(nWidth, 32) ;
	BYTE *pBoardBuffer = NULL;
	CreateARGBColorBuffer(
		&pBoardBuffer,
		nWidth,
		nHeight,
		0x7F000000);

	if ( !pBoardBuffer )
	{
		DbgOutput(DBG_ERR, "CreateARGBColorBuffer width:%d, height:%d failed.\n", nWidth, nHeight);
		return ;
	}
	RECT rcBoard = {0, 0, nWidth, nHeight};

	// 复制手写板信息到底图�?	
	AlphaDrawBufferOnBuffer_ARGB(
		pBoardBuffer,
		rcBoard,
		nBoardStride,
		mpLocusBuffer,
		mLocusWnd.mWndRect,
		mLocusBufferStride,
		TRUE);
	// 获取文字的图片内�?//	int nGreetingIndex = mGreetingBtnGroup.GetSelectIndex();
//	wchar_t wszGreetingText[128];
//	mbstowcs(wszGreetingText,mGreetingText[nGreetingIndex],strlen(mGreetingText[nGreetingIndex])+1);
//	int nTextWidth, nTextHeight;
//	BYTE *pTextBuffer = NULL;
//	CreateTextBuffer(
//		wszGreetingText,
//		40,
//		0xFFFF0000,
//		FALSE,
//		nWidth,
//		nHeight,
//		&nTextWidth,
//		&nTextHeight,
//		&pTextBuffer);
//	int nTextStride = BMP_LINE_WIDTH(nTextWidth, 32) ;
//	RECT rcText = {0, 0, nTextWidth, nTextHeight};

	// 合成字体图片到底板上
//	AlphaDrawBufferOnBuffer_ARGB(
//		pBoardBuffer,
//		rcBoard,
//		nBoardStride,
//		pTextBuffer,
//		rcText,
//		nTextStride,
//		TRUE);

	// 获取笑脸图片内存
	int nFaceIndex = mFaceBtnGroup.GetSelectIndex();
	int nFaceWidth, nFaceHeight, nBytesPerPixel;
	BYTE *pFaceBuffer = NULL;
	CBaseStringA sFileName;
	sFileName.Format("%s%s%d_s.mem", theBaseApp->GetResFolder(), "/Ambience/",nFaceIndex+1);
	if( (nFaceIndex < (FACEBTN_NUM-1) ) && ( IsFileExist(sFileName.GetString() ) ) )
	{
		LoadImageFile(
			sFileName.GetString(),
			&nFaceWidth,
			&nFaceHeight,
			&nBytesPerPixel,
			&pFaceBuffer);

		if ( pFaceBuffer )
		{
			int nFaceStride = BMP_LINE_WIDTH(nFaceWidth, 32) ;
			RECT rcFace = {0, 0, nFaceWidth, nFaceHeight};
			// 合成笑脸图片到底板上, 注意，这个BUFFER�?BGR格式�?
			RECT rcCopyTo = {10, 0, 10+nFaceWidth, 0+nFaceHeight};
			AlphaDrawBufferOnBuffer_ARGB(
					pBoardBuffer,
					rcCopyTo,
					nBoardStride,
					pFaceBuffer,
					rcFace,
					nFaceStride,
					FALSE);
		}
	}


	sFileName.Format("%s%s","/tmp/","Ambience.bmp");//add by yq,2012-12-04

	// 保存合成的图�?	
	SaveRgbBufferToBmpFile(
		pBoardBuffer,
		nWidth,
		nHeight,
		32,
		NULL,
		0,
		sFileName.GetString());

	// 记得要释放内�?	// 但不要简单的使用 delete []这样得方式，因为跨dll/so文件的时候，容易产生异常
	SysLibFreeImageData(pFaceBuffer);
//	DeleteTextBuffer(pTextBuffer);
	DeleteImageData(pBoardBuffer);
}

//void CAmbienceWnd::SaveScollTextToBmpFile()
//{
//	// 以下函数为测试字体转成图片，并依次移动一个像�?//
//	int nTextWidth, nTextHeight;
//	BYTE *pTextBuffer = NULL;
//	CreateTextBuffer(
//		"以下函数为测试字体转成图片，并依次移动一个像�?,
//		20,
//		0xFFFFFFFF,
//		FALSE,
//		800,
//		100,
//		&nTextWidth,
//		&nTextHeight,
//		&pTextBuffer);
//	int nTextStride = BMP_LINE_WIDTH(nTextWidth, 32) ;
//
//	CBaseStringA sBmpFileName;
//
//	// 首先创建一个同样大小的内存，用来保存移动后的图
//	BYTE *pTmpBuffer = new BYTE[nTextStride*nTextHeight];
//
//	// 文字图片依次往左移�?个像�? 移动10�?//	int i ;
//	for ( i  = 0 ; i < 10 ; i++ )
//	{
//		MoveText32Buffer(
//			pTmpBuffer,
//			pTextBuffer,
//			nTextWidth,
//			nTextHeight,
//			nTextStride,
//			i+1);
//
//		sBmpFileName.Format("/home/kyo/test/move_%d.bmp", i);
//
//		SaveRgbBufferToBmpFile(
//			pTmpBuffer,
//			nTextWidth,
//			nTextHeight,
//			32,
//			NULL,
//			0,
//			sBmpFileName.GetString());
//	}
//
//	DeleteTextBuffer(pTextBuffer);
//
//	// 记得要释放内�?//	delete []pTmpBuffer;
//
//}
#endif

