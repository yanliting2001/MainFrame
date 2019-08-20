#include "PopupSlideWnd.h"
#include "../GlobalUIClass.h"

CPopupSlideWnd::CPopupSlideWnd()
{
	mpBindWnd = NULL;
}

CPopupSlideWnd::~CPopupSlideWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CPopupSlideWnd::Create(
	const char *cClassName,
	CBaseWnd *pParent,
	UINT32 uFlag,
	RECT rcShow,
	RECT rcHide,
	BOOL bInitShow,
	BOOL bAutoHideWhenInactive)
{
	mbAutoHideWhenInactive = bAutoHideWhenInactive;

#if 0
	CreateMaskWnd(pParent, bInitShow);
#endif

	CParentClass::Create(
		cClassName,
		pParent,
		uFlag|WINDOWFLAG_POPUP,
		rcShow,
		rcHide,
		bInitShow);
}

void CPopupSlideWnd::OnWindowVisible(
	BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);

	mMaskWnd.SetWindowVisible(bVisible);
}

void CPopupSlideWnd::OnWindowActivate(
	BOOL bActivate)
{
	CParentClass::OnWindowActivate(bActivate);

	if (!IsWindowVisible())
	{
		return;
	}

	if (!bActivate)
	{
		CBaseWnd *pFocusWnd = theBaseApp->GetFocusWnd();

		BOOL bInvisible = TRUE;

		if (pFocusWnd && mpBindWnd)
		{
			if ((mpBindWnd == pFocusWnd) ||
				mpBindWnd->IsMyChild(pFocusWnd))
			{
				bInvisible = FALSE;
			}
		}
		else if (pFocusWnd)
		{
			if (IsMyChild(pFocusWnd))
			{
				bInvisible = FALSE;
			}
		}

		if (bInvisible && mbAutoHideWhenInactive)
		{
			SlideShowWindow(FALSE);
		}
	}
}

void CPopupSlideWnd::OnSlideWndEnd(
	CBaseWnd *pWnd,
	BOOL bVisible,
	UINT64 uUserData)
{
	if (bVisible)
	{
		SetFocus();
	}

	if (mpOnSlideWndEndListener)
	{
		mpOnSlideWndEndListener->OnSlideWndEnd(this, bVisible, mUserData);
	}
}

void CPopupSlideWnd::SetBindWnd(
	CBaseWnd *pBindWnd)
{
	mpBindWnd = pBindWnd;
}

void CPopupSlideWnd::SlideShowWindow(
	BOOL bVisible,
	ISlideWndEndListener *pOnSlideWndEndListener,
	UINT64 uUserData,
	UINT64 uSlideMS)
{
	DbgOutput(DBG_DEBUG, "slide Show window(classname=%s, visible=%s)\n",
		GetClassName(), bVisible?"TRUE":"FALSE");

	if (mpBindWnd)
	{
		if ( (mpBindWnd->GetClassName() != NULL) && (strcmp(mpBindWnd->GetClassName(), CLASSNAME_BUTTON) == 0) )
		{
			((CBaseButton*)mpBindWnd)->SetRadioBtnPressed(bVisible);
		}
	}

	mpOnSlideWndEndListener = pOnSlideWndEndListener;
	mUserData = uUserData;

	CParentClass::SlideShowWindow(bVisible, this, 0, uSlideMS);
}

void CPopupSlideWnd::SetAutoHideWhenInactive(BOOL bAuto)
{
	mbAutoHideWhenInactive = bAuto;
}

void CPopupSlideWnd::CreateMaskWnd(
	CBaseWnd *pParent,
	BOOL bInitShow)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	mMaskWnd.SetZorderNumber(WINDOWS_ZORDER_MASKWNDPOPUP);
	mMaskWnd.CreateStatic(
		pParent,
		rc,
		WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP);

	CTexture bkTexture;

	CreateImgTexture("Mask", &bkTexture);
	mMaskWnd.SetBackgroundTexture(&bkTexture);
	//BYTE *pImageData = NULL;
	//CreateARGBColorBuffer(&pImageData, 1, 1, 0x7F0000FF);
	//bkTexture.CreateFromData(1, 1, 4, pImageData);

	mMaskWnd.SetBackgroundTexture(&bkTexture);

	pParent->ConvertScreenRectToWndRect(&rc, rc);
	mMaskWnd.MoveWindow(&rc);

	mMaskWnd.SetWindowVisible(bInitShow);
}

