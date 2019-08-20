#include "DigitInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define BTN_DIGIT_BASE_ID	0x01
#define BTN_SLIDE_ID				0x31
#define BTN_CLOSE_ID			0x32

#define ITEM_PRESSED_SCALE 0.9

void CDigitlInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("DigitInput", pParent, rc);

	RECT rcControl = {0, 0, 68, 68};
	int i;
	for ( i = 0 ; i < digit_count; i++ )
	{
		mItemWnd[i].Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
		mItemWnd[i].SetWndID(BTN_DIGIT_BASE_ID + i);
		mItemWnd[i].SetOnClickListener(this);
	}

	mSlideBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mSlideBtn.SetWndID(BTN_SLIDE_ID);
	mSlideBtn.SetOnClickListener(this);

	mCloseBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	LoadResource();
}

void CDigitlInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "DigitInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT  rcControl, rcBase;
	CTexture btnTextures[4];
	char imgPath[64] = {0};
	int xOffset, yOffset, columnNum;

	XmlLoadRect(&parser, "DigitItemBtnInfo", &rcBase);
	xOffset = parser.GetIntValue("DigitItemBtnInfo", "xOffset", 4);
	yOffset = parser.GetIntValue("DigitItemBtnInfo", "yOffset", 4);
	columnNum = parser.GetIntValue("DigitItemBtnInfo", "columns", 4);
	SAFE_STRNCPY(imgPath,parser.GetStrValue("DigitItemBtnInfo", "path", "InputWnd/Region"), sizeof(imgPath));

	int i, j, k;
	for ( i = 0 ; i < digit_count; i++ )
	{
		char tmpImg[64] = {0};
		snprintf(tmpImg, sizeof(tmpImg), "%s/%d", imgPath, (i+1)%10);
		CreateBtnImgTextures(tmpImg, btnTextures);

		j = i % columnNum;
		k = i / columnNum;
		InflatRect(&rcControl, &rcBase, j*xOffset, k*yOffset);
		mItemWnd[i].SetTextures(btnTextures);
		mItemWnd[i].MoveWindow(&rcControl);
	}

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
}

void CDigitlInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_CLOSE_ID )
	{
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
	}
	else if ( nWndID == BTN_SLIDE_ID )
	{
		ReportMsg(INTERNETTV_MEDIA_CLOSE, 0, 0);
	}
	else if ( nWndID >= BTN_DIGIT_BASE_ID && nWndID < (BTN_DIGIT_BASE_ID + digit_count) )
	{
		int nClickID = nWndID - BTN_DIGIT_BASE_ID;
		wchar_t itemText = L'0' + (nClickID+1)%10;
		ReportMsg(INPUTWND_CHARMSG, (UINT64)itemText,	0);
	}
}
