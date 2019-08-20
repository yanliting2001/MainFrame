#include "LeftPierPanel.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "MainCtrlPage.h"
#include "UserMsgDefine.h"

// 音量加减step
#define VOLUME_CHANGE_STEP			2

#define KEEPALIVE_TIMERID			0x01

extern CMainCtrlPage *gMainCtrlPage;
extern CSongListFragment *gSongListFragment;

void CLeftPierPanel::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 2, 0, 1276, 74);
	SetRectXY(&rcHide, 2, -74, 1276, 74);
	CParentClass::Create(
		"TopPier",
		pParent,
		WINDOWFLAG_DEFAULT,
		rcShow,
		rcHide,
		FALSE);

	RECT rcControl = {10, 0, 500, 30};

  mLoginUserImgWnd.SetDrawCircle(TRUE);
	mLoginUserImgWnd.CreateStatic(this, rcControl);
  
  for (int i = 0; i < LeftItem_count; i++ )
	{
    if(i == Logo_item || i == TimeBg_item || i == NameBg_item || i == VolumeBG_item)
    {
      mLeftItem[i].CreateStatic(this, rcControl);
    }
    else
    {
  		mLeftItem[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
  		mLeftItem[i].SetWndID(Logo_item+i);
  		mLeftItem[i].SetOnClickListener(this);
    }
	}

  mLeftTimeWnd.CreateStatic(this, rcControl);
  mLoginUserWnd.CreateStatic(this,rcControl);
  mMicValueWnd.CreateStatic(this,rcControl);
  mVolValueWnd.CreateStatic(this,rcControl);
	LoadResource();
}

