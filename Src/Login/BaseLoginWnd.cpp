#include "BaseLoginWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../CommControl/MainCtrlPage.h"

extern CMainCtrlPage *gMainCtrlPage;

#define BTN_CLOSE_ID			0x01
#define BTN_CONFIRM_ID		0x02

void CBaseLoginWnd::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 30);
	CParentClass::Create(
		"BaseLoginWnd",
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

	mLoginTypeLabel.CreateStatic(this, rcControl);
	mLoginTypeLabel.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mLoginPromptLabel.CreateStatic(this, rcControl);
	mLoginPromptLabel.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mPwdEditText.Create(this, rcControl);
	mShutdownPromptLabel.CreateStatic(this, rcControl);
	mShutdownPromptLabel.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mConfirmBtn.Create(this, rcControl);
	mConfirmBtn.SetWndID(BTN_CONFIRM_ID);
	mConfirmBtn.SetOnClickListener(this);

	mInputWnd.Create(this, rcControl);
	mInputWnd.SetMsgRecvWnd(this);

	LoadResource();
}

void CBaseLoginWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Login/BaseLoginWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcShow, rcHide, rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];
	char colorbuf[16];
	int size;
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

	XmlLoadRect(&parser, "InputWndInfo", &rcControl);
	mInputWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LoginTypeLabelInfo", &rcControl);
	size = parser.GetIntValue("LoginTypeLabelInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("LoginTypeLabelInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mLoginTypeLabel.SetFontSize(size);
	mLoginTypeLabel.SetFontColor(color);
	mLoginTypeLabel.MoveWindow(&rcControl);
	for ( int i = 0; i < type_count; i++ )
	{
		char tmp[64] = {0};
		sprintf(tmp, "text%d", i);
		SAFE_STRNCPY(mLoginTypePrompt[i], parser.GetStrValue("LoginTypeLabelInfo", tmp, "未设"),  sizeof(mLoginTypePrompt[i]));
	}

	XmlLoadRect(&parser, "LoginPromptLabelInfo", &rcControl);
	size = parser.GetIntValue("LoginPromptLabelInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("LoginPromptLabelInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mLoginPromptLabel.SetFontSize(size);
	mLoginPromptLabel.SetFontColor(color);
	mLoginPromptLabel.MoveWindow(&rcControl);
	for ( int i = 0; i < prompt_count; i++ )
	{
		char tmp[64] = {0};
		sprintf(tmp, "text%d", i);
		SAFE_STRNCPY(mPwdTypePrompt[i], parser.GetStrValue("LoginPromptLabelInfo", tmp, "未设"),  sizeof(mPwdTypePrompt[i]));
	}

	XmlLoadRect(&parser, "PwdEditTextInfo", &rcControl);
	size = parser.GetIntValue("PwdEditTextInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("PwdEditTextInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PwdEditTextInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPwdEditText.SetFontSize(size);
	mPwdEditText.SetFontColor(color);
	mPwdEditText.SetTextures(btnTextures);
	mPwdEditText.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "ConfirmBtnInfo", &rcControl);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("ConfirmBtnInfo", "path", "RepayWnd/Ok"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mConfirmBtn.SetTextures(btnTextures);
	mConfirmBtn.MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "ShutdownPromptLabelInfo", &rcControl);
	size = parser.GetIntValue("ShutdownPromptLabelInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("ShutdownPromptLabelInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mShutdownPromptLabel.SetFontSize(size);
	mShutdownPromptLabel.SetFontColor(color);
	mShutdownPromptLabel.SetWindowTextA(parser.GetStrValue("ShutdownPromptLabelInfo", "text", "未设"));
	mShutdownPromptLabel.MoveWindow(&rcControl);

	SetLoginType(type_setting);
}

void CBaseLoginWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();

  DbgOutput(DBG_ERR, "OnClick %d nWndID = %d\n", __LINE__, nWndID);
	if ( nWndID == BTN_CLOSE_ID )
		SlideShowWindow(FALSE);
	else if ( nWndID == BTN_CONFIRM_ID )
	{
    DbgOutput(DBG_ERR, "OnClick %d mCurLoginType = %d\n", __LINE__, mCurLoginType);
		switch ( mCurLoginType )
		{
		case type_shutdown:
			OnShutdownClick();
			break;

		case type_setting:
      
      DbgOutput(DBG_ERR, "OnClick %d\n", __LINE__);
			OnSettingClick();
			break;

		case type_delete:
			OnDeleteClick();
			break;

		case type_favorit:
			OnFavoritClick();
			break;

		default:
			break;
		}
	}
}

void CBaseLoginWnd::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	switch ( uType )
	{
	case INPUTWND_BACKMSG:
		if ( mLoginID.GetLength() > 0 )
		{
			mLoginID.Trunk(mLoginID.GetLength() - 1);
			ShowLoginID();
		}
		break;

	case INPUTWND_CHARMSG:
	{
		char cNum = (char)wParam;
		if ( (mLoginID.GetLength() < 11) && ((cNum >='0' && cNum <= '9' ) || (cNum == ' ')) )
		{
			mLoginID.Append(cNum);
			ShowLoginID();
		}
	}
		break;

	default:
		break;
	}
}

