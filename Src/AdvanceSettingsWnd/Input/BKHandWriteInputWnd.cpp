#include "BKHandWriteInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../../Handwrite/include/WWHW.h"

extern const unsigned long arrayHwdataR[];
extern const unsigned long arrayDwBigR[];

#define RECOGNIZE_TIMERID				1
#define REGOGNIZE_DELAYTIME				500
#define CANDIDATEBTN_CLICK_BASEID		0x00
#define SPACEBTN_CLICK_ID				0x10
#define SPECIALSYMBOLBTN_CLICK_ID		0x11
#define BACKBTN_CLICK_ID				0x12

CBKHandWriteInputWnd::CBKHandWriteInputWnd()
{
	miLocusCount = 0;
	miLocusIndex = 0;
	mpLocusPoint = NULL;
	mbInSymbol   = FALSE;
}

CBKHandWriteInputWnd::~CBKHandWriteInputWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);

	if ( mpLocusPoint != NULL )
		DbgOutput(DBG_WARN, "mpLocusPoint NOT delete on destroy\n");
}

void CBKHandWriteInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("HandWriteInputWnd", pParent, rc);
	SetOnTouchListener(this);

	mLineWnd.CreateStatic(this, rc);
	mLineWnd.SetOnTouchListener(this);

	RECT rcControl = {0, 0, 57, 57};
	int i = 0;
	for ( i = 0; i < BKCANDIDATE_TEXT_COUNT; ++i )
	{
		mCandidateBtns[i].Create(this, rcControl);
		mCandidateBtns[i].SetWndID(CANDIDATEBTN_CLICK_BASEID + i);
		mCandidateBtns[i].SetOnClickListener(this);
	}

	mSpaceBtn.Create(this, rcControl);
	mSpaceBtn.SetWndID(SPACEBTN_CLICK_ID);
	mSpaceBtn.SetOnClickListener(this);

	mSpecialSymbolBtn.Create(this, rcControl);
	mSpecialSymbolBtn.SetWndID(SPECIALSYMBOLBTN_CLICK_ID);
	mSpecialSymbolBtn.SetOnClickListener(this);

	mBackBtn.Create(this, rcControl);
	mBackBtn.SetWndID(BACKBTN_CLICK_ID);
	mBackBtn.SetOnClickListener(this);

	LoadResource();
}

void CBKHandWriteInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "BKHandWriteInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl, rcItem;
	char strImgPath[64];
	int i = 0, xOffset = 0, yOffset = 0, column = 6, size = 20;
	UINT32 color = 0xFF000000;
	int x = 0, y = 0;

	XmlLoadRect(&parser, "ShowWindowInfo", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	CTexture bkTexture;
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("ShowWindowInfo","ImgPath","AdvanceSettings/Input/SpellBG"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4];
	XmlLoadRect(&parser, "HandWriteRectInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("HandWriteRectInfo", "color", "FF000000"), sizeof(strImgPath));
	color = strtoul(strImgPath, NULL, 16);
	mLineWnd.MoveWindow(&rcControl);
	mLineWnd.SetLineBuf();
	mLineWnd.SetLineColor(color);
	int iLocusWidth  = RECTWIDTH(rcControl);
	int iLocusHeight = RECTHEIGHT(rcControl);
	miLocusCount     = iLocusWidth * iLocusHeight;
	SAFE_DELETEARRAY(mpLocusPoint);
	mpLocusPoint     = new short[miLocusCount];
	DbgOutput(DBG_DEBUG, "locus count %d\n", miLocusCount);

	XmlLoadRect(&parser, "CandidateInfo", &rcControl);
	xOffset = parser.GetIntValue("CandidateInfo", "xOffset", 57);
	yOffset = parser.GetIntValue("CandidateInfo", "yOffset", 57);
	column  = parser.GetIntValue("CandidateInfo", "colNums", 6);
	size	= parser.GetIntValue("CandidateInfo", "size", 30);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CandidateInfo", "color", "FF000000"), sizeof(strImgPath));
	color = strtoul(strImgPath, NULL, 16);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CandidateInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);

	for ( i = 0 ; i < BKCANDIDATE_TEXT_COUNT; ++i )
	{
		x = i % column;
		y = i / column;
		InflatRect(&rcItem, &rcControl, x*xOffset, y*yOffset);
		mCandidateBtns[i].MoveWindow(&rcItem);
		mCandidateBtns[i].SetFontSize(size);
		mCandidateBtns[i].SetFontColor(color);
		mCandidateBtns[i].SetTextures(btnTextures);
	}

	XmlLoadRect(&parser, "SpaceBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SpaceBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSpaceBtn.MoveWindow(&rcControl);
	mSpaceBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "SpecialSymbolBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SpecialSymbolBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSpecialSymbolBtn.MoveWindow(&rcControl);
	mSpecialSymbolBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "BackBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("BackBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mBackBtn.MoveWindow(&rcControl);
	mBackBtn.SetTextures(btnTextures);

	WWExitRecognition();
	if ( theBaseApp->GetLanguageID() == LanguageID_Chinese )
	{
		WWInitRecognition(arrayHwdataR, 0);
	}
	else if ( theBaseApp->GetLanguageID() == LanguageID_CHT)
	{
		WWInitRecognition(arrayDwBigR, 0);
	}
	else
	{
		WWInitRecognition(arrayHwdataR, 0);
	}

}

void CBKHandWriteInputWnd::OnDestroy()
{
	CParentClass::OnDestroy();

	SAFE_DELETEARRAY(mpLocusPoint);
	miLocusCount = 0;

	WWExitRecognition();

}

void CBKHandWriteInputWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( !bVisible )
	{
		wchar_t str[2] = {0};
		int i = 0;
		for ( i = 0; i < BKCANDIDATE_TEXT_COUNT; ++i )
		{
			mCandidateBtns[i].SetWindowTextW(str);
			mCandidateText[i] = str[0];
		}
	}
}

void CBKHandWriteInputWnd::OnTimer(int iTimerID)
{
	CParentClass::OnTimer(iTimerID);
	if ( iTimerID == RECOGNIZE_TIMERID )
	{
		DelTimer(RECOGNIZE_TIMERID);
		DoRecognize();
	}
}

void CBKHandWriteInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int iWndID = pWnd->GetWndID();
	wchar_t c;

	if ( iWndID == SPECIALSYMBOLBTN_CLICK_ID )
	{
		OnSpecialSymbolClick();
		return ;
	}

	mbInSymbol = FALSE;
	if ( iWndID == BACKBTN_CLICK_ID )
	{
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
	}
	else if ( iWndID == SPACEBTN_CLICK_ID )
	{
		c = L' ';
		ReportMsg(INPUTWND_CHARMSG, (UINT64)c, 0);
	}
	else if ( (iWndID >= CANDIDATEBTN_CLICK_BASEID) && (iWndID < CANDIDATEBTN_CLICK_BASEID+BKCANDIDATE_TEXT_COUNT) )
	{
		int iClickID = iWndID - CANDIDATEBTN_CLICK_BASEID;
		c = mCandidateText[iClickID];
		ReportMsg(INPUTWND_CHARMSG, (UINT64)c, 0);
	}
}

void CBKHandWriteInputWnd::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd != &mLineWnd )
		return ;
	int i;
	for ( i = 0; i < BKCANDIDATE_TEXT_COUNT; ++i )
		mCandidateBtns[i].SetWindowVisible(FALSE);

	mLineWnd.LineStart(pt);
	AddPointToLocus(pt);

	if ( IsTimerSet(RECOGNIZE_TIMERID) )
		DelTimer(RECOGNIZE_TIMERID);
}

void CBKHandWriteInputWnd::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
	if ( pWnd != &mLineWnd )
		return ;

	if ( miLocusIndex <= miLocusCount-4 )
	{
		mpLocusPoint[miLocusIndex++] = -1;
		mpLocusPoint[miLocusIndex++] = 0;
	}
	else
	{
		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", miLocusIndex);
	}

	AddTimer(RECOGNIZE_TIMERID, REGOGNIZE_DELAYTIME);
}

