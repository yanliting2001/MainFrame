#include "AnimationWnd.h"
#include "CommApi.h"

LPVOID AnimationThreadFunc(LPVOID lpParam)
{
	//printf("the pthread of AnimationThreadFunc ID is:%d.\n",syscall(SYS_gettid));
	CAnimationWnd *pAnimationWnd = (CAnimationWnd *)lpParam;

	if (pAnimationWnd)
	{
		pAnimationWnd->AnimationThreadProc();
	}

	return NULL;
}


CAnimationWnd::CAnimationWnd()
{
	mbStopAnimation = TRUE;
	mLastAnimationIndex = 0;
	mUpdateTimer = 1000;
}

CAnimationWnd::~CAnimationWnd()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);

	RemoveAllTexture();
}

void CAnimationWnd::Create(
	CBaseWnd *pParent,
	RECT rc,
	UINT32 uFlag,
	UINT32 uState)
{
	CParentClass::Create("AnimationEffect", pParent, rc, uFlag, uState);
}

void CAnimationWnd::RemoveAllTexture()
{
	mAnimationTextureListLock.Lock();
	DelNormalList(&mAnimationTextureList, CTexture);
	mAnimationTextureListLock.Unlock();
}

void CAnimationWnd::OnDestroy()
{
	StopAnimation();

	CParentClass::OnDestroy();
}

void CAnimationWnd::SetAnimationSpeed(
	UINT64 uTimer)
{
	mUpdateTimer = uTimer;
}

void CAnimationWnd::AddAnimationTexture(
	CTexture* pTexture)
{
	CTexture *pAimationTexture = new CTexture;
	if (!pAimationTexture)
	{
		return;
	}

	pAimationTexture->SetTexture(pTexture);

	mAnimationTextureListLock.Lock();
	mAnimationTextureList.AddData(pAimationTexture);
	mAnimationTextureListLock.Unlock();
}

void CAnimationWnd::StartAnimation()
{
	if (!IsWindow())
	{
		return;
	}

	StopAnimation();

	mLastAnimationIndex = 0;

	DbgOutput(DBG_DEBUG, "StartAnimation\n");
	mbStopAnimation = FALSE;
	mAnimationThread.StartThread(AnimationThreadFunc, this, "AnimationThreadFunc");
}

void CAnimationWnd::StopAnimation()
{
	DbgOutput(DBG_DEBUG, "StopAnimation ...\n");

	mbStopAnimation = TRUE;
	if ( !IsInMonkeyMode() )
	{
		//mAnimationThread.StopThread();
	}

	DbgOutput(DBG_DEBUG, "StopAnimation Done\n");
}

void CAnimationWnd::AnimationThreadProc()
{
	while (!mbStopAnimation)
	{
		Sleep(mUpdateTimer);

		if (mbStopAnimation)
		{
			break;
		}

		mAnimationTextureListLock.Lock();

		int nCount = mAnimationTextureList.GetCount();
		if (nCount > 0)
		{
			mLastAnimationIndex = mLastAnimationIndex % nCount;
		}
		CTexture* pTexture = (CTexture *)
			mAnimationTextureList.GetAt(mLastAnimationIndex);
		mLastAnimationIndex ++;

		mAnimationTextureListLock.Unlock();

		if (pTexture)
		{
			SetBackgroundTexture(pTexture);
		}

		CParentClass::OnRedraw(mWndRect);
	}

	DbgOutput(DBG_DEBUG, "AnimationThreadProc exit\n");
}

