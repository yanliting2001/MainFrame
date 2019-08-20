#include "WiFiWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
#include "InternetCtrl/NetCtrlApi.h"

#define BUTTON_BASEID_ITEM	0x00

CWiFiWnd::CWiFiWnd()
{
	mpPressedWnd = NULL;
	mFirstShowItem = 0;
	mSongCurPage = 0;
	mSongPageCount = 0;
	memset(mConnSSID, 0, sizeof(mConnSSID));
	mConnectID = -1;
//	wf = NULL;
	memset(mLinkSuccPrompt, 0, sizeof(mLinkSuccPrompt));
	memset(mLinkFailPrompt, 0, sizeof(mLinkFailPrompt));
}

CWiFiWnd::~CWiFiWnd()
{
	FreeWiFiList();
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CWiFiWnd::Create(
	CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(
		"WifiItemWnd",
		pParent,
		rc,
		WINDOWFLAG_DEFAULT,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl;

	SetRectXY(&rcControl, 50, 36, 1121, 72);
	int i;
	for(i = 0; i < WIFI_LIST_COUNT; i++)
	{
		mWifiItem[i].mItemBgWnd.CreateStatic(this, rcControl);
		mWifiItem[i].mIocWnd.CreateStatic(&mWifiItem[i].mItemBgWnd, rcControl);
		mWifiItem[i].mLinkStatusWnd.CreateStatic(&mWifiItem[i].mItemBgWnd, rcControl);

		mWifiItem[i].mItemBtn.Create(&mWifiItem[i].mItemBgWnd, rcControl);
		mWifiItem[i].mItemBtn.SetWndID(BUTTON_BASEID_ITEM + i);
		mWifiItem[i].mItemBtn.SetOnClickListener(this);
		mWifiItem[i].mItemBtn.SetOnTouchListener(this);
		mWifiItem[i].mItemBtn.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	}

	mWifiPasswdWnd.Create(this, rcControl);
	mWifiPasswdWnd.SetMsgRecvWnd(this);
//	wf = new CWIFI();
	LoadResource();
}

void CWiFiWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "WiFiWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	int i, yOffset = 0;
	RECT rcItem;
	RECT rcIco, rcLinkStatus, rcItemBtn;
	int iSize_Wifi = 0, iSize_LinkStatus = 0;

	XmlLoadRect(&parser, "WifiItemListLocation", &rcItem);
	yOffset = parser.GetIntValue("WifiItemListLocation","yOffset",60);

	XmlLoadRect(&parser, "IcoWndLocation", &rcIco);
	XmlLoadRect(&parser, "LinkStatusLocation", &rcLinkStatus);
	XmlLoadRect(&parser, "ItemBtnLocation", &rcItemBtn);
	iSize_Wifi = parser.GetIntValue("ItemBtnLocation", "size", 20);
	iSize_LinkStatus = parser.GetIntValue("LinkStatusLocation", "size", 15);
	char imgPath[64] = {0};
	char temp[64] = {0};

	strcpy(imgPath, parser.GetStrValue("LinkStatusLocation", "text", "已连接"));
	sprintf(mLinkSuccPrompt, "(%s)", imgPath);

	strcpy(imgPath, parser.GetStrValue("LinkStatusLocation","errtext", "连接失败"));
	sprintf(mLinkFailPrompt, "(%s)", imgPath);

	for(i = 0; i < WIFI_LIST_COUNT; i++)
	{
		InflatRect(&rcControl, &rcItem, 0, i*yOffset);
		mWifiItem[i].mItemBgWnd.MoveWindow(&rcControl);
		mWifiItem[i].mIocWnd.MoveWindow(&rcIco);
		mWifiItem[i].mLinkStatusWnd.MoveWindow(&rcLinkStatus);
		mWifiItem[i].mLinkStatusWnd.SetFontSize(iSize_LinkStatus);
		mWifiItem[i].mLinkStatusWnd.SetFontColor(0xff000000);
		mWifiItem[i].mItemBtn.MoveWindow(&rcItemBtn);
		mWifiItem[i].mItemBtn.SetFontSize(iSize_Wifi);
		mWifiItem[i].mItemBtn.SetFontColor(0xff000000);
	}

	CTexture bkTexture;

	strcpy(imgPath, parser.GetStrValue("BgImg","path","KGo/top"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	strcpy(imgPath, parser.GetStrValue("LockSignalIcoImg","path","KGo/item"));
	for(i = 0; i < Level_Count; i++)
	{
		sprintf(temp,"%s%d",imgPath,i);
		CreateImgTexture(temp, &LockIcoTexture[i]);
	}

	strcpy(imgPath, parser.GetStrValue("UnlockSignalIcoImg","path","KGo/item"));
	for(i = 0; i < Level_Count; i++)
	{
		sprintf(temp,"%s%d",imgPath,i);
		CreateImgTexture(temp, &UnlockIcoTexture[i]);
	}
}

void CWiFiWnd::OnDestroy()
{
	CParentClass::OnDestroy();
	DelNormalList(&mWifiItemList, sWifiItemData);
}

void CWiFiWnd::OnWindowVisible(
		BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
	{
		mWifiPasswdWnd.SetWindowVisible(FALSE);
		InitWifiItem();
	}
	else
	{
		FreeWiFiList();
	}
}


void CWiFiWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	if (!pWnd)
	{
		return;
	}

	int nWndID = pWnd->GetWndID();
	int idx;

	if((nWndID >= BUTTON_BASEID_ITEM) &&
		(nWndID < BUTTON_BASEID_ITEM + WIFI_LIST_COUNT))
	{
		idx = nWndID - BUTTON_BASEID_ITEM;
		OnCLickWifiItem(idx);
	}
}

void CWiFiWnd::OnMsg(
	UINT32 uType,
	UINT64 wParam,
	UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	switch(uType)
	{
	case INPUT_WIFIINFO_COMFIRM:
	{
		int idx = (int)lParam;
		int nFirstIndex = mSongCurPage * WIFI_LIST_COUNT;
		sWifiItemData *pWifiInfo = (sWifiItemData *)mWifiItemList.GetAt(idx+nFirstIndex);
		if ( pWifiInfo )
		{
			DbgOutput(DBG_INFO, "the name:%s, flags:%s, pwd:%s\n", pWifiInfo->strWifiName, pWifiInfo->FLAGS, (char *)wParam);
			Connect2Ap(pWifiInfo->strWifiName, pWifiInfo->FLAGS, (char *)wParam, idx);
		}
	}
		break;
	default:
		break;
	}
}

void CWiFiWnd::OnCLickWifiItem(int idx)
{
	int nFirstIndex = mSongCurPage * WIFI_LIST_COUNT;
	sWifiItemData *pWifiInfo = (sWifiItemData *)mWifiItemList.GetAt(idx+nFirstIndex);
	if ( pWifiInfo )
	{
		//if ( pWifiInfo->bLockFlag )
		if ( (strstr(pWifiInfo->FLAGS, "WPA") != NULL) || (strstr(pWifiInfo->FLAGS, "WPE") != NULL) )
			mWifiPasswdWnd.SetPasswdTitle(pWifiInfo->strWifiName, idx);
		else
			Connect2Ap(pWifiInfo->strWifiName, (char *)"NONE", (char *)"", idx);
	}
}

void CWiFiWnd::UpdateWifiItem()
{
	int nFirstIndex = mSongCurPage * WIFI_LIST_COUNT;
	int i;
	for ( i = 0 ; i < WIFI_LIST_COUNT ; i++ )
	{
		sWifiItemData *pWifiInfo = (sWifiItemData *)mWifiItemList.GetAt(i+nFirstIndex);
		if ( pWifiInfo )
		{
			mWifiItem[i].mItemBgWnd.SetWindowVisible(TRUE);
			mWifiItem[i].mItemBtn.SetWindowTextA(pWifiInfo->strWifiName);
			//if ( pWifiInfo->bLockFlag )
			if ( (strstr(pWifiInfo->FLAGS, "WPA") != NULL) || (strstr(pWifiInfo->FLAGS, "WPE") != NULL) )
			{
				if ( pWifiInfo->iSignalLevel < Level_Count )
					mWifiItem[i].mIocWnd.SetBackgroundTexture(&LockIcoTexture[pWifiInfo->iSignalLevel]);
				else
					mWifiItem[i].mIocWnd.SetBackgroundTexture(&LockIcoTexture[ZeroLevel]);
			}
			else
			{
				if ( pWifiInfo->iSignalLevel < Level_Count )
					mWifiItem[i].mIocWnd.SetBackgroundTexture(&UnlockIcoTexture[pWifiInfo->iSignalLevel]);
				else
					mWifiItem[i].mIocWnd.SetBackgroundTexture(&UnlockIcoTexture[ZeroLevel]);
			}
			if ( i == mConnectID )
			{
				mWifiItem[i].mLinkStatusWnd.SetWindowTextA(mLinkSuccPrompt);
				mWifiItem[i].mLinkStatusWnd.SetWindowVisible(TRUE);
			}
			else
				mWifiItem[i].mLinkStatusWnd.SetWindowVisible(FALSE);
		}
		else
		{
			mWifiItem[i].mItemBgWnd.SetWindowVisible(FALSE);
		}
	}
}

void CWiFiWnd::AddWifiItem(sWifiItemData *pWifiItemInfo, int iPos)
{
	sWifiItemData *pNewWifiInfo = new sWifiItemData;
	if ( !pNewWifiInfo )
	{
		return;
	}

	memcpy(pNewWifiInfo, pWifiItemInfo, sizeof(sWifiItemData));

	mWifiItemList.AddData(pNewWifiInfo, iPos);

	mSongPageCount = (mWifiItemList.GetCount() + WIFI_LIST_COUNT - 1) / WIFI_LIST_COUNT;
}

BOOL CWiFiWnd::GetAutoConnWifiName()
{
	BOOL ret = FALSE;
	if ( CWIFI::getWifiStaticHander() == NULL )
	{
		DbgOutput(DBG_ERR, "================the new wifi ctrl failedn.\n");
		return ret;
	}
	CWIFI::getWifiStaticHander()->WF_GetLocalApList();
	int iWifiItemCount = CWIFI::getWifiStaticHander()->mLocalApList.GetCount();
	printf("==========the local wifi item:%d\n", iWifiItemCount);
	int i;
	for ( i = 0 ; i < iWifiItemCount ; i++ )
	{
		APLIST *pAp = (APLIST *)CWIFI::getWifiStaticHander()->mLocalApList.GetAt(i);
		if ( pAp )
		{
			if ( strstr(pAp->cFLAGS, "CURRENT") != NULL )
			{
				strcpy(mConnSSID, pAp->cSSID);
				char *p = NULL;
				if ( (p = strstr(mConnSSID, "\n")) != NULL )
					*p = '\0';
				ret = TRUE;
				break;
			}
		}
	}
	return ret;
}

void CWiFiWnd::GetWifiItemList()
{
	FreeWiFiList();
	int i;
#if !defined(__mips__) && !defined(__arm__)
	for ( i = 0 ; i < WIFI_LIST_COUNT*2 ; i++ )
	{
		sWifiItemData WifiItem;
		sprintf(WifiItem.strWifiName, "%s%d", "dddd", i);
		WifiItem.iSignalLevel = (50 / 25) - 1;
		strcpy(WifiItem.FLAGS, "WPA");
		AddWifiItem(&WifiItem);
	}
#endif

	if ( CWIFI::getWifiStaticHander() == NULL )
	{
		DbgOutput(DBG_ERR, "the new wifi ctrl failedn.\n");
		return;
	}

	CWIFI::getWifiStaticHander()->WF_Scan();
	int iWifiItemCount = CWIFI::getWifiStaticHander()->mScanApList.GetCount();
	DbgOutput(DBG_DEBUG, "=======gApList Count = %d, connect SSID:%s\n", iWifiItemCount, mConnSSID);

	for ( i = 0 ; i < iWifiItemCount ; i++ )
	{
		APINFO *pAp = (APINFO *)CWIFI::getWifiStaticHander()->mScanApList.GetAt(i);
		if ( pAp )
		{
			//DbgOutput(DBG_INFO, "ScanAP: siglvl = %d, flag = %s, ssid = %s\n",	pAp->SIGLVL, pAp->FLAGS,pAp->SSID);
			sWifiItemData WifiItem;
			snprintf(WifiItem.strWifiName, sizeof(WifiItem.strWifiName), "%s", pAp->SSID);
			char *p = NULL;
			if ( (p = strstr(WifiItem.strWifiName, "\n")) != NULL )
				*p = '\0';
			WifiItem.iSignalLevel = (pAp->SIGLVL / 25) - 1;
			if ( strstr(pAp->FLAGS, "WPA") != NULL )
			{
				strcpy(WifiItem.FLAGS, "WPA");
			}
			else if ( strstr(pAp->FLAGS, "WEP") != NULL )
			{
				strcpy(WifiItem.FLAGS, "WEP");
			}
			else if ( strstr(pAp->FLAGS, "ESS") != NULL )
			{
				strcpy(WifiItem.FLAGS, "NONE");
			}
			else
			{
				strcpy(WifiItem.FLAGS, "NONE");
			}

			if ( strcmp(WifiItem.strWifiName, mConnSSID) == 0 )
			{
				AddWifiItem(&WifiItem, 0);
				mConnectID = 0;			//自动连接的在放在最前面
			}
			else
				AddWifiItem(&WifiItem, -1);
			//delete pAp;
		}
	}
}

void CWiFiWnd::Connect2Ap(char *strName, char* nKeyMgmt, char *strPwd, int iWndID)
{
	if ( strName == NULL || strPwd == NULL || !nKeyMgmt )
		return ;

	gMainCtrlPage->StartWaiting();

	int ret = CWIFI::getWifiStaticHander()->WF_Connect2Ap(strName, nKeyMgmt, strPwd);
	ret = CWIFI::getWifiStaticHander()->GetStatus();

	if ( ret != 0 )		// 连接不成功
	{
		mConnectID = -1;
		memset(mConnSSID, 0, sizeof(mConnSSID));
		UpdateWifiItem();
		mWifiItem[iWndID].mLinkStatusWnd.SetWindowTextA(mLinkFailPrompt);
		mWifiItem[iWndID].mLinkStatusWnd.SetWindowVisible(TRUE);
	}
	else
	{
		mConnectID = iWndID;
		UpdateWifiItem();
		system("killall udhcpc");
		system("udhcpc -n -R -iwlan0");		// modify system("udhcpc -iwlan0 &");
		SetNetLinkType(WIFI_LinkType);
    SetWifiLinkInfo(strName,strPwd);
    SetIPConfig(g_sIPInfo);
	}
	DbgOutput(DBG_INFO,"end to Connect ap, the wifi name:%s, passwd:%s the result:%d.\n", strName, strPwd, ret);

	gMainCtrlPage->StopWaiting();
}

void CWiFiWnd::ClearPrevLinkIco()		// 清除原先 连接的状态
{
	if ( mConnectID != -1 && mConnectID >=0 && mConnectID < WIFI_LIST_COUNT )
		mWifiItem[mConnectID].mLinkStatusWnd.SetWindowVisible(FALSE);
}

void CWiFiWnd::SetNewLinkIco()			//
{
	if ( mConnectID != -1 && mConnectID >=0 && mConnectID < WIFI_LIST_COUNT )
		mWifiItem[mConnectID].mLinkStatusWnd.SetWindowVisible(TRUE);
}

void CWiFiWnd::InitWifiItem()
{
	gMainCtrlPage->StartWaiting();
	mSongCurPage = 0;
	mSongPageCount = 1;
	GetAutoConnWifiName();
	GetWifiItemList();
	UpdateWifiItem();
	gMainCtrlPage->StopWaiting();
}

void CWiFiWnd::OnTouchDown(
	CBaseWnd *pWnd,
	POINT pt)
{
	mFirstShowItem = mSongCurPage;

	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	pWnd->SetCapture();
}

void CWiFiWnd::OnTouchUp(
	CBaseWnd *pWnd,
	POINT pt,
	int xDistance,
	int yDistance,
	UINT64 uUsedTime)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CWiFiWnd::OnTouchMove(
	CBaseWnd *pWnd,
	POINT pt)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

	POINT ptDrag;
	pWnd->ConvertWndPointToScreenPoint(&ptDrag, pt);
	ConvertScreenPointToWndPoint(&ptDrag, ptDrag);

	int yDragLen = ptDrag.y-mPtPressedScreen.y;
	int nMoveItems = yDragLen / RECTHEIGHT(mWifiItem[0].mItemBgWnd.mWndRect);

	mSongCurPage = mFirstShowItem-nMoveItems;
	if (mSongCurPage < 0)
	{
		mSongCurPage = 0;
	}
	else if (mSongCurPage >= mSongPageCount)
	{
		mSongCurPage = mSongPageCount - 1;
	}

	UpdateWifiItem();
}

void CWiFiWnd::OnTouchLeave(
	CBaseWnd *pOldTouchWnd,
	CBaseWnd *pNewTouchWnd)
{
//	BoundRankWnd();
	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CWiFiWnd::FreeWiFiList()
{
	DelNormalList(&mWifiItemList, sWifiItemData);
}
