#include "DLNAWnd.h"
#include "XMLParser.h"
#include "GetResParentPath.h"
#include "../GlobalUIClass.h"
#include "QRCodeApi.h"
#include "InternetCtrl/NetCtrlApi.h"
#include "../PlayerApi/PlayerApi.h"

#define QRCODEPATH				"/tmp/QRCodeWng.png"

#define BTN_CLICK_RETURN		0x01
#define BTN_CLICK_QRCODE		0x02

CDlnaWnd::CDlnaWnd()
{
}

CDlnaWnd::~CDlnaWnd()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CDlnaWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	SetZorderNumber(WINDOWS_ZORDER_WAITINGWNDPOPUP);
	CreateStatic(
		pParent,
		rc,
		WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl;
	SetRectXY(&rcControl, 240, 210, 800, 300);
	mQRCodeWnd.CreateStatic(this, rcControl);

	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BTN_CLICK_RETURN);
	mReturnBtn.SetOnClickListener(this);

	mQRCodeBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mQRCodeBtn.SetWndID(BTN_CLICK_QRCODE);
	mQRCodeBtn.SetOnClickListener(this);

	LoadResource();

	DbgOutput(DBG_DEBUG, "DLNAWnd Create\n");
}

void CDlnaWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cParentName[MAX_PATH] = {0};
	char cFileName[MAX_PATH];

	GetXmlBasePath(cParentName, sizeof(cParentName));
	CombinePathName(cFileName, cParentName, "DLNAWnd.xml");

	if( !parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml

	RECT rcShow;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture;
	CTexture btnTextures[4];

	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "DLNA/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "QRCodeWndInfo", &rcControl);
	mQRCodeWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "Public/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "QRCodeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("QRCodeBtnInfo", "path", "DLNA/QRCodeBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mQRCodeBtn.SetTextures(btnTextures);
	mQRCodeBtn.MoveWindow(&rcControl);

//	GenerateQRCodeWnd();
}

void CDlnaWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);

	if(bVisible)
	{
		GenerateQRCodeWnd();
		OSDApi_ShowBarCode(QRCODEPATH, TRUE);
	}
	else
	{
		OSDApi_ShowBarCode(QRCODEPATH, FALSE);
	}
}

void CDlnaWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )
		return ;

	int nWndID = pWnd->GetWndID();

	if(nWndID == BTN_CLICK_RETURN)
	{
		SetWindowVisible(FALSE);
		gMainCtrlPage->SetPhantomVisible(TRUE);
	}
	else if ( nWndID == BTN_CLICK_QRCODE )
	{
		if ( mQRCodeBtn.GetRadioBtnPressed() )
		{
			OSDApi_ShowBarCode(QRCODEPATH, FALSE);
		}
		else
		{
			GenerateQRCodeWnd();
			OSDApi_ShowBarCode(QRCODEPATH, TRUE);
		}
	}
}

void CDlnaWnd::GenerateQRCodeWnd()
{
	char str[128] = "0";
	snprintf(str, sizeof(str), "http://%s:8080/%s", g_sIPInfo.cIpAddr, "wapktv/index.html");
	GenerateQRPngByString(str, QRCODEPATH);

	if( IsFileExist(QRCODEPATH) )
	{
		CTexture texture;
		texture.CreateFromImgFile(QRCODEPATH);
		mQRCodeWnd.SetBackgroundTexture(&texture);
	}
}
