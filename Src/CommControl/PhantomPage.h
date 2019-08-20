#pragma once

#include "BaseWnd.h"
#include "FragmentInterface.h"

class CPhantomPage : public CFragment,
	public IClickEventListener
{
	typedef CFragment CParentClass;

public:
	void Create(CBaseWnd *pParent);

public:
	virtual void OnWindowVisible(	BOOL bVisible);

	virtual void OnTimer(	int nTimerID);

	void StartTimer();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);
};
