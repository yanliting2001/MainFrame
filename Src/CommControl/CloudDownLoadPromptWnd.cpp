#include "CloudDownLoadPromptWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define BTN_CLICK_CONFIRM	0x01
#define BTN_CLICK_CANCEL	0x02

CCloudDownLoadPromptWnd::CCloudDownLoadPromptWnd()
{
	memset(&mSongInfo, 0, sizeof(SONGINFO));
}

CCloudDownLoadPromptWnd::~CCloudDownLoadPromptWnd()
{
	DbgOutput(DBG_DEBUG, "function %s quit\n", __FUNCTION__);
}

void CCloudDownLoadPromptWnd::Create(
	CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 100, 624, 311);
	SetRectXY(&rcHide, 300, -445, 624, 311);

	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 10);
	CParentClass::Create(
		"CloudDownLoadPromptWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	RECT rcItem;
	SetRectXY(&rcItem, 300, -445, 624, 311);
	mConfirmBtn.Create(this, rcItem);
	mConfirmBtn.SetWndID(BTN_CLICK_CONFIRM);
	mConfirmBtn.SetOnClickListener(this);

	mCancelBtn.Create(this, rcItem);
	mCancelBtn.SetWndID(BTN_CLICK_CANCEL);
	mCancelBtn.SetOnClickListener(this);

	LoadResource();
}

void CCloudDownLoadPromptWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "CloudDownLoadPromptWnd.xml");
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

	XmlLoadRect(&parser, "ConfirmBtnLocation", &rcControl);
	mConfirmBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CancelBtnLocation", &rcControl);
	mCancelBtn.MoveWindow(&rcControl);

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("BkGroundImg","path","SearchBar/BkGround"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4];
	strcpy(imgPath, parser.GetStrValue("ConfirmBtnImg","path","SearchBar/Selected"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mConfirmBtn.SetTextures(btnTextures);

	strcpy(imgPath, parser.GetStrValue("CancelBtnImg","path","SearchBar/Selected"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCancelBtn.SetTextures(btnTextures);
}

void CCloudDownLoadPromptWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();
	if(nWndID == BTN_CLICK_CONFIRM)
	{
		//确定
		ReportMsg(COMFIRM_TO_DOWNLOAD, 0, 0);
	}

	SlideShowWindow(FALSE);
}

void CCloudDownLoadPromptWnd::SetCloudSongInfo(SONGINFO *pSongInfo)
{
	if ( pSongInfo != NULL )
		memcpy(&mSongInfo, pSongInfo, sizeof(SONGINFO));
}

SONGINFO *CCloudDownLoadPromptWnd::GetCloudSongInfo()
{
	return &mSongInfo;
}

