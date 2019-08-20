#pragma once

#include "BaseWnd.h"
#include "BaseSlide.h"
#include "RadioButtonGroup.h"
#include "./Input/BKInputPanel.h"

#define SCROLLTEXT_COLOR_COUNT	7

class CSetScrollTextWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CSetScrollTextWnd();
	~CSetScrollTextWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	void LoadResource();

	virtual void OnWindowVisible(BOOL bVisible);
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);


private:
	CBaseWnd			mTitleWnd;

	CBaseWnd			mColorPromptWnd;
	UINT32 				mTextColor[SCROLLTEXT_COLOR_COUNT];
	CBaseButton 		mColorBtn[SCROLLTEXT_COLOR_COUNT];
	CRadioButtonGroup 	mColorBtnGroup;

	CBaseWnd			mTextPromptWnd;
	CBaseWnd			mTextBoxWnd;

	CBKInputPanel		mInputPanel;

	CBaseButton			mConfirmBtn;
	CBaseButton			mCancelBtn;
};
