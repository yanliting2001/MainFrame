#include "SharingIndexWnd.h"
#include "XMLParser.h"
#include "BaseApp.h"
#include "ClientSharing.h"
#include "../CommControl/MainCtrlPage.h"
#include "../Record/BaseRecord.h"
#include "../PlayerApi/PlayerInfo.h"
#if USE_MODULE_BCM
#include "../OSDControl/BCM_Control.h"
#endif


#define BTN_ACTIVETILE_ID	0x03

extern CMainCtrlPage *gMainCtrlPage;

#define KEEPALIVE_TIME_OUT		1000	// 1s

static LPVOID CheckLoginThreadFunc(LPVOID lpParam)
{
	pthread_detach(pthread_self());

	CSharingIndexWnd *pSharingWnd = (CSharingIndexWnd *)lpParam;
	if ( !pSharingWnd ) return NULL;

	pSharingWnd->ChekLoginFunc();
	return NULL;
}

static LPVOID HeartPackageThreadFunc(LPVOID lpParam)
{
	pthread_detach(pthread_self());

	CSharingIndexWnd *pSharingWnd = (CSharingIndexWnd *)lpParam;
	if ( !pSharingWnd ) return NULL;

	pSharingWnd->HeartFunc();

	return NULL;
}

CSharingIndexWnd::CSharingIndexWnd()
{
	mbLoginFlag = FALSE;
	mbExitThread = TRUE;
}

CSharingIndexWnd::~CSharingIndexWnd()
{
	StopCheckLogin();
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CPayInfo::RefreshPayInfo(void)
{
	for ( int i = 0; i < PACKAGE_MAX_COUNT; i++ )
	{
		char price[32] = {0};

		mPayItemsWnd[i].itemRemarkWnd.SetWindowTextA(CClientSharing::mPayItemsList[i].remark);

    snprintf(price, sizeof(price), "¥%.1f", CClientSharing::mPayItemsList[i].price_now); //CClientSharing::mPayItemsList[i].price_now
	  mPayItemsWnd[i].price_nowWnd.SetWindowTextA(price);
		snprintf(price, sizeof(price), "¥%.1f", CClientSharing::mPayItemsList[i].price_org);
		mPayItemsWnd[i].price_orgWnd.SetWindowTextA(price);

		
	  DbgOutput(DBG_INFO, "RefreshPayInfo price_now = %f, price_org = %f\n", CClientSharing::mPayItemsList[i].price_now, CClientSharing::mPayItemsList[i].price_org);
	}
}

void CPayInfo::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SharingIndexWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	
	// read from xml

	RECT rcShow, rcControl, rcItem;
	char imgPath[64] = {0};
	CTexture bkTexture;
	char colorbuf[16];
	unsigned long color;
	int size, i, xOffset;
	XmlLoadRect(&parser, "PayItemsBKWndInfo", &rcShow);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "PayItemsInfo", &rcControl);
	xOffset = parser.GetIntValue("PayItemsInfo", "xOffset", 150);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("PayItemsInfo", "path", "RepayWnd/"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);

	for ( i = 0; i < PACKAGE_MAX_COUNT; i++ )
	{
		InflatRect(&rcItem, &rcControl, i*xOffset, 0);
		mPayItemsWnd[i].itemBKWnd.SetBackgroundTexture(&bkTexture);
		mPayItemsWnd[i].itemBKWnd.MoveWindow(&rcItem);
		mPayItemsWnd[i].itemBtn.MoveWindow(&mPayItemsWnd[i].itemBKWnd.mWndRect);
	}
	
	XmlLoadRect(&parser, "PayItemRemarkInfo", &rcControl);
	size = parser.GetIntValue("PayItemRemarkInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("PayItemRemarkInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	for ( i = 0; i < PACKAGE_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].itemRemarkWnd.SetFontSize(size);
		mPayItemsWnd[i].itemRemarkWnd.SetFontColor(color);
		mPayItemsWnd[i].itemRemarkWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PayItemPriceNowInfo", &rcControl);
	size = parser.GetIntValue("PayItemPriceNowInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("PayItemPriceNowInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	for ( i = 0; i < PACKAGE_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].price_nowWnd.SetFontSize(size);
		mPayItemsWnd[i].price_nowWnd.SetFontColor(color);
		mPayItemsWnd[i].price_nowWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PayItemPriceOrgInfo", &rcControl);
	size = parser.GetIntValue("PayItemPriceOrgInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("PayItemPriceOrgInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	for ( i = 0; i < PACKAGE_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].price_orgWnd.SetFontSize(size);
		mPayItemsWnd[i].price_orgWnd.SetFontColor(color);
		mPayItemsWnd[i].price_orgWnd.MoveWindow(&rcControl);
	}
}

void CPayInfo::Create(CSharingIndexWnd *pParent)
{
	RECT rc = {0, 0, 100, 100};
	CreateStatic(pParent,	rc);

	RECT rcControl;
	SetRectXY(&rcControl, 240, 500, 800, 100);

	for ( int i = 0; i < PACKAGE_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].itemBKWnd.CreateStatic(this, rcControl);
		mPayItemsWnd[i].itemRemarkWnd.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].price_nowWnd.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].price_orgWnd.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].itemBtn.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].itemBtn.SetWndID(BTN_PACKAGE_BASE_ID + i);
	}
	LoadResource();

}

void CSharingIndexWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+8);
 // SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 30);
  CreateStatic(pParent,	rc,  WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP);

 	RECT rcControl;
	SetRectXY(&rcControl, 240, 210, 800, 300);
	mLoginQRCodeWnd.CreateStatic(this, rcControl);
  	mStbAddressWnd.CreateStatic(this, rcControl);
  	mWorkTimeWnd.CreateStatic(this, rcControl);

	mFoodBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
  	mFoodBtn.SetWndID(BTN_FOOD_BTN_ID);
  	mFoodBtn.SetOnClickListener(this);

	mShopBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
  	mShopBtn.SetWndID(BTN_SHOP_BTN_ID);
  	mShopBtn.SetOnClickListener(this);

  	mPayItemsWnd.Create(this);
  
  	mActivityTitle.Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
	mActivityTitle.SetWndID(BTN_ACTIVETILE_ID);
	mActivityTitle.SetOnClickListener(this);
  
	LoadResource();
	mPayItemsWnd.RefreshPayInfo();
	mPayItemsWnd.SetWindowVisible(TRUE);
}

void CSharingIndexWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SharingIndexWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml

	RECT rcShow;
  RECT rcHide;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture;
  CTexture btnTextures[4];
  char colorbuf[16];
	unsigned long color;
  int size;
    
	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	XmlLoadRect(&parser, "ShowWindowInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo",  "path",  "HomePageBK"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "QRCodeWndInfo", &rcControl);
	mLoginQRCodeWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "FoodInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("FoodInfo", "path", "Sharing/Food"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mFoodBtn.SetTextures(bkTexture);
	mFoodBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ShopInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShopInfo", "path", "Sharing/Shop"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mShopBtn.SetTextures(bkTexture);
	mShopBtn.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "StbAddressInfo", &rcControl);
	size = parser.GetIntValue("StbAddressInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("StbAddressInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mStbAddressWnd.SetFontSize(size);
	mStbAddressWnd.SetFontColor(color);
  mStbAddressWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
  mStbAddressWnd.SetWindowTextA(CClientSharing::mAddressWorkTime.anotherName);
	mStbAddressWnd.MoveWindow(&rcControl);

  XmlLoadRect(&parser, "WorkTimeInfo", &rcControl);
	size = parser.GetIntValue("WorkTimeInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("WorkTimeInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	mWorkTimeWnd.SetFontSize(size);
	mWorkTimeWnd.SetFontColor(color);
  mWorkTimeWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
  mWorkTimeWnd.SetWindowTextA(CClientSharing::mAddressWorkTime.workTime);
	mWorkTimeWnd.MoveWindow(&rcControl);
  
  #if USE_ACTIVITY_TITLE
  XmlLoadRect(&parser, "ActivityTitleInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ActivityTitleInfo", "path", "ActivityTitle"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mActivityTitle.SetTextures(btnTextures);
	mActivityTitle.MoveWindow(&rcControl);
	#endif
}

void CSharingIndexWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd )
	{
		int nWndID = pWnd->GetWndID();
		switch ( nWndID )
		{
		
		case BTN_ACTIVETILE_ID:
      #if USE_ACTIVITY_TITLE
      gMainCtrlPage->mActivityContentWnd.SetWindowVisible(TRUE);
      #endif
			break;
		case BTN_FOOD_BTN_ID:
			 gMainCtrlPage->mFoodWnd.SlideShowWindow(TRUE);
			break;
		case BTN_SHOP_BTN_ID:
			gMainCtrlPage->mShopWnd.SlideShowWindow(TRUE);
			break;
		default:
			break;
		}
	}
}

void CSharingIndexWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	gMainCtrlPage->mSearchInputWnd.SetWindowVisible(!bVisible);
	if ( bVisible )
	{
		mPayItemsWnd.RefreshPayInfo();
		mPayItemsWnd.SetWindowVisible(TRUE);
	}
	
}

void CSharingIndexWnd::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
  #if 0
	if ( nTimerID == KEEPALIVE_TIMERID )
	{
    
		//DecreaseLeftCount(payType_time);
		if(CClientSharing::mUserInfo.type!=0)
			DecreaseLeftCount(CClientSharing::mUserInfo.type);

	}
  #endif
}

