#pragma once

#include "BaseWnd.h"
#include "../Effect/AnimationWnd.h"

class CWaitingWnd: public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	CWaitingWnd();
	virtual ~CWaitingWnd();

public:
	void Create(
		CBaseWnd *pParent);

	virtual void LoadResource();
	void LoadResourceForMovieControl();					//现在 电影或多媒体 播放电影时加载信息

public:
	void StartWaiting();
	void StopWaiting();

private:
	CAnimationWnd mWaitCursorWnd;
};

