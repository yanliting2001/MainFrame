#include "MainCtrlPage.h"
#include "UserMsgDefine.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
#include "FileOperate.h"

CCalibrateWnd *gCalibrateWnd = NULL;

CMainCtrlPage::CMainCtrlPage()
{
	miCurVol = 0;
	mCurShowFragmentType = Fragment_HomePage;
}

CMainCtrlPage::~CMainCtrlPage()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CMainCtrlPage::Create(CBaseWnd *pParent)
{
#if USE_MODULE_BCM
	theE3DEngine->setTransparentRect(true, -320, -180, LAYOUT_WIDTH, LAYOUT_HEIGHT);

#endif
	DbgOutput(DBG_INFO, "CMainCtrlPage start Create.\n");
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	CParentClass::Create("MainCtrlPage", pParent, rc);

	miCurVol =  gs_SystemSetting.mucAttunementVol;

	mWaitingWnd.Create(this);
	mWaitingWnd.StartWaiting();

	theBaseApp->SuspendInput();

	RECT rcControl = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	mTopPierPanel.Create(this);
	mLeftPierPanel.Create(this);
	mBottomPierPanel.Create(this);

	mHomePageFragment.Create(this);
	mSingerTypeFragment.Create(this);
	mSingerListFragment.Create(this);

	mLangSongFragment.Create(this);
	mAlbumSongFragment.Create(this);
	mSongListFragment.Create(this);
	mSettingsFragment.Create(this);
	mSelectedFragment.Create(this);
  
	mCloudFragment.Create(this, &gSearchMonthType);
	mDownloadListFragment.Create(this);
	mUDiskFragment.Create(this);
  
	mBottomPierPanel.CreatePopupWnd(this);
	mBarCodeWnd.Create(this);
  mYiLangWnd.Create(this);
  
	mCalibrateWnd.Create(this,rcControl);
	mCalibrateWnd.SetWindowVisible(FALSE);
	gCalibrateWnd = &mCalibrateWnd;

	RECT rcCheck = {300, 150, 900, 550};
	mMacCheckWnd.SetZorderNumber(E3D_ZORDER_NUMBER_MIN);
	mMacCheckWnd.CreateStatic(pParent, rcCheck, WINDOWFLAG_MANUALSETZORDER, WINDOWSTATE_INVISIBLE);
	mMacCheckWnd.SetWindowTextW(L"未经本公司注册，请注册后使用！！");
	mMacCheckWnd.SetFontSize(30);
	mMacCheckWnd.SetFontColor(0xFFFF0000);

	CreatePopupWnd();

	RECT rcSearchInput = {50, -10, 328, 37};
	RECT rcSearchBtn = { 0, 2, 82, 33};
	RECT rcTextEdit = {82, 2, 240, 37};
	RECT rcInputPanel =  {50, 100, 455, 504};

	mSearchInputWnd.Create(
		this,
		rcSearchInput,
		rcSearchBtn,
		rcTextEdit,
		rcInputPanel,
		FALSE,
		TRUE);
	mSearchInputWnd.SetPromptText(L"搜索");

	mRepayWnd.Create(this);
	mSharingLogoutWnd.Create(this);
  
  
	mSharingIndexWnd.SetLoginQRCodeVisible(TRUE);
	mSharingIndexWnd.Create(this);
  mActivityContentWnd.Create(this);
	mUpgradeWnd.Create(this, rcControl);

  mDlnaWnd.Create(this);
	mServerQRCodeWnd.Create(this);
  mRecordQRCodeWnd.Create(this);
	mSwitchPromptWnd.Create(this);
	mDlnaWnd.GenerateQRCodeWnd();
  mDlnaList.Create(this);
  mShopWnd.Create(this);
  mFoodWnd.Create(this);
  mPromptMsgWnd.Create(this);
	LoadResource();
  
  #if USE_FREE_MODE
  mSharingIndexWnd.SetWindowVisible(FALSE);
  gPlayerInfo->ClearSelectedSongList();
  #endif
  
	mWaitingWnd.StopWaiting();
	DbgOutput(DBG_INFO, "CMainCtrlPage Create end.\n");
}

void CMainCtrlPage::CreatePopupWnd(void)
{
	//mAttunementWnd.Create(this);
	mBaseLoginWnd.Create(this);
	mAmbienceWnd.Create(this);
  mFavoritWnd.Create(this);
}

