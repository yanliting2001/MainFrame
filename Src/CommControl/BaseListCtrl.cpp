#include "BaseListCtrl.h"
#include "../GlobalUIClass.h"

#define TIMERID_EFFECT	1

#define FAST_TOUCH

#ifdef FAST_TOUCH
#define SPEEDEFFECT_LIMITTIME 500
#define ROTATESPEEDEFFECT_LIMITTIME 100
#define ROTATE_SPEED 3100//6400
#define ROTATE_ANGLE 20
#define ROTATE_ANGLE_SPEED 100
#define SLIDE_SPEED 6400
#define ALIGN_SPEED 1000
#define FLY_MOVE_SPEED 15000
#define FLY_ROTATE_SPEED 120
#define FLY_START_HIGH 1800
#else
#define SPEEDEFFECT_LIMITTIME 500
#define ROTATESPEEDEFFECT_LIMITTIME 100
#define ROTATE_SPEED 2000
#define ROTATE_ANGLE 20
#define ROTATE_ANGLE_SPEED 30
#define SLIDE_SPEED 3000
#define ALIGN_SPEED 500
#define FLY_MOVE_SPEED 15000
#define FLY_ROTATE_SPEED 120
#define FLY_START_HIGH 1800
#endif

#define ITEM_PRESSED_SCALE 0.9

#ifdef DELAY_GET_ITEMDATA
LPVOID GetShowItemThreadFunc(LPVOID lpParam)
{

	//printf("the pthread of GetShowItemThreadProc ID is:%d.\n",syscall(SYS_gettid));
	CBaseListCtrl *pListCtrl = (CBaseListCtrl *)lpParam;
	pListCtrl->GetShowItemThreadProc();

	return NULL;
}
#endif

CBaseListCtrl::CBaseListCtrl()
#ifdef DELAY_GET_ITEMDATA
:mGetShowItemPauseEndEvent(TRUE)
#endif
{
	mItemWidth   = 100;
	mItemHeight  = 100;
	mItemXSpace  = 20;
	mItemYSpace  = 30;

	mRowCount = 4;
	mColCount = 2;
	mCountPerPage = 8;
	mCurFirstItem = -1;
	mItemCount = 0;

	mE3DListView = NULL;
	mpSlideEffector = NULL;
	mpFlyinEffector = NULL;
	mpFadeInEffector = NULL;
	mEffectType = touchEffectType_None;
	mEffectState = touchEffectState_Stopped;
}

CBaseListCtrl::~CBaseListCtrl()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CBaseListCtrl::Create(
	CBaseWnd *pParent, 
	RECT rc)
{
	CParentClass::Create(CLASSNAME_LISTVIEW, pParent, rc);

	RECT rcShow = {0, 0, 0, 0};
	RECT rcHide = {0, 0, 0, 0};
	mFlyWnd.SetZorderNumber(WINDOWS_ZORDER_FLYWND);
	mFlyWnd.Create(
		"FlyEffect",
		this,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE);

	ComputeItemPosition();

	RenewE3DListView();

#ifdef DELAY_GET_ITEMDATA
	mExitGetShowItemThread = FALSE;
	mPauseGetShowItemThread = FALSE;
	mGetShowItemThread.StartThread(GetShowItemThreadFunc, this, "GetShowItemThreadFunc");
#endif
}

void CBaseListCtrl::MoveWindow(
	RECT* prc)
{
	CParentClass::MoveWindow(prc);

	ComputeItemPosition();

	RenewE3DListView();

#ifdef DELAY_GET_ITEMDATA
	ResumeGetShowItemThread();
#endif
}

void CBaseListCtrl::OnDestroy()
{
#ifdef DELAY_GET_ITEMDATA
	mExitGetShowItemThread = TRUE;
	ResumeGetShowItemThread();
	mGetShowItemThread.StopThread();
#endif

	DestroyE3DListView();

	DelAllItem();

	CParentClass::OnDestroy();
}

void CBaseListCtrl::OnWindowVisible(
	BOOL bVisible)
{
	DbgOutput(DBG_DEBUG, "mE3DListView=0x%08x is %s\n",
		mE3DListView, bVisible ? "Visible":"Invisible");

	CParentClass::OnWindowVisible(bVisible);

	if (bVisible)
	{
		theE3DEngine->ShowUIControl(mE3DListView, true);
	}
	else
	{
		theE3DEngine->ShowUIControl(mE3DListView, false);
	}
}

