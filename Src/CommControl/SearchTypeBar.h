#pragma once

#include "BaseWnd.h"
#include "../SQL/SearchSQL.h"

#define SEARCH_ITEM_COUNT	8

class CSearchTypeBar:public CBaseWnd,
	public IMsgRecvWndInterface,
	public ITouchEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CSearchTypeBar();
	~CSearchTypeBar();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rc);

	virtual void LoadResource();

	void LoadResourceForInternetTv();

public:
	virtual void OnDestroy();

	// interface
public:
	virtual void OnTouchDown(
		CBaseWnd *pWnd,
		POINT pt);
	virtual void OnTouchUp(
		CBaseWnd *pWnd,
		POINT pt,
		int xDistance,
		int yDistance,
		UINT64 uUsedTime);
	virtual void OnTouchMove(
		CBaseWnd *pWnd,
		POINT pt);
	virtual void OnTouchLeave(
		CBaseWnd *pOldTouchWnd,
		CBaseWnd *pNewTouchWnd);

public:
	void SetSearchItem(
		SEARCHITEM *pSearchItem,
		int nFirstShowItemIndex);
	SEARCHITEM * GetSelectedSubItem();

	SEARCHITEM * GetSelectedSubItemForSinger();

private:
	void SetDragFirstShowItem(
		int nFirstShowItem);
	void SetFirstShowItem(
		int nFirstShowItem);
	int SetSelectedIndex(
		int nSelectedIndex);
	void InitSubItem(
		int nFirstShowItem);
	void ShowSubItem(
		int nOffset);
	void GetSubItemRect(
		int nIndex,
		RECT *prc);
	int GetIndexByXPos(
		int xPos);

public:
	void SetOnClickValid(BOOL bValid);
private:
	BOOL mIsOnClickValid;

private:
	CPtrListCtrl mSubItemWndList;

	CBaseWnd mSelectedWnd;

	SEARCHITEM* mpCurSearchItem;
	int mFirstShowItemIndex;
	int mSelectedIndex;

	POINT mPtPressedScreen;
	int   mFirstShowItemWhenPressed;
};
