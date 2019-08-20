#include "SettingSystemWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"
#include "../GlobalUIClass.h"

#if defined(__arm__) || defined(__mips__)
static LPVOID CalibrateThreadFunc(LPVOID lpParam)
{
	gCalibrateWnd->SetWindowVisible(TRUE);
	gHomePageFragment->SetWindowVisible(FALSE);
	//gMainCtrlPage->SetPhantomVisible(FALSE);
	gCalibrateWnd->PromptVisible(FALSE);
	DbgOutput(DBG_ERR, "gCalibrateWnd=%x!!\n", gCalibrateWnd);

	gCalibrateWnd->InitCalibration();

	gCalibrateWnd->ClearBuf();
	gCalibrateWnd->GetSample(0, 100,100, "Top left");
	gCalibrateWnd->ClearBuf();

	gCalibrateWnd->GetSample(1, LAYOUT_WIDTH - 100, 100, "Top right");
	gCalibrateWnd->ClearBuf();

	gCalibrateWnd->GetSample(2, LAYOUT_WIDTH - 100, LAYOUT_HEIGHT - 200, "Bot right");
	gCalibrateWnd->ClearBuf();

	gCalibrateWnd->GetSample(3, 100, LAYOUT_HEIGHT - 200, "Bot left");
	gCalibrateWnd->ClearBuf();

	gCalibrateWnd->GetSample(4, (LAYOUT_WIDTH / 2)-50,  (LAYOUT_HEIGHT / 2)-50,  "Center");
	gCalibrateWnd->ClearBuf();

	DbgOutput(DBG_ERR, "PerformCalibration end!\n");


	if(gCalibrateWnd->PerformCalibration())
	{
		gCalibrateWnd->SaveToConfig();
	}
	DbgOutput(DBG_ERR, "SetWindowVisible end!\n");
	gCalibrateWnd->PromptVisible(TRUE);
	theBaseApp->RestartInput(LAYOUT_WIDTH, LAYOUT_HEIGHT);
	gCalibrateWnd->DeInitCalibration();
	gCalibrateWnd->SetWindowVisible(FALSE);
	gHomePageFragment->SetWindowVisible(TRUE);
	//gMainCtrlPage->SetPhantomVisible(TRUE);

	return NULL;
}
#endif

void CSystemItemWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("Startup", pParent, rc);

	RECT rcControl ={0, 0 , 100, 100};

	mVal_onVolWnd.CreateStatic(this, rcControl);
  mVal_effectVolWnd.CreateStatic(this, rcControl);
  
  for (int i = 0; i < Type_Sys_count; i++ )
	{
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
		mItemsBtn[i].SetWndID(Type_DecVolume_Btn+i);
		mItemsBtn[i].SetOnClickListener(this);
    
	}
  
	SetVolumeValue(gs_SystemSetting.mucAttunementVol, volume_on);
	SetVolumeValue(gs_SystemSetting.mucMicroVol, volume_effect);

	SetPwdValue(gs_SystemSetting.mSettingPwd, pwd_back);
	SetPwdValue(gs_SystemSetting.mDeletePwd, pwd_delete);
	SetPwdValue(gs_SystemSetting.mShutdownPwd, pwd_shutdown);
  
  DbgOutput(DBG_ERR, "%s, line = %d\n", __FUNCTION__, __LINE__);

	LoadResource();
}

void CSystemItemWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Settings/SettingSystemWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcBaseItem;
	CTexture bkTexture,  btnDescTextures[4],  btnIncTextures[4], btnOnTextures[4], btnTextures[4];
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

  XmlLoadRect(&parser, "Desc_onVolBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Desc_onVolBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_DecVolume_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_DecVolume_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "Inc_onVolBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Inc_onVolBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_IncVolume_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_IncVolume_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "Desc_effectVolBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Desc_effectVolBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_DesEffectVol_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_DesEffectVol_Btn].MoveWindow(&rcControl);
  
  XmlLoadRect(&parser, "Inc_effectVolBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Inc_effectVolBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_IncEffectVol_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_IncEffectVol_Btn].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "VolNumWndInfo", &rcControl);
	mVal_onVolWnd.SetFontSize(size);
	mVal_onVolWnd.SetFontColor(color);
	mVal_onVolWnd.MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "EffectNumWndInfo", &rcControl);
	mVal_effectVolWnd.SetFontSize(size);
	mVal_effectVolWnd.SetFontColor(color);
	mVal_effectVolWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "BackNumWndInfo", &rcControl);
	mItemsBtn[Type_BackPwd_Btn].SetFontSize(size);
	mItemsBtn[Type_BackPwd_Btn].SetFontColor(color);
	mItemsBtn[Type_BackPwd_Btn].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "DeleteNumWndInfo", &rcControl);
	mItemsBtn[Type_DeletePwd_Btn].SetFontSize(size);
	mItemsBtn[Type_DeletePwd_Btn].SetFontColor(color);
	mItemsBtn[Type_DeletePwd_Btn].MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "ShutNumWndInfo", &rcControl);
	mItemsBtn[Type_ShutPwd_Btn].SetFontSize(size);
	mItemsBtn[Type_ShutPwd_Btn].SetFontColor(color);
	mItemsBtn[Type_ShutPwd_Btn].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "Calibration_touchBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Calibration_touchBtnInfo", "path", "InputWnd/ok"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Touch_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Touch_Btn].MoveWindow(&rcControl);
}

void CSystemItemWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )  return ;

	int nWndID = pWnd->GetWndID();
	switch ( nWndID )
	{
	case Type_DecVolume_Btn:
	{
		int vol = gs_SystemSetting.mucAttunementVol ;

    vol -= 5;
    if ( vol <= 0 )
		{
			vol = 0;
		}
    
    gMainCtrlPage->mAttunementWnd.SetVolumeVal(vol, TRUE);
		SetVolumeValue(vol, volume_on);
	}
		break;
	case Type_IncVolume_Btn:
	{
		int vol = gs_SystemSetting.mucAttunementVol ;
    vol += 5;
    if ( vol >= 100 )
		{
			vol = 100;
		}
    
    gMainCtrlPage->mAttunementWnd.SetVolumeVal(vol, TRUE);
		SetVolumeValue(vol, volume_on);
	}
		break;
	case Type_DesEffectVol_Btn:
	{
		int vol = gs_SystemSetting.mucMicroVol ;

    vol -= 5;
    if ( vol <= 0 )
		{
			vol = 0;
		}
    
    gMainCtrlPage->mAttunementWnd.SetMircoVal(vol, TRUE);
		SetVolumeValue(vol, volume_effect);
	}
		break;
	case Type_IncEffectVol_Btn:
	{
		int vol = gs_SystemSetting.mucMicroVol ;
    
    vol += 5;
    if ( vol >= 100 )
		{
			vol = 100;
		}
    
    gMainCtrlPage->mAttunementWnd.SetMircoVal(vol, TRUE);
		SetVolumeValue(vol, volume_effect);
	}
		break;
	case Type_Touch_Btn:
		OnCalScreenClick();
		break;
	}

	SaveSystemSetting();
}

void CSystemItemWnd::SetVolumeValue(int val, VOLTYPE_E type)
{
	char tmp[32] = {0};
	sprintf(tmp, "%d", val);

  
  DbgOutput(DBG_ERR, "%s, line = %d val = %d\n", __FUNCTION__, __LINE__, val);
	switch( type )
	{
	case volume_on:
		mVal_onVolWnd.SetWindowTextA(tmp);
		break;

	case volume_effect:
		mVal_effectVolWnd.SetWindowTextA(tmp);
		break;
	}
}

void CSystemItemWnd::SetPwdValue(const char *pwd, PWDTYPE_E type)
{
	switch( type )
	{
	case pwd_back:
		mItemsBtn[Type_BackPwd_Btn].SetWindowTextA(pwd);
		break;

	case pwd_delete:
		mItemsBtn[Type_DeletePwd_Btn].SetWindowTextA(pwd);
		break;

	case pwd_shutdown:
		mItemsBtn[Type_ShutPwd_Btn].SetWindowTextA(pwd);
		break;
	}
}

void CSystemItemWnd::OnCalScreenClick()
{
#if defined(__arm__) || defined(__mips__)
	if ( IsInMonkeyMode() )
		return ;

	DbgOutput(DBG_ERR, "OnComfirmClick!!\n");
	mCalibrateThread.StopThread();
	mCalibrateThread.StartThread(CalibrateThreadFunc, NULL, "CalibrateThreadFunc");
#endif
}