void CBaseListCtrl::OnTimer(
	int nTimerID)
{
	CParentClass::OnTimer(nTimerID);

	if (TIMERID_EFFECT == nTimerID)
	{
		DbgOutput(DBG_DEBUG, "OnTimer mEffectType = %d, mEffectState=%d, mEffectStopTime=%d\n",
			mEffectType, mEffectState, mEffectStopTime);

		I3DListViewEffector *pEffector = mpSlideEffector;
		if (mEffectType == touchEffectType_Flyin)
		{
			pEffector = mpFlyinEffector;
		}
		else if (mEffectType == touchEffectType_FadeIn)
		{
			pEffector = mpFadeInEffector;
		}

		if (mEffectState == touchEffectState_Stopping)
		{
			if (theE3DEngine->IsEffectTimeOver(pEffector))
			{
				DbgOutput(DBG_DEBUG, "OnTimer mEffectType = %d, Stopped\n", mEffectType);

				DelTimer(TIMERID_EFFECT);

				if (mEffectType == touchEffectType_Flyin ||
					mEffectType == touchEffectType_FadeIn)
				{
					theE3DEngine->SetListViewEffect(mE3DListView, mpSlideEffector);

#ifdef DELAY_GET_ITEMDATA
					ResumeGetShowItemThread();
#endif
				}

				mEffectState = touchEffectState_Stopped;
				mEffectType = touchEffectType_None;

				int nBaseItem = theE3DEngine->GetListViewBaseItemIndex(mE3DListView);
				if (mCurFirstItem != (nBaseItem+mCountPerPage))
				{
					mCurFirstItem = nBaseItem+mCountPerPage;
					OnFirstItemIndexChanged();
				}

				if (!IsWindowVisible())
				{
					theE3DEngine->ShowUIControl(mE3DListView, FALSE);
				}
			}
			else
			{
				mEffectStopTime -= 200;

				if (mEffectStopTime <= 0)
				{
					DbgOutput(DBG_DEBUG, "StopEffect true\n");

					theE3DEngine->StopEffect(pEffector, true);
				}
			}
		}
		else if (mEffectState == touchEffectState_Running)
		{
			theE3DEngine->StopEffect(pEffector, false);

			if (mEffectType == touchEffectType_Speed)
			{
				mEffectStopTime = 800;
			}
			else if (mEffectType == touchEffectType_RotateSpeed)
			{
				mEffectStopTime = 200;
			}
			else if (mEffectType == touchEffectType_Flyin)
			{
				mEffectStopTime = 1000;
			}
			else if (mEffectType == touchEffectType_FadeIn)
			{
				mEffectStopTime = 1000;
			}
			else if (mEffectType == touchEffectType_Drag)
			{
				mEffectStopTime = 200;
			}
			mEffectState = touchEffectState_Stopping;

			DelTimer(TIMERID_EFFECT);
			AddTimer(TIMERID_EFFECT, 200);
		}
	}
}

void CBaseListCtrl::OnTouchDown(
	CBaseWnd *pWnd,
	POINT pt)
{
	DbgOutput(DBG_DEBUG, "%s: mEffectState=%d\n",
		__FUNCTION__, mEffectState);

	mIsTouchDown = FALSE;
	mIsItemMoved = FALSE;

	if (mItemList.GetCount() <= 0)
	{
		return;
	}

	if (mEffectState != touchEffectState_Stopped)
	{
		return;
	}

	mIsTouchDown = TRUE;
	mIsItemMoved = FALSE;
	mPtPressed = pt;
	mPtLastTouch = pt;

	int nIndex = GetItemByPoint(pt);

	if (nIndex >= 0)
	{
		RECT rcItem;
		GetItemRect(nIndex, &rcItem);

		pt.x -= rcItem.left;
		pt.y -= rcItem.top;

		SetItemPressed(nIndex);

		if (mpOperateProvider)
		{
			int nSubItem = mpOperateProvider->GetSubItemByPoint(pt);
			mpOperateProvider->OnItemPressed(nIndex, nSubItem);
		}
	}
}

void CBaseListCtrl::OnTouchUp(
	CBaseWnd *pWnd,
	POINT pt,
	int xDistance,
	int yDistance,
	UINT64 uUsedTime)
{
	DbgOutput(DBG_DEBUG, "%s: mEffectState=%d\n",
		__FUNCTION__, mEffectState);

	if (!mIsTouchDown)
	{
		return;
	}

	mIsTouchDown = FALSE;

	ClearItemPressed();

	if (mpOperateProvider)
	{
		mpOperateProvider->ClearItemPressed();
	}

	BOOL bNeedAlign = mIsItemMoved;

	if (abs(xDistance) < CLICK_MAX_DISTANCE &&
		abs(yDistance) < CLICK_MAX_DISTANCE)
	{
		int nIndex = GetItemByPoint(pt);
		if (nIndex >= 0)
		{
			RECT rcItem;
			GetItemRect(nIndex, &rcItem);

			pt.x -= rcItem.left;
			pt.y -= rcItem.top;

			if (mpOperateProvider)
			{
				int nSubItem = mpOperateProvider->GetSubItemByPoint(pt);
				mpOperateProvider->OnItemClicked(nIndex, nSubItem);
			}
		}
	}
	else
	{
		if ( StartSlideEffect(xDistance, uUsedTime) )
		{
			// NOT need align, slide effect will align when time over
			bNeedAlign = FALSE;
		}
	}

	if (bNeedAlign)
	{
		StartAlignSlideEffect(xDistance < 0 ? TRUE : FALSE);
	}
}

void CBaseListCtrl::OnTouchMove(
	CBaseWnd *pWnd,
	POINT pt)
{
	if (!mIsTouchDown)
	{
		return;
	}

	if (abs(pt.x-mPtPressed.x) < CLICK_MAX_DISTANCE)
	{
		return;
	}

	int xDistance = pt.x-mPtLastTouch.x;
	if (abs(xDistance) > 0)
	{
		DbgOutput(DBG_DEBUG, "%s: distance=%d, curpt(%d), downpt(%d)\n",
			__FUNCTION__, xDistance, pt.x, mPtPressed.x);
		WaitForEffectTimeOver();
		theE3DEngine->SetSlideEffectAttr(
			mpSlideEffector,
			true,
#ifdef FAST_TOUCH
			xDistance*4,
#else
			xDistance,
#endif
			0,
			0,
			0);
		theE3DEngine->StartEffect(mpSlideEffector);

		mPtLastTouch = pt;
		mIsItemMoved = TRUE;
	}
}

