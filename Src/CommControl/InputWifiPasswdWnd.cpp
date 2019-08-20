#include "InputWifiPasswdWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include <ctype.h>

#define BUTTON_ID_CANCEL	0x01
#define BUTTON_ID_COMFIRM	0x02
#define BUTTON_ID_BACK		0x03
#define BUTTON_ID_UNDERLINE	0x04
#define BUTTON_ID_UPTOLOWER	0x05
#define BUTTON_ID_OK		0x06
#define BUTTON_BASEID_SPELL	0x10

static char g_cSymbol[WIFIPASSWDSYMBOL_COUNT] =
{
		'!', '@', '#', '$', '%', '^', '&', '(', ')', '+', '-',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '?',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', '.', '/',
		'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z'
};

CInputWifiPasswdWnd::CInputWifiPasswdWnd()
{
}

CInputWifiPasswdWnd::~CInputWifiPasswdWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CInputWifiPasswdWnd::Create(
	CBaseWnd *pParent,
	RECT rc)
{
	CParentClass::Create("InputWifiPasswdWnd", pParent, rc);

	RECT rcControl;
	SetRectXY(&rcControl, 180, 290, 122, 42);

	int i;
	for ( i = 0 ; i < WIFIPASSWDSYMBOL_COUNT ; i++ )
	{
		mSymbolItem[i].Create(this, rcControl);
		mSymbolItem[i].SetWndID(BUTTON_BASEID_SPELL+i);
		mSymbolItem[i].SetOnClickListener(this);
	}

	mBackBtn.Create(this, rcControl);
	mBackBtn.SetWndID(BUTTON_ID_BACK);
	mBackBtn.SetOnClickListener(this);

	mUnderLineBtn.Create(this, rcControl);
	mUnderLineBtn.SetWndID(BUTTON_ID_UNDERLINE);
	mUnderLineBtn.SetOnClickListener(this);

	mUpToLowerBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mUpToLowerBtn.SetWndID(BUTTON_ID_UPTOLOWER);
	mUpToLowerBtn.SetOnClickListener(this);

	mOKBtn.Create(this, rcControl);
	mOKBtn.SetWndID(BUTTON_ID_OK);
	mOKBtn.SetOnClickListener(this);

	LoadResource();
}

void CInputWifiPasswdWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "InputWifiPasswdWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT  rcControl, rcItem;
	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SpellSizeLocation", &rcControl);
	int columnNum = parser.GetIntValue("SpellSizeLocation", "columns", 4);
	int xOffset = parser.GetIntValue("SpellSizeLocation", "xOffset", 4);
	int yOffset = parser.GetIntValue("SpellSizeLocation", "yOffset", 63);

	int i, j, k;
	for (i = 0 ; i < WIFIPASSWDSYMBOL_COUNT; i++)
	{
		j = i % columnNum;
		k = i / columnNum;
		InflatRect(&rcItem, &rcControl, j*xOffset, k*yOffset);
		mSymbolItem[i].MoveWindow(&rcItem);
	}

	XmlLoadRect(&parser, "BackBtnLocation", &rcControl);
	mBackBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "UnderLineBtnLocation", &rcControl);
	mUnderLineBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "UpToLowerBtnLocation", &rcControl);
	mUpToLowerBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "OKBtnLocation", &rcControl);
	mOKBtn.MoveWindow(&rcControl);

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("InputWifiPasswdBgImg","path","KGo/top"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4];
	strcpy(imgPath,parser.GetStrValue("ItemImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( i = 0; i < WIFIPASSWDSYMBOL_COUNT; i++ )
		mSymbolItem[i].SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("BackBtnImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mBackBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("UnderLineBtnImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mUnderLineBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("UpToLowerBtnImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mUpToLowerBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("OKBtnImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mOKBtn.SetTextures(btnTextures);
}

void CInputWifiPasswdWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();
	int nClickID;
	char cChar;
	if( nWndID == BUTTON_ID_BACK )
	{
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
		return;
	}
	else if ( nWndID == BUTTON_ID_UNDERLINE )
	{
		cChar = '_';
	}
	else if ( nWndID == BUTTON_ID_UPTOLOWER )
	{
		return ;
	}
	else if ( nWndID == BUTTON_ID_OK )
	{
		ReportMsg(INPUT_WIFIINFO_COMFIRM,	0, 0);
		return ;
	}
	else if ( nWndID < (BUTTON_BASEID_SPELL + WIFIPASSWDSYMBOL_COUNT))
	{
		nClickID = nWndID - BUTTON_BASEID_SPELL;
		cChar = g_cSymbol[nClickID];
		if ( !mUpToLowerBtn.GetRadioBtnPressed() )
		{
			if ( isalpha(cChar) )
				cChar = tolower(cChar);
		}
	}
	ReportMsg(INPUTWND_CHARMSG,	(UINT64)cChar, 0);
}

CWifiPasswdWnd::CWifiPasswdWnd()
{
	mPasswd.Trunk(0);
	miIdx = -1;
}

CWifiPasswdWnd::~CWifiPasswdWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}


void CWifiPasswdWnd::Create(
	CBaseWnd *pParent,
	RECT rc)
{
	CParentClass::Create("WifiPasswdWnd", pParent, rc);

	RECT rcControl;
	SetRectXY(&rcControl, 180, 290, 122, 42);

	mPasswdBgWnd.CreateStatic(this, rcControl);
	mWifiNameWnd.CreateStatic(this, rcControl);
	mWifiPassWdWnd.CreateStatic(this, rcControl);

	mInputWnd.Create(this, rcControl);
	mInputWnd.SetMsgRecvWnd(this);

	mCancelBtn.Create(this, rcControl);
	mCancelBtn.SetWndID(BUTTON_ID_CANCEL);
	mCancelBtn.SetOnClickListener(this);

	mComfirmBtn.Create(this, rcControl);
	mComfirmBtn.SetWndID(BUTTON_ID_COMFIRM);
	mComfirmBtn.SetOnClickListener(this);

	LoadResource();
}

void CWifiPasswdWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "InputWifiPasswdWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT  rcControl;
	XmlLoadRect(&parser, "rcWifiPasswdBgLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PasswdBgWndLocation", &rcControl);
	mPasswdBgWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "WifiNameWndLocation", &rcControl);
	mWifiNameWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "WifiPassWdWndLocation", &rcControl);
	mWifiPassWdWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CancelBtnLocation", &rcControl);
	mCancelBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ComfirmBtnLocation", &rcControl);
	mComfirmBtn.MoveWindow(&rcControl);

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("PasswdBgImg","path","KGo/top"));
	CreateImgTexture(imgPath, &bkTexture);
	mPasswdBgWnd.SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4];
	strcpy(imgPath,parser.GetStrValue("CancelBtnImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCancelBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("ComfirmBtnImg","path","InputWnd/Region"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mComfirmBtn.SetTextures(btnTextures);
}

void CWifiPasswdWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();
	switch ( nWndID )
	{
	case BUTTON_ID_CANCEL:
		miIdx = -1;
		SetWindowVisible(FALSE);
		break;

	case BUTTON_ID_COMFIRM:
		OnComfirmClick();
		break;
	}
}

void CWifiPasswdWnd::OnMsg(
		UINT32 uType,
		UINT64 wParam,
		UINT64 lParam)
{
	switch (uType)
	{
	case INPUTWND_BACKMSG:
		mPasswd.Trunk(mPasswd.GetLength() - 1);
		UpdateInputPromptText();
		break;

	case INPUT_WIFIINFO_COMFIRM:
		OnComfirmClick();
		break;

	case INPUTWND_CHARMSG:
		OnInputChar((char)wParam);
		break;
	}
}

void CWifiPasswdWnd::OnInputChar(char cChar)
{
	if ( mPasswd.GetLength() > 32 )
		return;

	mPasswd.Append(cChar);
	UpdateInputPromptText();
}

void CWifiPasswdWnd::SetPasswdTitle(char *strWifiName, int iWifiIdx)
{
	if ( strWifiName == NULL )
		return ;
	SetWindowVisible(TRUE);
	miIdx = iWifiIdx;
	mPasswd.Trunk(0);
	mWifiNameWnd.SetWindowTextA(strWifiName);
	UpdateInputPromptText();
}

void CWifiPasswdWnd::UpdateInputPromptText()
{
	if ( mPasswd.GetLength() > 0 )
		mWifiPassWdWnd.SetWindowTextA(mPasswd.GetString());
	else
		mWifiPassWdWnd.SetWindowTextA(NULL);
}

void CWifiPasswdWnd::OnComfirmClick()
{
	SetWindowVisible(FALSE);
	ReportMsg(INPUT_WIFIINFO_COMFIRM,	(UINT64)mPasswd.GetString(), miIdx);
}

