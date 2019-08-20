#include "MusicLibWnd.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
#include "UserMsgDefine.h"
#include "CSCloudSongLib.h"
#include "../Settings/SettingsFragment.h"

#include <sys/stat.h>
#include <sys/vfs.h>

#define BUTTON_ID_BASE				0x01

#if defined(__arm__)
#define SONGPATH				"/stb/config/media"
#else
#define SONGPATH				"/media"
#endif

static char g_strSongCount[64] = "歌曲总数";
static char g_strSongUnit[16] = "首";
static char g_strPublicSong[64] = "公播歌曲";
static char g_strNewSong[64] = "新歌歌曲";
static char g_strDiskMemory[64] = "总磁盘空间";
static char g_strDiskStatus[64] = "磁盘状况(已用)";

CMusicLibWnd::CMusicLibWnd()
{
	miSongCount = 0;
	miPubBroadCastCount = 0;
	miNewSongCount = 0;
	miDiskMemory = 0;
	miDiskStatus = 0;
}

CMusicLibWnd::~CMusicLibWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CMusicLibWnd::Create(
		CBaseWnd *pParent,
		RECT rc)
{
	CParentClass::Create("MusicLib", pParent, rc);

	RECT rcControl;
	SetRectXY(&rcControl, 236, 30, 382, 55);
	for(int i = 0; i < MUSIC_ITEM_COUNT; i++)
	{
		mItemBtn[i].Create(this, rcControl);
		mItemBtn[i].SetWndID(BUTTON_ID_BASE + i);
		mItemBtn[i].SetOnClickListener(this);
	}

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mPubBroadCastCountWnd.CreateStatic(this, rcControl);
  mPubBroadCastCountWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mSongCountWnd.CreateStatic(this, rcControl);
  mSongCountWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mNewSongCountWnd.CreateStatic(this, rcControl);
  mNewSongCountWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mDiskMemoryWnd.CreateStatic(this, rcControl);
  mDiskMemoryWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mDiskStatusWnd.CreateStatic(this, rcControl);
  mDiskStatusWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
  
	//mWaitingWnd.Create(this);

	LoadResource();
}

void CMusicLibWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "AdvanceSettings.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT rcControl,rcItem;
	int i = 0, xOffset =  0;
  
  XmlLoadRect(&parser, "rcLocation", &rcControl);
	MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "ItemBtnLocation", &rcItem);
	xOffset = parser.GetIntValue("ItemBtnLocation","xOffset",20);
	for(i = 0; i < MUSIC_ITEM_COUNT; i++)
	{
		InflatRect(&rcControl, &rcItem, i*xOffset, 0);
		mItemBtn[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SongCountLocation", &rcControl);
	mSongCountWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PubBroadCastCountLocation", &rcControl);
	mPubBroadCastCountWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NewSongCountLocation", &rcControl);
	mNewSongCountWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "DiskMemoryLocation", &rcControl);
	mDiskMemoryWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "DiskStatusLocation", &rcControl);
	mDiskStatusWnd.MoveWindow(&rcControl);

	char imgPath[64] = {0};
	CTexture btnTextures[4];

	strcpy(imgPath, parser.GetStrValue("ItemBtnImg","path","AdvanceSettings/StartUp/OpenRoom"));
	for(i = 0; i < MUSIC_ITEM_COUNT; i++)
	{
    
		char temp[64] = {0};
		sprintf(temp,"%s/%d",imgPath,(i+1));
    
		CreateBtnImgTextures(temp, btnTextures);
		mItemBtn[i].SetTextures(btnTextures);
	}

	strcpy(g_strSongCount, parser.GetStrValue("Prompt", "songcountmsg", "歌曲总数"));
	strcpy(g_strSongUnit, parser.GetStrValue("Prompt", "songUnitmsg", "首"));
	strcpy(g_strPublicSong, parser.GetStrValue("Prompt", "publicsongmsg", "公播歌曲"));
	strcpy(g_strNewSong, parser.GetStrValue("Prompt", "newsongmsg", "新歌歌曲"));
	strcpy(g_strDiskMemory, parser.GetStrValue("Prompt", "diskmemorymsg", "总磁盘空间"));
	strcpy(g_strDiskStatus, parser.GetStrValue("Prompt", "diskstatusmsg", "磁盘状况(已用)"));

	SetMusicLibInfo();
}

void CMusicLibWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();

	if(nWndID >= BUTTON_ID_BASE &&
		(nWndID < BUTTON_ID_BASE + MUSIC_ITEM_COUNT))
	{
		int idx = nWndID - BUTTON_ID_BASE;
		switch (idx)
		{
		//case UDISKSONG_ADD:
		//	OnU_DiskClick();
		//	break;

		case SONG_DELETE:
			OnBatchDeleteClick();
			break;

		case NEWSONG_SET:
			OnNewSonSetClick();
			break;

		case PUBLIC_SET:
			OnPubBroadcastClick();
			break;

		//case BATCH_ADD:
		//	OnUserClick();
		//	break;

		//case SONG_SEARCH:
		//	OnSongSearchClick();
		//	break;

//		case SONG_ENCRYPT:
//			OnSongEncryptClick();
//			break;
		}
	}
}