void CBaseListCtrl::OnTouchLeave(
	CBaseWnd *pOldTouchWnd,
	CBaseWnd *pNewTouchWnd)
{
	DbgOutput(DBG_DEBUG, "%s: mEffectState=%d\n",
		__FUNCTION__, mEffectState);

	if (!mIsTouchDown)
	{
		return;
	}

	mIsTouchDown = FALSE;

	ClearItemPressed();

	if (mpOperateProvider)
	{
		mpOperateProvider->ClearItemPressed();
	}

	if (mIsItemMoved)
	{
		StartAlignSlideEffect(TRUE);
	}
}

void CBaseListCtrl::SetEngine(I3DEngine* engine)
{

}

RESULT CBaseListCtrl::GetItemData(ITEM_DATA_TYPE type, void* data)
{
	if(NULL == data || itemDataListView != type)
	{
		return resultFailed;
	}

	RESULT ret = resultFailed;

	ITEM_DATA_LISTVIEW* currData = (ITEM_DATA_LISTVIEW*)data;

	currData->show = false;

	do
	{
		LISTITEM *pli = GetListItem(currData->itemIndex);
		if (!pli)
		{
			break;
		}

		if (currData->itemIndex < 0 ||
			currData->subItemIndex >= pli->nSubItemCount)
		{
			break;
		}

		if (currData->subItemIndex == 0 && mpOperateProvider)
		{
			DbgOutput(DBG_DEBUG, "GetItemData item(%d, %d)\n",
				currData->itemIndex, currData->subItemIndex);

			if ( (!pli->pSubItem) && (pli->nSubItemCount > 0) )
			{
				pli->pSubItem = new LISTSUBITEM[pli->nSubItemCount];
			}
			mpOperateProvider->GetFixedSubItemInfo(currData->itemIndex);

#ifdef DELAY_GET_ITEMDATA
			AddItemToShowList(currData->itemIndex);
			ResumeGetShowItemThread();
#else
			mpOperateProvider->GetUnFixedSubItemInfo(currData->itemIndex);
#endif
		}

		float zDelta = currData->subItemIndex+1;
		float fScaleX = theE3DEngine->GetXScale(WINDOWS_ZORDER_LISTVIEW_EFFECT - zDelta) ;
		float fScaleY = theE3DEngine->GetYScale(WINDOWS_ZORDER_LISTVIEW_EFFECT - zDelta) ;

		LISTSUBITEM *pSubItem = &(pli->pSubItem[currData->subItemIndex]);
		if ( pSubItem == NULL )
			break;

		UINT32 uColor = pSubItem->color;
		float fAlpha = (float)(((uColor&0xFF000000)>>24)/255.0);
		float fRed = (float)(((uColor&0x00FF0000)>>16)/255.0);
		float fGreen = (float)(((uColor&0x0000FF00)>>8)/255.0);
		float fBlue = (float)((uColor&0x000000FF)/255.0);

		currData->isFont = pSubItem->font ? true : false;
		currData->texture = pSubItem->texture.GetTextureID();
		currData->isDrawCircle = pSubItem->drawCircle ? true : false;

		if (!ISVALID_TEXTURE(currData->texture))
		{
			currData->show = false;
		}
		else
		{
			currData->show = true;

			currData->color = vec4(fRed, fGreen, fBlue, fAlpha);
			currData->position = vec3(
				pSubItem->rect.left * fScaleX,
				-pSubItem->rect.top * fScaleY,
				zDelta);
			currData->size = vec2(
				RECTWIDTH(pSubItem->rect) * fScaleX,
				RECTHEIGHT(pSubItem->rect) * fScaleY);
		}

		ret = resultOK;
	}
	while (0);

	return ret;
}

RESULT CBaseListCtrl::ReleaseItemData(ITEM_DATA_TYPE type, const void* data)
{
	if(NULL == data || itemDataListView != type)
	{
		return resultFailed;
	}

	ITEM_DATA_LISTVIEW* currData = (ITEM_DATA_LISTVIEW*)data;

	LISTITEM *pli = GetListItem(currData->itemIndex);

	do
	{
		if (!pli)
		{
			break;
		}

		if (currData->subItemIndex == 0)
		{
			DbgOutput(DBG_DEBUG, "ReleaseItemData item(%d, %d)\n",
				currData->itemIndex, currData->subItemIndex);

#ifdef DELAY_GET_ITEMDATA
			DelItemFromShowList(currData->itemIndex);
#endif
			pli->bItemDataAvailble = FALSE;
		}

		if (pli->pSubItem)
		{
			pli->pSubItem[currData->subItemIndex].texture.Delete();
		}

	} while (0);

	return resultOK;
}

void CBaseListCtrl::SetItemSize(
	int nWidth, 
	int nHeight)
{
	mItemWidth = nWidth;
	mItemHeight = nHeight;
}

void CBaseListCtrl::SetItemSpace(
	int nXSpace, 
	int nYSpace)
{
	mItemXSpace = nXSpace;
	mItemYSpace = nYSpace;
}

