#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"

class ISlideWndEndListener
{
public:
	virtual void OnSlideStep(
		CBaseWnd *pWnd,
		int nCurStep,
		int nTotalStep,
		UINT64 uUserData)
	{}

	virtual void OnSlideWndEnd(
		CBaseWnd *pWnd,
		BOOL bVisible,
		UINT64 uUserData)=0;
};

class CSlideWnd: public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	CSlideWnd();
	virtual ~CSlideWnd();

public:
	void Create(
		const char *cClassName,
		CBaseWnd *pParent,
		UINT32 uFlag,
		RECT rcShow,
		RECT rcHide,
		BOOL bInitShow);

	void MoveWindow(
		RECT rcShow,
		RECT rcHide);

public:
	virtual void OnTimer(
		int nTimerID);

	void SlideShowWindow(
		BOOL bVisible,
		ISlideWndEndListener *pOnSlideWndEndListener=NULL,
		UINT64 uUserData=0,
		UINT64 uSlideMS=300);

public:
	void StartSlideWindow(
		int xSlideLength,
		int ySlideLength,
		int nSlideTimeMS);
	void StopSlideWindow();

private:
	BOOL mSliding;
	int  mSlideTotalSteps;
	int  mSlideCurrentStep;
	int  mXSlideStepLen;
	int  mYSlideStepLen;

	ISlideWndEndListener *mpOnSlideWndEndListener;
	UINT64 mUserData;

	BOOL mEndVisible;
	RECT mrcShow;
	RECT mrcHide;
};
