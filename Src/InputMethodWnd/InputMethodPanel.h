#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "RadioButtonGroup.h"
#include "../Effect/PopupSlideWnd.h"
#include "InputMethodDef.h"
#include "HandWriteInputWnd.h"
#include "SpellInputWnd.h"
#include "WordLengthInputWnd.h"
#include "DigitInputWnd.h"

class CInputMethodPanel: public CPopupSlideWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CPopupSlideWnd CParentClass;

public:
	CInputMethodPanel();
	virtual ~CInputMethodPanel();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rcRect,
		BOOL bInitShowInput,
		BOOL bPopup);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

public:
	BOOL IsInputPanelPopup();

	void SetInputRecvWnd(
		CBaseWnd *pRecvWnd);

	void SetInputMethodType(
		INPUTMETHODTYPE inputtype);

	void SetInputMethodTypeEnable(
		INPUTMETHODTYPE inputtype,
		BOOL bEnable);

	void SetWordLengthType(
		WORDLENGTHTYPE wordLengthType);

	INPUTMETHODTYPE GetInputMethodType();
	WORDLENGTHTYPE GetWordLengthType();

	void SetInputMethodTypeByPinyin(INPUTMETHODTYPE inputmethodtype);

private:
	BOOL mInputPanelPopup;

private:
	CBaseButton mSpellBtn;
	CBaseButton mHandWriteBtn;
	CBaseButton mWordLengthBtn;
	CRadioButtonGroup mInputGroup;

	CBaseWnd mRegionWnd;

	CHandWriteInputWnd mHandwriteInput;
	CSpellInputWnd mSpellInput;
	CWordLengthInputWnd mLengthSearchWnd;
	CDigitlInputWnd mDigitInput;

	INPUTMETHODTYPE mSelInputMethodType;
};
