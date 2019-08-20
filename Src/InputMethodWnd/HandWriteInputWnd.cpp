#include "HandWriteInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../Handwrite/include/WWHW.h"

extern const unsigned long arrayHwdataR[];

#define TIMERID_RECOGNIZE		0x01
#define TIMERID_CLEARLINE		0x02

#define TIME_RECOGNIZE			10
#define TIME_CLEARLINE			2000

INPUTITEM gHandWriteInputItem[] =
{
	{INPUTID_HANDWRITE_BASE+0, L' ', {2, 5, 52+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+1, L' ', {52+2, 5, 52*2+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+2, L' ', {52*2+2, 5, 52*3+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+3, L' ', {52*3+2, 5, 52*4+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+4, L' ', {52*4+2, 5, 52*5+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+5, L' ', {52*4+2, 5, 52*5+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+6, L' ', {52*4+2, 5, 52*5+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+7, L' ', {52*4+2, 5, 52*5+2, 40}, ""},
	{INPUTID_HANDWRITE_BASE+8, L' ', {0, 0, 0, 0}, ""},
	{INPUTID_HANDWRITE_BASE+9, L' ', {0, 0, 0, 0}, ""},
};

#define BTN_REPUT_ID			0x01
#define BTN_SLIDE_ID				0x02
#define BTN_CLOSE_ID			0x03
#define ITEM_PRESSED_SCALE 0.9

CHandWriteInputWnd::CHandWriteInputWnd()
{
	mpLocusPoint = NULL;
	mpLocusCount = 0;
	mLocusIndex = 0;
}

CHandWriteInputWnd::~CHandWriteInputWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
	if ( mpLocusPoint != NULL )
	{
		DbgOutput(DBG_WARN, "mpLocusPoint NOT delete on destroy\n");
	}
}

void CHandWriteInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("HandWriteInputWnd", pParent, rc);

	RECT rcControl = {0, 0, 70, 70};

	mWriteRegionWnd.CreateStatic(this, rcControl);
	mLineWnd.CreateStatic(this, rcControl);
	mLineWnd.SetOnTouchListener(this);

	for ( int i = 0 ; i < INPUTID_HANDWRITE_COUNT ; i++ )
	{
		mCandidateBtns[i].Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
		mCandidateBtns[i].SetWndID(INPUTID_HANDWRITE_BASE + i);
		mCandidateBtns[i].SetOnClickListener(this);
		mCandidateBtns[i].SetOnTouchListener(this);
		mCandidateBtns[i].SetWindowVisible(FALSE);
		mCandidateBtns[i].SetFontColor(0xFF000000);
	}

	mReputBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mReputBtn.SetWndID(BTN_REPUT_ID);
	mReputBtn.SetOnClickListener(this);

	mSlideBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mSlideBtn.SetWndID(BTN_SLIDE_ID);
	mSlideBtn.SetOnClickListener(this);

	mCloseBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	LoadResource();
}

void CHandWriteInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "HandWriteInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT  rcControl, rcBase;
	CTexture bkTexture, btnTextures[4];
	char imgPath[64] = {0};
	int columnNum, xOffset, yOffset;

	XmlLoadRect(&parser, "HandWriteRegionInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("HandWriteRegionInfo", "path", "InputWnd/SpellBoard"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mLineWnd.MoveWindow(&rcControl);
	mLineWnd.SetLineBuf();
	mWriteRegionWnd.MoveWindow(&rcControl);
	mWriteRegionWnd.SetBackgroundTexture(&bkTexture);

	SAFE_DELETEARRAY(mpLocusPoint);
	mpLocusCount = 0;
	int nLocusWidth = RECTWIDTH(mLineWnd.mWndRect);
	int nLocusHeight = RECTHEIGHT(mLineWnd.mWndRect);
	mpLocusCount = nLocusWidth*nLocusHeight;
	mpLocusPoint = new short[mpLocusCount];

	ClearBoardBuf();
	DbgOutput(DBG_DEBUG, "locus count %d\n", mpLocusCount);

	XmlLoadRect(&parser, "CandidateItemInfo", &rcBase);
	xOffset = parser.GetIntValue("CandidateItemInfo", "xOffset",80);
	yOffset = parser.GetIntValue("CandidateItemInfo", "yOffset",100);
	columnNum = parser.GetIntValue("CandidateItemInfo", "columns", 4);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CandidateItemInfo", "path", "InputWnd/Symbol"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);

	for ( int i = 0 ; i < INPUTID_HANDWRITE_COUNT ; i++ )
	{
		int j,k;
		j = i % columnNum;
		k =i / columnNum;
		InflatRect(&rcControl, &rcBase, j*xOffset, k*yOffset);
		mCandidateBtns[i].SetTextures(btnTextures);
		mCandidateBtns[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "ReputBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReputBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReputBtn.SetTextures(btnTextures);
	mReputBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SlideBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SlideBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSlideBtn.SetTextures(btnTextures);
	mSlideBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);

	WWExitRecognition();
	WWInitRecognition(arrayHwdataR, 0);
}

void CHandWriteInputWnd::OnDestroy()
{
	CParentClass::OnDestroy();

	SAFE_DELETEARRAY(mpLocusPoint);
	mpLocusCount = 0;

	WWExitRecognition();
}

void CHandWriteInputWnd::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);

	if (TIMERID_RECOGNIZE == nTimerID)
	{
		DelTimer(TIMERID_RECOGNIZE);
		DoRecognize();
	}
	else if ( TIMERID_CLEARLINE == nTimerID )
	{
		DelTimer(TIMERID_CLEARLINE);
		ClearBoardBuf();
	}
}

void CHandWriteInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();

	if ( nWndID == BTN_REPUT_ID )
		ReportMsg(INPUTWND_CLEARMSG, 0, 0);
	else if ( nWndID == BTN_CLOSE_ID )
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
	else if ( nWndID == BTN_SLIDE_ID )
		ReportMsg(INTERNETTV_MEDIA_CLOSE, 0, 0);
	else if ( nWndID >= INPUTID_HANDWRITE_BASE && nWndID < (INPUTID_HANDWRITE_BASE+INPUTID_HANDWRITE_COUNT) )
	{
		int nClickID = nWndID - INPUTID_HANDWRITE_BASE;
		ReportMsg(	INPUTWND_CHARMSG, 	(UINT64)gHandWriteInputItem[nClickID].wszChar, 0);
	}
	else
		return;

	ClearBoardBuf();
}

void CHandWriteInputWnd::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	if (pWnd != &mLineWnd) return;

	mLineWnd.LineStart(pt);
	AddPointToLocus(pt);

	DelTimer(TIMERID_RECOGNIZE);
	DelTimer(TIMERID_CLEARLINE);
}

void CHandWriteInputWnd::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
	if (pWnd != &mLineWnd)
		return;

	if (mLocusIndex <= mpLocusCount-4)
	{
		mpLocusPoint[mLocusIndex++] = -1;
		mpLocusPoint[mLocusIndex++] = 0;
	}
	else
	{
		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", mLocusIndex);
	}

	AddTimer(TIMERID_RECOGNIZE, TIME_RECOGNIZE);
	AddTimer(TIMERID_CLEARLINE, TIME_CLEARLINE);
}

void CHandWriteInputWnd::OnTouchMove(CBaseWnd *pWnd, POINT pt)
{
	if (pWnd != &mLineWnd)
		return;

	mLineWnd.LineTo(pt);
	AddPointToLocus(pt);
}

void CHandWriteInputWnd::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
	AddTimer(TIMERID_RECOGNIZE, TIME_RECOGNIZE);
	AddTimer(TIMERID_CLEARLINE, TIME_CLEARLINE);
}

