#include "SlideWnd.h"
#include "BaseApp.h"

#define SLIDEWND_EFFECT_TIMERID 0x12345678

CSlideWnd::CSlideWnd()
{
	mSliding = FALSE;
	mpOnSlideWndEndListener = NULL;
}

CSlideWnd::~CSlideWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CSlideWnd::Create(
	const char *cClassName,
	CBaseWnd *pParent,
	UINT32 uFlag,
	RECT rcShow,
	RECT rcHide,
	BOOL bInitShow)
{
	mrcShow = rcShow;
	mrcHide = rcHide;

	RECT rc = bInitShow ? rcShow : rcHide;

	UINT32 uState = WINDOWSTATE_DEFAULT;
	if (!bInitShow)
	{
		uState |= WINDOWSTATE_INVISIBLE;
	}

	CParentClass::Create(cClassName, pParent, rc, uFlag, uState);
}

void CSlideWnd::OnTimer(
	int nTimerID)
{
	CParentClass::OnTimer(nTimerID);

	if (nTimerID == SLIDEWND_EFFECT_TIMERID)
	{
		DbgOutput(DBG_DEBUG, "slider window class=%s curstep=%d, totalstep=%d\n",
			GetClassName(), mSlideCurrentStep, mSlideTotalSteps);

		mSlideCurrentStep --;

		RECT rcSlideTo;
		if (mSlideCurrentStep <= 0)
		{
			rcSlideTo = mEndVisible ? mrcShow : mrcShow;

			StopSlideWindow();
		}
		else
		{
			rcSlideTo = mRectRelativeToParent;
			rcSlideTo.left += mXSlideStepLen;
			rcSlideTo.right += mXSlideStepLen;
			rcSlideTo.top += mYSlideStepLen;
			rcSlideTo.bottom += mYSlideStepLen;

			CBaseWnd::MoveWindow(&rcSlideTo);

			if (mpOnSlideWndEndListener)
			{
				mpOnSlideWndEndListener->OnSlideStep(
					this, mSlideCurrentStep, mSlideTotalSteps, mUserData);
			}
		}
	}
}

void CSlideWnd::MoveWindow(
	RECT rcShow,
	RECT rcHide)
{
	mrcShow = rcShow;
	mrcHide = rcHide;

	if  (!mSliding)
	{
		RECT rcSlideTo = mEndVisible ? mrcShow : mrcShow;
		CParentClass::MoveWindow(&rcSlideTo);
	}
}

void CSlideWnd::SlideShowWindow(
	BOOL bVisible,
	ISlideWndEndListener *pOnSlideWndEndListener,
	UINT64 uUserData,
	UINT64 uSlideMS)
{
	if (mSliding)
	{
		StopSlideWindow();
	}

	DbgOutput(DBG_DEBUG, "SlideShowWindow(%s) visible=%s\n",
		GetClassName(), bVisible?"TRUE":"FALSE");

	SetWindowVisible(TRUE);

	mEndVisible = bVisible;

	if (mEndVisible)
	{
		SetFocus();
	}

	mpOnSlideWndEndListener = pOnSlideWndEndListener;
	mUserData = uUserData;

	if ( bVisible )
	{
		CBaseWnd::MoveWindow(&mrcHide);

		StartSlideWindow(
			mrcShow.left - mrcHide.left,
			mrcShow.top - mrcHide.top,
			uSlideMS);
	}
	else
	{
		CBaseWnd::MoveWindow(&mrcShow);

		StartSlideWindow(
			mrcHide.left-mrcShow.left,
			mrcHide.top-mrcShow.top,
			uSlideMS);
	}
}

void CSlideWnd::StartSlideWindow(
	int xSlideLength,
	int ySlideLength,
	int nSlideTimeMS)
{
	DbgOutput(DBG_DEBUG, "SlideShowWindow(%s) Start\n", GetClassName());

	mSlideTotalSteps = nSlideTimeMS / 25;
	if (mSlideTotalSteps > 0)
	{
		mSlideCurrentStep = mSlideTotalSteps;

		mXSlideStepLen = xSlideLength / mSlideTotalSteps;
		mYSlideStepLen = ySlideLength / mSlideTotalSteps;

		AddTimer(SLIDEWND_EFFECT_TIMERID, 25);

		mSliding = TRUE;
	}
	else
	{
		StopSlideWindow();
	}
}

void CSlideWnd::StopSlideWindow()
{
	DbgOutput(DBG_DEBUG, "SlideShowWindow(%s) Stop\n", GetClassName());

	if (mSliding)
	{
		DelTimer(SLIDEWND_EFFECT_TIMERID);
	}

	CBaseWnd::MoveWindow(mEndVisible ? (&mrcShow) : (&mrcHide));
	SetWindowVisible(mEndVisible);

	if (mpOnSlideWndEndListener)
	{
		mpOnSlideWndEndListener->OnSlideWndEnd(this, mEndVisible, mUserData);

		mpOnSlideWndEndListener = NULL;
	}

	mSliding = FALSE;
}
