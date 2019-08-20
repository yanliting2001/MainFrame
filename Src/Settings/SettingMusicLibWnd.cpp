#include "SettingMusicLibWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"
#include "../CommControl/MainCtrlPage.h"
#include "../PlayerApi/PlayerInfo.h"
#include "CSqlite3.h"
#include "JsonParser/cJsonParser.h"

extern CMainCtrlPage *gMainCtrlPage;

void CMusicItemWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("Startup", pParent, rc);

	RECT rcControl ={0, 0 , 100, 100};

   for (int i = 0; i < Type_Music_Count; i++ )
	{
    if(i == Type_OrderTime_Btn || i == Type_SongWord_Btn || i == Type_SongName_Btn)
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO, false, PRESSED_SCALE);
    else
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
		mItemsBtn[i].SetWndID(Type_SmartDel_Btn+i);
		mItemsBtn[i].SetOnClickListener(this);
	}

	mOrderBtnGroup.AddToGroup(&mItemsBtn[Type_OrderTime_Btn]);
	mOrderBtnGroup.AddToGroup(&mItemsBtn[Type_SongWord_Btn]);
	mOrderBtnGroup.AddToGroup(&mItemsBtn[Type_SongName_Btn]);
	SetOrderType();
  
	LoadResource();
}

void CMusicItemWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Settings/SettingMusicLibWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcBaseItem;
	CTexture bkTexture,  btnSelectedTextures[4], btnOnTextures[4], btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	int size, yOffset;
	UINT32 color;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("rcLocation", "path", "Settings/Musicbk"), sizeof(imgPath));
  CreateImgTexture(imgPath, &bkTexture);
  SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcControl);
  
  DbgOutput(DBG_ERR, "%s, line = %d \n", __FUNCTION__, __LINE__);
	size = parser.GetIntValue("rcLocation", "fontSize", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("rcLocation", "fontColor", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);

  XmlLoadRect(&parser, "SmartDelBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SmartDelBtnInfo", "path", "Settings/ok"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_SmartDel_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_SmartDel_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "NewSongBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NewSongBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_NewSong_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_NewSong_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "OrderTimesBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("OrderTimesBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_OrderTime_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_OrderTime_Btn].MoveWindow(&rcControl);
 
  DbgOutput(DBG_ERR, "%s, line = %d \n", __FUNCTION__, __LINE__);
  XmlLoadRect(&parser, "SongWordBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SongWordBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_SongWord_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_SongWord_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "SongNameBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SongNameBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_SongName_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_SongName_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "UpgradeSongBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("UpgradeSongBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_UpgradeSong_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_UpgradeSong_Btn].MoveWindow(&rcControl);
  
  DbgOutput(DBG_ERR, "%s, line = %d \n", __FUNCTION__, __LINE__);
  
}


void CMusicItemWnd::ConverSongDb()
{

  mpCloudData = gPlayerInfo->GetCloudSongHandle();
	if ( mpCloudData == NULL ) return ;
  
  DbgOutput(DBG_ERR, "Start Upgrade Song ! \n");
  gMainCtrlPage->StartWaiting();
	mpCloudData->SearchFile();

  DbgOutput(DBG_ERR, "正在转换数据,请稍等... \n");
	mpCloudData->Convert(0, gs_SystemSetting.miOrderType);
	DeinitSQLNotSaveList();
	InitSQL();
  
  DbgOutput(DBG_ERR, "Upgrade Song success! \n");
  gMainCtrlPage->StopWaiting();
}

void CMusicItemWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )  return ;
	int nWndID = pWnd->GetWndID();
	switch ( nWndID )
	{
	case Type_SmartDel_Btn:
    gs_SystemSetting.mbOnDelete = 1;
		SmartDeleteSong();
		break;

	case Type_NewSong_Btn:
		//if ( gs_SystemSetting.mbOnSaveNew )
			SaveSingedToNewSong();
		break;
	case Type_OrderTime_Btn:
		gs_SystemSetting.miOrderType = 0;
		SetOrderType(TRUE);
		break;
	case Type_SongWord_Btn:
		gs_SystemSetting.miOrderType = 1;
		SetOrderType(TRUE);
		break;
	case Type_SongName_Btn:
		gs_SystemSetting.miOrderType = 2;
		SetOrderType(TRUE);
		break;
  case Type_UpgradeSong_Btn:
    ConverSongDb();
		break;
	}

	SaveSystemSetting();
}

void CMusicItemWnd::SetOrderType(BOOL bSearchSong)
{
	if ( gs_SystemSetting.miOrderType < 0 || gs_SystemSetting.miOrderType > 2 )
		gs_SystemSetting.miOrderType = 0;
	mOrderBtnGroup.SetSelectButton(gs_SystemSetting.miOrderType);
	if ( bSearchSong )
	{
		gMainCtrlPage->StartWaiting();
		CCSqlite3::Sqltie3_CloseDB();
		CCSqlite3::Sqlite3_OpenDB(CCSqlite3::mcDBFile);

		DelNormalList(&gAllSongList, SONGINFO);

		CCSqlite3::Sqlite3_LoadSongList(&gAllSongList, (ORDERTYPE_E)gs_SystemSetting.miOrderType);

		FastSave_AllSongList();

		gMainCtrlPage->StopWaiting();
	}
}

void CMusicItemWnd::SmartDeleteSong(void)
{
	if ( gs_SystemSetting.mbOnDelete )
		gMainCtrlPage->SetPopuWndVisible(login_item, TRUE, CBaseLoginWnd::type_delete);
}

void CMusicItemWnd::SaveSingedToNewSong(void)
{
	SaveSongListToNewSong(&gPlayerInfo->mSingedList);
	gMainCtrlPage->mSettingsFragment.selectedSettingItem(CSettingsFragment::settingType_play);
}
