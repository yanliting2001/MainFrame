#ifndef _TOPPIERPANEL_H
#define _TOPPIERPANEL_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"

typedef enum _eTopSelectType
	{
    //BTN_CLOUDDOWN_ID		= 0,
    //BTN_PHONESONG_ID,	
    //BTN_WIFI_ID,	
   // BTN_USBSONG_ID,	
    BTN_PLAYINGBG_ID = 0,									
    BTNTOPTYPE_COUNDT,
	} TOPSELECTTYPE_E;

class CTopPierPanel : public CSlideWnd,
  public IClickEventListener
{
	typedef CSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);
	virtual void LoadResource();
  virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	virtual void OnMsg(	UINT32 uType, UINT64 wParam, UINT64 lParam);

public:
	void RefreshPlayNotice(void);			// 更新播放预告

private:
	CBaseWnd mPlayNoticeWnd;			// 播放预告
	CBaseButton mTopItem[BTNTOPTYPE_COUNDT];
};

#endif 	// _TOPPIERPANEL_H
