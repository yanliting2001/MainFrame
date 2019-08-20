#ifndef _NETINFOBASEWND_H
#define _NETINFOBASEWND_H

#include "BaseWnd.h"
#include "BaseButton.h"
#include "RadioButtonGroup.h"

//#include "TreasureBoxDef.h"
#include "NumInputWnd.h"
#include "InternetCtrl/NetCtrlApi.h"

typedef enum _eSetNetItemType
{
	ItemType_ClientIP = 0,
	ItemType_Mask,
	ItemType_Gateway,
	ItemType_DNS,
	ItemType_Count,
} eSetNetItemType;

class CNetInfoBaseWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CNetInfoBaseWnd();
	~CNetInfoBaseWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	void LoadResource();
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	void OnWindowVisible(BOOL bVisible);

public:
	void UpdateIP();

private:
	BOOL IsValidIP();

	void OnDHCPSwitchClick();

	void OnCancelClick();

	void OnConfirmClick();

	void OnNumberBtnClick(char cNum);

	void OnBackBtnClick();

private:
	CBaseWnd 		mInValidIPPromptWnd;		// 提示输入的IP 错误
	CBaseWnd 		mPromptWnd[ItemType_Count];
	CBaseButton 	mTextBtn[ItemType_Count];
	CRadioButtonGroup mTabGroup;

	CBaseButton 	mDHCPBtn;
	CBaseButton 	mCancelBtn;
	CBaseButton 	mConfirmBtn;

	CNumInputWnd 	mNumInputWnd;

	BOOL 			mbDHCPOn;		//0:禁用  1:启用

private:
	void LoadResource_v500();
};

#endif
