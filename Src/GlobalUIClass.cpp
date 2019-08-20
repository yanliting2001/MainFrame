#include "GlobalUIClass.h"

// NOTICE: CE3DCommand must set to the first global class,
// so it will be de-construct lastest
//
CPageManager *gPageManager=NULL;

CPhantomPage *gPhantomPage=NULL;
CMainCtrlPage *gMainCtrlPage=NULL;

CHomePageFragment *gHomePageFragment=NULL;
CSingerListFragment *gSingerListFragment=NULL;
CSongListFragment *gSongListFragment=NULL;
//CRankSongFragment *gRankSongFragment=NULL;

void InitGlobalUIClass()
{
	gPlayerInfo = new CPlayerInfo();
	gPageManager = new CPageManager();

	gPhantomPage = &(gPageManager->mPhantomPage);
	gMainCtrlPage = &(gPageManager->mMainCtrlPage);

	gHomePageFragment = (&gMainCtrlPage->mHomePageFragment);
	gSingerListFragment = (&gMainCtrlPage->mSingerListFragment);
	gSongListFragment = (&gMainCtrlPage->mSongListFragment);
	//gRankSongFragment = (&gMainCtrlPage->mRankSongFragment);


}

void DeInitGlobalUIClass()
{
	delete gPageManager;

	delete gPlayerInfo;
	DbgOutput(DBG_INFO, "DeInitGlobalUIClass.\n");
}