void CMainCtrlPage::LoadResource()
{
	CParentClass::LoadResource();
	XMLParser parser;
	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "MainCtrlPage.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	CTexture bkTexture;
	char imgPath[64] = {0};

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("rcLocation", "path", "HomePageBK"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);

	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcControl);

	RECT rcSearchInput;
	RECT rcSearchBtn;
	RECT rcTextEdit;
	RECT rcInputPanel;

	XmlLoadRect(&parser, "SearchInputLocation", &rcSearchInput);
	XmlLoadRect(&parser, "SearchBtnLocation", &rcSearchBtn);
	XmlLoadRect(&parser, "TextEditLocation", &rcTextEdit);
	XmlLoadRect(&parser, "InputWndLocation", &rcInputPanel);

	mSearchInputWnd.MoveWindow(
		rcSearchInput,
		rcSearchBtn,
		rcTextEdit,
		rcInputPanel);
	mSearchInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
}

void CMainCtrlPage::OnClick(CBaseWnd *pWnd, POINT pt)
{
}

void CMainCtrlPage::OnSlideWndEnd(CBaseWnd *pWnd, BOOL bVisible, UINT64 uUserData)
{
	if (pWnd == &mBottomPierPanel)
	{
		SetWindowVisible(bVisible);
	}
}

void CMainCtrlPage::OnReturnClick()
{
	CFragment *pFragment = GetCurFragment();
	if (!pFragment->OnReturn())
	{
		ReturnToLastFragment();
	}
}

void CMainCtrlPage::OnChildFragmentChange()
{
	mBottomPierPanel.OnChildFragmentChange();
}

void CMainCtrlPage::Show(BOOL bShow)
{
	SetWindowVisible(bShow);
	SetFocus();

	mBottomPierPanel.SlideShowWindow(bShow);
	mTopPierPanel.SlideShowWindow(bShow);
	mLeftPierPanel.SlideShowWindow(bShow);
  if ( mSongListFragment.IsWindowVisible())
  {
    SetCurrentFragment(Fragment_HomePage);
  }
}

void CMainCtrlPage::SetCurrentFragment(
	CHILDPAGE_FRAGMENT_TYPE eFragmentType)
{
	CFragment *pCurFragment = GetChildFragmentByID(mCurShowFragmentType);
	if(eFragmentType == (CHILDPAGE_FRAGMENT_TYPE)pCurFragment->GetLastFragmentType())
	{
		CFragment *pFragment = GetChildFragmentByID(eFragmentType);
		pFragment->SwitchTo(Fragment_HomePage, eFragmentType);
	}
	else
	{
		CFragment *pFragment = GetChildFragmentByID(eFragmentType);
		pFragment->SwitchTo(mCurShowFragmentType, eFragmentType);
	}

	ShowChildFragment(eFragmentType);
}

void CMainCtrlPage::ReturnToLastFragment()
{
	CFragment *pFragment = GetChildFragmentByID(mCurShowFragmentType);
	CHILDPAGE_FRAGMENT_TYPE eFragmentType = (CHILDPAGE_FRAGMENT_TYPE)pFragment->GetLastFragmentType();
	ShowChildFragment(eFragmentType);
}

CHILDPAGE_FRAGMENT_TYPE CMainCtrlPage::GetCurFragmentType()
{
	return mCurShowFragmentType;
}

CFragment* CMainCtrlPage::GetCurFragment()
{
	return GetChildFragmentByID(mCurShowFragmentType);
}

