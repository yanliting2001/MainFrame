#ifndef _SHARINGLOGOUTWND_H
#define _SHARINGLOGOUTWND_H
#include "BaseButton.h"
#include "ClientSharing.h"

class CSharingLogoutWnd: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnWindowVisible(BOOL bVisible);

private:
	CBaseButton mLogoutBtn;
	CBaseButton mCloseBtn;
	CBaseWnd mLoginUserBKWnd;
	CBaseWnd mLoginUserImgWnd;
};
#endif	// _SHARINGLOGOUTWND_H
