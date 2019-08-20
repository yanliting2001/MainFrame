#ifndef _REPAYWND_H
#define _REPAYWND_H
#include "BaseButton.h"
#include "ClientSharing.h"

typedef struct _sPayItemWnd
{
	CBaseWnd	itemBKWnd;	//
	CBaseWnd	itemRemarkWnd;
	CBaseWnd	price_nowWnd;
	CBaseWnd 	price_orgWnd;
	CBaseWnd	itemBtn;
} PAYITEMWND_S;

class CRepayWnd;

class CPayItemsInfo : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;
public:
	void Create(CRepayWnd *pParent);

	virtual void LoadResource();

	virtual void OnClick(CBaseWnd *pWnd, POINT pt){};

	void RefreshPayInfo(void);

private:
	PAYITEMWND_S mPayItemsWnd[PAYINFO_MAX_COUNT];
};

// 付费页面
class CPayQRCodeWnd: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void ShowPayQRCode(int idx);

private:
	CBaseButton mCloseBtn;
	CBaseWnd mQRCodeWnd;
};

// 套餐页面
class CRepayWnd: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnWindowVisible(BOOL bVisible);

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	void SetPayQRCodeWnd(BOOL bVisible);
private:
	CPayItemsInfo mPayItemsWnd;
	CBaseButton mCloseBtn;

	CPayQRCodeWnd mPayQRCodeWnd;
};
#endif	// _REPAYWND_H
