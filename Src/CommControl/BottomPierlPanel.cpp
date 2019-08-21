#include "BottomPierPanel.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "CommApi.h"
#include "UserMsgDefine.h"
#include"../PlayerApi/PlayerInfo.h"
#include "MainCtrlPage.h"
#include "../Sharing/ClientSharing.h"
#include "PageManager.h"

extern CPageManager *gPageManager;

extern CMainCtrlPage *gMainCtrlPage;

void CBottomPierPanel::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
  
	SetRectXY(&rcShow, 0, 593, 1280, 127);
	SetRectXY(&rcHide, 0, 720, 1280, 127);
	CParentClass::Create(
		"BottomPier",
		pParent,
		WINDOWFLAG_DEFAULT,
		rcShow,
		rcHide,
		FALSE);

	RECT rcControl;

	SetRectXY(&rcControl, 0, 25, 98, 106);
	
  for (int i = 0; i < BTNTYPE_COUNDT; i++ )
	{
    if(i == BTN_ORIGINAL_ID || i == BTN_RESUME_ID)
      mSubItem[i].Create(this, rcControl, BUTTONTYPE_RADIO, false, PRESSED_SCALE);
    else
		mSubItem[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
		mSubItem[i].SetWndID(BTN_HOMEPAGE_ID+i);
		mSubItem[i].SetOnClickListener(this);
	}
  //mSingedCntWnd.CreateStatic(this, rcControl);
  
  if ( gs_SystemSetting.miStartUp_orignal == 1 )
		SetAccompany(FALSE, TRUE);
  
	RefreshSelectedCount();

	LoadResource();
}

void CBottomPierPanel::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "BottomPierlPanel.xml");
	if(false == parser.Load(cFileName))
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
	CTexture btnTextures[4];
	UINT32 numColor;
	char colorbuf[16];
	int size = 20;


  XmlLoadRect(&parser, "HomeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("HomeBtnInfo", "path", "BottomPier/Home"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_HOMEPAGE_ID].SetTextures(btnTextures);
	mSubItem[BTN_HOMEPAGE_ID].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "ServerBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ServerBtnInfo", "path", "BottomPier/Server"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_SERVER_ID].SetTextures(btnTextures);
	mSubItem[BTN_SERVER_ID].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PushBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PushBtnInfo", "path", "BottomPier/dlna"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_PUSH_ID].SetTextures(btnTextures);
	mSubItem[BTN_PUSH_ID].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PlayPauseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PlayPauseBtnInfo", "path", "BottomPier/Resume"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_RESUME_ID].SetTextures(btnTextures);
	mSubItem[BTN_RESUME_ID].MoveWindow(&rcControl);


  XmlLoadRect(&parser, "OriginalBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("OriginalBtnInfo", "path", "BottomPier/Original"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_ORIGINAL_ID].SetTextures(btnTextures);
	mSubItem[BTN_ORIGINAL_ID].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReplayBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReplayBtnInfo", "path", "BottomPier/Replay"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_REPLAY_ID].SetTextures(btnTextures);
	mSubItem[BTN_REPLAY_ID].MoveWindow(&rcControl);



	XmlLoadRect(&parser, "SwitchBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SwitchBtnInfo", "path", "BottomPier/Switch"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_SWITCH_ID].SetTextures(btnTextures);
	mSubItem[BTN_SWITCH_ID].MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "SelectedBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SelectedBtnInfo", "path", "BottomPier/Selected"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSubItem[BTN_SELECTED_ID].SetTextures(btnTextures);
	mSubItem[BTN_SELECTED_ID].MoveWindow(&rcControl);

  /*
  XmlLoadRect(&parser, "SingedCntWndInfo", &rcControl);
	size = parser.GetIntValue("SingedCntWndInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("SingedCntWndInfo", "color", "FF0000FF"), sizeof(colorbuf));
	numColor = strtoul(colorbuf,NULL,16);
	mSingedCntWnd.SetFontSize(size);
	mSingedCntWnd.SetFontColor(numColor);
	mSingedCntWnd.MoveWindow(&rcControl);
*/
  
}

void CBottomPierPanel::CreatePopupWnd(CBaseWnd *pParent)
{
}

void CBottomPierPanel::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	switch (uType)
	{
	case PLAYERINFO_ORIGINAL_CHANGE:
		SetAccompany(gs_SystemSetting.miStartUp_stwichOrignal != 1, TRUE);
		break;
	case PLAYERINFO_SONGSTATE_CHANGE:
		RefreshSelectedCount();
		break;
	}
}

