#ifndef _DLNAWND_H
#define _DLNAWND_H
#include "AppVerb.h"

#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"

class CDlnaWnd: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CDlnaWnd();
	virtual ~CDlnaWnd();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
public:
	virtual void OnWindowVisible(BOOL bVisible);

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void GenerateQRCodeWnd();

private:
	CBaseWnd	mQRCodeWnd;
	CBaseButton mReturnBtn;
	CBaseButton	mQRCodeBtn;
};
#endif
