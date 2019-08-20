#pragma once

//#include "../InputMethodWnd/HandWriteInputWnd.h"
#include "RadioButtonGroup.h"
#include "AddSongWnd.h"
#include "Input/BKInputPanel.h"

#define SONGINFONUM		6

class CUDiskSongInfo : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CUDiskSongInfo();
	~CUDiskSongInfo();

public:
	void Create(
		CBaseWnd *pParent);

	virtual void LoadResource();

	virtual void OnWindowVisible(
			BOOL bVisible);

public:
	virtual void OnMsg(
		UINT32 uType,
		UINT64 wParam,
		UINT64 lParam);

	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

	void SetSongInfo(UDSONGINFO *pSong);

	void OnRightBtnClick(int idx);
	void OnLeftBtnClick(int idx);

	void OnConfrimClick();
	void OnCancelClick();

	void GetSongType(int idx);

	void GetLangType(int idx);

private:
	CBaseWnd mFileNameTitleWnd;
	CBaseWnd mSongItemPromptWnd[SONGINFONUM];

	CBaseButton mHandItemBtn[3];
	CRadioButtonGroup mTabGroup;

	CBaseButton mSelectItemWnd[3];
	CBaseButton mSelectItemLeftBtn[3];
	CBaseButton mSelectItemRightBtn[3];

	CBKInputPanel	mInputPanel;

	CBaseButton mConfirmBtn;
	CBaseButton mCancelBtn;

	RECT mPreviewRect;
	UDSONGINFO *mCurSongInfo;

	char mcSongType[32];
	char mcLanguage[32];

	int miHandIdx;

	int miSongCount;
	int miCurrSongIdx;

	int miSongTypeCount;
	int miCurrSongTypeIdx;

	int miLangTypeCount;
	int miCurrLangIdx;
};
