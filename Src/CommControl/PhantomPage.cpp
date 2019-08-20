#include "PhantomPage.h"
#include "../GlobalUIClass.h"

#define TIMERID_SCREENSAVER 1
#define TIMER_INTERVAL	1000

void CPhantomPage::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	CParentClass::Create("PhantomPage", pParent, rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	SetOnClickListener(this);

	StartTimer();
}

void CPhantomPage::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);

	if (bVisible)
	{
		gPlayerInfo->StartPhantom();
		DelTimer(TIMERID_SCREENSAVER);
	}
	else
	{
		gPlayerInfo->StopPhantom();
		StartTimer();
	}
}

void CPhantomPage::OnClick(CBaseWnd *pWnd, POINT pt)
{
	gPageManager->ReturnToLastPage();
}

void CPhantomPage::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);

	if  ( TIMERID_SCREENSAVER == nTimerID )
	{
		if ( gs_SystemSetting.miScreenSave_type && (GetTickCount() - theBaseApp->GetLastInputEventTime() > (gs_SystemSetting.miScreenSave_time*1000U)))
		{
			DbgOutput(DBG_INFO, "Enter Phantom Saver!!\n");

			DelTimer(TIMERID_SCREENSAVER);

			gPageManager->SetCurrentPage(Page_PhantomPage);
		}
	}
}

void CPhantomPage::StartTimer()
{
	if ( !IsTimerSet(TIMERID_SCREENSAVER) )
	{
		AddTimer(TIMERID_SCREENSAVER, TIMER_INTERVAL);
	}
}
