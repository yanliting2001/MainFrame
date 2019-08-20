#ifndef _ACTIVITYCONTENTWND_H
#define _ACTIVITYCONTENTWND_H
#include "BaseButton.h"
#include "BaseWnd.h"

class CActivityContentWnd: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	CBaseButton mCloseBtn;
	CBaseWnd mActivityWnd;
};
#endif	
