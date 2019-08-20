#ifndef _DIGITINPUTWND_H
#define _DIGITINPUTWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "InputMethodDef.h"

class CDigitlInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public IClickEventListener
{
	enum _eDigitType
	{
		digit_count = 10,
	};
	typedef CBaseWnd CParentClass;
public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	CBaseButton mItemWnd[digit_count];
	CBaseButton mSlideBtn;
	CBaseButton mCloseBtn;
};

#endif		// _DIGITINPUTWND_H