void CBaseListCtrl::GetItemRect(
	int nItemIndex,
	RECT *prc)
{
	SetRectXY(prc, 0, 0, 0, 0);

	int nItemStart = mCurFirstItem;
	int nItemEnd = mCurFirstItem + mRowCount * mColCount;

	if (nItemIndex >= nItemStart && nItemIndex <= nItemEnd)
	{
		int nRow = (nItemIndex - nItemStart) % mRowCount;
		int nCol = (nItemIndex - nItemStart) / mRowCount;

		prc->left = mListCtrlRect.left + nCol * (mItemWidth + mItemXSpace);
		prc->top = mListCtrlRect.top + nRow * (mItemHeight + mItemYSpace);
		prc->right = prc->left + mItemWidth;
		prc->bottom = prc->top + mItemHeight;
	}
}

int CBaseListCtrl::GetItemWidth()
{
	return mItemWidth;
}

int CBaseListCtrl::GetItemHeight()
{
	return mItemHeight;
}

int CBaseListCtrl::GetFirstItemIndex()
{
	return mCurFirstItem;
}

void CBaseListCtrl::SetFirstItemIndex(
	int nFirstItemIndex)
{
	if (!mE3DListView)
	{
		return;
	}

	WaitForEffectTimeOver();

	BOOL bChanged = (nFirstItemIndex != mCurFirstItem) ? TRUE : FALSE;
	mCurFirstItem = nFirstItemIndex;

	if (mCountPerPage > 0)
	{
		mCurPageIndex = (mCurFirstItem+mCountPerPage-1) / mCountPerPage;
	}
	else
	{
		mCurPageIndex = 0;
	}

	if (bChanged)
	{
		theE3DEngine->SetListViewBaseItemIndex(
			mE3DListView, mCurFirstItem-mCountPerPage);
	}

	OnFirstItemIndexChanged();
}

int CBaseListCtrl::GetCountPerPage()
{
	return mCountPerPage;
}

int CBaseListCtrl::GetPageCount()
{
	return mPageCount;
}

int CBaseListCtrl::GetCurPageIndex()
{
	return mCurPageIndex;
}

int CBaseListCtrl::GetItemCount()
{
	return mItemCount;
}

LISTITEM* CBaseListCtrl::GetListItem(
	int nItemIndex)
{
	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(nItemIndex);

	return pli;
}

void CBaseListCtrl::SetOperateProvider(
	IListCtrlOperateProvider *pOperateProvider)
{
	mpOperateProvider = pOperateProvider;
}

void CBaseListCtrl::RenewE3DListView()
{
	int nCurFirstItem = mCurFirstItem;

	DestroyE3DListView();

	int nMaxCount = Get2Max(mCountPerPage, mItemList.GetCount());

	RECT rcScreen ;
	ConvertWndRectToScreenRect(&rcScreen, mListCtrlRect);
	mE3DListView = theE3DEngine->CreateListView(
		WINDOWS_ZORDER_LISTVIEW_EFFECT,
		rcScreen,
		mpOperateProvider==NULL? 0 : mpOperateProvider->GetSubItemCount(),
		mRowCount,
		mColCount*3,
		-mCountPerPage,
		nMaxCount-1,
		mColCount,
		mItemWidth,
		mItemHeight,
		mItemXSpace,
		mItemYSpace,
		this,
		FALSE,
		TRUE);

	theE3DEngine->ShowUIControl(mE3DListView, IsWindowVisible());

	mpSlideEffector = theE3DEngine->CreateEffect(Effect_ListviewSliding);
	mpFlyinEffector = theE3DEngine->CreateEffect(Effect_ListviewFlying);
	mpFadeInEffector = theE3DEngine->CreateEffect(Effect_ListviewFadeIn);

	SetFirstItemIndex(nCurFirstItem);

	theE3DEngine->SetListViewEffect(mE3DListView, mpSlideEffector);

	mEffectType = touchEffectType_None;
	mEffectState = touchEffectState_Stopped;

	SetOnTouchListener(this);

	DbgOutput(DBG_DEBUG, "mE3DListView=0x%08x is %s, the item size:%d:%d\n",
		mE3DListView, IsWindowVisible() ? "Visible":"Invisible", rcScreen.right, rcScreen.bottom);
}

void CBaseListCtrl::DestroyE3DListView()
{
	if (mE3DListView)
	{
		WaitForEffectTimeOver();

#ifdef DELAY_GET_ITEMDATA
		PauseGetShowItemThread();

		mItemShowListLock.Lock();
		mItemShowList.DeleteAll();
		mItemShowListLock.Unlock();
#endif

		SetOnTouchListener(NULL);

		theE3DEngine->StopEffect(mpSlideEffector, true);
		theE3DEngine->StopEffect(mpFlyinEffector, true);
		theE3DEngine->StopEffect(mpFadeInEffector, true);

		theE3DEngine->SetListViewEffect(mE3DListView, NULL);
		theE3DEngine->DestroyEffect(mpSlideEffector);
		theE3DEngine->DestroyEffect(mpFlyinEffector);
		theE3DEngine->DestroyEffect(mpFadeInEffector);
		mpSlideEffector = NULL;
		mpFlyinEffector = NULL;
		mpFlyinEffector = NULL;

		theE3DEngine->DestroyListView(mE3DListView);

		mE3DListView = NULL;

		mEffectType = touchEffectType_None;
		mEffectState = touchEffectState_Stopped;

		mCurFirstItem = 0;
	}
}

