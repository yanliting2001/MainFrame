#include "UpgradeWnd.h"
#include "UpgradeApi.h"
#include "../SQL/SearchSQL.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"
#include "CommApi.h"
#include "JsonParser/cJsonParser.h"
#include "../PlayerApi/PlayerInfo.h"
#include"define.h"
#include "XMLParser.h"

static CUpgradeWnd *gpUpgradeWnd = NULL;

static void upgradeapp_callback(struct stbboot_event *event, void *title)
{
	if ( (gpUpgradeWnd != NULL) && (event != NULL) )
		gpUpgradeWnd->UpgradeAppProcess(event);
}

static LPVOID UpgradeThreadFunc(LPVOID lpParam)
{
	gpUpgradeWnd = (CUpgradeWnd *)lpParam;
	if ( gpUpgradeWnd )
		gpUpgradeWnd->Upgrade();

	DbgOutput(DBG_INFO, "========the %s quit=========\n", __FILE__);
	return NULL;
}

void CUpgradeWnd::Create(CBaseWnd *pParent, RECT rc)
{
	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+6);
	CreateStatic(pParent,	rc,  WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP, WINDOWSTATE_INVISIBLE);
	SetFontSize(28);
	SetFontColor(0xFFFF0000);
	mpCloudData = NULL;
	mUpgradeThread.StartThread(UpgradeThreadFunc, this, "UpgradeThreadFunc");
	LoadResource();
}

void CUpgradeWnd::LoadResource()
{
  CParentClass::LoadResource();
  XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SharingIndexWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
  
	RECT rcShow;
	char imgPath[64] = {0};
	CTexture bkTexture,  btnTextures[4];
	
  XmlLoadRect(&parser, "mainWnd", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("mainWnd", "path", "UpgradeBK"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);
  
}

void CUpgradeWnd::Upgrade(void)
{
	if ( IsDataOld() ) UpgradeData();
	if ( IsAppOld() ) UpgradeApp();
}

BOOL CUpgradeWnd::IsDataOld(void)
{
	if ( mpCloudData == NULL && gPlayerInfo != NULL )
		mpCloudData = gPlayerInfo->GetCloudSongHandle();
	if ( mpCloudData == NULL )
		return FALSE;

	char serverDBMsg[128] = {0};
	char ServerDbVer[10] = "0";
	char LocalDbVer[10] = "0";
	strcpy(LocalDbVer, mpCloudData->GetLocalDbVer());
	if ( mpCloudData->GetSongServerDbVer(serverDBMsg, sizeof(serverDBMsg)) == 0 )
	{
		JsonParser_getServerDBVer(serverDBMsg, ServerDbVer, mpCloudData->getDBID());
	}

	DbgOutput(DBG_INFO, "the local db ver:%d, the server db ver:%d.\n", atoi(LocalDbVer), atoi(ServerDbVer));
	return  (atoi(ServerDbVer) > atoi(LocalDbVer)) ? TRUE : FALSE;		//需要修改
}

BOOL CUpgradeWnd::IsAppOld(void)
{
	return CheckNewUpgrade() ? TRUE : FALSE;
}

void CUpgradeWnd::UpgradeData(void)
{
	if ( mpCloudData == NULL )
		return ;

	SetWindowVisible(TRUE);
	miScreenSave_Type = gs_SystemSetting.miScreenSave_type;
	gs_SystemSetting.miScreenSave_type = 0;

	theBaseApp->SuspendInput();

	// 导出用户自定义的数据
	SetWindowTextA("正在导出用户自定义的数据,请稍等...");
	mpCloudData->ExportUserDat();
	// 下载数据库
	SetWindowTextA("正在下载数据,请稍等...");
	if ( mpCloudData->DownLoadDBThreadProc()  )
	{
		// 下载OK
		ConverDb();
	}
	else
	{
		// 下载failed
		SetWindowTextA("下载数据失败!\n升级数据库失败!");
	}

	gs_SystemSetting.miScreenSave_type = miScreenSave_Type;
	theBaseApp->ResumeInput();
	SetWindowVisible(FALSE);
}

void CUpgradeWnd::UpgradeApp(void)
{
	SetWindowVisible(TRUE);
	miScreenSave_Type = gs_SystemSetting.miScreenSave_type;
	gs_SystemSetting.miScreenSave_type = 0;

	theBaseApp->SuspendInput();

	StartUpgrade(upgradeapp_callback);

	sleep(1);
	gs_SystemSetting.miScreenSave_type = miScreenSave_Type;
	theBaseApp->ResumeInput();
	SetWindowVisible(FALSE);
}

void CUpgradeWnd::ConverDb()
{
	if ( mpCloudData == NULL ) return ;

	SetWindowTextA("导入用户歌曲,请稍等...");
	mpCloudData->ImportUserDat();

	SetWindowTextA("正在搜索歌曲,请稍等...");
	mpCloudData->SearchFile();

	SetWindowTextA("正在转换数据,请稍等...");
	mpCloudData->Convert(0, gs_SystemSetting.miOrderType);
	DeinitSQLNotSaveList();
	InitSQL();

	SetWindowTextA("升级数据库成功!");
}

void CUpgradeWnd::UpgradeAppProcess(struct stbboot_event *event)
{
	CBaseStringA tmp;
	switch ( event->type )
	{
	    case STBAPP_EVENT_DATA:
	    printf("STBAPP_EVENT_DATA percent: %d\n", event->param);
		if ( (event->param % 5) == 0 )
		{
			tmp.Format("已下载:%d%%", event->param);
			SetWindowTextA(tmp.GetString());
		}
		break;

	    case STBAPP_EVENT_BEGIN:
		//printf("STBAPP_EVENT_BEGIN \n");
		tmp.Format("开始下载升级包,请稍等...");
		SetWindowTextA(tmp.GetString());
		break;

	    case STBAPP_EVENT_END:
		//printf("STBAPP_EVENT_END \n");
		break;

	    case STBUPGRADE_EVENT_SUCCESS:
		//printf("STBUPGRADE_EVENT_SUCCESS\n");
	    SetWindowTextA("下载成功，稍后重启升级");
		sleep(1);
		gs_SystemSetting.miScreenSave_type = miScreenSave_Type;
		system("reboot");
		break;

	    case STBUPGRADE_EVENT_FAIL:
		//printf("STBUPGRADE_EVENT_FAIL\n");
		SetWindowTextA("下载失败,请联系厂商");
		break;

		default:
			break;
	}
}
