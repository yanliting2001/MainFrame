#include "NetInfoBaseWnd.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "../CommApi/GetResParentPath.h"
#include "../CommControl/MainCtrlPage.h"
#include "../CommApi/CommApi.h"

extern CMainCtrlPage *gMainCtrlPage;

#define BUTTON_ID_BASEIP		0x01
#define BUTTON_ID_DHCP			0x10
#define BUTTON_ID_CANCEL		0x11
#define BUTTON_ID_CONFIRM		0x12

#define MAUALTEXT_COLOR			0xFFFFFFFF
#define AUTOTEXT_COLOR			0xFF3B3B3B

CNetInfoBaseWnd::CNetInfoBaseWnd()
{
	mbDHCPOn = TRUE;
}

CNetInfoBaseWnd::~CNetInfoBaseWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CNetInfoBaseWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("CNetInfoBaseWnd", pParent, rc);

	RECT rcControl;
	SetRectXY(&rcControl, 236, 30, 382, 55);
	mInValidIPPromptWnd.CreateStatic(this,rcControl);

	for(int i = 0; i < ItemType_Count; i++)
	{
		mPromptWnd[i].CreateStatic(this, rcControl);
		mPromptWnd[i].SetFontDirect(DRAWTEXT_DIRECT_RIGHT|DRAWTEXT_DIRECT_VCENTER);

		mTextBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mTextBtn[i].SetWndID(BUTTON_ID_BASEIP + i);
		mTextBtn[i].SetOnClickListener(this);
		mTabGroup.AddToGroup(&mTextBtn[i]);
	}
	mTabGroup.SetSelectButton(&mTextBtn[0]);

	SetRectXY(&rcControl, 246, 293, 105, 50);
	mDHCPBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mDHCPBtn.SetWndID(BUTTON_ID_DHCP);
	mDHCPBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 246, 293, 105, 50);
	mCancelBtn.Create(this, rcControl);
	mCancelBtn.SetWndID(BUTTON_ID_CANCEL);
	mCancelBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 246, 293, 105, 50);
	mConfirmBtn.Create(this, rcControl);
	mConfirmBtn.SetWndID(BUTTON_ID_CONFIRM);
	mConfirmBtn.SetOnClickListener(this);

	mNumInputWnd.Create(this, rcControl);
	mNumInputWnd.SetMsgRecvWnd(this);

	LoadResource();
}

void CNetInfoBaseWnd::LoadResource()
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
void CNetInfoBaseWnd::LoadResource_v500()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cParentName[MAX_PATH] = {0};
	char cFileName[MAX_PATH];

	GetXmlBasePath(cParentName, sizeof(cParentName));
	CombinePathName(cFileName, cParentName, "TreasureBox/NetInfoBaseWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	char imgPath[64] = {0};
	char colorbuf[16];
	CTexture btnTextures[4];

	RECT rcPormpt,rcText;
	int i, yOffset = 0, size=0;
	UINT32	color;
  XmlLoadRect(&parser, "rcLocation", &rcControl);
	MoveWindow(&rcControl);
  
	XmlLoadRect(&parser, "InValidIPPromptWndInfo", &rcControl);
	size = parser.GetIntValue("InValidIPPromptWndInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("InValidIPPromptWndInfo", "color", "FFFF00FF"), sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mInValidIPPromptWnd.MoveWindow(&rcControl);
	mInValidIPPromptWnd.SetFontSize(size);
	mInValidIPPromptWnd.SetFontColor(color);
	mInValidIPPromptWnd.SetWindowTextA(parser.GetStrValue("InValidIPPromptWndInfo", "text", "输入的IP错误"));

	XmlLoadRect(&parser, "PormptWndInfo", &rcPormpt);
	XmlLoadRect(&parser, "TextBtnInfo", &rcText);
	yOffset = parser.GetIntValue("PormptWndInfo", "yOffset", 50);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("TextBtnInfo", "path", "Box/Text"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);

	for ( i = 0; i < ItemType_Count; i++ )
	{
		char cTmpText[64] = {0};
		snprintf(cTmpText, sizeof(cTmpText), "txt%d", i);
		mPromptWnd[i].SetWindowTextA(parser.GetStrValue("PormptWndInfo", cTmpText, "IP"));

		InflatRect(&rcControl, &rcPormpt, 0, i*yOffset);
		mPromptWnd[i].MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcText, 0, i*yOffset);
		mTextBtn[i].MoveWindow(&rcControl);
		mTextBtn[i].SetTextures(btnTextures);
	}

	XmlLoadRect(&parser, "InputNumWndLocation", &rcControl);
	mNumInputWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "DHCPBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("DHCPBtnInfo", "path", "Box/SetIp/DHCP"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mDHCPBtn.SetTextures(btnTextures);
	mDHCPBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CancelBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CancelBtnInfo", "path", "Box/Cancel"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCancelBtn.SetTextures(btnTextures);
	mCancelBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ConfirmBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ConfirmBtnInfo", "path", "Box/Comfirm"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mConfirmBtn.SetTextures(btnTextures);
	mConfirmBtn.MoveWindow(&rcControl);
}
#elif APP_ID == QIKUN510
void CNetInfoBaseWnd::LoadResource_v510()
{

}
#elif APP_ID == QIKUN501
void CNetInfoBaseWnd::LoadResource_v501()
{

}
#elif APP_ID == QIKUN520
void CNetInfoBaseWnd::LoadResource_v520()
{

}
#endif

