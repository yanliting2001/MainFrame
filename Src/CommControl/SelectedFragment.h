#ifndef _SELECTEDFRAGMENT_H
#define _SELECTEDFRAGMENT_H
#include "FragmentInterface.h"
#include "BaseButton.h"
#include "Song.h"

typedef enum _eSelectedTabType
{
	selectedTab_Selected=0,
	selectedTab_Singed,
	selectedTab_Count
} SELECTED_TAB_TYPE_S;

class CSelectedFragment : public CFragment,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CFragment CParentClass;

	enum _eItemCount
	{
		itemCount = 7,
	};

	typedef struct _sSelectedItem
	{
		CBaseWnd itemBKWnd;
		CBaseWnd songNameWnd;
		CBaseWnd singerWnd;
		CBaseButton rushBtn;
		CBaseButton deleteBtn;
		CBaseWnd selecteItemWnd;
	} SELECTED_ITEM_S;

public:
	CSelectedFragment();
	virtual ~CSelectedFragment();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnMsg(	UINT32 uType, UINT64 wParam, UINT64 lParam);
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, 	POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);

public:
	void SetSelectedType(SELECTED_TAB_TYPE_S type);

private:
private:
	void UpdateItemList(void);

	void SetSongInfo(int nIndex, const SONGINFO *pSongInfo);

	void UpdatePageInfo(void);

	void OnRushBtnClick(int nIndex);

	void OnDelBtnClick(int nIndex);

	void RefreshPlayingSinger(void);

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;
	int mFirstShowItem;

private:
	CBaseWnd mTabBKGroundWnd;
	CBaseWnd mTabImgWnd;

	SELECTED_ITEM_S mSelectedItems[itemCount];

  CBaseButton	mPlaySongBtn;
	CBaseButton	mReturnBtn;
	CBaseButton mPrevPageBtn;
	CBaseWnd mPageInfoWnd;
	CBaseButton mNextPageBtn;

	int miFirstItemIdx;
	int miSelectedCount;
	int miPageCount;

	SELECTED_TAB_TYPE_S mCurrentTab;
};

#endif /* _SELECTEDFRAGMENT_H */
