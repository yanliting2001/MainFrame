#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "SlideWnd.h"

class CPopupSlideWnd: public CSlideWnd,
	public ISlideWndEndListener
{
	typedef CSlideWnd CParentClass;

public:
	CPopupSlideWnd();
	virtual ~CPopupSlideWnd();

public:
	void Create(
		const char *cClassName,
		CBaseWnd *pParent,
		UINT32 uFlag,
		RECT rcShow,
		RECT rcHide,
		BOOL bInitShow,
		BOOL bAutoHideWhenInactive);

public:
	virtual void OnWindowVisible(
		BOOL bVisible);

	virtual void OnWindowActivate(
		BOOL bActivate);

	// interface
public:
	virtual void OnSlideWndEnd(
		CBaseWnd *pWnd,
		BOOL bVisible,
		UINT64 uUserData);

public:
	void SetBindWnd(
		CBaseWnd *pBindWnd);

	void SlideShowWindow(
		BOOL bVisible,
		ISlideWndEndListener *pOnSlideWndEndListener=NULL,
		UINT64 uUserData=0,
		UINT64 uSlideMS=300);

	void SetAutoHideWhenInactive(BOOL bAuto);

private:
	void CreateMaskWnd(
		CBaseWnd *pParent,
		BOOL bInitShow);

private:
	BOOL mbAutoHideWhenInactive;

	CBaseWnd* mpBindWnd;

	CBaseWnd mMaskWnd;

	ISlideWndEndListener *mpOnSlideWndEndListener;
	UINT64 mUserData;
};
