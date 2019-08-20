#include "SettingsFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../CommControl/MainCtrlPage.h"
#include "../PlayerApi/PlayerInfo.h"

CMusicLibWnd* gMusicLibWnd = NULL;
CAddSongWnd* gAddSongWnd = NULL;
CUDiskSongInfo* gUdiskSongInfoWnd = NULL;
CMusicLibTypeWnd *gMusicLibTypeWnd = NULL;

extern CMainCtrlPage *gMainCtrlPage;

#define BTN_RETURN_ID				0x01
#define BTN_ITEM_BASE_ID		0x02

CSettingsFragment::CSettingsFragment()
{
}

CSettingsFragment::~CSettingsFragment()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CSettingsFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
  
	CParentClass::Create(	"SettingsFragment", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

	mItemBKGroundWnd.CreateStatic(this, rcControl);
	mSelectedSettingItemWnd.CreateStatic(this, rcControl);

	for ( int i = 0; i < settingType_count; ++i )
	{
		mSettingItem[i].CreateStatic(this, rcControl);
		mSettingItem[i].SetWndID(BTN_ITEM_BASE_ID + i);
		mSettingItem[i].SetOnClickListener(this);
	}

	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	//
	//mSettingInputWnd.Create(this, rcControl);
	mPlayItemWnd.Create(this, rcControl);
	mMusicLibItemWnd.Create(this, rcControl);
	mPublicItemWnd.Create(this, rcControl);
	mSystemItemWnd.Create(this, rcControl);
  
  //add music
  mAddSongWnd.Create(this, rcControl);
	gAddSongWnd = &mAddSongWnd;

  mUdiskSongInfoWnd.Create(this);
  gUdiskSongInfoWnd = &mUdiskSongInfoWnd;
  
  mMusicLibTypeWnd.Create(this, rcControl);
	gMusicLibTypeWnd = &mMusicLibTypeWnd;
  
  mMusicLibWnd.Create(this, rcControl);
	mMusicLibWnd.SetMsgRecvWnd(this);
	gMusicLibWnd = &mMusicLibWnd ;

  mChooseSongWnd.Create(this, rcControl);
	mMusicLibTypeWnd.SetChooseSongWnd(&mChooseSongWnd);

  mIpSetWnd.Create(this,rcControl);
	mIpSetWnd.SetMsgRecvWnd(this);

  mSetScrollTextWnd.Create(this, rcControl);
	mSetScrollTextWnd.SetMsgRecvWnd(this);
	LoadResource();
}

void CSettingsFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Settings/SettingsFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcBaseItem;
	CTexture bkTexture, btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	unsigned long color;
	int size, xOffset;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "FragmentPublic/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SettingsBKGroundWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SettingsBKGroundWndInfo", "path", "Settings/SettingsBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mItemBKGroundWnd.SetBackgroundTexture(&bkTexture);
	mItemBKGroundWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SelectedSettingItemInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SelectedSettingItemInfo", "path", "Settings/SettingItemTab"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	mSelectedSettingItemWnd.SetBackgroundTexture(&bkTexture);
	mSelectedSettingItemWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SettingsItemInfo", &rcBaseItem);
	xOffset = parser.GetIntValue("SettingsItemInfo", "xOffset", 20);
	size = parser.GetIntValue("SettingsItemInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("SettingsItemInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);

	for ( int i = 0; i < settingType_count; i++ )
	{
		char tmp[64] = {0};
		sprintf(tmp, "text%d", i);

		InflatRect(&rcControl, &rcBaseItem, i*xOffset, 0);
		mSettingItem[i].SetFontSize(size);
		mSettingItem[i].SetFontColor(color);
		mSettingItem[i].SetWindowTextA(parser.GetStrValue("SettingsItemInfo", tmp, "未设置"));
		mSettingItem[i].MoveWindow(&rcControl);
	}

	selectedSettingItem(settingType_play);
}

void CSettingsFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_RETURN_ID )
	{
		gMainCtrlPage->ReturnToLastFragment();
	}
	else if ( nWndID >= BTN_ITEM_BASE_ID && nWndID < (BTN_ITEM_BASE_ID + settingType_count) )
	{
		int idx = nWndID - BTN_ITEM_BASE_ID;
		selectedSettingItem((SETTING_TYPE_E)idx);
	}
}

void CSettingsFragment::selectedSettingItem(SETTING_TYPE_E type)
{
	moveSelectedItemWnd(type);

	mPlayItemWnd.SetWindowVisible(FALSE);
	mMusicLibItemWnd.SetWindowVisible(FALSE);
	mPublicItemWnd.SetWindowVisible(FALSE);
	mSystemItemWnd.SetWindowVisible(FALSE);
  mMusicLibWnd.SetWindowVisible(FALSE);
  mAddSongWnd.SetWindowVisible(FALSE);
	mMusicLibTypeWnd.SetWindowVisible(FALSE);
	mChooseSongWnd.SetWindowVisible(FALSE);
  mIpSetWnd.SetWindowVisible(FALSE);
	mSetScrollTextWnd.SetWindowVisible(FALSE);
  //mSettingInputWnd.SetWindowVisible(TRUE);
  if(mUdiskSongInfoWnd.IsWindowVisible())
	{
		gPlayerInfo->StopPreview();
		mUdiskSongInfoWnd.SetWindowVisible(FALSE);
	}
  DbgOutput(DBG_ERR, "%s, line = %d type = %d\n", __FUNCTION__, __LINE__, type);
	switch( type )
	{
	case settingType_play:
		mPlayItemWnd.SetWindowVisible(TRUE);
		break;

	case settingType_musicLib:
		mMusicLibItemWnd.SetWindowVisible(TRUE);
		break;

	case settingType_public:
		mPublicItemWnd.SetWindowVisible(TRUE);
		break;

	case settingType_system:
		mSystemItemWnd.SetWindowVisible(TRUE);
		break;
  case settingType_musicmanage:
    //mSettingInputWnd.SetWindowVisible(FALSE);
    mMusicLibWnd.SetWindowVisible(TRUE);    
    break;
  case settingType_ipset:
   // mSettingInputWnd.SetWindowVisible(FALSE);
    mIpSetWnd.UpdateIP();
		mIpSetWnd.SetWindowVisible(TRUE);
    break;
  case settingType_lightset:
   // mSettingInputWnd.SetWindowVisible(FALSE);
    mSetScrollTextWnd.SetWindowVisible(TRUE);
    break;

	default:
		break;
	}
}

void CSettingsFragment::moveSelectedItemWnd(SETTING_TYPE_E type)
{
	RECT rcXian;

	CopyRect(&rcXian, &mSelectedSettingItemWnd.mRectRelativeToParent);
	int width = RECTWIDTH(mSelectedSettingItemWnd.mWndRect);

	rcXian.left = mSettingItem[type].mRectRelativeToParent.left;
	rcXian.right = rcXian.left + width;
	mSelectedSettingItemWnd.MoveWindow(&rcXian);
}

void CSettingsFragment::deleteSongs(void)
{
	deleteLeastOrdersSongs(30);
	selectedSettingItem(settingType_play);
}
