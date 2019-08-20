#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "InputMethodPanel.h"

class CSearchInputWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CSearchInputWnd();
	virtual ~CSearchInputWnd();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rc,
		RECT rcSearchBtn,
		RECT rcTextEdit,
		RECT rcInputPanel,
		BOOL bInitShowInput,
		BOOL bPopup);

	void MoveWindow(
		RECT rc,
		RECT rcSearchBtn,
		RECT rcTextEdit,
		RECT rcInputPanel);

	virtual void LoadResource();

public:
	virtual void OnMsg(	UINT32 uType, UINT64 wParam, UINT64 lParam);

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void SetPromptText(const wchar_t* wszPromptText);

	void SetTextMaxLength(int nTextMaxLength);

	void SetInputMethodTypeEnable(INPUTMETHODTYPE inputmethodtype, BOOL bEnable);

	void GetSearchInfo(SEARCHINFO *pSearchInfo);

	void ResetSearchInfo();

	void SetInputWndVisible(BOOL IsVisible);

	void SetInputMethodType(INPUTMETHODTYPE inputmethodtype);

	BOOL IsPopupSlide();

private:
	void OnInputChar(wchar_t wszChar);
	void OnBackText();
	void OnClearText();
	void OnSetWordLength();
	
	void ShowPromptText(	BOOL bShow);

	void ConvertStrokeToNumber(const wchar_t *wszStroke, wchar_t *wszNumber);

private:
	CBaseButton mSearchBtn;
	CBaseWnd mTextEdit;
	CInputMethodPanel mInputMethodPanel;

	int mTextMaxLength;
	BOOL mbShowPromptTextShown;
	CBaseStringW mPromptText;

	wchar_t *mwszStrokeNumber;

	unsigned long mTextColor;
	unsigned long mPromptColor;
};