void CMusicLibWnd::SetMusicLibInfo()
{
	GetAllSongsCount();
	GetPubBroadCastCount();
	GetNewSongsCount();
	GetDiskMemory();

	char buf[256];

	snprintf(buf, sizeof(buf), "%s:%d %s", g_strSongCount, miSongCount, g_strSongUnit);
	mSongCountWnd.SetWindowTextA(buf);

	//sprintf(buf,"公播歌曲:%d 首",miPubBroadCastCount);
	snprintf(buf, sizeof(buf), "%s:%d %s", g_strPublicSong, miPubBroadCastCount, g_strSongUnit);
	mPubBroadCastCountWnd.SetWindowTextA(buf);

	//sprintf(buf,"新歌歌曲:%d 首",miNewSongCount);
	snprintf(buf, sizeof(buf), "%s:%d %s", g_strNewSong, miNewSongCount, g_strSongUnit);
	mNewSongCountWnd.SetWindowTextA(buf);

	//sprintf(buf,"总磁盘空间:%u MB",miDiskMemory);
	snprintf(buf, sizeof(buf), "%s:%u MB", g_strDiskMemory, miDiskMemory);
	mDiskMemoryWnd.SetWindowTextA(buf);

	//sprintf(buf,"磁盘状况(已用):%d%%",miDiskStatus);
	snprintf(buf, sizeof(buf), "%s:%d%%", g_strDiskStatus, miDiskStatus);
	mDiskStatusWnd.SetWindowTextA(buf);
}

void CMusicLibWnd::OnU_DiskClick()
{
	SetWindowVisible(FALSE);
	gAddSongWnd->SetAddSongType(TYPE_MANUAL);
	gAddSongWnd->SetWindowVisible(TRUE);
}

void CMusicLibWnd::OnBatchDeleteClick()
{
	SetWindowVisible(FALSE);
	gMusicLibTypeWnd->SetMusicLibType(TYPE_DELETE);
	gMusicLibTypeWnd->SetWindowVisible(TRUE);
}

void CMusicLibWnd::OnPubBroadcastClick()
{
	SetWindowVisible(FALSE);
	gMusicLibTypeWnd->SetMusicLibType(TYPE_PUBLIC);
	gMusicLibTypeWnd->SetWindowVisible(TRUE);
}

void CMusicLibWnd::OnNewSonSetClick()
{
	SetWindowVisible(FALSE);
	gMusicLibTypeWnd->SetMusicLibType(TYPE_NEWSONG);
	gMusicLibTypeWnd->SetWindowVisible(TRUE);
}

void CMusicLibWnd::OnUserClick()
{
	SetWindowVisible(FALSE);
	gAddSongWnd->SetAddSongType(TYPE_BATCH);
	gAddSongWnd->SetWindowVisible(TRUE);
}

int CMusicLibWnd::GetAllSongsCount()
{
	miSongCount = FastSearch_GetSongCount();
	return miSongCount;
}
int CMusicLibWnd::GetPubBroadCastCount()
{
	miPubBroadCastCount = FastSearch_GetPubBroadCastSongCount();
	return miPubBroadCastCount;
}
int CMusicLibWnd::GetNewSongsCount()
{
	miNewSongCount = FastSearch_GetNewSongCount();
	return miNewSongCount;
}
int CMusicLibWnd::GetDiskMemory()
{
	struct statfs diskInfo;
	statfs(SONGPATH,&diskInfo);
	miDiskMemory = (int)(diskInfo.f_blocks * (diskInfo.f_bsize /1024))/ 1024 ;			// 1024*1024 =1MB  换算成MB单位
	miDiskStatus = (int)((diskInfo.f_blocks - diskInfo.f_bfree)*100.0/(diskInfo.f_blocks - diskInfo.f_bfree + diskInfo.f_bavail)+1);

	return miDiskMemory;
}

void CMusicLibWnd::OnSongSearchClick()
{
	gMainCtrlPage->StartWaiting();
	CSongLibrary *pCloudData = new CSongLibrary;
	if ( pCloudData != NULL)
	{
//		mCloudData.ExportUserDat();
//		pCloudData->ImportUserDat();
		DbgOutput(DBG_INFO, "%s.\n", "SearchFile");
		pCloudData->SearchFile();
		DbgOutput(DBG_INFO, "%s.\n", "Convert Data");
		pCloudData->Convert(0);
		DbgOutput(DBG_INFO, "%s.\n", "Init Sql");
		DeinitSQLNotSaveList();
		InitSQL();
		delete pCloudData;
	}
	gMainCtrlPage->StopWaiting();
	ReportMsg(BOX_ITEM_INIT, 0, 0);
}

void CMusicLibWnd::OnSongEncryptClick()
{
//#if ENCRY_MODE
//	SetWindowVisible(FALSE);
//	gEncryptWnd->SetWindowVisible(TRUE);
//#endif
}