void CSharingIndexWnd::SetLoginQRCodeVisible(BOOL bVisible)
{
	if ( bVisible )
	{
		char errMsg[256] = {0};

		CClientSharing::clearUserInfo();
		CClientSharing::clearPayInfo();
    CClientSharing::clearAddressInfo();
	    CClientSharing::clearRangClickInfo();

		if ( CClientSharing::GetQRCode(errMsg, sizeof(errMsg)) != JSON_RET_OK )
		{
			DbgOutput(DBG_WARN, "getQRCode failed:%s\n", errMsg);
		}
    if ( CClientSharing::getPayInfo( errMsg, sizeof(errMsg)) != JSON_RET_OK )
		{
			DbgOutput(DBG_WARN, "getPayInfo failed:%s\n", errMsg);
		}
    if ( CClientSharing::getAddressInfo( errMsg, sizeof(errMsg)) != JSON_RET_OK )
		{
			DbgOutput(DBG_WARN, "getAddressInfo failed:%s\n", errMsg);
		}
    
    if ( CClientSharing::getTotalRankingInfo( errMsg, sizeof(errMsg)) != JSON_RET_OK )
  	{
  		DbgOutput(DBG_WARN, "getTotalRankingInfo failed:%s\n", errMsg);
  	}
		if ( IsFileExist(LOAD_QRCODE_PNG) )
		{
			CTexture texture;
			texture.CreateFromImgFile(LOAD_QRCODE_PNG);
			mLoginQRCodeWnd.SetBackgroundTexture(&texture);
		}
	}
	mLoginQRCodeWnd.SetWindowVisible(bVisible);
}

void CSharingIndexWnd::StartCheckLogin(void)
{

	if ( mbExitThread )
	{
		mbExitThread = FALSE;
		mCheckLoginThread.StartThread(CheckLoginThreadFunc, this, "CheckLoginThreadFunc");
		mHeartPackageThread.StartThread(HeartPackageThreadFunc,this,"HeartPackageThreadFunc");
    
   gMainCtrlPage->mLeftPierPanel.StartTime(); //在leftier菜单中开启定时
	}
	CClientSharing::ConnectHeartServer();
  
}

void CSharingIndexWnd::Logout(void)
{
	char errMsg[128] = {0};
	CClientSharing::logoutUser(errMsg, sizeof(errMsg));

	gMainCtrlPage->SetCurrentFragment(Fragment_HomePage);
	gMainCtrlPage->CloseAllPopWindows();

	mbLoginFlag = FALSE;
	#if !USE_FREE_MODE
	SetWindowVisible(TRUE);
	#endif
	CClientSharing::clearUserInfo();

	remove(LOAD_USER_JPG);

	// 停止录音
	SONGINFO *curPlayingSong = gPlayerInfo->GetPlayingSongInfo();
	CBaseRecord::Stop_Record(curPlayingSong ? curPlayingSong->cName : NULL);
	CBaseRecord::SetRecordOn(FALSE);
	
	//g_pBCM_Ctrl->AudioCompareStop();

	gPlayerInfo->ClearSelectedSongList();
	gPlayerInfo->PlayNext(TRUE);
#if !USE_FREE_MODE
	gMainCtrlPage->mLeftPierPanel.SetLoginUserInfo(NULL);

#endif
}

void CSharingIndexWnd::Login(void)
{
	gMainCtrlPage->mLeftPierPanel.SetLoginUserInfo(CClientSharing::mUserInfo.username);

	mbLoginFlag = TRUE;
	SetWindowVisible(FALSE);
	if ( CClientSharing::mUserInfo.t_count > 0 )
	{
		gMainCtrlPage->mLeftPierPanel.RefreshLeftTime(payType_time, CClientSharing::mUserInfo.t_count);
	}
	else if ( CClientSharing::mUserInfo.n_count > 0 )
	{
		gMainCtrlPage->mLeftPierPanel.RefreshLeftTime(payType_count, CClientSharing::mUserInfo.n_count);
	}
	// 添加显示包月信息
	gMainCtrlPage->mLeftPierPanel.RefreshLeftDate(3, CClientSharing::mUserInfo.m_count);
	CBaseRecord::SetRecordOn(TRUE);
}

void CSharingIndexWnd::StopCheckLogin(void)
{
	mbExitThread = TRUE;
}

BOOL CSharingIndexWnd::GetCurLoginState(void)
{
	return mbLoginFlag;
}