void CNetInfoBaseWnd::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	if ( mbDHCPOn )
		return;

	if ( uType == INPUTWND_BACKMSG )
	{
		OnBackBtnClick();
	}
	else if ( uType == INPUTWND_WORDLENGTHMSG )
	{
		OnNumberBtnClick((char)wParam);
	}
}

void CNetInfoBaseWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
	{
		UpdateIP();
	}
}

void CNetInfoBaseWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )
		return ;

	int nWndID = pWnd->GetWndID();

	if ( nWndID >= BUTTON_ID_BASEIP &&
		(nWndID < BUTTON_ID_BASEIP + ItemType_Count) )
	{
		int idx = nWndID - BUTTON_ID_BASEIP;
		mTabGroup.SetSelectButton(&mTextBtn[idx]);
	}
	else if ( nWndID == BUTTON_ID_CONFIRM )
	{
		OnConfirmClick();
	}
	else if ( nWndID == BUTTON_ID_CANCEL )
	{
		OnCancelClick();
	}
	else if ( nWndID == BUTTON_ID_DHCP )
	{
		OnDHCPSwitchClick();
	}
}

void CNetInfoBaseWnd::UpdateIP()
{
	wchar_t buf[32];

	mInValidIPPromptWnd.SetWindowVisible(FALSE);

	GetIPInfoFromConfig(CONFIG_NETINFONAME, g_sIPInfo);

	swprintf(buf, 20, L"%s", g_sIPInfo.cIpAddr);
	mTextBtn[ItemType_ClientIP].SetWindowTextW(buf);

	swprintf(buf, 20, L"%s", g_sIPInfo.cMask);
	mTextBtn[ItemType_Mask].SetWindowTextW(buf);

	swprintf(buf, 20, L"%s", g_sIPInfo.cGateway);
	mTextBtn[ItemType_Gateway].SetWindowTextW(buf);

	swprintf(buf, 20, L"%s", g_sIPInfo.cDNS);
	mTextBtn[ItemType_DNS].SetWindowTextW(buf);


	if ( mbDHCPOn )
	{
		mDHCPBtn.SetRadioBtnPressed(TRUE);
		for ( int i = 0; i < ItemType_Count; ++i )
		{
			mTextBtn[i].SetFontColor(AUTOTEXT_COLOR);
		}
	}
	else
	{
		mDHCPBtn.SetRadioBtnPressed(FALSE);
		for ( int i = 0; i < ItemType_Count; ++i )
		{
			mTextBtn[i].SetFontColor(MAUALTEXT_COLOR);
		}
	}
}

void CNetInfoBaseWnd::OnDHCPSwitchClick()
{
	if ( mDHCPBtn.GetRadioBtnPressed() )
	{
		if ( !IsInMonkeyMode() )
		{
			DELAYCLICK(10000);

			mbDHCPOn = TRUE;
			g_sIPInfo.LinkMode = Auto_LinkMode;

			gMainCtrlPage->StartWaiting();

			SetIPConfig(g_sIPInfo);

			UpdateIP();

			CheckNetLink();

			ReportMsg(STARTDHCP_CLEARMSG, 0, 0);

			gMainCtrlPage->StopWaiting();
		}
	}
	else
	{
		mbDHCPOn = FALSE;

		for ( int i = 0; i < ItemType_Count; ++i )
		{
			mTextBtn[i].SetFontColor(MAUALTEXT_COLOR);
		}
	}
}