void CBaseListCtrl::SetItemCount(
	int nItemCount)
{
	mItemListLock.Lock();

	int nSubItemCount = 0;
	if (mpOperateProvider)
	{
		nSubItemCount = mpOperateProvider->GetSubItemCount();
	}

	int i;
	for ( i = 0 ; i < nItemCount ; i++ )
	{
		LISTITEM *pli = new LISTITEM;
		if (!pli)
		{
			continue;
		}

		memset(pli, 0, sizeof(LISTITEM));

		mItemList.AddData(pli);

		if (mpOperateProvider)
		{
			if (nSubItemCount > 0)
			{
				pli->nSubItemCount = nSubItemCount;
				pli->bItemDataAvailble = FALSE;
			}
		}
	}

	mItemListLock.Unlock();

	mItemCount = nItemCount;

	mPageCount = (mItemCount+mCountPerPage-1) / mCountPerPage;
	mCurPageIndex =  (mCurFirstItem+mCountPerPage-1) / mCountPerPage;
}

BOOL CBaseListCtrl::SetItemData(
	int nItemIndex,
	UINT64 uItemData)
{
	BOOL bSetOK = FALSE;

	mItemListLock.Lock();
	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(nItemIndex);
	mItemListLock.Unlock();

	if (pli)
	{
		if (pli->uItemData && mpOperateProvider)
		{
			mpOperateProvider->OnDeleteItem(pli);
		}

		pli->uItemData = uItemData;

		bSetOK = TRUE;
	}

	return bSetOK;
}

void CBaseListCtrl::DelItem(
	int nItemIndex)
{
	LISTITEM *pli = (LISTITEM *)mItemList.DeleteAt(nItemIndex);
	if ( pli == NULL )
		return ;

	int nCount = mItemList.GetCount();

	if (mpOperateProvider)
	{
		mpOperateProvider->OnDeleteItem(pli);
	}

	if ( pli->pSubItem )
	{
		if ( pli->nSubItemCount > 0 )
			delete []pli->pSubItem;
		else
			pli->pSubItem = NULL;
	}

	delete pli;

	if (nCount == 0)
	{
		mCurFirstItem = 0;
		OnFirstItemIndexChanged();
	}
}

void CBaseListCtrl::DelAllItem()
{
	WaitForEffectTimeOver();

	mItemListLock.Lock();
	int nCount = mItemList.GetCount();
	mItemListLock.Unlock();

	int i;
	for ( i = nCount-1 ; i >= 0 ; i-- )
	{
		DelItem(i);
	}

	mCurFirstItem = 0;
}

int CBaseListCtrl::FindItem(
	UINT64 uFindData,
	LISTITEMFINDFUNC pfnFind)
{
	int nIndex = -1;

	mItemListLock.Lock();

	int nCount = mItemList.GetCount();

	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		const LISTITEM *pli = (LISTITEM *)mItemList.GetAt(i);
		if ( pli == NULL)
			continue;

		if ( pfnFind(pli, (const void *)uFindData) )
		{
			nIndex = i;
			break;
		}
	}

	mItemListLock.Unlock();

	return nIndex;
}

int CBaseListCtrl::GetItemByPoint(
	POINT pt)
{
	if (!PtInRect(&mListCtrlRect, pt))
	{
		return -1;
	}

	pt.x -= mListCtrlRect.left;
	pt.y -= mListCtrlRect.top;

	// pt in space
	if (pt.x % (mItemWidth + mItemXSpace) > mItemWidth ||
		pt.y % (mItemHeight + mItemYSpace) > mItemHeight)
	{
		return -1;
	}

	int nCol = pt.x / (mItemWidth + mItemXSpace);
	int nRow = pt.y / (mItemHeight + mItemYSpace);

	int nItemIndex = mCurFirstItem + mRowCount * nCol + nRow;

	return nItemIndex;
}

void CBaseListCtrl::SetItemTexture(
	int nItemIndex,
	int nSubItemIndex,
	CTexture *pTexture)
{
	mItemListLock.Lock();
	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(nItemIndex);
	mItemListLock.Unlock();

	if (!pli || !pli->pSubItem)
	{
		return;
	}

	if (nSubItemIndex < 0 || nSubItemIndex >= pli->nSubItemCount)
	{
		return;
	}

	LISTSUBITEM *pSubItem = &(pli->pSubItem[nSubItemIndex]);
	if ( pSubItem == NULL )
		return;

	pSubItem->texture.SetTexture(pTexture);

	theE3DEngine->SetListViewItemTexture(
		mE3DListView,
		nItemIndex,
		nSubItemIndex,
		pli->pSubItem[nSubItemIndex].texture.GetTextureID());
}

void CBaseListCtrl::SetItemColor(
	int nItemIndex,
	int nSubItemIndex,
	UINT32 uARGB)
{
	mItemListLock.Lock();
	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(nItemIndex);
	mItemListLock.Unlock();

	if (!pli || !pli->pSubItem)
	{
		return;
	}

	if (nSubItemIndex < 0 || nSubItemIndex >= pli->nSubItemCount)
	{
		return;
	}

	LISTSUBITEM *pSubItem = &(pli->pSubItem[nSubItemIndex]);
	if ( pSubItem == NULL )
		return ;

	pSubItem->color = uARGB;

	theE3DEngine->SetListViewItemColor(
		mE3DListView,
		nItemIndex,
		nSubItemIndex,
		pli->pSubItem[nSubItemIndex].color);
}

