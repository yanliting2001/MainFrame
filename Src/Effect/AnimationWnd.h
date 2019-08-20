#pragma once

#include "BaseWnd.h"

class CAnimationWnd: public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	CAnimationWnd();
	virtual ~CAnimationWnd();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rc,
		UINT32 uFlag=WINDOWFLAG_DEFAULT,
		UINT32 uState=WINDOWSTATE_DEFAULT);

	void RemoveAllTexture();

public:
	virtual void OnDestroy();

public:
	void SetAnimationSpeed(
		UINT64 uTimer);

	void AddAnimationTexture(
		CTexture* pTexture);

	void StartAnimation();

	void StopAnimation();

	void AnimationThreadProc();

private:
	CBaseThread mAnimationThread;
	BOOL mbStopAnimation;

	CPtrListCtrl mAnimationTextureList;
	CBaseLock mAnimationTextureListLock;
	int mLastAnimationIndex;

	UINT64 mUpdateTimer;

};