void CSharingIndexWnd::ChekLoginFunc(void)
{
	char errMsg[256] = {0};
	BOOL tcpconnected = true;
	int saveTime = 0;	
	if ( CClientSharing::getPayInfo( errMsg, sizeof(errMsg)) != JSON_RET_OK )
	{
		DbgOutput(DBG_WARN, "getPayInfo failed:%s\n", errMsg);
	}
  
	while ( !mbExitThread )
	{
		LOGIN_USERINFO_S user;
		int ret = 0;
		char loginImg[512] = {0};
		memset(&user, 0x00, sizeof(user));
		if(tcpconnected){

		    ret = CClientSharing::RecvHeartPackage(user, loginImg, sizeof(loginImg), errMsg, sizeof(errMsg),mbLoginFlag);
 
		    if ( ret == 0 )
		    {
			if ((strcmp(user.cmd,"open")==0) && user.userid > 0 )
			{
				if ( !mbLoginFlag )
				{
					// 未登录
					memcpy(&CClientSharing::mUserInfo, &user, sizeof(LOGIN_USERINFO_S));
					if ( strlen(loginImg) > 0 )
					{
							if ( !UrlLoadToFile(LOAD_USER_JPG, loginImg) )
							{
									printf("load QR code from:%s failed.\n", loginImg);
							}
					}
					Login();
				}
				else if ( user.userid == CClientSharing::mUserInfo.userid )
				{
					// 有客户续费了
					CClientSharing::mUserInfo.type = user.type;
					CClientSharing::mUserInfo.n_count = user.n_count;
					CClientSharing::mUserInfo.t_count = user.t_count;
					gMainCtrlPage->mRepayWnd.SetPayQRCodeWnd(FALSE);
					// 更新显示包月信息
					gMainCtrlPage->mLeftPierPanel.RefreshLeftDate(3, CClientSharing::mUserInfo.m_count);
				}
			}
			else if(strcmp(user.cmd,"logout")==0){
				Logout();
			}
			else if(strcmp(user.cmd,"reboot")==0) {
				system("reboot");	
			}
		    }else if(ret ==-1) {
			tcpconnected = false;
		    }
		}else {
			if(CClientSharing::ReconnectHeartServer()) {
				tcpconnected = true;
			}else {
				sleep(3);
			}
		}
	}
	DbgOutput(DBG_INFO, "ChekLogin Thread  exit \n");
}

void CSharingIndexWnd::HeartFunc(void)
{
	while ( !mbExitThread )
	{
		CClientSharing::SendHeartPackage(mbLoginFlag);
		sleep(10);
	}
}
void CSharingIndexWnd::DecreaseLeftCount(int type)
{
	if ( mbLoginFlag )
	{
		// 可能同时都存在，以时间为先
		// 如果时间没有了，再看有没有歌曲数量的
		if ( type == payType_time )
		{
			if ( CClientSharing::mUserInfo.t_count > 0 )
			{
        
				CClientSharing::mUserInfo.t_count -= (KEEPALIVE_TIME_OUT/1000);
				gMainCtrlPage->mLeftPierPanel.RefreshLeftTime(payType_time, CClientSharing::mUserInfo.t_count);
				if ( CClientSharing::mUserInfo.t_count == 120 ||
					  CClientSharing::mUserInfo.t_count == 60 )
				{
					// 2分钟和1分钟后提示一次续费
					gMainCtrlPage->mLeftPierPanel.ShowRepayVisible(TRUE);
				}
			}
			/*
			else if ( CClientSharing::mUserInfo.n_count > 0 )
			{
				gMainCtrlPage->mLeftPierPanel.RefreshLeftTime(payType_count, CClientSharing::mUserInfo.n_count);
			}
			*/
			else if(CClientSharing::mUserInfo.n_count <=0)
			{
        int curPosition = 0;
			  int duration = 0;
        
        GetCurPosition(&curPosition);
			  GetDuration(&duration);
        if(duration >= curPosition)
        {
          if((duration - curPosition) <=4000)//当付费时间到了还有歌曲没结束，则在歌曲结束前4秒内停止。
          {
  				  Logout();
          }
        }
        else
        {
          Logout();
        }
			}
		}
		else if ( (type == payType_count) &&
				        (CClientSharing::mUserInfo.n_count >0)  &&
				        (CClientSharing::mUserInfo.t_count <= 0) )				// 只有在时间用完了，才减歌曲数量
		{
			CClientSharing::mUserInfo.n_count--;
			gMainCtrlPage->mLeftPierPanel.RefreshLeftTime(payType_count, CClientSharing::mUserInfo.n_count);
			if ( CClientSharing::mUserInfo.n_count <= 0 )
				Logout();
		}
	}
}

int CSharingIndexWnd::GetLeftTime(int *leftType)
{
	int retLeft = 0;
	if ( CClientSharing::mUserInfo.t_count > 0 )
	{
		*leftType = payType_time;
		retLeft = CClientSharing::mUserInfo.t_count;
	}
	else if ( CClientSharing::mUserInfo.n_count > 0 )
	{
		*leftType = payType_count;
		retLeft = CClientSharing::mUserInfo.n_count;
	}
	return retLeft;
}