void CLeftPierPanel::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;
	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();
  
	CombinePathName(cFileName, cXmlFolder, "LeftPierlPanel.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcShow;
	RECT rcHide;
	XmlLoadRect(&parser, "ShowWindowLocation", &rcShow);
	XmlLoadRect(&parser, "HideWindowLocation", &rcHide);
	CParentClass::MoveWindow(rcShow, rcHide);

	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture[4];
	char colorbuf[16];
	int size;
	UINT32 color;
  
  XmlLoadRect(&parser, "SetupBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SetupBtnInfo", "path", "BottomPier/Seting"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[Setup_item].SetTextures(bkTexture);
	mLeftItem[Setup_item].MoveWindow(&rcControl);
  
  XmlLoadRect(&parser, "LogoWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LogoWndInfo", "path", "LeftPier/Logo"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[Logo_item].SetTextures(bkTexture);
	mLeftItem[Logo_item].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "UserBKBntInfo", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("UserBKBntInfo", "path", "LeftPier/UserBK"), sizeof(imgPath));
  CreateBtnImgTextures(imgPath, bkTexture);
  mLeftItem[UserBK_item].SetTextures(bkTexture);
  mLeftItem[UserBK_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LoginUserImgWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LoginUserImgWndInfo", "path", "LeftPier/User"), sizeof(imgPath));
	CreateImgTexture(imgPath, &mDefaultUserImgTexture);
	mLoginUserImgWnd.SetBackgroundTexture(&mDefaultUserImgTexture);
	mLoginUserImgWnd.MoveWindow(&rcControl);

  	XmlLoadRect(&parser, "NameWndBg", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NameWndBg", "path", "LeftPier/TimeBg"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[NameBg_item].SetTextures(bkTexture);
	mLeftItem[NameBg_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NameWndInfo", &rcControl);
	size = parser.GetIntValue("NameWndInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("NameWndInfo", "color", "FFFFFFFF"), sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mLoginUserWnd.SetFontSize(size);
	mLoginUserWnd.SetFontColor(color);
	mLoginUserWnd.MoveWindow(&rcControl);

	  XmlLoadRect(&parser, "VolWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("VolWndInfo", "path", "LeftPier/VolumeBg"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[VolumeBG_item].SetTextures(bkTexture);
	mLeftItem[VolumeBG_item].MoveWindow(&rcControl);

  	XmlLoadRect(&parser, "VolBtnDown", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("VolBtnDown", "path", "LeftPier/VolumeDown"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[VolDown_item].SetTextures(bkTexture);
	mLeftItem[VolDown_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "VolBtnUp", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("VolBtnUp", "path", "LeftPier/VolumeUp"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[VolUp_item].SetTextures(bkTexture);
	mLeftItem[VolUp_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "MicBtnDown", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("MicBtnDown", "path", "LeftPier/VolumeDown"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[MicDown_item].SetTextures(bkTexture);
	mLeftItem[MicDown_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "MicBtnUp", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("MicBtnUp", "path", "LeftPier/VolumeDown"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[MicUp_item].SetTextures(bkTexture);
	mLeftItem[MicUp_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "EffectBtnStand", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("EffectBtnStand", "path", "LeftPier/Stand"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[EffectStand_item].SetTextures(bkTexture);
	mLeftItem[EffectStand_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "EffectBtnSinger", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("EffectBtnSinger", "path", "LeftPier/Singer"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[EffectSinger_item].SetTextures(bkTexture);
	mLeftItem[EffectSinger_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "EffectBtnPopular", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("EffectBtnPopular", "path", "LeftPier/Popular"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[EffectPopular_item].SetTextures(bkTexture);
	mLeftItem[EffectPopular_item].MoveWindow(&rcControl);


  
  	XmlLoadRect(&parser, "PayBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PayBntInfo", "path", "LeftPier/Pay"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[Pay_item].SetTextures(bkTexture);
	mLeftItem[Pay_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LeftTimeWndBg", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LeftTimeWndBg", "path", "LeftPier/TimeBK"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mLeftItem[TimeBg_item].SetTextures(bkTexture);
	mLeftItem[TimeBg_item].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LeftTimeWndInfo", &rcControl);
	size = parser.GetIntValue("LeftTimeWndInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("LeftTimeWndInfo", "color", "FFFFFFFF"), sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mLeftTimeWnd.SetFontSize(size);
	mLeftTimeWnd.SetFontColor(color);
	mLeftTimeWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "VolValue", &rcControl);
	size = parser.GetIntValue("VolValue", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("VolValue", "color", "FFFFFFFF"), sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mVolValueWnd.SetFontSize(size);
	mVolValueWnd.SetFontColor(color);
	mVolValueWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "MicValue", &rcControl);
	size = parser.GetIntValue("MicValue", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("MicValue", "color", "FFFFFFFF"), sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mMicValueWnd.SetFontSize(size);
	mMicValueWnd.SetFontColor(color);
	mMicValueWnd.MoveWindow(&rcControl);
  
	mPreampDevCtrl.OpenPreampDev();
	mAttuneType = gs_SystemSetting.mucPcmTuneType;

	SetTune(gs_SystemSetting.mucPcmTuneType,true);
	SetMircoVal(gs_SystemSetting.mucMicroVol,true);
	SetVolumeVal(gs_SystemSetting.mucAttunementVol,true);

}

void CLeftPierPanel::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;
	int nWndID = pWnd->GetWndID();
	
        
    switch ( nWndID )
		{
   /*
    case Home_item:
			if ( gMainCtrlPage->GetCurFragmentType() != Fragment_HomePage )
          gMainCtrlPage->SetCurrentFragment(Fragment_HomePage);
			break;
     */ 
	case Setup_item:
    
  		gMainCtrlPage->SetPopuWndVisible(login_item, TRUE, CBaseLoginWnd::type_setting);
  		break;
      
	case UserBK_item:

     		 if ( gMainCtrlPage->mSharingLogoutWnd.IsWindowVisible() )
			gMainCtrlPage->mSharingLogoutWnd.SetWindowVisible(FALSE);
		  else
			gMainCtrlPage->mSharingLogoutWnd.SetWindowVisible(TRUE);
			break;
	case  Pay_item:
		ShowRepayVisible(TRUE);
      		break;
	case VolDown_item:
		if(gs_SystemSetting.mucAttunementVol>0){
			gs_SystemSetting.mucAttunementVol-=1;
			SetVolumeVal(gs_SystemSetting.mucAttunementVol,true);
		}
		break;
    	case VolUp_item:
		if(gs_SystemSetting.mucAttunementVol<100){
			gs_SystemSetting.mucAttunementVol+=1;
			SetVolumeVal(gs_SystemSetting.mucAttunementVol,true);
		}
		break;
    	case MicDown_item:
		if(gs_SystemSetting.mucMicroVol>0){
			gs_SystemSetting.mucMicroVol-=1;
			SetMircoVal(gs_SystemSetting.mucMicroVol,true);
		}
		break;
    	case MicUp_item:
		if(gs_SystemSetting.mucMicroVol<100){
			gs_SystemSetting.mucMicroVol+=1;
			SetMircoVal(gs_SystemSetting.mucMicroVol,true);
		}
		break;
	case EffectStand_item:
		SetTune(MealKtv_Attune,true);
		break;
    	case EffectSinger_item:
		SetTune(SingBar_Attune,true);
		break;
    	case EffectPopular_item:
		SetTune(Vietnam_Attune,true);
		break;
      /*
    case  Push_item:
      gMainCtrlPage->SetDlnaInfoVisible();
      break;
      
    case Vol_item:
      gMainCtrlPage->SetPopuWndVisible(attunement_item, TRUE);
      break;
      */
	default:
		break;
	}
    
 }

void CLeftPierPanel::SetLoginUserInfo(const char *name)
{
	if ( name )
	{
		if ( IsFileExist(LOAD_USER_JPG) )
		{
			CTexture texture;
			texture.CreateFromImgFile(LOAD_USER_JPG);
			mLoginUserImgWnd.SetBackgroundTexture(&texture);
		}
		mLoginUserWnd.SetWindowTextA(name);
	}
	else
	{
		mLoginUserImgWnd.SetBackgroundTexture(&mDefaultUserImgTexture);
	}
}

void CLeftPierPanel::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
	if ( nTimerID == 1 )
	{
		if(CClientSharing::mUserInfo.type!=0)
			gMainCtrlPage->mSharingIndexWnd.DecreaseLeftCount(CClientSharing::mUserInfo.type);

	}
}

void CLeftPierPanel::StartTime(void)
{
	BOOL ret = AddTimer(KEEPALIVE_TIMERID, 1000);  
}

void CLeftPierPanel::RefreshLeftTime(int type, int left)
{
	char tmp[32] = {0};
	if ( type == payType_count )
	{
		snprintf(tmp, sizeof(tmp), "%d首", left);
		mLeftTimeWnd.SetWindowTextA(tmp);
	}
	else if ( type == payType_time )
	{
		snprintf(tmp, sizeof(tmp), "%02d:%02d:%02d", left/3600, left/60%60, left%60);
		mLeftTimeWnd.SetWindowTextA(tmp);
    
	}
}

void CLeftPierPanel::ShowRepayVisible(BOOL bVisible)
{
	gMainCtrlPage->mRepayWnd.SetWindowVisible(bVisible);
}

// type:类型
// left:天数, 只有在type在包月时有效
void CLeftPierPanel::RefreshLeftDate(int type, int left)
{
  #if 0
	if ( type == 3 )
	{
		if ( left == 0 )
		{
			mLeftDateWnd.SetWindowTextA("时段套餐");
		}
		else
		{
			char msg[64] = {0};
			snprintf(msg, sizeof(msg), "包月剩余:%d天", left);
			mLeftDateWnd.SetWindowTextA(msg);
      
      DbgOutput(DBG_ERR, "包月剩余:%d天\n", left);
		}
	}
  #endif
}

void CLeftPierPanel::UpdateMircoVal(int val)
{
	char tmp[32] = {0};
	sprintf(tmp, "%d", val);
	mMicValueWnd.SetWindowTextA(tmp);
}

void CLeftPierPanel::UpdageVolumeVal(int val)
{
	char tmp[32] = {0};
	sprintf(tmp, "%d", val);
	//mLeftItem[MusicVol_item].SetWindowTextA(tmp);
	mVolValueWnd.SetWindowTextA(tmp);
}

void CLeftPierPanel::ResetAttunement(void)
{
	SetTune(tune_KSong,true);

	gs_SystemSetting.mucMicroVol = (unsigned char)72;
	SetMircoVal(gs_SystemSetting.mucMicroVol,true);

	gs_SystemSetting.mucAttunementVol = (unsigned char)64;
	SetVolumeVal(gs_SystemSetting.mucAttunementVol,true);

	SaveSystemSetting();
}

void CLeftPierPanel::SetTune(int idx, BOOL bSave)
{
	if ( idx >= tune_profession && idx < tune_count )
	{
		//mToneBtnGroup.SetSelectButton(idx);
		gs_SystemSetting.mucPcmTuneType = (unsigned char) idx;

    		///for test
    		//mAttuneType = MealKtv_Attune ;
		mAttuneType =  gs_SystemSetting.mucPcmTuneType;

    		if(mAttuneType == MealKtv_Attune)
			mPreampDevCtrl.MealKtvSetPCMTuneType(gs_SystemSetting.mucPcmTuneType);
    		else if(mAttuneType == SingBar_Attune)
      			mPreampDevCtrl.SetPCMTuneType(gs_SystemSetting.mucPcmTuneType);
    		else
      			mPreampDevCtrl.SetPCMTuneType(gs_SystemSetting.mucPcmTuneType);
    
		DbgOutput(DBG_ERR, "SetTune = %d\n", mAttuneType);
		if ( bSave )
			SaveSystemSetting();
	}
}

void CLeftPierPanel::SetMircoVal(int val, BOOL bSave)
{
	UpdateMircoVal(val);

	gs_SystemSetting.mucMicroVol = (unsigned char)val;

  	///for test
  	mAttuneType = MealKtv_Attune ;
  
  	DbgOutput(DBG_ERR, "SetTune = %d\n", mAttuneType);
  	if(mAttuneType == MealKtv_Attune)
		mPreampDevCtrl.MealKtvSetMicroVol(gs_SystemSetting.mucMicroVol);
  	else if(mAttuneType == SingBar_Attune)
    		mPreampDevCtrl.SetMicroVol(gs_SystemSetting.mucMicroVol);
  	else
    		mPreampDevCtrl.SetMicroVol(gs_SystemSetting.mucMicroVol);
  
	if ( bSave )
		SaveSystemSetting();

}

void CLeftPierPanel::SetVolumeVal(int val, BOOL bSave)
{
	UpdageVolumeVal(val);
	gs_SystemSetting.mucAttunementVol = (unsigned char)val;

  	///for test
  	//mAttuneType = MealKtv_Attune ;
  
	if(mAttuneType == MealKtv_Attune)
	  	mPreampDevCtrl.MealKtvSetVolume(gs_SystemSetting.mucAttunementVol);
	else if(mAttuneType == SingBar_Attune)
		mPreampDevCtrl.SetVolume(gs_SystemSetting.mucAttunementVol);
	else
		mPreampDevCtrl.SetVolume(gs_SystemSetting.mucAttunementVol);
  
	if ( bSave )
		SaveSystemSetting();
}