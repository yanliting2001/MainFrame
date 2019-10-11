#ifndef _SHARINGINDEXWND_H
#define _SHARINGINDEXWND_H
#include "BaseButton.h"
#include "ClientSharing.h"
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"

#define BTN_CLOSE_ID	0x01
#define BTN_PACKAGE_BASE_ID	0x10
#define PACKAGE_MAX_COUNT		5		// 主页套餐4条

#define BTN_FOOD_BTN_ID	0x20
#define BTN_SHOP_BTN_ID	0x21

typedef struct _tPayItemWnd
{
	CBaseWnd	itemBKWnd;	//
	CBaseWnd	itemRemarkWnd;
	CBaseWnd	price_nowWnd;
	CBaseWnd 	price_orgWnd;
	CBaseWnd	itemBtn;
} PAYITEMWND_t;

class CSharingIndexWnd;

// 付费金额
class CPayInfo: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;
public:
	void Create(CSharingIndexWnd *pParent);

	virtual void LoadResource();

	virtual void OnClick(CBaseWnd *pWnd, POINT pt){};

	void RefreshPayInfo(void);

private:
	PAYITEMWND_t mPayItemsWnd[PACKAGE_MAX_COUNT];
};

class CSharingIndexWnd: public CBaseWnd,
  public IClickEventListener
{
	typedef CBaseWnd CParentClass;
  //typedef CSlideWnd CParentClass;

public:
	CSharingIndexWnd();
	virtual ~CSharingIndexWnd();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
public:
	virtual void OnWindowVisible(BOOL bVisible);

	virtual void OnTimer(int nTimerID);
  virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	// leftType:返回类型，是歌曲数量还是时间
	// 0:歌曲数量  1:时长
	int GetLeftTime(int *leftType);
  CBaseButton mActivityTitle;                 //活动提示
public:
	void SetLoginQRCodeVisible(BOOL bVisible);

	void DecreaseLeftCount(int type);

	void StartCheckLogin(void);

	void StopCheckLogin(void);

	BOOL GetCurLoginState(void);	// 获取当前登录状态，已登录or未登录

	void ChekLoginFunc(void);
	void HeartFunc(void);

	void Logout(void);

	void Login(void);
	
private:
	CBaseWnd mLoginQRCodeWnd;		// 登录二维码
	CBaseWnd mStbAddressWnd;		// 机器地址
	CBaseWnd mWorkTimeWnd;		  // 工作时间
	CBaseButton mFoodBtn;
	CBaseButton mShopBtn;
	
	BOOL mbLoginFlag;					// 检测登录标志
  
	CPayInfo mPayItemsWnd;    //登录提示套餐
	CBaseThread mCheckLoginThread;
	CBaseThread mHeartPackageThread;
	BOOL mbExitThread;
};
#endif