void CNetInfoBaseWnd::OnCancelClick()
{
	UpdateIP();

	//ReportMsg(INPUTWND_CLEARMSG, 0, 0);
}

void CNetInfoBaseWnd::OnConfirmClick()
{
	if ( !IsInMonkeyMode() )
	{
		if ( !mbDHCPOn && IsValidIP() )
		{
			CBaseStringA numString;

			g_sIPInfo.LinkMode = Maual_LinkMode;
			numString.Set(mTextBtn[ItemType_ClientIP].GetWindowTextW());
			if ( numString.GetString() != NULL)
				SAFE_STRNCPY(g_sIPInfo.cIpAddr, numString.GetString(), sizeof(g_sIPInfo.cIpAddr));

			numString.Set(mTextBtn[ItemType_Mask].GetWindowTextW());
			if ( numString.GetString() != NULL )
				SAFE_STRNCPY(g_sIPInfo.cMask, numString.GetString(), sizeof(g_sIPInfo.cMask));

			numString.Set(mTextBtn[ItemType_Gateway].GetWindowTextW());
			if ( numString.GetString() != NULL )
				SAFE_STRNCPY(g_sIPInfo.cGateway, numString.GetString(), sizeof(g_sIPInfo.cGateway));

			numString.Set(mTextBtn[ItemType_DNS].GetWindowTextW());
			if ( numString.GetString() != NULL )
				SAFE_STRNCPY(g_sIPInfo.cDNS, numString.GetString(), sizeof(g_sIPInfo.cDNS));

			gMainCtrlPage->StartWaiting();

			SetIPConfig(g_sIPInfo);

			CheckNetLink();

			gMainCtrlPage->StopWaiting();
		}
		ReportMsg(INPUTWND_CLEARMSG, 1, 0);
	}
}

BOOL CNetInfoBaseWnd::IsValidIP()
{
	char msIpTemp[16] = {'\0'};
	int ip[4] = {0};
	int len = 0;
	int count = 0;

	int Index = mTabGroup.GetSelectIndex();
	CBaseStringW numString;

	numString.Set(mTextBtn[Index].GetWindowTextW());

	if ( ( len = numString.GetLength() ) == 0 )
	{
		return TRUE;
	}

	sprintf(msIpTemp, "%ls",numString.GetString());

    char *p=NULL, *q=NULL;

    p = msIpTemp;
    while( (q = strchr(p,'.')) != NULL )
    {
    	*q = '\0';
    	ip[count] = atoi(p);
    	count++;
    	p = q + 1;
    }

	ip[count] = atoi(p);

	if( count > 3 )
	{
		mInValidIPPromptWnd.SetWindowVisible(TRUE);
		return FALSE;
	}

	for ( int i = 0; i < 4; i++ )
	{
		if( ip[i] > 255 )
		{
			mInValidIPPromptWnd.SetWindowVisible(TRUE);
			return FALSE;
		}
	}

	mInValidIPPromptWnd.SetWindowVisible(FALSE);

	if( ( count == 3) && (ip[3] > 0) )			//将01这种改成1
	{
		numString.Format("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		mTextBtn[Index].SetWindowTextW(numString.GetString());
	}

	return TRUE;
}

void CNetInfoBaseWnd::OnNumberBtnClick(char cNum)
{
	if ( !IsValidIP() )
		return ;

	int Index = mTabGroup.GetSelectIndex();

	CBaseStringW numString;
	numString.Set(mTextBtn[Index].GetWindowTextW());

	numString.Append(cNum);

	mTextBtn[Index].SetWindowTextW(numString.GetString());

	IsValidIP();
}

void CNetInfoBaseWnd::OnBackBtnClick()
{
	int Index = mTabGroup.GetSelectIndex();
	CBaseStringW numString;
	int len = 0;

	numString.Set(mTextBtn[Index].GetWindowTextW());
	if((len = numString.GetLength()) == 0)
		return;
	numString.Trunk(len-1);
	mTextBtn[Index].SetWindowTextW(numString.GetString());
	IsValidIP();
}
