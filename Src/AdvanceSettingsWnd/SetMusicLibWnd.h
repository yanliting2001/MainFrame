#ifndef _SETMUSICLIBWND_H
#define _SETMUSICLIBWND_H
#include "SelectMusicItem.h"
#include "Input/BKInputPanel.h"
#include "RadioButtonGroup.h"

class CSetMusicLibWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;
public:
	CSetMusicLibWnd();
	virtual ~CSetMusicLibWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);
public:
	// 初始化已选择的链表
	void InitSelectedItemList(CPtrListCtrl *pSelectedList);

	void OnInputPanelClick(UINT64 wParam);

	void OnTypeItemClick(void);

	void ClearCondition(void);

private:
	CBaseWnd	mConditionIcoWnd;
	CBaseWnd	mSongNameWnd;
	CBaseButton	mSongNameBtn;
	CBaseWnd	mSingerWnd;
	CBaseButton	mSingerBtn;
	CBaseWnd	mLangTypeWnd;
	CBaseButton	mLangTypeBtn;
	CBaseWnd	mSongTypeWnd;
	CBaseButton	mSongTypeBtn;
	CBaseButton	mWdCtBtn;
	CBaseButton mOrdertimersBtn;
	CRadioButtonGroup mTabGroup;

	CTypeConditionWnd 	mTypeCondWnd;
	CSelectedItem		mSelectedItemWnd;
	CBKInputPanel		mInputWnd;

	CBaseButton	mConfirmBtn;
	CBaseButton mCancelBtn;

	sSearchCondition mCondition;
	CPtrListCtrl	*mpSelectedList;
};
#endif
