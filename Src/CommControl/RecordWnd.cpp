#include "RecordWnd.h"
#include "../Sharing/ClientSharing.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../CommControl/MainCtrlPage.h"

extern CMainCtrlPage * gMainCtrlPage;

#define BTN_USERIMG_ID			0x01
#define BTN_LOGOUT_ID			0x02
#define BTN_RETURN_ID			0x03
#define BTN_QRCODE_ID			0x04

void CRecordWnd::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+17);
	CParentClass::Create(
		"RecordWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);
  RECT rcControl = {10, 0, 500, 30};
  
	mLoginQRCodeWnd.CreateStatic(this, rcControl);
  
  mLoginUserImgWnd.SetDrawCircle(TRUE);
	mLoginUserImgWnd.CreateStatic(this, rcControl);

  mLoginUserBKWnd.CreateStatic(this, rcControl);
	mLoginUserBKWnd.SetWndID(BTN_USERIMG_ID);
	mLoginUserBKWnd.SetOnClickListener(this);

  mLogout.Create(this, rcControl);
  mLogout.SetWndID(BTN_LOGOUT_ID);
  mLogout.SetOnClickListener(this);
  
  mReturn.Create(this, rcControl);
  mReturn.SetWndID(BTN_RETURN_ID);
  mReturn.SetOnClickListener(this);
	LoadResource();
}

void CRecordWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "RecordWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl;
	RECT rcShow, rcHide;
	char imgPath[64] = {0};
  CTexture bkTexture,  btnTextures[4];

	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	XmlLoadRect(&parser, "HideWindowInfo", &rcHide);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
  SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(rcShow, rcHide);

  XmlLoadRect(&parser, "LoginUserImgWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LoginUserImgWndInfo", "path", "LeftPier/DecVol"), sizeof(imgPath));
	CreateImgTexture(imgPath, &mDefaultUserImgTexture);
	mLoginUserImgWnd.SetBackgroundTexture(&mDefaultUserImgTexture);
	mLoginUserImgWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "LoginUserBKWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LoginUserBKWndInfo", "path", "LeftPier/DecVol"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mLoginUserBKWnd.SetBackgroundTexture(&bkTexture);
	mLoginUserBKWnd.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "LogoutBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("LogoutBtnInfo", "path", "LeftPier/DecVol"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mLogout.SetTextures(btnTextures);
	mLogout.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "LeftPier/DecVol"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturn.SetTextures(btnTextures);
	mReturn.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "QRCodeWndInfo", &rcControl);
  if ( IsFileExist(LOAD_QRCODE_PNG) )
	{
		CTexture texture;
		texture.CreateFromImgFile(LOAD_QRCODE_PNG);
		mLoginQRCodeWnd.SetBackgroundTexture(&texture);
	}
	mLoginQRCodeWnd.MoveWindow(&rcControl);

}

void CRecordWnd::SetLoginUserInfo(const char *name)
{
	if ( name )
	{
		if ( IsFileExist(LOAD_USER_JPG) )
		{
			CTexture texture;
			texture.CreateFromImgFile(LOAD_USER_JPG);
			mLoginUserImgWnd.SetBackgroundTexture(&texture);
		}
	}
	else
	{
		mLoginUserImgWnd.SetBackgroundTexture(&mDefaultUserImgTexture);
	}
}


void CRecordWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();

	switch (nWndID)
	{
	case BTN_LOGOUT_ID:
    
     gMainCtrlPage->mSharingIndexWnd.Logout();
     mLoginUserImgWnd.SetBackgroundTexture(&mDefaultUserImgTexture);
		break;

	case BTN_RETURN_ID:
    SlideShowWindow(FALSE);
		break;

	default:
		break;
	}
}