void CBaseListCtrl::SetItemPosition(
	int nItemIndex,
	int nSubItemIndex,
	int nLeft,
	int nTop,
	int nWidth,
	int nHeight)
{
	mItemListLock.Lock();
	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(nItemIndex);
	mItemListLock.Unlock();

	if (!pli || !pli->pSubItem)
	{
		return;
	}

	if (nSubItemIndex < 0 || nSubItemIndex >= pli->nSubItemCount)
	{
		return;
	}

	LISTSUBITEM *pSubItem = &(pli->pSubItem[nSubItemIndex]);
	SetRectXY(&(pSubItem->rect), nLeft, nTop, nWidth, nHeight);

	theE3DEngine->SetListViewItemPosition(
		mE3DListView,
		nItemIndex,
		nSubItemIndex,
		nSubItemIndex+1,
		nLeft,
		nTop,
		nWidth,
		nHeight);
}

BOOL CBaseListCtrl::StartSlideEffect(
	int xDistance,
	UINT64 uUsedTime)
{
	DbgOutput(DBG_DEBUG, "StartSlideEffect move distance = %d, used time = %llu ms\n",
		xDistance, uUsedTime);

	if ((abs(xDistance) < 200) && (abs(xDistance) > 20) && (uUsedTime < 200))
	{
		StartRotateSlideEffect(xDistance < 0 ? TRUE : FALSE);
		return TRUE;
	}
	//if ((abs(xDistance) > 100) && (uUsedTime < 500))  //orig
	if ( (abs(xDistance) > 150) )
	{
		StartSpeedSlideEffect(xDistance < 0 ? TRUE : FALSE);
		return TRUE;
	}

	return FALSE;
}

void CBaseListCtrl::StartRotateSlideEffect(
	BOOL bSlideLeft)
{
	if (mItemList.GetCount() <= 0)
	{
		return;
	}

	WaitForEffectTimeOver();

	if (bSlideLeft)
	{
		theE3DEngine->SetSlideEffectAttr(
			mpSlideEffector,
			false,
			0,
			-ROTATE_SPEED,
			-ROTATE_ANGLE, -ROTATE_ANGLE_SPEED);
	}
	else
	{
		theE3DEngine->SetSlideEffectAttr(
			mpSlideEffector,
			false,
			0,
			ROTATE_SPEED,
			ROTATE_ANGLE,
			ROTATE_ANGLE_SPEED);
	}

	theE3DEngine->StartEffect(mpSlideEffector);

	mEffectType = touchEffectType_RotateSpeed;
	mEffectState = touchEffectState_Running;
	AddTimer(TIMERID_EFFECT, 700);
}

void CBaseListCtrl::StartSpeedSlideEffect(
	BOOL bSlideLeft)
{
	if (mItemList.GetCount() <= 0)
	{
		return;
	}

	WaitForEffectTimeOver();

	theE3DEngine->SetSlideEffectAttr(
		mpSlideEffector,
		false,
		0,
		bSlideLeft?-SLIDE_SPEED:SLIDE_SPEED,
		0,
		0);

	theE3DEngine->StartEffect(mpSlideEffector);

	mEffectType = touchEffectType_Speed;
	mEffectState = touchEffectState_Running;
	AddTimer(TIMERID_EFFECT, 200);
}

void CBaseListCtrl::StartAlignSlideEffect(
	BOOL bSlideLeft)
{
	if (mItemList.GetCount() <= 0)
	{
		return;
	}

	WaitForEffectTimeOver();

	DbgOutput(DBG_DEBUG, "StartAlignEffect %llu\n", GetTickCount());

	theE3DEngine->SetSlideEffectAttr(
		mpSlideEffector,
		false,
		0,
		ALIGN_SPEED,
		0,
		0);
	theE3DEngine->StartEffect(mpSlideEffector);

	mEffectType = touchEffectType_Drag;
	mEffectState = touchEffectState_Running;
	AddTimer(TIMERID_EFFECT, 300);
}

void CBaseListCtrl::StartFlyinEffect()
{
	WaitForEffectTimeOver();

	if (mItemCount <= 0)
	{
		return;
	}

	DbgOutput(DBG_DEBUG, "StartFlyinEffect, %llu\n", GetTickCount());

	theE3DEngine->SetFlyinEffectAttr(
		mpFlyinEffector,
		FLY_MOVE_SPEED,
		FLY_ROTATE_SPEED,
		true,
		FLY_START_HIGH);
	theE3DEngine->SetListViewEffect(mE3DListView, mpFlyinEffector);
	theE3DEngine->StartEffect(mpFlyinEffector);

	mEffectType = touchEffectType_Flyin;
	mEffectState = touchEffectState_Running;
	AddTimer(TIMERID_EFFECT, 1000);
}

