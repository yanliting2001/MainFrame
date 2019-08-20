#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "InputMethodDef.h"

class CSpellInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;
public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	CBaseButton mSpellItemWnd[INPUTID_SPELL_COUNT];
	CBaseButton mReputBtn;
	CBaseButton mSlideBtn;
	CBaseButton mCloseBtn;
};