void CBaseLoginWnd::SetLoginType(LOGINTYPE_E type)
{
	mCurLoginType = type;
	mLoginTypeLabel.SetWindowTextA(mLoginTypePrompt[mCurLoginType]);
	ClearLoginID();

	switch ( mCurLoginType )
	{
	case type_setting:
		SetPwdPromptType(prompt_normalPwd);
		mShutdownPromptLabel.SetWindowVisible(FALSE);
		break;

	case type_favorit:
		SetPwdPromptType(prompt_normalFavoirtID);
		mShutdownPromptLabel.SetWindowVisible(FALSE);
		break;

	case type_delete:
		SetPwdPromptType(prompt_normalPwd);
		mShutdownPromptLabel.SetWindowVisible(FALSE);
		break;

	case type_shutdown:
		SetPwdPromptType(prompt_normalPwd);
		mShutdownPromptLabel.SetWindowVisible(TRUE);
		break;

	default:
		break;
	}
}

CBaseLoginWnd::LOGINTYPE_E CBaseLoginWnd::GetLoginType(void)
{
	return mCurLoginType;
}

void CBaseLoginWnd::SetPwdPromptType(PROMPTTYPE_E type)
{
	mLoginPromptLabel.SetWindowTextA(mPwdTypePrompt[type]);
}

void CBaseLoginWnd::ShowLoginID(void)
{
	mPwdEditText.SetWindowTextA(mLoginID.GetString());
}

void CBaseLoginWnd::ClearLoginID()
{
	mLoginID.Trunk(0);
	ShowLoginID();
}

void CBaseLoginWnd::AddSongToFavorit(const char *cSongID)
{
//	if ( !cSongID )
//		return ;
  DbgOutput(DBG_INFO, "%s, line = %d\n", __FUNCTION__, __LINE__);

	SetLoginType(type_favorit);
	if ( !IsValidFavoritID() )
	{
		SlideShowWindow(TRUE);
		if ( cSongID )
			memcpy(mSongID, cSongID, sizeof(mSongID));
		else
			memset(mSongID, 0, sizeof(mSongID));
    
    DbgOutput(DBG_INFO, "%s, line = %d\n", __FUNCTION__, __LINE__);
	}
	else
	{
		AddSongToFavoritByFavoritID(cSongID);
    
    DbgOutput(DBG_INFO, "%s, line = %d\n", __FUNCTION__, __LINE__);
	}
}

void CBaseLoginWnd::SaveFavorit()
{
	if ( IsValidFavoritID() )
	{
		FastSearch_SaveFavoritSongList(mFavoritID.GetString());
	}
}

void CBaseLoginWnd::ExitFavoritLogin()
{
	FastSearch_SaveFavoritSongList(mFavoritID.GetString());
	mFavoritID.Trunk(0);
}

BOOL CBaseLoginWnd::IsValidFavoritID()
{
	if ( mFavoritID.GetLength() == 11 )
	{
		return TRUE;
	}
	return FALSE;
}

void CBaseLoginWnd::AddSongToFavoritByFavoritID(const char *cSongID)
{
	FastSearch_AddSongToFavorit(mFavoritID.GetString(), cSongID);
	if ( IsWindowVisible() )
	{
		SlideShowWindow(FALSE);
	}
	if ( gMainCtrlPage )
	{
		gMainCtrlPage->SetPopuWndVisible(favorit_item, TRUE);
	}
}

void 	CBaseLoginWnd::OnShutdownClick(void)
{
	if ( (strlen(gs_SystemSetting.mShutdownPwd) == 0 && mLoginID.GetLength() == 0) ||
		  mLoginID.Equal(gs_SystemSetting.mShutdownPwd) )
	{
		DbgOutput(DBG_INFO, "Add quit message...\n");
		theBaseApp->AddQuitMsg();
	}
	else
	{
		SetPwdPromptType(prompt_errPwd);
		ClearLoginID();
	}
}

void CBaseLoginWnd::OnSettingClick(void)
{
	if ( (strlen(gs_SystemSetting.mSettingPwd) == 0 && mLoginID.GetLength() == 0) ||
		  mLoginID.Equal(gs_SystemSetting.mSettingPwd) )
	{
		if ( gMainCtrlPage->GetCurFragmentType() != Fragment_Settings )
			gMainCtrlPage->SetCurrentFragment(Fragment_Settings);
		SlideShowWindow(FALSE);
	}
	else
	{
		SetPwdPromptType(prompt_errPwd);
		ClearLoginID();
	}
}

void CBaseLoginWnd::OnDeleteClick(void)
{
	if ( (strlen(gs_SystemSetting.mDeletePwd) == 0 && mLoginID.GetLength() == 0) ||
		  mLoginID.Equal(gs_SystemSetting.mDeletePwd) )
	{
		SlideShowWindow(FALSE);
		gMainCtrlPage->mSettingsFragment.deleteSongs();
	}
	else
	{
		SetPwdPromptType(prompt_errPwd);
		ClearLoginID();
	}
}

void CBaseLoginWnd::OnFavoritClick(void)
{
	mFavoritID.Set(mLoginID.GetString());
	if ( IsValidFavoritID() )
	{
		FastSearch_LoadFavoritSongList(mFavoritID.GetString());
		AddSongToFavoritByFavoritID(mSongID);
		memset(mSongID, 0, sizeof(mSongID));
	}
	else
	{
		SetPwdPromptType(prompt_errFavoritID);
		ClearLoginID();
	}
}