void CHandWriteInputWnd::AddPointToLocus(POINT pt)
{
	if (mLocusIndex >= mpLocusCount-3)
	{
		return;
	}

	if (mLocusIndex <= mpLocusCount-4)
	{
		mpLocusPoint[mLocusIndex++] = pt.x;
		mpLocusPoint[mLocusIndex++] = pt.y;
		//DbgOutput(DBG_INFO, "idx:%d,[%d, %d], pt.x %d, pt.y %d\n", mLocusIndex-2, mpLocusPoint[mLocusIndex-2], mpLocusPoint[mLocusIndex-1], pt.x, pt.y);
	}
	else
	{
		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", mLocusIndex);
	}
}

void CHandWriteInputWnd::DoRecognize()
{
	int i = 0;
	unsigned short result[INPUTID_HANDWRITE_COUNT];
	memset(result, 0, sizeof(result));

	if ( mLocusIndex <= 0 )
		return ;
	/*
	 * 新建内存是因为 识别字体的时候，WWRecognizeChar()会修改
	 * mpLocusPoint中的内容，，所以需要另开辟内容去拷贝识别
	 */
	short *pLocus = NULL;
	short *pLocusPoint = new short[mLocusIndex+2];
	if ( pLocusPoint == NULL )
		pLocus = mpLocusPoint;
	else
	{
		pLocus = pLocusPoint;
		memcpy(pLocusPoint, mpLocusPoint, sizeof(short)*mLocusIndex);
	}

	if (mLocusIndex <= mpLocusCount-2)
	{
		pLocus[mLocusIndex] = -1;
		pLocus[mLocusIndex+1] = -1;

		//printf("================index:%d, mpLocusCount:%d=======end recognize\n", mLocusIndex, mpLocusCount);
	}
	else
	{
		pLocus[mpLocusCount-2] = -1;
		pLocus[mpLocusCount-1] = -1;

		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", mLocusIndex);
	}

#if !defined(__arm__)
		WWRecognizeChar(pLocus, (unsigned short*)result, INPUTID_HANDWRITE_COUNT, WWHW_RANGE_GB2312|0x2);
#else
		WWRecognize(pLocus, (unsigned short*)result, INPUTID_HANDWRITE_COUNT, WWHW_RANGE_GB2312|0x2);
#endif

	if ( pLocusPoint )
	{
		delete []pLocusPoint;
		pLocusPoint = NULL;
	}
	pLocus = NULL;

	wchar_t str[2] = {0};
	for(i = 0; i < INPUTID_HANDWRITE_COUNT; i++)
	{
		str[0] = result[i];

		mCandidateBtns[i].SetWindowTextW(str);

		gHandWriteInputItem[i].wszChar = str[0];
	}

	for(int i = 0; i < INPUTID_HANDWRITE_COUNT; i++)
		mCandidateBtns[i].SetWindowVisible(TRUE);
}

void CHandWriteInputWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if(!bVisible)
	{
		wchar_t str[2] = {0};
		for(int i = 0; i < INPUTID_HANDWRITE_COUNT; i++)
		{
			mCandidateBtns[i].SetWindowTextW(str);
			gHandWriteInputItem[i].wszChar = str[0];
		}
		ClearBoardBuf();
		DelTimer(TIMERID_RECOGNIZE);
		DelTimer(TIMERID_CLEARLINE);
	}
}

void CHandWriteInputWnd::ClearBoardBuf()
{
	mLocusIndex = 0;
	mLineWnd.Clear();
	if ( mpLocusPoint )
	{
		memset(mpLocusPoint, 0x00, sizeof(short) * mpLocusCount );
	}
}
