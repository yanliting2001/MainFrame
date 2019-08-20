#pragma once

#include "./CommControl/PageManager.h"
#include "./CommControl/MainCtrlPage.h"
#include "./CommControl/PhantomPage.h"
#include "./CommControl/HomePageFragment.h"
#include "./CommControl/SingerListFragment.h"
#include "./CommControl/SongListFragment.h"
//#include "./CommControl/RankSongFragment.h"
#include "./PlayerApi/PlayerInfo.h"

#include "./AdvanceSettingsWnd/AdvanceSettingApi.h"
#include "CommApi.h"

extern CPageManager *gPageManager;

extern CPhantomPage *gPhantomPage;
extern CMainCtrlPage *gMainCtrlPage;

extern CHomePageFragment *gHomePageFragment;
extern CSingerListFragment *gSingerListFragment;
extern CSongListFragment *gSongListFragment;
//extern CRankSongFragment *gRankSongFragment;

void InitGlobalUIClass();
void DeInitGlobalUIClass();

