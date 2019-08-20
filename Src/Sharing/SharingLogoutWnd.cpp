#include "SharingLogoutWnd.h"
#include "XMLParser.h"
#include "BaseApp.h"
#include "../CommControl/MainCtrlPage.h"

extern CMainCtrlPage * gMainCtrlPage;

#define BTN_CLOSE_ID		0x01
#define BTN_LOGOUT_ID	0x02

void CSharingLogoutWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+18);

	CreateStatic(pParent,	rc,  WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {0, 0, 100, 100};

	mCloseBtn.Create(this, rcControl);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	mLogoutBtn.Create(this, rcControl);
	mLogoutBtn.SetWndID(BTN_LOGOUT_ID);
	mLogoutBtn.SetOnClickListener(this);

	mLoginUserBKWnd.CreateStatic(this, rcControl);

	mLoginUserImgWnd.SetDrawCircle(TRUE);
	mLoginUserImgWnd.CreateStatic(this, rcControl);

	LoadResource();
}

void CSharingLogoutWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SharingLogoutWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml

	RECT rcShow;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];

	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "RepayWnd/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LogoutBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LogoutBtnInfo", "path", "RepayWnd/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mLogoutBtn.SetTextures(btnTextures);
	mLogoutBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LoginUserImgWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LoginUserImgWndInfo", "path", "LeftPier/DecVol"), sizeof(imgPath));
	mLoginUserImgWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LoginUserBKWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LoginUserBKWndInfo", "path", "LeftPier/DecVol"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mLoginUserBKWnd.SetBackgroundTexture(&bkTexture);
	mLoginUserBKWnd.MoveWindow(&rcControl);
}

void CSharingLogoutWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
	{
		if ( IsFileExist(LOAD_USER_JPG) )
		{
			CTexture texture;
			texture.CreateFromImgFile(LOAD_USER_JPG);
			mLoginUserImgWnd.SetBackgroundTexture(&texture);
		}
	}
}

void CSharingLogoutWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd )
	{
		int nWndID = pWnd->GetWndID();
		if ( nWndID == BTN_CLOSE_ID )
		{
			SetWindowVisible(FALSE);
		}
		else if ( nWndID == BTN_LOGOUT_ID )
		{
#if USE_FREE_MODE
      SetWindowVisible(FALSE);
#else
      gMainCtrlPage->mSharingIndexWnd.Logout();
      SetWindowVisible(FALSE);
      CClientSharing::mUserInfo.t_count = 0;
#endif
		}
	}
}

