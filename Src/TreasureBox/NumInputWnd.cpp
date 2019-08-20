#include "NumInputWnd.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "../CommApi/GetResParentPath.h"

#define BUTTON_ID_NUMBERBASE	0x01
#define BUTTON_ID_DOT			0x10
#define BUTTON_ID_BACK			0x11

CNumInputWnd::CNumInputWnd()
{
}

CNumInputWnd::~CNumInputWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CNumInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("CNumInputWnd", pParent,rc);

	RECT rcControl;
	int i;
  char text[8] = {0};
  
	SetRectXY(&rcControl, 145, 368, 60, 48);
	for ( i = 0; i < NUMINPUT_MAX; ++i )
	{
		mNumBtn[i].Create(this, rcControl);
		mNumBtn[i].SetWndID(BUTTON_ID_NUMBERBASE + i);
		mNumBtn[i].SetOnClickListener(this);
    if ( i >= 0 && i < 9 )
		{
			snprintf(text, sizeof(text), "%d", i+1);
			mNumBtn[i].SetWindowTextA(text);
		}
		else if ( i == 10 )
			mNumBtn[i].SetWindowTextA("0");
	}
 

	LoadResource();
}

void CNumInputWnd::LoadResource()
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
void CNumInputWnd::LoadResource_v500()
{
	char cParentName[MAX_PATH] = {0};
	char cFileName[MAX_PATH];

	GetXmlBasePath(cParentName, sizeof(cParentName));
	CombinePathName(cFileName, cParentName, "TreasureBox/UserLoginWnd.xml");

	LoadResourceByXmlFile(cFileName);

}
#elif APP_ID == QIKUN510
void CNumInputWnd::LoadResource_v510()
{

}
#elif APP_ID == QIKUN501
void CNumInputWnd::LoadResource_v501()
{

}
#elif APP_ID == QIKUN520
void CNumInputWnd::LoadResource_v520()
{

}
#endif

void CNumInputWnd::LoadResourceByXmlFile(const char *cXmlFileName)
{
	if ( !cXmlFileName )
		return ;

	CParentClass::LoadResource();

	XMLParser parser;

	if(false == parser.Load(cXmlFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cXmlFileName);
	}

	RECT rcControl;
	RECT rcItem;
	CTexture btnTextures[4], btnSpaceTextures[4], btnBackTextures[4];
	char imgPath[64] = {0};
  char colorbuf[16];
	int size, columnNum;
	UINT32 color;
  
	XmlLoadRect(&parser, "OtherNumBtnInfo", &rcItem);
	int xOffset = parser.GetIntValue("OtherNumBtnInfo", "xSpace", 77);
	int yOffset = parser.GetIntValue("OtherNumBtnInfo", "ySpace", 55);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("OtherNumBtnInfo", "basePath", "Box/Number/WordItem"), sizeof(imgPath));
  size = parser.GetIntValue("OtherNumBtnInfo", "size", 26);
  SAFE_STRNCPY(colorbuf, parser.GetStrValue("OtherNumBtnInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
  color = strtoul(colorbuf,NULL,16);
  CreateBtnImgTextures(imgPath, btnTextures);


  XmlLoadRect(&parser, "BackBtnInfo", &rcControl);
  SAFE_STRNCPY(imgPath,parser.GetStrValue("BackBtnInfo", "path", "Box/Number/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnSpaceTextures);
  
	SAFE_STRNCPY(imgPath,parser.GetStrValue("DotBtnInfo", "path", "Box/Number/Dot"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnBackTextures);
  
  int i, j, k;
	for ( i = 0 ; i < NUMINPUT_MAX; i++ )
	{
		j = i % 3;
		k = i / 3;
		InflatRect(&rcControl, &rcItem, j*xOffset, k*yOffset);
		mNumBtn[i].SetFontSize(size);
		mNumBtn[i].SetFontColor(color);
		if ( i == 9 )
			mNumBtn[i].SetTextures(btnSpaceTextures);
		else if ( i == 11 )
			mNumBtn[i].SetTextures(btnBackTextures);
		else
			mNumBtn[i].SetTextures(btnTextures);
		mNumBtn[i].MoveWindow(&rcControl);
	}

}

void CNumInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();
  char num;
  nWndID = nWndID - BUTTON_ID_NUMBERBASE;
	if ( nWndID == 9 )
	{
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
	}
	else if ( nWndID == 11 )
	{
		ReportMsg(INPUTWND_WORDLENGTHMSG, '.', 0);
	}
  else if (nWndID == 10)
	{
		num =  '0';
		ReportMsg(INPUTWND_WORDLENGTHMSG, num, 0);
	}
	else
	{
		num = nWndID + '1';
		ReportMsg(INPUTWND_WORDLENGTHMSG, num, 0);
	}
}

