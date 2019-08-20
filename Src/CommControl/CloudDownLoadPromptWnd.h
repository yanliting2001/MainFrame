#pragma once

#include "BaseWnd.h"
#include "BaseButton.h"
#include "../Effect/PopupSlideWnd.h"
#include "../CommDef/Song.h"

class CCloudDownLoadPromptWnd:public CPopupSlideWnd,
	public IMsgRecvWndInterface,
	public IClickEventListener
{
	typedef CPopupSlideWnd CParentClass;

public:
	CCloudDownLoadPromptWnd();
	~CCloudDownLoadPromptWnd();

public:
	void Create(
		CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

	void SetCloudSongInfo(SONGINFO *pSongInfo);

	SONGINFO *GetCloudSongInfo();

private:
	CBaseButton mConfirmBtn;
	CBaseButton mCancelBtn;
	SONGINFO 	mSongInfo;
};


