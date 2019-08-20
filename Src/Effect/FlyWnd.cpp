#include "FlyWnd.h"
#include "../GlobalUIClass.h"

CFlyWnd::CFlyWnd()
{
	mFlyScale = 1.0;
}

CFlyWnd::~CFlyWnd()
{
	RemoveAllFlyItem();
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CFlyWnd::OnDestroy()
{
	CParentClass::OnDestroy();

	RemoveAllFlyItem();
}

void CFlyWnd::OnSlideStep(
	CBaseWnd *pWnd,
	int nCurStep,
	int nTotalStep,
	UINT64 uUserData)
{
	mFlyScale = (float)nCurStep / (float)nTotalStep;

	mFlyItemListLock.Lock();

	int nCount = mFlyItemList.GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		FLYITEM *pFlyItem = (FLYITEM *)mFlyItemList.GetAt(i);
		if ( pFlyItem == NULL )
			continue;

		pFlyItem->fZorder = mZorderNumber + MANUAL_ZORDER_DELTA_BASE * (nCount-i) / nCount;

		DrawFlyItem(pFlyItem);
	}

	mFlyItemListLock.Unlock();

	if (mpOnSlideWndEndListener)
	{
		mpOnSlideWndEndListener->OnSlideStep(
			this, nCurStep,nTotalStep, mUserData);
	}
}

void CFlyWnd::OnSlideWndEnd(
	CBaseWnd *pWnd,
	BOOL bVisible,
	UINT64 uUserData)
{
	RemoveAllFlyItem();

	if (mpOnSlideWndEndListener)
	{
		mpOnSlideWndEndListener->OnSlideWndEnd(this, bVisible, mUserData);
	}

	//theBaseApp->ResumeInput();
}

void CFlyWnd::RemoveAllFlyItem()
{
	mFlyItemListLock.Lock();
	DelNormalList(&mFlyItemList, FLYITEM);
	mFlyItemListLock.Unlock();
}

void CFlyWnd::SlideShowWindow(
	BOOL bVisible,
	ISlideWndEndListener *pOnSlideWndEndListener,
	UINT64 uUserData,
	UINT64 uSlideMS)
{
	mpOnSlideWndEndListener = pOnSlideWndEndListener;
	mUserData = uUserData;

	mFlyItemListLock.Lock();

	int nCount = mFlyItemList.GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		FLYITEM *pFlyItem = (FLYITEM *)mFlyItemList.GetAt(i);
		if ( pFlyItem != NULL )
			pFlyItem->itemPictureBox.Show(TRUE);
	}

	mFlyItemListLock.Unlock();

	CParentClass::SlideShowWindow(bVisible, this, 0, uSlideMS);

	//theBaseApp->SuspendInput();
}

void CFlyWnd::DrawFlyItem(
	FLYITEM *pFlyItem)
{
	if ( pFlyItem == NULL )
		return ;

	RECT rcItem = pFlyItem->itemRect;

	rcItem.left *= mFlyScale;
	rcItem.top *= mFlyScale;
	rcItem.right *= mFlyScale;
	rcItem.bottom *= mFlyScale;

	RECT rcScreen;
	ConvertWndRectToScreenRect(&rcScreen, rcItem);

	pFlyItem->itemPictureBox.Draw(pFlyItem->fZorder, rcScreen, rcScreen);
}

void CFlyWnd::AddFlyItem(
	CTexture* pItemTexture,
	UINT32 uColor,
	RECT rcItem)
{
	if (!pItemTexture->HasTexture())
	{
		return;
	}

	FLYITEM *pFlyItem = new FLYITEM;
	if (!pFlyItem)
	{
		return;
	}

	CopyRect(&(pFlyItem->itemRect), &rcItem);

	pFlyItem->itemPictureBox.Create();
	pFlyItem->itemPictureBox.SetColor(uColor);
	pFlyItem->itemPictureBox.SetTexture(pItemTexture);

	mFlyItemListLock.Lock();
	mFlyItemList.AddData(pFlyItem);
	mFlyItemListLock.Unlock();
}