void CMainCtrlPage::ShowChildFragment(
	CHILDPAGE_FRAGMENT_TYPE eFragmentType)
{
	DbgOutput(DBG_DEBUG, "Show child fragment %d\n", eFragmentType);
	mCurShowFragmentType = eFragmentType;
	OnChildFragmentChange();

	mHomePageFragment.SetWindowVisible(FALSE);
	mSingerTypeFragment.SetWindowVisible(FALSE);
	mSingerListFragment.SetWindowVisible(FALSE);

	mLangSongFragment.SetWindowVisible(FALSE);
	mAlbumSongFragment.SetWindowVisible(FALSE);
	mSongListFragment.SetWindowVisible(FALSE);

	mSettingsFragment.SetWindowVisible(FALSE);
	mSelectedFragment.SetWindowVisible(FALSE);
	mUDiskFragment.SetWindowVisible(FALSE);
  mDownloadListFragment.SetWindowVisible(FALSE);
	mCloudFragment.SetWindowVisible(FALSE);
  mYiLangWnd.SetWindowVisible(FALSE);
  
	switch(eFragmentType)
	{
	case Fragment_HomePage:
    
		mHomePageFragment.SetWindowVisible(TRUE);
    mHomePageFragment.mHomeItemBKWnd.MoveWindow(&mHomePageFragment.mrcOrigItemsBKShow);
		mHomePageFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(NULL);
		break;

	case Fragment_SingerType:
		mSingerTypeFragment.SetWindowVisible(TRUE);
		mSingerTypeFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(NULL);
		break;

	case Fragment_SingerList:
		mSingerListFragment.SetWindowVisible(TRUE);
		mSingerListFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(&mSingerListFragment);
		mSearchInputWnd.SetInputWndVisible(TRUE);
		break;

	case Fragment_LangSong:
		mLangSongFragment.SetWindowVisible(TRUE);
		mLangSongFragment.SetFocus();
		break;

	case Fragment_AlbumSong:
		mAlbumSongFragment.SetWindowVisible(TRUE);
		mAlbumSongFragment.SetFocus();
		break;

	case Fragment_SongList:
    //SetBackgroundTexture(&mSongPagebkTexture);
		mSongListFragment.SetWindowVisible(TRUE);
		mSongListFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(&mSongListFragment);
		mSearchInputWnd.SetInputWndVisible(TRUE);
		break;

	case Fragment_Settings:
		mSettingsFragment.SetWindowVisible(TRUE);
		mSettingsFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(NULL);
		break;

	case Fragment_Selected:
		mSelectedFragment.SetWindowVisible(TRUE);
		mSelectedFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(NULL);
		break;
  
  case Fragment_UDisk:
    //SetBackgroundTexture(&mHomePagebkTexture);
    mUDiskFragment.InitUDiskFileList();
    mUDiskFragment.SetWindowVisible(TRUE);
    mUDiskFragment.SetFocus();
    mSearchInputWnd.SetMsgRecvWnd(&mUDiskFragment);
    break;
  case Fragment_Download:
		//SetBackgroundTexture(&mHomePagebkTexture);
		mDownloadListFragment.InitDownloadList();
		mDownloadListFragment.SetWindowVisible(TRUE);
		mDownloadListFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(NULL);
		break;
  case Fragment_Cloud:
		mCloudFragment.SetWindowVisible(TRUE);
		mCloudFragment.SetFocus();
		mSearchInputWnd.SetMsgRecvWnd(&mCloudFragment);
		break;
  case Fragment_YiLang:
		mYiLangWnd.SetWindowVisible(TRUE);
		mYiLangWnd.SetFocus();
		break;
    
	default:
		break;
	}
}

void CMainCtrlPage::SetPhantomVisible(BOOL iVisible)
{
	if(iVisible)
	{
		if(mCurShowFragmentType == Fragment_HomePage
				/*&& !mMovieFragment.IsWindowVisible()*/
				//&& !mAdvanceSettingsWnd.IsWindowVisible()
				&& !mAmbienceWnd.IsWindowVisible()/*
				&& !mBottomPierPanel.mSelectedList.IsWindowVisible() */)
		{
			//mMTVBaseWnd.SetMTVVisible(TRUE);

		}
	}
	else
	{
	//	mPhantomBtn.SetWindowVisible(FALSE);
//		if ( !(mBottomPierPanel.mSelectedList.IsWindowVisible())/* ||
//			 ( mBottomPierPanel.mSelectedList.mCurrentTab == headTab_Mtv) */)
//		{
//			//mMTVBaseWnd.SetMTVVisible(FALSE);
//
//		}
	}
}


CFragment* CMainCtrlPage::GetChildFragmentByID(
	CHILDPAGE_FRAGMENT_TYPE eFragmentType)
{
	CFragment *pFragment = &mHomePageFragment;

	switch(eFragmentType)
	{
	case Fragment_HomePage:
		pFragment = &mHomePageFragment;
		break;

	case Fragment_SingerType:
		pFragment = &mSingerTypeFragment;
		break;

	case Fragment_SingerList:
		pFragment = &mSingerListFragment;
		break;

	case Fragment_LangSong:
		pFragment = &mLangSongFragment;
		break;

	case Fragment_AlbumSong:
		pFragment = &mAlbumSongFragment;
		break;

	case Fragment_SongList:
		pFragment = &mSongListFragment;
		break;

	case Fragment_Settings:
		pFragment = &mSettingsFragment;
		break;

	case Fragment_Selected:
		pFragment = &mSelectedFragment;
		break;

	default:
		break;
	}

	return pFragment;
}

void CMainCtrlPage::AddToFavorit(const char *pSongID)
{
	//f(!pSongID)
	//	return ;
	
  DbgOutput(DBG_INFO, "%s, line = %d\n", __FUNCTION__, __LINE__);
	mBaseLoginWnd.AddSongToFavorit(pSongID);
}