void CBKHandWriteInputWnd::OnTouchMove(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd != &mLineWnd )
		return ;

	mLineWnd.LineTo(pt);
	AddPointToLocus(pt);
}

void CBKHandWriteInputWnd::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
	AddTimer(RECOGNIZE_TIMERID, REGOGNIZE_DELAYTIME);
}

void CBKHandWriteInputWnd::AddPointToLocus(POINT pt)
{
	int iLocusWidth = RECTWIDTH(mWndRect);
	int iLocusHeight = RECTHEIGHT(mWndRect);

	if ( miLocusIndex >= iLocusWidth*iLocusHeight-3 )
		return;

	if ( miLocusIndex <= miLocusCount-4 )
	{
		mpLocusPoint[miLocusIndex++] = pt.x;
		mpLocusPoint[miLocusIndex++] = pt.y;
		DbgOutput(DBG_DEBUG, "pt.x %d, pt.y %d\n", pt.x, pt.y);
	}
	else
	{
		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", miLocusIndex);
	}
}

void CBKHandWriteInputWnd::DoRecognize()
{
	int i = 0;
	unsigned short result[BKCANDIDATE_TEXT_COUNT];
	memset(result, 0, sizeof(result));

	if ( miLocusIndex <= miLocusCount-2 )
	{
		mpLocusPoint[miLocusIndex++] = -1;
		mpLocusPoint[miLocusIndex++] = -1;
	}
	else
	{
		mpLocusPoint[miLocusCount-2] = -1;
		mpLocusPoint[miLocusIndex-1] = -1;

		DbgOutput(DBG_WARN, "locus index count %d, out bound\n", miLocusIndex);
	}
#if !defined(__arm__)

	if ( theBaseApp->GetLanguageID() == LanguageID_Chinese )
		WWRecognizeChar(mpLocusPoint, (unsigned short*)result, BKCANDIDATE_TEXT_COUNT, WWHW_RANGE_GB2312|0x2);
	else if ( theBaseApp->GetLanguageID() == LanguageID_CHT)
		WWRecognizeChar(mpLocusPoint, (unsigned short*)result, BKCANDIDATE_TEXT_COUNT, WWHW_RANGE_BIG5);
	else
		WWRecognizeChar(mpLocusPoint, (unsigned short*)result, BKCANDIDATE_TEXT_COUNT, WWHW_RANGE_GB2312|0x2);
#else
	if ( theBaseApp->GetLanguageID() == LanguageID_Chinese )
	{
		WWRecognize(mpLocusPoint, (unsigned short*)result, BKCANDIDATE_TEXT_COUNT, WWHW_RANGE_GB2312|0x2);
	}
	else if ( theBaseApp->GetLanguageID() == LanguageID_CHT)
	{
		WWRecognize(mpLocusPoint, (unsigned short*)result, BKCANDIDATE_TEXT_COUNT, WWHW_RANGE_BIG5);
	}
	else
	{
		WWRecognize(mpLocusPoint, (unsigned short*)result, BKCANDIDATE_TEXT_COUNT, WWHW_RANGE_GB2312|0x2);
	}
#endif

	wchar_t str[2] = {0};
	for( i = 0; i < BKCANDIDATE_TEXT_COUNT; ++i )
	{
		str[0] = result[i];

		mCandidateBtns[i].SetWindowTextW(str);

		mCandidateText[i] = str[0];
	}

	miLocusIndex = 0;
	mLineWnd.Clear();

	for( i = 0; i < BKCANDIDATE_TEXT_COUNT; ++i )
		mCandidateBtns[i].SetWindowVisible(TRUE);

}

void CBKHandWriteInputWnd::OnSpecialSymbolClick()
{
	static int iClickNum = 0;
	if ( mbInSymbol )
	{
		iClickNum++;
		iClickNum %= 2;
	}
	else
	{
		mbInSymbol = TRUE;
		iClickNum = 0;
	}

	if ( iClickNum == 0 )
		ReportMsg(INPUTWND_SPECIALCHARMSG, (UINT64)'[', 0);
	else
		ReportMsg(INPUTWND_SPECIALCHARMSG, (UINT64)']', 1);
}
