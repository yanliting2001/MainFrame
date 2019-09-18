#pragma once

#include "BaseButton.h"
#include "BaseSlide.h"
#include "SingerListCtrl.h"
#include "SearchTypeBar.h"
#include "FragmentInterface.h"

class CSingerListFragment: public CFragment,
	public IClickEventListener
{
	typedef CFragment CParentClass;

public:
	CSingerListFragment() { memset(mTypeName, 0x0, sizeof(mTypeName)); }

public:
	void Create(	CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	virtual void OnClick(CBaseWnd *pWnd, 	POINT pt);

	virtual BOOL OnReturn();

private:
	void OnPositionChange(CBaseWnd *pWnd,int nOldPosition,int nNewPostion,BOOL bChangeEnd);
	void OnSearchItemChange();

	void OnListCtrlItemClicked(int nItemIndex, int nSubItemIndex);

	void OnListCtrlFirstItemChange(int nItemIndex);

public:
	void SetSearchBarItem(SEARCHITEM *pSearchItem,int nFirstShowItemIndex);
	void SetSingerCount(	int nSingerCount);
	void AddSingerItem(const SINGERINFO *pSinger, int nIndex);

private:
	void UpdateSubSearchType(SEARCHITEM *pSearchItem);

	void SetPageCountInfo(int curPage, int pageCount);

private:
	CBaseButton	mReturnBtn;
	CSingerListCtrl mSingerListCtrl;
	CSearchTypeBar mSearchTypeBar;
	char mTypeName[32];

	CBaseButton mPrevPageBtn;
	CBaseWnd mPageInfoWnd;
	CBaseButton mNextPageBtn;
	CBaseWnd mTitleWnd;
};
