#ifndef _SETTINGSFRAGMENT_H
#define _SETTINGSFRAGMENT_H
#include "../CommControl/FragmentInterface.h"
#include "BaseButton.h"
#include "SettingInputWnd.h"

#include "SettingPlayWnd.h"
#include "SettingMusicLibWnd.h"
#include "SettingPublicWnd.h"
#include "SettingSystemWnd.h"
#include "../AdvanceSettingsWnd/MusicLibWnd.h"

#include "../AdvanceSettingsWnd/AddSongWnd.h"
#include "../AdvanceSettingsWnd/MusicLibItemWnd.h"
#include "../AdvanceSettingsWnd/UDiskSongInfoWnd.h"
#include "../AdvanceSettingsWnd/SetScrollTextWnd.h"

#define USE_NEWIPAPI 	1
#if USE_NEWIPAPI
#include "../TreasureBox/NetInfoBaseWnd.h"
#else
#include "../Box/SetIPWnd.h"
#endif


class CSettingsFragment : public CFragment,
	public IClickEventListener
{
	typedef CFragment CParentClass;

public:
	typedef enum _eSettingType
	{
		settingType_play = 0,		// 播放设置
		settingType_musicLib,		// 歌库设置
		settingType_public,			// 公播设置
		settingType_system,		   // 系统设置
		settingType_musicmanage,		// 歌库管理
		settingType_ipset,		     // ip设置
		settingType_lightset,		     // 跑马灯设置
		settingType_count
	} SETTING_TYPE_E;

public:
	CSettingsFragment();
	virtual ~CSettingsFragment();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void deleteSongs(void);

	void selectedSettingItem(SETTING_TYPE_E type);

private:
	void moveSelectedItemWnd(SETTING_TYPE_E type);

private:
	CBaseButton	mReturnBtn;
	CBaseWnd mSettingItem[settingType_count];
	CBaseWnd mSelectedSettingItemWnd;
	CBaseWnd mItemBKGroundWnd;

private:
	CSettingInputWnd mSettingInputWnd;

	CPlayItemWnd mPlayItemWnd;
	CMusicItemWnd mMusicLibItemWnd;
	CPublicItemWnd mPublicItemWnd;
	CSystemItemWnd mSystemItemWnd; 
  
	CMusicLibWnd mMusicLibWnd;
  CAddSongWnd mAddSongWnd;
	CUDiskSongInfo mUdiskSongInfoWnd;
	CMusicLibTypeWnd mMusicLibTypeWnd;
	CSetMusicLibWnd		mChooseSongWnd;
  CSetScrollTextWnd mSetScrollTextWnd;
  #if USE_NEWIPAPI
	CNetInfoBaseWnd mIpSetWnd;
#else
	CIPSetWnd mIpSetWnd;
#endif
  
};

extern CMusicLibWnd* gMusicLibWnd;
extern CAddSongWnd* gAddSongWnd;
extern CUDiskSongInfo* gUdiskSongInfoWnd;
extern CMusicLibTypeWnd *gMusicLibTypeWnd;


#endif /* _SETTINGSFRAGMENT_H */