void CBaseListCtrl::StartFadeInEffect()
{
	WaitForEffectTimeOver();

	if (mItemCount <= 0)
	{
		return;
	}

	DbgOutput(DBG_DEBUG, "StartFadeInEffect, %llu\n", GetTickCount());

	theE3DEngine->SetListViewEffect(mE3DListView, mpFadeInEffector);
	theE3DEngine->StartEffect(mpFadeInEffector);

	mEffectType = touchEffectType_FadeIn;
	mEffectState = touchEffectState_Running;
	AddTimer(TIMERID_EFFECT, 500);
}

void CBaseListCtrl::DumpMsg(
	UINT32 uTypeMin,
	UINT32 uTypeMax,
	UINT64 uTime)
{
	DbgOutput(DBG_DEBUG, "DumpMsg Start, effect type is %d\n", mEffectType);

	UINT64 uTime1 = GetTickCount();

	while (GetTickCount()-uTime1 <= uTime)
	{
		if (mEffectType == touchEffectType_None)
		{
			break;
		}

		MESSAGE msg;
		if (theBaseApp->PeekMsg(this, uTypeMin, uTypeMax, PEEKMESSAGE_REMOVE, &msg))
		{
			theBaseApp->DispatchMsg(&msg);
		}
		else
		{
			Sleep(50);
		}
	}

	DbgOutput(DBG_DEBUG, "DumpMsg End, effect type is %d, used %llu ms\n",
		mEffectType, GetTickCount()-uTime1);
}

void CBaseListCtrl::WaitForEffectTimeOver()
{
	if (mEffectType != touchEffectType_None)
	{
		theBaseApp->SuspendInput();

		DumpMsg(WNDMSG_TIMER, WNDMSG_TIMER, 5000);

		theBaseApp->ResumeInput();
	}
}

void CBaseListCtrl::StartFlyItem(
	int nItemIndex)
{
	LISTITEM *pli = GetListItem(nItemIndex);
	if (!pli || !pli->pSubItem)
	{
		return;
	}

	mFlyWnd.StopSlideWindow();

	RECT rcStart;
	GetItemRect(nItemIndex, &rcStart);
	RECT rcEnd;
	SetRectXY(&rcEnd, LAYOUT_WIDTH, LAYOUT_HEIGHT, mItemWidth, mItemHeight);
	mFlyWnd.MoveWindow(rcStart, rcEnd);

//	SetRectXY(&rcEnd, LAYOUT_WIDTH, LAYOUT_HEIGHT, 1, 1);
//	this->ConvertScreenRectToWndRect(&rcEnd, rcEnd);
//	mFlyWnd.MoveWindow(rcStart, rcEnd);

//	mFlyWnd.RemoveAllFlyItem();

	int i;
	for ( i = 0 ; i < pli->nSubItemCount ; i++ )
	{
		LISTSUBITEM *pSubItem = &(pli->pSubItem[i]);

		mFlyWnd.AddFlyItem(
			&(pSubItem->texture),
			pSubItem->color,
			pSubItem->rect);
	}

	mFlyWnd.SlideShowWindow(FALSE);
}

void CBaseListCtrl::SetItemPressed(
	int nItemIndex)
{
	mItemListLock.Lock();

	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(nItemIndex);

	mItemListLock.Unlock();

	if (pli && pli->pSubItem)
	{
		int i;
		for ( i = 0 ; i < pli->nSubItemCount ; i++ )
		{
			RECT rcItem = pli->pSubItem[i].rect;

			int nWidth = RECTWIDTH(rcItem)*ITEM_PRESSED_SCALE;
			int nHeight = RECTHEIGHT(rcItem)*ITEM_PRESSED_SCALE;
			rcItem.left = mItemWidth*(1-ITEM_PRESSED_SCALE)/2+rcItem.left*ITEM_PRESSED_SCALE;
			rcItem.top = mItemHeight*(1-ITEM_PRESSED_SCALE)/2+rcItem.top*ITEM_PRESSED_SCALE;

			theE3DEngine->SetListViewItemPosition(
				mE3DListView,
				nItemIndex,
				i,
				i+1,
				rcItem.left,
				rcItem.top,
				nWidth,
				nHeight);

			mItemPressed = nItemIndex;
		}
	}
}

void CBaseListCtrl::ClearItemPressed()
{
	mItemListLock.Lock();

	LISTITEM *pli = (LISTITEM *)mItemList.GetAt(mItemPressed);

	mItemListLock.Unlock();

	if (pli && pli->pSubItem)
	{
		int i;
		for ( i = 0 ;  i < pli->nSubItemCount ; i++ )
		{
			RECT rcItem = pli->pSubItem[i].rect;

			int nWidth = RECTWIDTH(rcItem);
			int nHeight = RECTHEIGHT(rcItem);

			theE3DEngine->SetListViewItemPosition(
				mE3DListView,
				mItemPressed,
				i,
				i+1,
				rcItem.left,
				rcItem.top,
				nWidth,
				nHeight);
		}
	}
}

void CBaseListCtrl::ComputeItemPosition()
{
	int nWidth = RECTWIDTH(mWndRect) ;
	int nHeight = RECTHEIGHT(mWndRect) ;

	mColCount = nWidth / (mItemWidth + mItemXSpace) ;
	mRowCount = nHeight  / (mItemHeight + mItemYSpace) ;
	mCountPerPage = mColCount * mRowCount;
	DbgOutput(DBG_INFO, "ItemPosition nWidth=%d,nHeight=%d, mColCount: %d,mRowCount:%d,mCountPerPage:%d\n",nWidth,nHeight,mColCount,mRowCount,mCountPerPage);
	mListCtrlRect.left = mWndRect.left;
	mListCtrlRect.top = mWndRect.top;
	mListCtrlRect.right = mListCtrlRect.left + mColCount *  (mItemWidth + mItemXSpace) ;
	mListCtrlRect.bottom = mListCtrlRect.top + mRowCount *  (mItemHeight + mItemYSpace) ;
}

