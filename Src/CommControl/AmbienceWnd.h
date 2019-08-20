#pragma once

#include "../Effect/PopupSlideWnd.h"
#include "RadioButtonGroup.h"
#include "LineWnd.h"

#define ENABLE_SAVE_LOCUS

#ifdef ENABLE_SAVE_LOCUS
#define LOCUS_BPP	   32
#endif

#define FACEBTN_NUM		13

class CAmbienceWnd : public CPopupSlideWnd,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CPopupSlideWnd CParentClass;

public:
	CAmbienceWnd();
	~CAmbienceWnd();

public:
	void Create(
		CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnDestroy();
	virtual void OnWindowVisible(
		BOOL bVisible);

	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

	virtual void OnTouchDown(
		CBaseWnd *pWnd,
		POINT pt);

	virtual void OnTouchMove(
		CBaseWnd *pWnd,
		POINT pt);

	virtual void OnTouchUp(
		CBaseWnd *pWnd,
		POINT pt,
		int xDistance,
		int yDistance);

private:
#ifdef ENABLE_SAVE_LOCUS
	void SetLocusToBuffer(
		POINT pt);
	void SaveDrawBoxToBmpFile();
#endif

private:
	void AddPointToLocus(
		POINT pt);

private:
	CBaseButton mColorBtn[6];
	CRadioButtonGroup mColorBtnGroup;

	CLineWnd mLocusWnd;
	UINT32 mLocusColor[6];

	CBaseButton mFaceBtn[FACEBTN_NUM];
	CRadioButtonGroup mFaceBtnGroup;

	//CBaseButton mCheerBtn;
	//CBaseButton mHootingBtn;
	CBaseButton mClearBtn;
	CBaseButton mSendBtn;
	CBaseButton mReturnBtn;

#ifdef ENABLE_SAVE_LOCUS
	UINT32 mLocusBufferStride;
	BYTE* mpLocusBuffer;
	POINT mPtLastLocus;
#endif
	short *mpLocusPoint;
	int mpLocusCount;
	int mLocusIndex;
};
