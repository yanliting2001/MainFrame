#include "NetInfoWnd.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "../CommApi/GetResParentPath.h"

#include "QRCodeApi.h"

#include "../CommControl/MainCtrlPage.h"
#include "../CommApi/CommApi.h"

extern CMainCtrlPage *gMainCtrlPage;

#define BUTTON_ID_NETINFO		0x01
#define BUTTON_ID_WIFIINFO		0x02
#define BUTTON_ID_CHECKNET		0x03
#define BUTTON_ID_RETURN		0x04

typedef enum _eLinkState
{
	LinkState_Connect=0,
	LinkState_Disconnect,
	LinkState_Count,
} eLinkState;

static char g_LinkPrompt[LinkState_Count][64] =
{
		"您的网络连接正常",
		"您的网络连接失败"
};

CNetInfoWnd::CNetInfoWnd()
{
	memset(mBarCodeFileName, 0, sizeof(mBarCodeFileName));
	memset(mSerialIDPormpt, 0, sizeof(mSerialIDPormpt));
	miConnState = -1;
}

CNetInfoWnd::~CNetInfoWnd()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CNetInfoWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	//CreateStatic(pParent, rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);
	SetZorderNumber(WINDOWS_ZORDER_WAITINGWNDPOPUP);
	CreateStatic(
		pParent,
		rc,
		WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl;
	SetRectXY(&rcControl, 240, 210, 800, 300);
	mSerialIDWnd.CreateStatic(this, rcControl);
	mNetPromptBgWnd.CreateStatic(this, rcControl);
	mLinkPromptWnd.CreateStatic(this, rcControl);

	mNetBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mNetBtn.SetWndID(BUTTON_ID_NETINFO);
	mNetBtn.SetOnClickListener(this);

	mWifiBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mWifiBtn.SetWndID(BUTTON_ID_WIFIINFO);
	mWifiBtn.SetOnClickListener(this);

	mTabGroup.AddToGroup(&mNetBtn);
	mTabGroup.AddToGroup(&mWifiBtn);
	mTabGroup.SetSelectButton(&mNetBtn);

	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BUTTON_ID_RETURN);
	mReturnBtn.SetOnClickListener(this);

	mCheckNetBtn.Create(this, rcControl);
	mCheckNetBtn.SetWndID(BUTTON_ID_CHECKNET);
	mCheckNetBtn.SetOnClickListener(this);

	mIpSetWnd.Create(this,rcControl);
	mIpSetWnd.SetMsgRecvWnd(this);

	mWifiWnd.Create(this);

	LoadResource();

	DbgOutput(DBG_INFO, "BarCodeWnd Create\n");
}

void CNetInfoWnd::LoadResource()
{
#if APP_ID == QIKUN500
	LoadResource_v500();
#elif APP_ID == QIKUN510
	LoadResource_v510();
#elif APP_ID == QIKUN501
	LoadResource_v501();
#elif APP_ID == QIKUN520
	LoadResource_v520();
#endif
}

#if APP_ID == QIKUN500
void CNetInfoWnd::LoadResource_v500()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cParentName[MAX_PATH] = {0};
	char cFileName[MAX_PATH];

	GetXmlBasePath(cParentName, sizeof(cParentName));
	CombinePathName(cFileName, cParentName, "NetInfoWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture btnTextures[4];
	CTexture bkTexture;
	int size=0;

	XmlLoadRect(&parser, "ShowWindowInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "BarCode/bg"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	CParentClass::MoveWindow(&rcControl);
	SetBackgroundTexture(&bkTexture);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "Public/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SerialIDInfo", &rcControl);
	mSerialIDWnd.MoveWindow(&rcControl);
	SAFE_STRNCPY(mSerialIDPormpt, parser.GetStrValue("SerialIDInfo", "text", "序列号为:"), sizeof(mSerialIDPormpt));

	XmlLoadRect(&parser, "NetPromptBgInfo", &rcControl);
	mNetPromptBgWnd.MoveWindow(&rcControl);

	SAFE_STRNCPY(imgPath, parser.GetStrValue("NetPromptBgInfo", "NormalPath","BarCode/Normal"), sizeof(imgPath));
	CreateImgTexture(imgPath, &mNetNormaTexture);

	SAFE_STRNCPY(imgPath, parser.GetStrValue("NetPromptBgInfo", "ErrPath", "BarCode/Err"), sizeof(imgPath));
	CreateImgTexture(imgPath, &mNetErrTexture);

	XmlLoadRect(&parser, "LinkPromptInfo", &rcControl);
	mLinkPromptWnd.MoveWindow(&rcControl);
	size = parser.GetIntValue("LinkPromptInfo", "size", 20);
	mLinkPromptWnd.SetFontSize(size);

	XmlLoadRect(&parser, "NetBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NetBtnInfo", "path", "BarCode/Net"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNetBtn.SetTextures(btnTextures);
	mNetBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "WifiBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("WifiBtnInfo", "path", "BarCode/Wifi"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mWifiBtn.SetTextures(btnTextures);
	mWifiBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CheckNetBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CheckNetBtnInfo", "path", "BarCode/CheckNet"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCheckNetBtn.SetTextures(btnTextures);
	mCheckNetBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SetIPWndLocation", &rcControl);
	mIpSetWnd.MoveWindow(&rcControl);

	SAFE_STRNCPY(mBarCodeFileName, parser.GetStrValue("BarCodePath", "path", "/tmp/BarCode.png"), sizeof(mBarCodeFileName));

	SAFE_STRNCPY(g_LinkPrompt[LinkState_Connect], parser.GetStrValue("BarCodePrompt", "connectmsg", "您的网络连接正常"), sizeof(g_LinkPrompt[LinkState_Connect]));
	SAFE_STRNCPY(g_LinkPrompt[LinkState_Disconnect], parser.GetStrValue("BarCodePrompt", "disconnectmsg", "您的网络连接失败"), sizeof(g_LinkPrompt[LinkState_Disconnect]));

	GetSerialID();
}
#elif APP_ID == QIKUN510
void CNetInfoWnd::LoadResource_v510()
{

}
#elif APP_ID == QIKUN501
void CNetInfoWnd::LoadResource_v501()
{

}
#elif APP_ID == QIKUN520
void CNetInfoWnd::LoadResource_v520()
{

}
#endif

void CNetInfoWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
	{
		if ( miConnState != g_bIsNetConn )
		{
			miConnState = g_bIsNetConn;
			ShowConnectInfo(g_bIsNetConn);
		}
	}
	else
	{
		gMainCtrlPage->SetPhantomVisible(TRUE);
	}
}