void CMainCtrlPage::UserLoginExit()
{
	mBaseLoginWnd.ExitFavoritLogin();
}

void CMainCtrlPage::SetVolumePos(int iPos)
{
	SetVolume(iPos, TRUE);
}

int CMainCtrlPage::GetVolumePos()
{
	return miCurVol;
}

void CMainCtrlPage::SetPalyState(BOOL bState)
{
	mBottomPierPanel.mSubItem[BTN_RESUME_ID].SetRadioBtnPressed(bState);
}

void CMainCtrlPage::SetChannelState(BOOL bState)
{
	mBottomPierPanel.mSubItem[BTN_ORIGINAL_ID].SetRadioBtnPressed(bState);
}

BOOL CMainCtrlPage::IsOriginal()
{
	return mBottomPierPanel.mSubItem[BTN_ORIGINAL_ID].GetRadioBtnPressed() ? TRUE : FALSE;
}

void CMainCtrlPage::StartWaiting()
{
	mWaitingWnd.StartWaiting();
	theBaseApp->SuspendInput();
}

void CMainCtrlPage::StopWaiting()
{
	mWaitingWnd.StopWaiting();
	theBaseApp->ResumeInput();
}

void CMainCtrlPage::SetMacCheckWndVisible(BOOL bVisible)
{
		mMacCheckWnd.SetWindowVisible(bVisible);
}

void CMainCtrlPage::SetInvalidReason(const char *cText)
{
		mMacCheckWnd.SetWindowTextA(cText);
		mMacCheckWnd.SetWindowVisible(TRUE);
}

// new
void CMainCtrlPage::SetPopuWndVisible(POPUPWNDITEM_E item, BOOL bVisible, int subItem)
{
	switch ( item )
	{
	case attunement_item:
		mAttunementWnd.SlideShowWindow(bVisible);
		break;

	case login_item:
		if ( bVisible )
		{
			CBaseLoginWnd::LOGINTYPE_E subIdx = (CBaseLoginWnd::LOGINTYPE_E) subItem;
			mBaseLoginWnd.SetLoginType(subIdx);
		}
		mBaseLoginWnd.SlideShowWindow(bVisible);
		break;
  case ambienceWnd_item:
		mAmbienceWnd.SlideShowWindow(bVisible);
		break;
  case favorit_item:
		mFavoritWnd.SlideShowWindow(bVisible);
		break;
 
	default:
		break;
	}
}

void CMainCtrlPage::SetVolume(int iVol, BOOL bSendCmd)
{
	if ( iVol < 0 )
		iVol =0;
	if ( iVol > 100 )
		iVol = 100;

	miCurVol = iVol;
	SetMute(FALSE, FALSE);

	if ( bSendCmd && gPlayerInfo )
	{
		gPlayerInfo->SetVolume(miCurVol);
	}
}

int CMainCtrlPage::GetVolume(void)
{
	return miCurVol;
}

void CMainCtrlPage::SetMute(BOOL bMute, BOOL bSendCmd)
{
	if ( bSendCmd && gPlayerInfo )
	{
		gPlayerInfo->SetVolume(bMute ? 0 : miCurVol);
	}
}

void CMainCtrlPage::CloseAllPopWindows(void)
{
	if ( mRepayWnd.IsWindowVisible() )
		mRepayWnd.SetWindowVisible(FALSE);
	mRepayWnd.SetPayQRCodeWnd(FALSE);
	mServerQRCodeWnd.SetWindowVisible(FALSE);
  mRecordQRCodeWnd.SetWindowVisible(FALSE);
}
void CMainCtrlPage::SetServerQRCodeVisible(void)
{
	if ( mServerQRCodeWnd.IsWindowVisible() )
		mServerQRCodeWnd.SlideShowWindow(FALSE);
	else
		mServerQRCodeWnd.SlideShowWindow(TRUE);
}

void CMainCtrlPage::SetRecordQRCodeVisible(void)
{
	if ( mRecordQRCodeWnd.IsWindowVisible() )
		mRecordQRCodeWnd.SlideShowWindow(FALSE);
	else
		mRecordQRCodeWnd.SlideShowWindow(TRUE);
}

void CMainCtrlPage::SetDlnaInfoVisible(void)
{
	if ( mDlnaList.IsWindowVisible() )
		mDlnaList.SlideShowWindow(FALSE);
	else
		mDlnaList.SlideShowWindow(TRUE);
}

void CMainCtrlPage::SetPromptMsgVisible(void)
{
		if ( !mPromptMsgWnd.IsWindowVisible() )
		mPromptMsgWnd.SlideShowWindow(TRUE);
}




