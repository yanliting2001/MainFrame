#include "SetScrollTextWnd.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
#include "UserMsgDefine.h"
#include "AdvanceSettingApi.h"
#include "../OSDControl/BCM_Control.h"

#define CONFIRMBTN_CLICK_ID		0x00
#define CANCELBTN_CLICK_ID		0x01
#define COLORBTN_CLICK_BASEID	0x02

CSetScrollTextWnd::CSetScrollTextWnd()
{

}

CSetScrollTextWnd::~CSetScrollTextWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CSetScrollTextWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("ReStore", pParent, rc);

	RECT rcControl;

	SetRectXY(&rcControl, 64, 147, 206, 40);
	mTitleWnd.CreateStatic(this, rcControl);
	mColorPromptWnd.CreateStatic(this, rcControl);
	mTextPromptWnd.CreateStatic(this, rcControl);
	mTextBoxWnd.CreateStatic(this, rcControl);
	//mTextBoxWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mInputPanel.Create(this, rcControl, TRUE, FALSE);
	mInputPanel.SetMsgRecvWnd(this);

	int i;
	for ( i = 0; i < SCROLLTEXT_COLOR_COUNT; ++i )
	{
		memset(&mTextColor[i], 0xFFFFFFFF, sizeof(mTextColor[i]));
		mColorBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mColorBtn[i].SetWndID(COLORBTN_CLICK_BASEID + i);
		mColorBtn[i].SetOnClickListener(this);

		mColorBtnGroup.AddToGroup(&mColorBtn[i]);
	}

	mColorBtnGroup.SetSelectButton(&mColorBtn[0]);

	mConfirmBtn.Create(this, rcControl);
	mConfirmBtn.SetWndID(CONFIRMBTN_CLICK_ID);
	mConfirmBtn.SetOnClickListener(this);

	mCancelBtn.Create(this, rcControl);
	mCancelBtn.SetWndID(CANCELBTN_CLICK_ID);
	mCancelBtn.SetOnClickListener(this);

	LoadResource();
}