void CNetInfoWnd::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	if ( (uType == INPUTWND_CLEARMSG) || (uType == STARTDHCP_CLEARMSG) )
	{
		if ( miConnState != g_bIsNetConn )
		{
			miConnState = g_bIsNetConn;
			ShowConnectInfo(g_bIsNetConn);
		}
	}
}

void CNetInfoWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )
		return ;

	int nWndID = pWnd->GetWndID();
	switch ( nWndID )
	{
	case BUTTON_ID_RETURN:
		SetWindowVisible(FALSE);
		break;

	case BUTTON_ID_NETINFO:
		OnNetLinkClick();
		break;

	case BUTTON_ID_WIFIINFO:
		OnWiFiLinkClick();
		break;

	case BUTTON_ID_CHECKNET:
		OnClickCheckNet();
		break;

	default:
		break;
	}
}

void CNetInfoWnd::OnClickCheckNet()
{
	DELAYCLICK(10000);

	gMainCtrlPage->StartWaiting();

	CheckNetLink();
	if ( miConnState != g_bIsNetConn )
	{
		miConnState = g_bIsNetConn;
		ShowConnectInfo(g_bIsNetConn);
	}

	gMainCtrlPage->StopWaiting();
}

void CNetInfoWnd::OnNetLinkClick()
{
	mTabGroup.SetSelectButton(&mNetBtn);
	mWifiWnd.SetWindowVisible(FALSE);
	mIpSetWnd.SetWindowVisible(TRUE);
}

void CNetInfoWnd::OnWiFiLinkClick()
{
#if defined(__mips__) || defined(__arm__)
	if ( DetectWifiExist(DEVNAME_WIFI) )
	{
		mTabGroup.SetSelectButton(&mWifiBtn);
		mWifiWnd.SetWindowVisible(TRUE);
		mIpSetWnd.SetWindowVisible(FALSE);
	}
	else
	{
		mTabGroup.SetSelectButton(&mNetBtn);
		if ( strcmp(g_sIPInfo.cDevName, DEVNAME_WIFI) == 0 )
			SetNetLinkType(Net_LinkType);
	}
#else
	mTabGroup.SetSelectButton(&mWifiBtn);
	mWifiWnd.SetWindowVisible(TRUE);
	mIpSetWnd.SetWindowVisible(FALSE);
#endif
}

void CNetInfoWnd::GetSerialID()
{
	CBaseStringW tmp;
	tmp.Format("%s%s", mSerialIDPormpt, g_sIPInfo.cMac);
	mSerialIDWnd.SetWindowTextW(tmp.GetString());
}

void CNetInfoWnd::ShowConnectInfo(BOOL bConnOK)	// 显示链接信息，是否连上网
{
	if ( bConnOK )
	{
		mNetPromptBgWnd.SetBackgroundTexture(&mNetNormaTexture);
		mLinkPromptWnd.SetWindowTextA(g_LinkPrompt[LinkState_Connect]);
	}
	else
	{
		mNetPromptBgWnd.SetBackgroundTexture(&mNetErrTexture);
		mLinkPromptWnd.SetWindowTextA(g_LinkPrompt[LinkState_Disconnect]);
	}
}
