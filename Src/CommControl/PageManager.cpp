#include "PageManager.h"
#include "../GlobalUIClass.h"

CPageManager::CPageManager()
{
	mCurPageType = Page_MainCtrlPage;
}

CPageManager::~CPageManager()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CPageManager::InitPages()
{
	DbgOutput(DBG_INFO, "########InitPages start, the LangugageID:%d.\n", gs_SystemSetting.miLanguageID);
	theBaseApp->SetLanguageID((LANGUAGEID)gs_SystemSetting.miLanguageID);

	gMainCtrlPage->Create(theBaseApp->GetMainWindow());
	gPhantomPage->Create(theBaseApp->GetMainWindow());

	gMainCtrlPage->Show(TRUE);
	gPhantomPage->Show(TRUE);

	UINT64 uTime1 = GetTickCount();
	while (GetTickCount()-uTime1 <= 1000)
	{
		MESSAGE msg;
		if (theBaseApp->PeekMsg(NULL, WNDMSG_MIN, WNDMSG_MAX, PEEKMESSAGE_REMOVE, &msg))
		{
			theBaseApp->DispatchMsg(&msg);
		}
		else
		{
			Sleep(10);
		}
	}

	gMainCtrlPage->SetCurrentFragment(Fragment_HomePage);

//	gHomePageFragment->StartScrollEffect();
//	gHomePageFragment->WaitEffectOver();

	DbgOutput(DBG_INFO, "########InitPages end.\n");
}

void CPageManager::SetCurrentPage(
	PAGE_TYPE ePageType)
{
	CFragment *pPage = GetPageByID(ePageType);
	pPage->SwitchTo(mCurPageType, ePageType);

	ShowPage(ePageType);
}

void CPageManager::ReturnToLastPage()
{
	CFragment *pPage = GetPageByID(mCurPageType);
	PAGE_TYPE ePageType = (PAGE_TYPE)pPage->GetLastFragmentType();

	ShowPage(ePageType);
}

PAGE_TYPE CPageManager::GetCurPageType()
{
	return mCurPageType;
}

CFragment* CPageManager::GetCurPage()
{
	return GetPageByID(mCurPageType);
}

void CPageManager::ShowPage(
	PAGE_TYPE ePageType)
{
	DbgOutput(DBG_INFO, "Show Page %d\n", ePageType);

	mCurPageType = ePageType;
	switch(ePageType)
	{
	case Page_PhantomPage:
	{
		CBaseWnd *pActivateWnd = theBaseApp->GetActivateWnd();
		if (pActivateWnd && pActivateWnd->IsPopupWindow() && pActivateWnd->IsWindowVisible())
		{
			((CPopupSlideWnd *)pActivateWnd)->SetWindowVisible(FALSE);
		}
		gMainCtrlPage->Show(FALSE);
		gPhantomPage->SetWindowVisible(TRUE);
		gPhantomPage->SetFocus();
		break;
	}

	case Page_MainCtrlPage:
	{
		gPhantomPage->SetWindowVisible(FALSE);
		gMainCtrlPage->Show(TRUE);
		gMainCtrlPage->SetFocus();
	}
		break;

	default:
		break;
	}
}

CFragment* CPageManager::GetPageByID(
	PAGE_TYPE ePageType)
{
	CFragment *pPage = gMainCtrlPage;

	switch(ePageType)
	{
	case Page_MainCtrlPage:
		pPage = gMainCtrlPage;
		break;

	case Page_PhantomPage:
		pPage = gPhantomPage;
		break;

	default:
		break;
	}

	return pPage;
}