void CBottomPierPanel::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();

  #if !USE_FREE_MODE  //当免费登陆时，时间和歌曲数量为零时不禁止操作。
  if ( CClientSharing::mUserInfo.t_count==0 && CClientSharing::mUserInfo.n_count==0 )
	{
		gMainCtrlPage->mLeftPierPanel.ShowRepayVisible(TRUE);
		return ;
	}
#endif

	switch (nWndID)
	{
	case BTN_HOMEPAGE_ID:
		if ( gMainCtrlPage->GetCurFragmentType() != Fragment_HomePage )
          		gMainCtrlPage->SetCurrentFragment(Fragment_HomePage);
		break;

  	case BTN_SERVER_ID:
    		gMainCtrlPage->SetServerQRCodeVisible();
		break;
	case BTN_PUSH_ID:
		//gMainCtrlPage->SetDlnaInfoVisible();
		gPageManager->SetCurrentPage(Page_PhantomPage);
		break;
  /*
  case BTN_MOVIE_ID:    
		gPageManager->SetCurrentPage(Page_PhantomPage);
		break;
  case BTN_AMBIENCE_ID:
      
    if ( gMainCtrlPage->GetCurFragmentType() != ambienceWnd_item )
		{
			gMainCtrlPage->SetPopuWndVisible(ambienceWnd_item, TRUE);
		}
		break;
        */
	case BTN_ORIGINAL_ID:
		SetAccompany(!mSubItem[BTN_ORIGINAL_ID].GetRadioBtnPressed(), TRUE);
		break;

	case BTN_REPLAY_ID:
		DELAYCLICK(3000);
		if ( CanBeSwitch() )
		{
			if ( gPlayerInfo )
				gPlayerInfo->Replay();
			SetPausePlay(FALSE, FALSE);
		}
		else
		{
			gMainCtrlPage->mSwitchPromptWnd.SwitchPrompt();
		}
		break;

	case BTN_RESUME_ID:
		if ( CanBeSwitch() )
		{
			SetPausePlay(mSubItem[BTN_RESUME_ID].GetRadioBtnPressed(), TRUE);
		}
		else
		{
			mSubItem[BTN_RESUME_ID].SetRadioBtnPressed(FALSE);
			gMainCtrlPage->mSwitchPromptWnd.SwitchPrompt();
		}
		break;

	case BTN_SWITCH_ID:
		DELAYCLICK(3000);
		if ( CanBeSwitch() )
		{
			if ( gPlayerInfo )
				gPlayerInfo->PlayNext(TRUE);
			SetPausePlay(FALSE, FALSE);
		}
		else
		{
			gMainCtrlPage->mSwitchPromptWnd.SwitchPrompt();
		}
		break;

	case BTN_SELECTED_ID:
		gMainCtrlPage->mSelectedFragment.SetSelectedType(selectedTab_Selected);
		gMainCtrlPage->SetCurrentFragment(Fragment_Selected);
		break;

	default:
    
    DbgOutput(DBG_ERR, "OnClick=  %d LINE = %d !!\n", nWndID, __LINE__);
		break;
	}
}

void CBottomPierPanel::SetAccompany(BOOL bAccompany, BOOL bSendCmd)
{
	mSubItem[BTN_ORIGINAL_ID].SetRadioBtnPressed(!bAccompany);
	if ( bSendCmd )
	{
		gPlayerInfo->SetOriginal(bAccompany);
	}
}

void CBottomPierPanel::SetPausePlay(BOOL bPause, BOOL bSendCmd)
{
	mSubItem[BTN_RESUME_ID].SetRadioBtnPressed(bPause);
	if ( bSendCmd && gPlayerInfo )
	{
		if ( bPause )
			gPlayerInfo->Pause();
		else
			gPlayerInfo->Resume();
	}
}

void CBottomPierPanel::RefreshSelectedCount(void)
{
	int iSelectedCnt = 0, iSingedCnt = 0;
	if ( gPlayerInfo )
	{
		iSelectedCnt = gPlayerInfo->GetSelectedSongCount();
		iSingedCnt = gPlayerInfo->GetSingedSongCount();
	}
	CBaseStringW SelectedCnt;
	//SelectedCnt.Format("   %d", iSelectedCnt);
	//mSelectedBtn.SetWindowTextW(SelectedCnt.GetString());

	SelectedCnt.Format("%d", iSelectedCnt);
	mSingedCntWnd.SetWindowTextW(SelectedCnt.GetString());
}

void CBottomPierPanel::OnChildFragmentChange()
{

}
BOOL CBottomPierPanel::CanBeSwitch(void)
{
  #if !USE_FREE_MODE
	if ( CClientSharing::mUserInfo.t_count == 0 &&
			CClientSharing::mUserInfo.n_count > 0 )
		return FALSE;
  #endif
	return TRUE;
}
