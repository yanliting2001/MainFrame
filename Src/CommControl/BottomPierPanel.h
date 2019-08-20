#ifndef _BOTTOMPIERPANEL_H
#define _BOTTOMPIERPANEL_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"

typedef enum _eBottomSelectType
	{
    //BTN_SOUND_ID	= 0,	
    BTN_HOMEPAGE_ID = 0,
    BTN_SERVER_ID,
    BTN_PUSH_ID,	
    BTN_RESUME_ID,	
    BTN_ORIGINAL_ID,
    BTN_REPLAY_ID,			
    BTN_SWITCH_ID,					
    BTN_SELECTED_ID,			
    BTNTYPE_COUNDT,
	} BOTTOMSELECTTYPE_E;

class CBottomPierPanel : public CSlideWnd,
	public IClickEventListener
{
	typedef CSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);
	virtual void LoadResource();

	void CreatePopupWnd(CBaseWnd *pParent);

public:
	virtual void OnMsg(	UINT32 uType, UINT64 wParam, UINT64 lParam);
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void SetAccompany(BOOL bAccompany, BOOL bSendCmd=FALSE);
	void SetPausePlay(BOOL bPause, BOOL bSendCmd = FALSE);
	void RefreshSelectedCount(void);

public:
	void OnChildFragmentChange();

private:
	BOOL CanBeSwitch(void);
public:

	CBaseButton mSingedCntWnd;

  
  CBaseButton mSubItem[BTNTYPE_COUNDT];
};

#endif