void CSetScrollTextWnd::LoadResource()
{
	/*init position by xml*/
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SetScrollTextWnd.xml");
	if(false == parser.Load(cFileName))
	{
		printf("failed to load xml file!!\n");
	}

	// read from xml
	RECT rcShow;
	XmlLoadRect(&parser, "ShowWindowLocation", &rcShow);
	CParentClass::MoveWindow(&rcShow);

	RECT rcControl;
	int i, xOffset, size;
	char strImgPath[64] = {0};
	CTexture bkTexture;
	CTexture btnTextures[4];

	XmlLoadRect(&parser, "TitleWndInfo", &rcControl);
	size = parser.GetIntValue("TitleWndInfo", "size", 25);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("TitleWndInfo", "text", "跑马灯设置"), sizeof(strImgPath));
	mTitleWnd.MoveWindow(&rcControl);
	mTitleWnd.SetFontSize(size);
	mTitleWnd.SetWindowTextA(strImgPath);

	XmlLoadRect(&parser, "ColorPromptWndInfo", &rcControl);
	size = parser.GetIntValue("ColorPromptWndInfo", "size", 20);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("ColorPromptWndInfo", "text", "文字颜色"), sizeof(strImgPath));
	mColorPromptWnd.MoveWindow(&rcControl);
	mColorPromptWnd.SetFontSize(size);
	mColorPromptWnd.SetWindowTextA(strImgPath);

	RECT rcItem;
	char color[16] = {0};
	XmlLoadRect(&parser, "ColorBtnInfo", &rcControl);
	xOffset = parser.GetIntValue("ColorBtnInfo", "xOffset", 40);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("ColorBtnInfo","BaseImgPath", "AdvanceSettings/BaseColor"), sizeof(strImgPath));
	for ( i = 0; i < SCROLLTEXT_COLOR_COUNT; ++i )
	{
		InflatRect(&rcItem, &rcControl, i*xOffset, 0);
		mColorBtn[i].MoveWindow(&rcItem);

		char tmp[64] = {0};
		snprintf(tmp, sizeof(tmp), "%s%d", strImgPath, i);
		CreateBtnImgTextures(tmp, btnTextures);
		mColorBtn[i].SetTextures(btnTextures);

		snprintf(tmp, sizeof(tmp), "colorVal%d", i);
		SAFE_STRNCPY(color, parser.GetStrValue("ColorBtnInfo", tmp, "FFFFFFFF"), sizeof(color));
		mTextColor[i] = strtoul(color, NULL, 16);
	}

	XmlLoadRect(&parser, "TextPromptWndInfo", &rcControl);
	size = parser.GetIntValue("TextPromptWndInfo", "size", 20);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("TextPromptWndInfo", "text", "文字信息"), sizeof(strImgPath));
	mTextPromptWnd.MoveWindow(&rcControl);
	mTextPromptWnd.SetFontSize(size);
	mTextPromptWnd.SetWindowTextA(strImgPath);

	XmlLoadRect(&parser, "TextBoxWndInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("TextBoxWndInfo","ImgPath","AdvanceSettings/Input/SpellBG"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	mTextBoxWnd.MoveWindow(&rcControl);
	mTextBoxWnd.SetBackgroundTexture(&bkTexture);
	mTextBoxWnd.SetFontSize(size);

	XmlLoadRect(&parser, "InputPanelInfo", &rcControl);
	mInputPanel.MoveWindow(rcControl, rcControl);
	mInputPanel.SetInputMethodType(INPUTMETHODTYPE_SPELL);

	XmlLoadRect(&parser, "ConfirmBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("ConfirmBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mConfirmBtn.MoveWindow(&rcControl);
	mConfirmBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "CancelBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CancelBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mCancelBtn.MoveWindow(&rcControl);
	mCancelBtn.SetTextures(btnTextures);
}

void CSetScrollTextWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if (bVisible )
	{
		CBaseStringW tmp;
#if USE_MODULE_BCM
		tmp.Set(g_pBCM_Ctrl->GetMainScrollInfo());
#endif
		mInputPanel.SetInputText(tmp);
		mTextBoxWnd.SetWindowTextW(tmp.GetString());
		//mInputPanel.ClearInputText();
		//mTextBoxWnd.SetWindowTextW(NULL);
	}
}

void CSetScrollTextWnd::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	if ( uType == INPUTWND_CHARMSG )
	{
		wchar_t *pText = (wchar_t *)wParam;
		mTextBoxWnd.SetWindowTextW(pText);
	}
}

void CSetScrollTextWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int iWndID = pWnd->GetWndID();

	if ( iWndID == CONFIRMBTN_CLICK_ID )
	{
		int idx = mColorBtnGroup.GetSelectIndex();
		CBaseStringA text;
		text.Set(mTextBoxWnd.GetWindowTextW());
		if ( text.GetString() == NULL )
			return ;

		SaveScroolText(text, mTextColor[idx]);
//TODO:
#if USE_MODULE_BCM
		if ( g_pBCM_Ctrl )
		{
			g_pBCM_Ctrl->SetMainScrollInfo(text, mTextColor[idx]);
			g_pBCM_Ctrl->UpdateMainScrollInfo();
		}
#endif
		ReportMsg(INPUTWND_CLEARMSG, 0, 0);
	}
	else if ( iWndID == CANCELBTN_CLICK_ID )
	{
		ReportMsg(INPUTWND_CLEARMSG, 0, 0);
	}
	else if ( iWndID >= COLORBTN_CLICK_BASEID && (iWndID < COLORBTN_CLICK_BASEID + SCROLLTEXT_COLOR_COUNT) )
	{
		int iColorClickID = iWndID - COLORBTN_CLICK_BASEID;
		mColorBtnGroup.SetSelectButton(&mColorBtn[iColorClickID]);
		mTextBoxWnd.SetFontColor(mTextColor[iColorClickID]);
	}
}


