#include "BKInputPanel.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "BaseApp.h"

#define BKSPELLBTN_CLICK_ID		0x01
#define BKHANDWRITEBTN_CLICK_ID	0x02

CBKInputPanel::CBKInputPanel()
{
	mInputType = INPUTMETHODTYPE_HANDWRITE;
}

CBKInputPanel::~CBKInputPanel()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CBKInputPanel::Create(CBaseWnd *pParent, RECT rcRect, BOOL bInitShowInput, BOOL bPopup)
{
	RECT rcShow;
	RECT rcHide;
	CopyRect(&rcShow, &rcRect);
	CopyRect(&rcHide, &rcRect);

	//SetZorderNumber(WINDOWS_ZORDER_INPUTWNDPOPUP-25);
	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 10);

	CParentClass::Create("InputPannel", pParent, WINDOWFLAG_MANUALSETZORDER, rcShow, rcHide, bInitShowInput, bPopup ? TRUE : FALSE);

	RECT rcControl = {446, 63, 31, 90};
	mSpellInput.Create(this, rcControl);
	mSpellInput.SetMsgRecvWnd(this);

	mHandWirteInput.Create(this, rcControl);
	mHandWirteInput.SetMsgRecvWnd(this);

	mSpellBtn.Create(this, rcControl);
	mSpellBtn.SetWndID(BKSPELLBTN_CLICK_ID);
	mSpellBtn.SetOnClickListener(this);

	mHandWriteBtn.Create(this, rcControl);
	mHandWriteBtn.SetWndID(BKHANDWRITEBTN_CLICK_ID);
	mHandWriteBtn.SetOnClickListener(this);

	SetInputMethodType(mInputType);

	LoadResource();
}

void CBKInputPanel::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "BKInputPanel.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcShow;
	RECT rcHide;
	XmlLoadRect(&parser, "ShowWindowLocation", &rcShow);
	XmlLoadRect(&parser, "HideWindowLocation", &rcHide);
	CParentClass::MoveWindow(rcShow, rcHide);

	RECT rcControl;
	char strImgPath[64];
	CTexture btnTextures[4];

	XmlLoadRect(&parser, "SpellBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SpellBtnInfo", "imgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSpellBtn.MoveWindow(&rcControl);
	mSpellBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "HandWriteBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("HandWriteBtnInfo", "imgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mHandWriteBtn.MoveWindow(&rcControl);
	mHandWriteBtn.SetTextures(btnTextures);
}

void CBKInputPanel::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);

//	if ( bVisible )
//	{
//		ClearInputText();
//	}
}

void CBKInputPanel::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

//	if ( mInputText.GetLength() >= 10 )
//		return ;

	switch(uType)
	{
	case INPUTWND_BACKMSG:
	{
		// 回删
		int iLen = mInputText.GetLength();
		if ( iLen > 0 )
			mInputText.Trunk(iLen - 1);
	}
		break;

	case INPUTWND_CHARMSG:
		// 字符
		mInputText.Append((wchar_t)wParam);
		break;

	case INPUTWND_WORDLENGTHMSG:
		// 数字
		mInputText.Append((wchar_t)wParam);
		break;

	case INPUTWND_SPECIALCHARMSG:
	{
		// 特殊字符  [ ]
		if ( lParam == 1 )
		{
			int iLen = mInputText.GetLength();
			if ( iLen > 0 )
				mInputText.Trunk(mInputText.GetLength() - 1);
			else
				DbgOutput(DBG_ERR, "the Input text size:%d, put you put ] , is not match.\n", mInputText.GetLength());
		}
		mInputText.Append((wchar_t)wParam);
	}
		break;

	default:
	{
		return;
	}
		break;
	}

	//printf("=======the text:%ls\n", mInputText.GetString());
	ReportMsg(INPUTWND_CHARMSG, (UINT64)(mInputText.GetString()), 0);
}

void CBKInputPanel::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int iWnd = pWnd->GetWndID();
	switch ( iWnd )
	{
	case BKSPELLBTN_CLICK_ID:
		SetInputMethodType(INPUTMETHODTYPE_SPELL);
		break;

	case BKHANDWRITEBTN_CLICK_ID:
		SetInputMethodType(INPUTMETHODTYPE_HANDWRITE);
		break;
	}
}

void CBKInputPanel::ClearInputText()
{
	mInputText.Trunk(0);
}

void CBKInputPanel::GetInputText(CBaseStringW &text)
{
	text.Set(mInputText.GetString());
}

void CBKInputPanel::SetInputText(CBaseStringW &text)
{
	mInputText.Set(text.GetString());
}

void CBKInputPanel::SetInputMethodType(INPUTMETHODTYPE inputtype)
{
	if ( mInputType == inputtype )
		return ;

	mInputType = inputtype;

	switch ( inputtype )
	{
	case INPUTMETHODTYPE_SPELL:
		mSpellInput.SetWindowVisible(TRUE);
		mHandWirteInput.SetWindowVisible(FALSE);
		break;

	case INPUTMETHODTYPE_HANDWRITE:
		mSpellInput.SetWindowVisible(FALSE);
		mHandWirteInput.SetWindowVisible(TRUE);
		break;

	default:
		break;
	}
}
