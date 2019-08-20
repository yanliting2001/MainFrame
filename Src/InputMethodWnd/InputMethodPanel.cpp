#include "InputMethodPanel.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define SPELLBTN_CLICKID 1
#define HANDWRITEBTN_CLICKID 2
#define WORDLENGTH_CLICKID 3

CInputMethodPanel::CInputMethodPanel()
{
	mInputPanelPopup = TRUE;
	mSelInputMethodType = INPUTMETHODTYPE_SPELL;
}

CInputMethodPanel::~CInputMethodPanel()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CInputMethodPanel::Create(
	CBaseWnd *pParent,
	RECT rcRect,
	BOOL bInitShowInput,
	BOOL bPopup)
{
	RECT rcShow;
	RECT rcHide;
	CopyRect(&rcShow, &rcRect);
	CopyRect(&rcHide, &rcRect);
	rcHide.top = -rcHide.bottom;

	SetZorderNumber(WINDOWS_ZORDER_INPUTWNDPOPUP);

	CParentClass::Create(
		"InputPannel",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		bInitShowInput,
		bPopup ? TRUE:FALSE);

	mInputPanelPopup = bPopup;

	RECT rcControl = {0, 0, 100, 100};
	mRegionWnd.CreateStatic(this, rcControl);

	mSpellBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mSpellBtn.SetWndID(SPELLBTN_CLICKID);
	mSpellBtn.SetOnClickListener(this);

	mHandWriteBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mHandWriteBtn.SetWndID(HANDWRITEBTN_CLICKID);
	mHandWriteBtn.SetOnClickListener(this);

	mWordLengthBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mWordLengthBtn.SetWndID(WORDLENGTH_CLICKID);
	mWordLengthBtn.SetOnClickListener(this);

	mInputGroup.AddToGroup(&mSpellBtn);
	mInputGroup.AddToGroup(&mHandWriteBtn);
	mInputGroup.AddToGroup(&mWordLengthBtn);

	SetRectXY(&rcControl, 158, 358, 270, 120);
	mSpellInput.Create(this, rcControl);
	mLengthSearchWnd.Create(this, rcControl);
	mHandwriteInput.Create(this, rcControl);
	mDigitInput.Create(this, rcControl);

	SetInputMethodType(INPUTMETHODTYPE_HANDWRITE);

	LoadResource();
}

void CInputMethodPanel::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "InputPanel.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT rcControl;
	CTexture bkTexture, btnTextures[4];
	char imgPath[64] = {0};

	SAFE_STRNCPY(imgPath, parser.GetStrValue("RegionWndInfo", "path", "InputWnd/SpellBoard"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mRegionWnd.SetBackgroundTexture(&bkTexture);
	mRegionWnd.MoveWindow(&mWndRect);

	XmlLoadRect(&parser, "SpellTabBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SpellTabBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSpellBtn.SetTextures(btnTextures);
	mSpellBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "HandWriteTabBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("HandWriteTabBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mHandWriteBtn.SetTextures(btnTextures);
	mHandWriteBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "StrokesTabBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("StrokesTabBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mWordLengthBtn.SetTextures(btnTextures);
	mWordLengthBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SpellInputRegionWndInfo", &rcControl);
	mSpellInput.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LengthInputRegionWndInfo", &rcControl);
	mLengthSearchWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "HandwriteInputRegionWndInfo", &rcControl);
	mHandwriteInput.MoveWindow(&rcControl);
	mDigitInput.MoveWindow(&rcControl);

	SetInputMethodType(INPUTMETHODTYPE_SPELL);
}

void CInputMethodPanel::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();

	switch (nWndID)
	{
	case SPELLBTN_CLICKID:
		SetInputMethodType(INPUTMETHODTYPE_SPELL);
		break;

	case HANDWRITEBTN_CLICKID:
		SetInputMethodType(INPUTMETHODTYPE_HANDWRITE);
		break;

	case WORDLENGTH_CLICKID:
		SetInputMethodType(INPUTMETHODTYPE_STROKES);
		break;

	default:
		break;
	}
}

BOOL CInputMethodPanel::IsInputPanelPopup()
{
	return mInputPanelPopup;
}

void CInputMethodPanel::SetInputRecvWnd(
	CBaseWnd *pRecvWnd)
{
	mHandwriteInput.SetMsgRecvWnd(pRecvWnd);
	mSpellInput.SetMsgRecvWnd(pRecvWnd);
	mLengthSearchWnd.SetMsgRecvWnd(pRecvWnd);
	mDigitInput.SetMsgRecvWnd(pRecvWnd);

	SetMsgRecvWnd(pRecvWnd);

	SetBindWnd(pRecvWnd);
}

