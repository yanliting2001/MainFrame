#ifndef _NETINFOWND_H
#define _NETINFOWND_H

#include "AppVerb.h"

#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"
#include "RadioButtonGroup.h"
#include "../TreasureBox/NetInfoBaseWnd.h"
#include "WiFiWnd.h"


class CNetInfoWnd: public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CNetInfoWnd();
	virtual ~CNetInfoWnd();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	virtual void OnWindowVisible(BOOL bVisible);

	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void OnClickCheckNet();

	void OnNetLinkClick();

	void OnWiFiLinkClick();

	void GetSerialID();

	void ShowConnectInfo(BOOL bConnOK);	// 显示链接信息，是否连上网

private:
	CBaseWnd	mNetPromptBgWnd;
	CBaseWnd	mSerialIDWnd;
	CBaseWnd 	mLinkPromptWnd;

	CBaseButton mNetBtn;
	CBaseButton	mWifiBtn;
	CRadioButtonGroup mTabGroup;
	CBaseButton mReturnBtn;
	CBaseButton mCheckNetBtn;

	CWiFiWnd 	mWifiWnd;

	CNetInfoBaseWnd 	mIpSetWnd;
	CTexture 	mNetNormaTexture;
	CTexture 	mNetErrTexture;

	char 		mBarCodeFileName[64];
	char 		mSerialIDPormpt[64];
	int			miConnState;

private:
#if APP_ID == QIKUN500
	void LoadResource_v500();
#elif APP_ID == QIKUN510
	void LoadResource_v510();
#elif APP_ID == QIKUN501
	void LoadResource_v501();
#elif APP_ID == QIKUN520
	void LoadResource_v520();
#endif
};

#endif

