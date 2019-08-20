#pragma once
#include "BaseButton.h"
#include "FragmentInterface.h"
#include "SongListCtrl.h"
#include "CloudDownLoadPromptWnd.h"


class CSongListFragment: public CFragment,
	public IClickEventListener
{
	typedef CFragment CParentClass;

public:
	CSongListFragment();
	virtual ~CSongListFragment();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	virtual void OnWindowVisible(	BOOL bVisible);

	virtual BOOL OnReturn();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void OnSearchItemChange();

	void OnSongItemStateChange(int nItemIndex);

	void OnListCtrlItemClicked(int nItemIndex, int nSubItemIndex);

	void OnListCtrlFirstItemChange(int nItemIndex);

	void OnListItemStateChange(	const char* cID);

public:
	void SetSearchBarItem(SEARCHITEM *pSearchItem, int nFirstShowItemIndex);
	void SetSongCount(int nSongCount);
	void AddSongItem(const SONGINFO *pSong, int nIndex);

	void SetPageCountInfo(int curPage, int pageCount);
  
   int SendClickSongID(const char* cSongID);

private:
	SEARCHITEM *mCurSearchType;
	CSongListCtrl mSongListCtrl;
  CCloudDownLoadPromptWnd	mCloudDownLoadPromptWnd;
  
	CBaseButton	mReturnBtn;
	CBaseButton mPrevPageBtn;
	CBaseWnd mPageInfoWnd;
	CBaseButton mNextPageBtn;

	uint32 mSongNameColor;
	uint32 mSingerNameColor;
	uint32 mSelectedColor;
};
