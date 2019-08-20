#pragma once

#include "BaseWnd.h"
#include "SlideWnd.h"

typedef struct tagFLYITEM
{
	RECT itemRect;
	CPictureBox itemPictureBox;
	float fZorder;
} FLYITEM;

class CFlyWnd: public CSlideWnd,
	public ISlideWndEndListener
{
	typedef CSlideWnd CParentClass;

public:
	CFlyWnd();
	virtual ~CFlyWnd();

public:
	virtual void OnDestroy();

	// interface
public:
	virtual void OnSlideStep(
		CBaseWnd *pWnd,
		int nCurStep,
		int nTotalStep,
		UINT64 uUserData);
	virtual void OnSlideWndEnd(
		CBaseWnd *pWnd,
		BOOL bVisible,
		UINT64 uUserData);

public:
	void AddFlyItem(
		CTexture* pItemTexture,
		UINT32 uColor,
		RECT rcItem);
	void RemoveAllFlyItem();

	void SlideShowWindow(
		BOOL bVisible,
		ISlideWndEndListener *pOnSlideWndEndListener=NULL,
		UINT64 uUserData=0,
		UINT64 uSlideMS=600);

private:
	void DrawFlyItem(
		FLYITEM *pFlyItem);

private:
	CPtrListCtrl mFlyItemList;
	CBaseLock mFlyItemListLock;

	float mFlyScale;

	ISlideWndEndListener *mpOnSlideWndEndListener;
	UINT64 mUserData;
};