void CBaseListCtrl::OnFirstItemIndexChanged()
{
	mCurPageIndex =  (mCurFirstItem+mCountPerPage-1) / mCountPerPage;

	if (mpOperateProvider)
	{
		mpOperateProvider->OnFirstItemIndexChanged(mCurFirstItem);
	}

#ifdef DELAY_GET_ITEMDATA
	ResumeGetShowItemThread();
#endif
}

int FindShowItemByIndex(
	const void *pItem,
	const void *pData)
{
	UINT64 uIndexValue = (UINT64)pItem ;
	UINT64 uIndexFind = (UINT64)pData ;

	if (uIndexFind == uIndexValue)
	{
		return 1 ;
	}

	return 0 ;
}

#ifdef DELAY_GET_ITEMDATA
void CBaseListCtrl::AddItemToShowList(
	UINT32 uIndex)
{
	mItemShowListLock.Lock();

	if ( mItemShowList.FindFirst((const void *)((UINT64)uIndex), FindShowItemByIndex) < 0 )
	{
		mItemShowList.AddData((const void *)((UINT64)uIndex));
		DbgOutput(DBG_DEBUG, "Add Show Item index=%lu\n", uIndex);
	}
	else
	{
		DbgOutput(DBG_DEBUG, "Add Show Item index=%lu ALREADY exist\n", uIndex);
	}

	mItemShowListLock.Unlock();
}

void CBaseListCtrl::DelItemFromShowList(
	UINT32 uIndex)
{
	mItemShowListLock.Lock();

	int nIndex = mItemShowList.FindFirst((const void *)((UINT64)uIndex), FindShowItemByIndex);
	if (nIndex >= 0)
	{
		mItemShowList.DeleteAt(nIndex);
		DbgOutput(DBG_DEBUG, "Delete Show Item index=%lu\n", uIndex);
	}
	else
	{
		DbgOutput(DBG_DEBUG, "Delete Show Item index=%lu NOT found\n", uIndex);
	}

	mItemShowListLock.Unlock();
}

void CBaseListCtrl::GetShowItemThreadProc()
{
	while (!mExitGetShowItemThread)
	{
		UINT32 uIndex = 0xFFFFFFFF;

		mGetShowItemPauseEndEvent.Set();

		DbgOutput(DBG_DEBUG, "Waiting Get show item Info...\n");

		mGetShowItemEvent.Wait();

		mGetShowItemPauseEndEvent.Reset();

		while (!mPauseGetShowItemThread && !mExitGetShowItemThread)
		{
			DbgOutput(DBG_DEBUG, "Wait OK\n");

			mItemShowListLock.Lock();
			if (mItemShowList.GetCount() > 0)
			{
				uIndex = (UINT64)mItemShowList.DeleteHead();
				mItemShowListLock.Unlock();
			}
			else
			{
				mItemShowListLock.Unlock();
				break;
			}

			DbgOutput(DBG_DEBUG, "Prepare to get show item\n");

			UINT64 uTime1 = GetTickCount();

			LISTITEM *pli = GetListItem(uIndex);
			if (!pli || !pli->pSubItem)
			{
				continue;
			}

			if (pli->bItemDataAvailble)
			{
				continue;
			}

			pli->bItemDataAvailble = TRUE;

			if (mpOperateProvider)
			{
				mpOperateProvider->GetUnFixedSubItemInfo(uIndex);
			}

			UINT64 uTime2 = GetTickCount();

			int i;
			for ( i = 0 ; i < pli->nSubItemCount ; i++ )
			{
				LISTSUBITEM *pSubItem = &(pli->pSubItem[i]);

				SetItemTexture(uIndex, i, &(pSubItem->texture));

				SetItemColor(uIndex, i, pSubItem->color);

				SetItemPosition(
					uIndex,
					i,
					pSubItem->rect.left,
					pSubItem->rect.top,
					RECTWIDTH(pSubItem->rect),
					RECTHEIGHT(pSubItem->rect));
			}

			UINT64 uTime3 = GetTickCount();

			DbgOutput(DBG_DEBUG, "Show Item %lu, total %d, get used %llums, draw used %llu ms\n",
				uIndex, mItemShowList.GetCount(), uTime2-uTime1, uTime3-uTime2);
		}

		mGetShowItemPauseEndEvent.Set();
	}
}

void CBaseListCtrl::ResumeGetShowItemThread()
{
	mPauseGetShowItemThread = FALSE;
	mGetShowItemEvent.Set();
}

void CBaseListCtrl::PauseGetShowItemThread()
{
	mPauseGetShowItemThread = TRUE;
	mGetShowItemEvent.Reset();

	mGetShowItemPauseEndEvent.Wait();
}
#endif

void CBaseListCtrl::EnableScissor(RECT rcScissor)
{
	if ( mE3DListView )
	{
		theE3DEngine->EnableScissor(mE3DListView, rcScissor);
	}
}
