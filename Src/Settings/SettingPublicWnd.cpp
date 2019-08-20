#include "SettingPublicWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"
#include "../CommControl/MainCtrlPage.h"
#include "../PlayerApi/PlayerInfo.h"

extern CMainCtrlPage *gMainCtrlPage;

void CPublicItemWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("PublicSet", pParent, rc);

	RECT rcControl ={0, 0 , 100, 100};

  
  mVal_effectVolWnd.CreateStatic(this, rcControl);
  for (int i = 0; i < Type_Public_Count; i++ )
	{
    if(i == Type_Des_Volume_Btn || i == Type_Inc_Volume_Btn)
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
    else
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO, false, PRESSED_SCALE);
		mItemsBtn[i].SetWndID(Type_Des_Volume_Btn+i);
		mItemsBtn[i].SetOnClickListener(this);
    
	}
	
	//SetPublicTune();
	//SetPublicSetting();

	SetVolumeVal();
	//SetPublicTune();
	//SetPublicSetting();

	LoadResource();
}

void CPublicItemWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Settings/SettingPublicWnd.xml");
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
	SAFE_STRNCPY(imgPath, parser.GetStrValue("rcLocation", "path", "Settings/Publicbk"), sizeof(imgPath));
  CreateImgTexture(imgPath, &bkTexture);
  SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcControl);
  
	size = parser.GetIntValue("rcLocation", "fontSize", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("rcLocation", "fontColor", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);

  XmlLoadRect(&parser, "Desc_effectVolBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Desc_effectVolBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Des_Volume_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Des_Volume_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "Inc_effectVolBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Inc_effectVolBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Inc_Volume_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Inc_Volume_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "Val_effectVolWndInfo", &rcControl);
	mVal_effectVolWnd.SetFontSize(size);
	mVal_effectVolWnd.SetFontColor(color);
	mVal_effectVolWnd.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "TuneBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("TuneBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Tune_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Tune_Btn].MoveWindow(&rcControl);
  if(gs_SystemSetting.miPublicTune_set == 1)
	mItemsBtn[Type_Tune_Btn].SetRadioBtnPressed(TRUE);
  else
	mItemsBtn[Type_Tune_Btn].SetRadioBtnPressed(FALSE);
  

  XmlLoadRect(&parser, "PublicSetBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PublicSetBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Setting_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Setting_Btn].MoveWindow(&rcControl);
  if(gs_SystemSetting.mbPublicPlay_open == 0)
	mItemsBtn[Type_Tune_Btn].SetRadioBtnPressed(FALSE);
  else
	mItemsBtn[Type_Tune_Btn].SetRadioBtnPressed(TRUE);
  
  XmlLoadRect(&parser, "SingedListBtnInfo", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("SingedListBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
  CreateBtnImgTextures(imgPath, btnTextures);
  mItemsBtn[Type_SingedList_Btn].SetTextures(btnTextures);
  mItemsBtn[Type_SingedList_Btn].MoveWindow(&rcControl);

  
}

void CPublicItemWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )  return ;
	int nWndID = pWnd->GetWndID();
	switch ( nWndID )
	{
//	case BTN_KEEP_PUBLICVOL_ID:
//		gs_SystemSetting.mbPublicVol_Keep = !mKeep_publicVolBtn.GetRadioBtnPressed();
//		break;
	case Type_Des_Volume_Btn:
    gs_SystemSetting.miPublicVol_Val -= 5;
		if ( gs_SystemSetting.miPublicVol_Val < 0 )
			gs_SystemSetting.miPublicVol_Val = 0;
		SetVolumeVal(TRUE);
		break;
	case Type_Inc_Volume_Btn	:
    gs_SystemSetting.miPublicVol_Val += 5;
    if ( gs_SystemSetting.miPublicVol_Val >= 100 )
		{
			gs_SystemSetting.miPublicVol_Val = 100;
		}
		SetVolumeVal();
		break;
	//case BTN_KEEP_TUNE_ID:
	//	gs_SystemSetting.miPublicTune_set = 0;
	//	SetPublicTune();
	//	break;
	case Type_Tune_Btn:
    if(mItemsBtn[Type_Tune_Btn].GetRadioBtnPressed() == 0)
		gs_SystemSetting.miPublicTune_set = 1;
    else
		gs_SystemSetting.miPublicTune_set = 2;
		SetPublicTune();
		break;
	case Type_Setting_Btn:
    if(mItemsBtn[Type_Setting_Btn].GetRadioBtnPressed() == 0)
		gs_SystemSetting.mbPublicPlay_open = 0;
    else
		gs_SystemSetting.mbPublicPlay_open = 1;
   	SetPublicSetting();
		break;
//	case BTN_OPEN_PUBLICLIST_ID:
//		gs_SystemSetting.mbPublic_save = !mbOpen_publicListBtn.GetRadioBtnPressed();
//		break;
	case Type_SingedList_Btn:
		//if ( gs_SystemSetting.mbPublic_save )
			SaveSingedToNewSong();
		break;
	}

	SaveSystemSetting();
}

void CPublicItemWnd::SetVolumeVal(BOOL bSave)
{
	char tmp[32] = {0};
	sprintf(tmp, "%d", gs_SystemSetting.miPublicVol_Val);
	mVal_effectVolWnd.SetWindowTextA(tmp);
  DbgOutput(DBG_ERR, "SetVolumeVal<%d>!!\n", bSave);
	if ( bSave )
		SaveSystemSetting();
}

void CPublicItemWnd::SetPublicTune(BOOL bSave)
{
	if ( gs_SystemSetting.miPublicTune_set < 0 || gs_SystemSetting.miPublicTune_set > 2 )
		gs_SystemSetting.miPublicTune_set = 0;
	//mItemsBtn[Type_Tune_Btn].SetSelectButton(gs_SystemSetting.miPublicTune_set);
	if ( bSave )
		SaveSystemSetting();
}

void CPublicItemWnd::SetPublicSetting(BOOL bSave)
{
	//mItemsBtn[Type_Tune_Btn].SetSelectButton(gs_SystemSetting.mbPublicPlay_open?1:0);
	if ( bSave )
		SaveSystemSetting();
}

void CPublicItemWnd::SaveSingedToNewSong(void)
{
	SaveSongListToPublicSong(&gPlayerInfo->mSelectedList);
	gMainCtrlPage->mSettingsFragment.selectedSettingItem(CSettingsFragment::settingType_play);
}
