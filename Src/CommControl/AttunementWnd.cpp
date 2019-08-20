#include "AttunementWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "MainCtrlPage.h"
#include "../PlayerApi/PlayerInfo.h"


#define BTN_CLOSE_ID			0x01
#define BTN_MICRO_ID			0x02
#define BTN_VOLUME_ID		0x03
#define BTN_RESET_ID				0x04
#define BTN_TUNE_BASE_ID	0x10

extern CMainCtrlPage *gMainCtrlPage;

void CAttunementWnd::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 30);
	CParentClass::Create(
		"AttunementWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	RECT rcControl = {0, 0, 100, 100};
	mCloseBtn.Create(this, rcControl);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	for ( int i = 0; i < tune_count; i++ )
	{
		mTuneItemsBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mTuneItemsBtn[i].SetWndID(BTN_TUNE_BASE_ID + i);
		mTuneItemsBtn[i].SetOnClickListener(this);
		mToneBtnGroup.AddToGroup(&mTuneItemsBtn[i]);
	}

	RECT rcSlide = {0, 0, 100, 100};
	RECT rcTailBtn = {0, 0, 100, 100};
	RECT rcHeadBtn = {0, 0, 100, 100};
	RECT rcBar = {0, 0, 100, 100};
	RECT rcThumb = {0, 0, 100, 100};

	mMicroBoxWnd.CreateStatic(this, rcControl);
	//mMicroIcoWnd.CreateStatic(&mMicroBoxWnd, rcControl);
	mMicroValWnd.CreateStatic(&mMicroBoxWnd, rcControl);

	mMicroSlide.SetMinMax(0, 100);
	mMicroSlide.Create(&mMicroBoxWnd, rcSlide, rcBar, rcThumb, rcHeadBtn, rcTailBtn, TRUE, FALSE);
	mMicroSlide.SetWndID(BTN_MICRO_ID);
	mMicroSlide.SetOnPositionChangeListener(this);

	mVolBoxWnd.CreateStatic(this, rcControl);
	//mVolIcoWnd.CreateStatic(&mVolBoxWnd, rcControl);
	mVolValWnd.CreateStatic(&mVolBoxWnd, rcControl);

	mVolumeSlide.SetMinMax(0, 100);
	mVolumeSlide.Create(&mVolBoxWnd, rcSlide, rcBar, rcThumb, rcHeadBtn, rcTailBtn, TRUE, FALSE);
	mVolumeSlide.SetWndID(BTN_VOLUME_ID);
	mVolumeSlide.SetOnPositionChangeListener(this);

	mResetWnd.Create(this, rcControl);
	mResetWnd.SetWndID(BTN_RESET_ID);
	mResetWnd.SetOnClickListener(this);

	mPreampDevCtrl.OpenPreampDev();
  
  mAttuneType = 1;

	SetTune(gs_SystemSetting.mucPcmTuneType);
	SetMircoVal(gs_SystemSetting.mucMicroVol);
	SetVolumeVal(gs_SystemSetting.mucAttunementVol);

	LoadResource();
}

void CAttunementWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "AttunementWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcShow, rcHide, rcControl, rcBaseItem;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];
	char colorbuf[16];
	int size, xOffset;
	UINT32 color;

	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	XmlLoadRect(&parser, "HideWindowInfo", &rcHide);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(rcShow, rcHide);

	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "RepayWnd/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "TuneBaseItemInfo", &rcBaseItem);
	xOffset = parser.GetIntValue("TuneBaseItemInfo", "xOffset", 63);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("TuneBaseItemInfo", "basePath", "Settings/SettingsBKGround"), sizeof(imgPath));

	for ( int i = 0 ; i < tune_count; i++ )
	{
		char path[128] = {0};
		sprintf(path, "%s/Tune%d", imgPath, i);

		InflatRect(&rcControl, &rcBaseItem, i*xOffset, 0);

		CreateBtnImgTextures(path, btnTextures);
		mTuneItemsBtn[i].SetTextures(btnTextures);
		mTuneItemsBtn[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "MicroBoxWndInfo", &rcControl);
	mMicroBoxWnd.MoveWindow(&rcControl);

	//XmlLoadRect(&parser, "MicroIcoWndInfo", &rcControl);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("MicroIcoWndInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	//CreateImgTexture(imgPath, &bkTexture);
	//mMicroIcoWnd.SetBackgroundTexture(&bkTexture);
	//mMicroIcoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "MicroValWndInfo", &rcControl);
	size = parser.GetIntValue("MicroValWndInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("MicroValWndInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mMicroValWnd.SetFontSize(size);
	mMicroValWnd.SetFontColor(color);
	mMicroValWnd.MoveWindow(&rcControl);

	RECT rcSlide, rcTailBtn, rcHeadBtn, rcBar, rcThumb;
	CTexture barBkTexture, barProcessTexture, thumbTextures[4], headTextures[4], tailTextures[4];

	XmlLoadRect(&parser, "MicroSlideBKGroundInfo", &rcSlide);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("MicroSlideBKGroundInfo", "path", "Attunement/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &barBkTexture);

	SAFE_STRNCPY(imgPath, parser.GetStrValue("BarProcessBtnInfo", "path", "Attunement/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &barProcessTexture);

	SAFE_STRNCPY(imgPath, parser.GetStrValue("ThumbBtnInfo", "path", "Attunement/thumb"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, thumbTextures);

	XmlLoadRect(&parser, "HeadBtnInfo", &rcHeadBtn);
	XmlLoadRect(&parser, "TailBtnInfo", &rcTailBtn);
	XmlLoadRect(&parser, "BarProcessBtnInfo", &rcBar);
	XmlLoadRect(&parser, "ThumbBtnInfo", &rcThumb);

	mMicroSlide.SetTextures(&barBkTexture, &barProcessTexture, thumbTextures, headTextures, tailTextures);
	mMicroSlide.MoveWindow(rcSlide, rcBar, rcThumb, rcHeadBtn, rcTailBtn);

	XmlLoadRect(&parser, "VolumeSlideBKGroundInfo", &rcSlide);

	mVolumeSlide.SetTextures(&barBkTexture, &barProcessTexture, thumbTextures, headTextures, tailTextures);
	mVolumeSlide.MoveWindow(rcSlide, rcBar, rcThumb, rcHeadBtn, rcTailBtn);

	XmlLoadRect(&parser, "VolBoxWndInfo", &rcControl);
	mVolBoxWnd.MoveWindow(&rcControl);

	//XmlLoadRect(&parser, "VolIcoWndInfo", &rcControl);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("VolIcoWndInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	//CreateImgTexture(imgPath, &bkTexture);
	//mVolIcoWnd.SetBackgroundTexture(&bkTexture);
	//mVolIcoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "VolValWndInfo", &rcControl);
	size = parser.GetIntValue("VolValWndInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("VolValWndInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mVolValWnd.SetFontSize(size);
	mVolValWnd.SetFontColor(color);
	mVolValWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ResetWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ResetWndInfo", "path", "RepayWnd/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mResetWnd.SetTextures(btnTextures);
	mResetWnd.MoveWindow(&rcControl);
}

void CAttunementWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_CLOSE_ID )
		SlideShowWindow(FALSE);
	else if ( nWndID == BTN_RESET_ID )
		ResetAttunement();
	else if ( nWndID >=  (BTN_TUNE_BASE_ID + tune_profession) && nWndID < (BTN_TUNE_BASE_ID + tune_count) )
		SetTune(nWndID - BTN_TUNE_BASE_ID, TRUE );
}

void CAttunementWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
}

void CAttunementWnd::OnPositionChange(CBaseWnd *pWnd, int nOldPosition, int nNewPostion, BOOL bChangeEnd)
{
	int nWndID = pWnd->GetWndID();
	if ( bChangeEnd )
	{
		switch (nWndID)
		{
		case BTN_MICRO_ID:
			SetMircoVal(nNewPostion, TRUE);
			break;

		case BTN_VOLUME_ID:
			SetVolumeVal(nNewPostion, TRUE);
			break;

		default:
			break;
		}
	}
}

void CAttunementWnd::ResetAttunement(void)
{
	SetTune(tune_KSong);

	gs_SystemSetting.mucMicroVol = (unsigned char)72;
	SetMircoVal(gs_SystemSetting.mucMicroVol);

	gs_SystemSetting.mucAttunementVol = (unsigned char)64;
	SetVolumeVal(gs_SystemSetting.mucAttunementVol);

	SaveSystemSetting();
}

void CAttunementWnd::SetTune(int idx, BOOL bSave)
{
	if ( idx >= tune_profession && idx < tune_count )
	{
		mToneBtnGroup.SetSelectButton(idx);
		gs_SystemSetting.mucPcmTuneType = (unsigned char) idx;

    ///for test
    mAttuneType = MealKtv_Attune ;

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

void CAttunementWnd::SetMircoVal(int val, BOOL bSave)
{
	mMicroSlide.SetPos(val);
	char tmp[32] = {0};
	sprintf(tmp, "%d", val);
	mMicroValWnd.SetWindowTextA(tmp);

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

	gMainCtrlPage->mLeftPierPanel.UpdateMircoVal(val);
}

void CAttunementWnd::SetVolumeVal(int val, BOOL bSave)
{
	mVolumeSlide.SetPos(val);
	char tmp[32] = {0};
	sprintf(tmp, "%d", val);
	mVolValWnd.SetWindowTextA(tmp);
	gs_SystemSetting.mucAttunementVol = (unsigned char)val;

  ///for test
  mAttuneType = MealKtv_Attune ;
  if(mAttuneType == MealKtv_Attune)
	  mPreampDevCtrl.MealKtvSetVolume(gs_SystemSetting.mucAttunementVol);
  else if(mAttuneType == SingBar_Attune)
	  mPreampDevCtrl.SetVolume(gs_SystemSetting.mucAttunementVol);
  else
	  mPreampDevCtrl.SetVolume(gs_SystemSetting.mucAttunementVol);
  
	if ( bSave )
		SaveSystemSetting();

  if(0)
   {
       gPlayerInfo->SetPadChangeState(TRUE);
       gPlayerInfo->SetVolume(val);
       gMainCtrlPage->SetVolumePos(val);
       gPlayerInfo->SetPadChangeState(FALSE);
  
  }

	gMainCtrlPage->mLeftPierPanel.UpdageVolumeVal(val);
}

