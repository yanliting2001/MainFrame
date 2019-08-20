#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "BaseApp.h"
#include "InputMethodDef.h"
#include "LineWnd.h"

class CHandWriteInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public ITouchEventListener,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CHandWriteInputWnd();
	virtual ~CHandWriteInputWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

public:
	virtual void OnDestroy();

	virtual void OnWindowVisible(	BOOL bVisible);

	virtual void OnTimer(	int nTimerID);

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

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

private:
	void AddPointToLocus(POINT pt);

	void DoRecognize();

	void ClearBoardBuf();

private:
	CBaseButton mCandidateBtns[INPUTID_HANDWRITE_COUNT];
	CBaseButton mReputBtn;
	CBaseButton mSlideBtn;
	CBaseButton mCloseBtn;

	CBaseWnd mWriteRegionWnd;
	CLineWnd mLineWnd;
	short *mpLocusPoint;
	int mpLocusCount;
	int mLocusIndex;
};
