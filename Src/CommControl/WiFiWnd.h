#pragma once

#include "BaseWnd.h"
#include "BaseSlide.h"
#include "InputWifiPasswdWnd.h"
//#include "WaitingWnd.h"
#include "wifictrl.h"

#define WIFI_LIST_COUNT 8

typedef enum _eSignalLevel
{
	ZeroLevel=0,
	OneLevel,
	TwoLevel,
	ThreeLevel,
	Level_Count
} eSignalLevel;

typedef struct _sWifiItemWnd
{
	CBaseWnd	mItemBgWnd;
	CBaseWnd	mLinkStatusWnd;
	CBaseWnd	mIocWnd;
	CBaseButton mItemBtn;
} sWifiItemWnd;

typedef struct _sWifiItemData
{
	char 		strWifiName[256];
	char 		FLAGS[8];  //Flags
	int			iSignalLevel;	// 信号等级
} sWifiItemData;

class CWiFiWnd : public CBaseWnd,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CWiFiWnd();
	~CWiFiWnd();

public:
	void Create(
		CBaseWnd *pParent);

	virtual void LoadResource();

	virtual void OnDestroy();

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

public:
	BOOL GetAutoConnWifiName();

private:

	void GetWifiItemList();

	void AddWifiItem(sWifiItemData *pWifiItemInfo, int iPos=-1);

	void UpdateWifiItem();

	void InitWifiItem();

	void OnCLickWifiItem(int idx);

	void Connect2Ap(char *strName, char* nKeyMgmt, char *strPwd, int iWndID);

	void ClearPrevLinkIco();		// 清除原先 连接的状态

	void SetNewLinkIco();			//

	void FreeWiFiList();

private:

	sWifiItemWnd mWifiItem[WIFI_LIST_COUNT];

	CWifiPasswdWnd mWifiPasswdWnd;

//	CWIFI *wf;
	CPtrArrayCtrl mWifiItemList;
	int mSongCurPage;
	int mSongPageCount;

	CTexture LockIcoTexture[Level_Count];
	CTexture UnlockIcoTexture[Level_Count];
	char 	mConnSSID[64];
	int 	mConnectID;

	char 	mLinkSuccPrompt[64];
	char 	mLinkFailPrompt[64];

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;
	int mFirstShowItem;

};

