#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "RadioButtonGroup.h"
#include "InputMethodDef.h"

class CWordLengthInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public ITouchEventListener,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, 	POINT pt);

	virtual void OnTouchDown(
		CBaseWnd *pWnd,
		POINT pt);
	virtual void OnTouchUp(
		CBaseWnd *pWnd,
		POINT pt,
		int xDistance,
		int yDistance,
		UINT64 uUsedTime);
	virtual void OnTouchLeave(
		CBaseWnd *pOldTouchWnd,
		CBaseWnd *pNewTouchWnd);

public:
	void SetWordLengthType(WORDLENGTHTYPE wordLengthType);

	WORDLENGTHTYPE GetWordLengthType();

private:
	void RefreshItemText(int clickIdx);

private:
	CBaseButton mLengthBtns[WordLength_TypeCount];
	CRadioButtonGroup mLengthGroup;
};
