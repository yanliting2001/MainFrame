#ifndef _CLOUDFRAGMENT_H
#define _CLOUDFRAGMENT_H
#include "FragmentInterface.h"
#include "../Effect/PopupSlideWnd.h"
#include "BaseButton.h"
#include "Song.h"

class CCloudFragment : public CFragment,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CFragment CParentClass;

public:
	CCloudFragment();
	virtual ~CCloudFragment();

public:
	void Create(CBaseWnd *pParent, SEARCHITEM *pRankTypeList);

	virtual void LoadResource();

	virtual void OnDestroy();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, 	POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);
private:
	CBaseButton	mReturnBtn;
	CBaseButton mShadowBtn;

	CBaseWnd	mCloudItemBKWnd;
	SEARCHITEM *mpCurSearchItemType;
	CPtrListCtrl mItemsList;

	RECT mrcBaseItem;
	int miXoffset;
	RECT mrcOrigItemsBKShow;

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;
};

#endif /* CCloudFragment */
