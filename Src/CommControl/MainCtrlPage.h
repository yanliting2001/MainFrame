#ifndef _MAINCTRLPAGE_H
#define _MAINCTRLPAGE_H
#include "AppVerb.h"
#include "BaseWnd.h"
#include "BaseButton.h"
#include "BaseSlide.h"

#include "WaitingWnd.h"
#include "../Effect/AnimationWnd.h"

#include "../Upgrade/UpgradeWnd.h"
#include "../AdvanceSettingsWnd/CalibrateWnd.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"

#include "TopPierPanel.h"
#include "BottomPierPanel.h"
#include "LeftPierPanel.h"
// popup windows
#include "../InputMethodWnd/SearchInputWnd.h"
#include "AttunementWnd.h"
#include "ServerWnd.h"
#include "../CommControl/RecordWnd.h"

#include "HomePageFragment.h"
#include "SingerTypeFragment.h"
#include "SingerListFragment.h"
//#include "SingerSongsFragment.h"
#include "LangSongFragment.h"
#include "AlbumSongFragment.h"
#include "SongListFragment.h"
#include "../Settings/SettingsFragment.h"
#include "SelectedFragment.h"

#include "../Sharing/SharingIndexWnd.h"
#include "../Sharing/RepayWnd.h"
#include "../Sharing/SharingLogoutWnd.h"
#include "../Login/BaseLoginWnd.h"
#include "../Sharing/ActivityContentWnd.h"
#include "../UDisk/UDiskFragment.h"
#include "AmbienceWnd.h"
#include "DownloadListFragment.h"
#include "NetInfoWnd.h"
#include "CloudFragment.h"
#include "../TreasureBox/FavoritSubBoxWnd.h"

#include "DLNAWnd.h"
#include "DlnaList.h"
#include "YiLang.h"
#include "ShopWnd.h"
#include "FoodWnd.h"
#include "PromptMsg.h"

#include "SwitchPromptWnd.h"

typedef enum eCHILDPAGE_FRAGMENT_TYPE
{
	Fragment_HomePage,
	Fragment_SingerType,
	Fragment_SingerList,

	Fragment_LangSong,
	Fragment_AlbumSong,
	Fragment_SongList,
	Fragment_Selected,
	Fragment_Settings,
	Fragment_UDisk,
	Fragment_Cloud,
	Fragment_Download,
	Fragment_YiLang,
} CHILDPAGE_FRAGMENT_TYPE;

#define USE_NEWPHANTOM	1

// new
typedef enum _ePopupWndItem
{
	attunement_item,							// 调音
	login_item,										// 登录
	ambienceWnd_item,             // 气氛
	favorit_item,                 // 收藏
	popupWnd_count,               
} POPUPWNDITEM_E;

class CMainCtrlPage: public CFragment,
	public IClickEventListener,
	public ISlideWndEndListener
{
	typedef CFragment CParentClass;

public:
	CMainCtrlPage();
	virtual ~CMainCtrlPage();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, 	POINT pt);

	virtual void OnSlideWndEnd(CBaseWnd *pWnd, BOOL bVisible, UINT64 uUserData);

public:
	void OnReturnClick();
	void OnChildFragmentChange();

public:
	void Show(BOOL bShow);
	void SetCurrentFragment(CHILDPAGE_FRAGMENT_TYPE eFragmentType);
	void ReturnToLastFragment();
	CHILDPAGE_FRAGMENT_TYPE GetCurFragmentType();
	CFragment* GetCurFragment();
  
	void SetPhantomVisible(BOOL iVisible);
	void AddToFavorit(const char *pSongID);
	void UserLoginExit();

	void SetVolumePos(int iPos);
	int GetVolumePos();
	void SetPalyState(BOOL bState);
	void SetChannelState(BOOL bState);

	BOOL IsOriginal();

private:
	void ShowChildFragment(CHILDPAGE_FRAGMENT_TYPE eFragmentType);
	CFragment* GetChildFragmentByID(CHILDPAGE_FRAGMENT_TYPE eFragmentType);

public:
	void StartWaiting();

	void StopWaiting();

private:
	// UI LAYOUT
	CWaitingWnd mWaitingWnd;
	CHILDPAGE_FRAGMENT_TYPE mCurShowFragmentType;
	CCalibrateWnd mCalibrateWnd;

protected:
	CBaseWnd	mMacCheckWnd;
public:
	void SetMacCheckWndVisible(BOOL bVisible);
	void SetInvalidReason(const char *cText);

// new
// 下面是新版本的开始
public:
	CHomePageFragment mHomePageFragment;
	CSingerTypeFragment mSingerTypeFragment;
	CSingerListFragment mSingerListFragment;

	CLangSongFragment mLangSongFragment;
	CAlbumSongFragment mAlbumSongFragment;
	CSongListFragment mSongListFragment;

	CSelectedFragment mSelectedFragment;
	CSettingsFragment mSettingsFragment;
  CUDiskFragment mUDiskFragment;
  CDownloadListFragment mDownloadListFragment;
  CNetInfoWnd mBarCodeWnd;
  CCloudFragment mCloudFragment;
   

public:
	CBottomPierPanel mBottomPierPanel;
	CTopPierPanel mTopPierPanel;
	CLeftPierPanel mLeftPierPanel;
	CSearchInputWnd mSearchInputWnd;
  CRecordWnd mRecordQRCodeWnd;
  
	CSharingIndexWnd mSharingIndexWnd;
	CRepayWnd mRepayWnd;
	CSharingLogoutWnd mSharingLogoutWnd;
  CActivityContentWnd mActivityContentWnd;
  CYiLang mYiLangWnd;
	// popup windows
public:
	void CreatePopupWnd(void);
	void SetPopuWndVisible(POPUPWNDITEM_E item, BOOL bVisible, int subItem=0);
	void CloseAllPopWindows(void);
public:
	CAttunementWnd mAttunementWnd;
	CBaseLoginWnd mBaseLoginWnd;
	CSwitchPromptWnd mSwitchPromptWnd;
  CAmbienceWnd mAmbienceWnd;
  CFavoritSubBoxWnd mFavoritWnd;
  CPromptMsg mPromptMsgWnd;
  CDlnaWnd 	mDlnaWnd;
  CShopWnd  mShopWnd;
  CFoodWnd  mFoodWnd;
  CDlnaList mDlnaList;
public:
	void SetVolume(int iVol, BOOL bSendCmd = FALSE);
	int GetVolume(void);
	void SetMute(BOOL bMute, BOOL bSendCmd = FALSE);		// 设置静音
	void SetServerQRCodeVisible(void);
  
  void SetRecordQRCodeVisible(void);

  void SetDlnaInfoVisible(void);
  
  void SetPromptMsgVisible(void);

private:
	int miCurVol;		// 当前音量值
	RECT mPhantomRect;

private:
	CUpgradeWnd mUpgradeWnd;
	CServerWnd mServerQRCodeWnd;
  
};

extern CCalibrateWnd *gCalibrateWnd;

#endif

