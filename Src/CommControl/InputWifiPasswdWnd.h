#pragma once

#include "BaseWnd.h"
#include "BaseSlide.h"

#define WIFIPASSWDSYMBOL_COUNT		50

class CInputWifiPasswdWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CInputWifiPasswdWnd();
	~CInputWifiPasswdWnd();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rc);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

private:
	CBaseButton mSymbolItem[WIFIPASSWDSYMBOL_COUNT];
	CBaseButton mBackBtn;
	CBaseButton mUnderLineBtn;
	CBaseButton	mUpToLowerBtn;
	CBaseButton	mOKBtn;
};

class CWifiPasswdWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CWifiPasswdWnd();

	~CWifiPasswdWnd();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rc);

	virtual void LoadResource();

	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

	virtual void OnMsg(
		UINT32 uType,
		UINT64 wParam,
		UINT64 lParam);

	void SetPasswdTitle(char *strWifiName, int iWifiIdx);

private:
	void OnInputChar(char cChar);

	void UpdateInputPromptText();

	void OnComfirmClick();

private:
	CBaseWnd	mPasswdBgWnd;
	CBaseWnd	mWifiNameWnd;
	CBaseWnd	mWifiPassWdWnd;
	CBaseButton	mCancelBtn;
	CBaseButton	mComfirmBtn;

	CInputWifiPasswdWnd mInputWnd;

	CBaseStringA mPasswd;
	int 	miIdx;
};


