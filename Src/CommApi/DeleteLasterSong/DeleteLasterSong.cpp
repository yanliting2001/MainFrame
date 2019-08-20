#include "DeleteLasterSong.h"
#include "../../CommControl/MainCtrlPage.h"

extern CMainCtrlPage *gMainCtrlPage;

void startDelete(void)
{
	if ( gMainCtrlPage )
	{
		gMainCtrlPage->SetInvalidReason("空间已满,正在删除不常用歌曲...结束请重试");
	}
}

void stopDelete(void)
{
	if ( gMainCtrlPage )
	{
		gMainCtrlPage->SetMacCheckWndVisible(FALSE);
	}
}
