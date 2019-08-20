#pragma once

#include "BaseWnd.h"
#include "../Effect/FlyWnd.h"

#define DELAY_GET_ITEMDATA

typedef struct tagLISTSUBITEM
{
	CTexture texture;
	UINT32 color;
	RECT rect;
	BOOL font;
	BOOL drawCircle;
} LISTSUBITEM ;

typedef struct tagLISTITEM
{
	UINT32 uItemState;
	UINT64 uItemData;

	int nSubItemCount;
	LISTSUBITEM *pSubItem;

	BOOL bItemDataAvailble;
} LISTITEM ;

typedef enum eTOUCH_EFFECT_TYPE
{
	touchEffectType_None=0,
	touchEffectType_Drag,
	touchEffectType_Speed,
	touchEffectType_RotateSpeed,
	touchEffectType_Flyin,
	touchEffectType_FadeIn,
} TOUCH_EFFECT_TYPE;

typedef enum eTOUCH_EFFECT_STATE
{
	touchEffectState_Stopped=0,
	touchEffectState_Running,
	touchEffectState_Stopping
} TOUCH_EFFECT_STATE;

class IListCtrlOperateProvider
{
public:
	virtual void OnDeleteItem(
		LISTITEM *pli)
	{}
	virtual void OnItemClicked(
		int nItemIndex,
		int nSubItemIndex)
	{}
	virtual void OnItemPressed(
		int nItemIndex,
		int nSubItemIndex)
	{}
	virtual void ClearItemPressed()
	{}
	virtual void OnFirstItemIndexChanged(
		int nNewFirstItemIndex)
	{}
	virtual int GetSubItemCount()=0;
	virtual int GetSubItemByPoint(
		POINT pt)=0;
	virtual void GetFixedSubItemInfo(
		int nItemIndex)=0;
	virtual void GetUnFixedSubItemInfo(
		int nItemIndex)=0;
};

typedef int (*LISTITEMFINDFUNC)(const LISTITEM *pItem, const void *pData) ;

class CBaseListCtrl: public CBaseWnd,
	public I3DListViewDataProvider,
	public ITouchEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CBaseListCtrl();
	virtual ~CBaseListCtrl();

public:
	void Create(
		CBaseWnd *pParent, 
		RECT rc);

	void MoveWindow(
		RECT* prc);

public:
	virtual void OnDestroy();

	virtual void OnWindowVisible(
		BOOL bVisible);

	virtual void OnTimer(
		int nTimerID);

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

	virtual void SetEngine(
		I3DEngine* engine);
	virtual RESULT GetItemData(
		ITEM_DATA_TYPE type,
		void* data);
	virtual RESULT ReleaseItemData(
		ITEM_DATA_TYPE type,
		const void* data);

public:
	void SetOperateProvider(
		IListCtrlOperateProvider *pOperateProvider);
	void RenewE3DListView();
	void DestroyE3DListView();
	void SetItemCount(
		int nItemCount);
	BOOL SetItemData(
		int nItemIndex,
		UINT64 uItemData);
	void DelItem(
		int nItemIndex);
	void DelAllItem();
	int FindItem(
		UINT64 uFindData,
		LISTITEMFINDFUNC pfnFind);

public:
	void SetItemSize(
		int nWidth, 
		int nHeight);
	void SetItemSpace(
		int nXSpace, 
		int nYSpace);

	void GetItemRect(
		int nItemIndex,
		RECT *prc);
	int GetItemWidth();
	int GetItemHeight();
	int GetFirstItemIndex();
	void SetFirstItemIndex(
		int nFirstItemIndex);
	int GetCountPerPage();
	int GetPageCount();
	int GetCurPageIndex();
	int GetItemCount();
	LISTITEM* GetListItem(
		int nItemIndex);
	int GetItemByPoint(
		POINT pt);
	void SetItemTexture(
		int nItemIndex,
		int nSubItemIndex,
		CTexture *pTexture);
	void SetItemColor(
		int nItemIndex,
		int nSubItemIndex,
		UINT32 uARGB);
	void SetItemPosition(
		int nItemIndex,
		int nSubItemIndex,
		int nLeft,
		int nTop,
		int nWidth,
		int nHeight);

	BOOL StartSlideEffect(
		int xDistance,
		UINT64 uUsedTime);
	void StartRotateSlideEffect(
		BOOL bSlideLeft);
	void StartSpeedSlideEffect(
		BOOL bSlideLeft);
	void StartAlignSlideEffect(
		BOOL bSlideLeft);
	void StartFlyinEffect();
	void StartFadeInEffect();

	void DumpMsg(
		UINT32 uTypeMin,
		UINT32 uTypeMax,
		UINT64 uTime);
	void WaitForEffectTimeOver();

	void StartFlyItem(
		int nItemIndex);

	void SetItemPressed(
		int nItemIndex);
	void ClearItemPressed();

public:
	void EnableScissor(RECT rcScissor);

private:
	void ComputeItemPosition();
	void OnFirstItemIndexChanged();

#ifdef DELAY_GET_ITEMDATA
public:
	void GetShowItemThreadProc();
private:
	void AddItemToShowList(
		UINT32 uIndex);
	void DelItemFromShowList(
		UINT32 uIndex);

	void PauseGetShowItemThread();
	void ResumeGetShowItemThread();

private:
	CPtrListCtrl mItemShowList;
	CBaseLock	 mItemShowListLock;
	BOOL 		 mExitGetShowItemThread;
	BOOL    	 mPauseGetShowItemThread;
	CBaseThread  mGetShowItemThread;
	CBaseEvent   mGetShowItemEvent;
	CBaseEvent   mGetShowItemPauseEndEvent;
#endif

private:
	RECT		mListCtrlRect;
	int			mItemWidth;
	int          mItemHeight;

	int          mItemXSpace;
	int          mItemYSpace;

	int          mRowCount;
	int          mColCount;
	int			 mCountPerPage;
	int			 mPageCount;
	int          mCurFirstItem;
	int          mCurPageIndex;
	int  		 mItemCount;

	int  		 mItemPressed;

	CPtrArrayCtrl mItemList;
	CBaseLock	 mItemListLock;

	IListCtrlOperateProvider* mpOperateProvider;

private:
	I3DListView* mE3DListView;
	I3DListViewEffector *mpSlideEffector;
	I3DListViewEffector *mpFlyinEffector;
	I3DListViewEffector *mpFadeInEffector;
	TOUCH_EFFECT_TYPE mEffectType;
	TOUCH_EFFECT_STATE mEffectState;
	int mEffectStopTime;

	BOOL mIsTouchDown;
	BOOL mIsItemMoved;
	POINT mPtPressed;
	POINT mPtLastTouch;

private:
	CFlyWnd mFlyWnd;
};