void CInputMethodPanel::SetInputMethodType(
	INPUTMETHODTYPE inputmethodtype)
{
	ReportMsg(INPUTWND_CLEARMSG, 0, 0);
	mSelInputMethodType = inputmethodtype;

	if (inputmethodtype == INPUTMETHODTYPE_SPELL)
	{
		mHandwriteInput.SetWindowVisible(FALSE);
		mLengthSearchWnd.SetWindowVisible(FALSE);
		mDigitInput.SetWindowVisible(FALSE);
		mSpellInput.SetWindowVisible(TRUE);

		mInputGroup.SetSelectButton(&mSpellBtn);
	}
	else if (inputmethodtype == INPUTMETHODTYPE_HANDWRITE)
	{
		mHandwriteInput.SetWindowVisible(TRUE);
		mSpellInput.SetWindowVisible(FALSE);
		mLengthSearchWnd.SetWindowVisible(FALSE);
		mDigitInput.SetWindowVisible(FALSE);

		mInputGroup.SetSelectButton(&mHandWriteBtn);
	}
	else if (inputmethodtype == INPUTMETHODTYPE_STROKES)
	{
		mLengthSearchWnd.SetWindowVisible(FALSE);
		mHandwriteInput.SetWindowVisible(FALSE);
		mSpellInput.SetWindowVisible(FALSE);
		mDigitInput.SetWindowVisible(TRUE);

		mInputGroup.SetSelectButton(&mWordLengthBtn);
	}
}

void CInputMethodPanel::SetInputMethodTypeEnable(
	INPUTMETHODTYPE inputmethodtype,
	BOOL bEnable)
{
	if (inputmethodtype == INPUTMETHODTYPE_SPELL)
	{
		if (bEnable)
		{
			mInputGroup.AddToGroup(&mSpellBtn);
			mSpellBtn.SetWindowVisible(TRUE);
			mSpellInput.SetWindowVisible(TRUE);
		}
		else
		{
			mInputGroup.DelFromGroup(&mSpellBtn);
			mSpellBtn.SetWindowVisible(FALSE);
			mSpellInput.SetWindowVisible(FALSE);
		}
	}
	else if (inputmethodtype == INPUTMETHODTYPE_HANDWRITE)
	{
		if (bEnable)
		{
			mInputGroup.AddToGroup(&mHandWriteBtn);
			mHandWriteBtn.SetWindowVisible(TRUE);
			mHandwriteInput.SetWindowVisible(TRUE);
		}
		else
		{
			mInputGroup.DelFromGroup(&mHandWriteBtn);
			mHandWriteBtn.SetWindowVisible(FALSE);
			mHandwriteInput.SetWindowVisible(FALSE);
		}
	}
	else if (inputmethodtype == INPUTMETHODTYPE_STROKES)
	{
		if (bEnable)
		{
			mInputGroup.AddToGroup(&mWordLengthBtn);
			mWordLengthBtn.SetWindowVisible(TRUE);
			//mLengthSearchWnd.SetWindowVisible(TRUE);
			mDigitInput.SetWindowVisible(TRUE);
		}
		else
		{
			mInputGroup.DelFromGroup(&mWordLengthBtn);
			mWordLengthBtn.SetWindowVisible(FALSE);
			//mLengthSearchWnd.SetWindowVisible(FALSE);
			mDigitInput.SetWindowVisible(FALSE);
		}
	}
}		

void CInputMethodPanel::SetWordLengthType(
	WORDLENGTHTYPE wordLengthType)
{
	mLengthSearchWnd.SetWordLengthType(wordLengthType);
}

INPUTMETHODTYPE CInputMethodPanel::GetInputMethodType()
{
	return mSelInputMethodType;
}

WORDLENGTHTYPE CInputMethodPanel::GetWordLengthType()
{
	return mLengthSearchWnd.GetWordLengthType();
}

void CInputMethodPanel::SetInputMethodTypeByPinyin(INPUTMETHODTYPE inputmethodtype)
{
	mSelInputMethodType = inputmethodtype;

	if (inputmethodtype == INPUTMETHODTYPE_SPELL)
	{
		mHandwriteInput.SetWindowVisible(FALSE);
		mSpellInput.SetWindowVisible(TRUE);
		mLengthSearchWnd.SetWindowVisible(FALSE);

		mInputGroup.SetSelectButton(&mSpellBtn);
	}
	else if (inputmethodtype == INPUTMETHODTYPE_HANDWRITE)
	{
		mHandwriteInput.SetWindowVisible(TRUE);
		mSpellInput.SetWindowVisible(FALSE);
		mLengthSearchWnd.SetWindowVisible(FALSE);

		mInputGroup.SetSelectButton(&mHandWriteBtn);
	}
	else if (inputmethodtype == INPUTMETHODTYPE_STROKES)
	{
		mLengthSearchWnd.SetWindowVisible(TRUE);
		mHandwriteInput.SetWindowVisible(FALSE);
		mSpellInput.SetWindowVisible(FALSE);

		mInputGroup.SetSelectButton(&mWordLengthBtn);
	}
}


