#include "SettingInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define BTN_ITEM_BASE_ID		0x02
#define COMFIRMBTN_CLICKID 	30
#define CANCLEBTN_CLICKID 	31

#define ITEM_PRESSED_SCALE 0.9

void CSettingInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("SettingInput", pParent, rc);

	RECT rcControl = {0, 0, 68, 68};
	int i;
	char text[8] = {0};
	for ( i = 0 ; i < inputItem_Count; i++ )
	{
		mItemsWnd[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, ITEM_PRESSED_SCALE);
		mItemsWnd[i].SetWndID(BTN_ITEM_BASE_ID + i);
		mItemsWnd[i].SetOnClickListener(this);
		mItemsWnd[i].SetOnTouchListener(this);

		if ( i >= 0 && i < 9 )
		{
			snprintf(text, sizeof(text), "%d", i+1);
			mItemsWnd[i].SetWindowTextA(text);
		}
		else if ( i == 10 )
			mItemsWnd[i].SetWindowTextA("0");
	}



	LoadResource();
}

void CSettingInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Settings/SettingInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT  rcControl, rcBase;
	CTexture btnTextures[4], btnSpaceTextures[4], btnBackTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	int size, columnNum, xOffset, yOffset;
	UINT32 color;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	MoveWindow(&rcControl);

	XmlLoadRect(&parser, "InputItemBtnInfo", &rcBase);
	size = parser.GetIntValue("InputItemBtnInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("InputItemBtnInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	columnNum = parser.GetIntValue("InputItemBtnInfo", "columns", 6);
	xOffset = parser.GetIntValue("InputItemBtnInfo", "xOffset", 4);
	yOffset = parser.GetIntValue("InputItemBtnInfo", "yOffset", 63);
	SAFE_STRNCPY(imgPath,parser.GetStrValue("InputItemBtnInfo", "path", "InputWnd/Region"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);

	SAFE_STRNCPY(imgPath,parser.GetStrValue("SpaceBtnInfo", "path", "InputWnd/Region"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnSpaceTextures);
	SAFE_STRNCPY(imgPath,parser.GetStrValue("BackBtnInfo", "path", "InputWnd/Region"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnBackTextures);

	int i, j, k;
	for ( i = 0 ; i < inputItem_Count; i++ )
	{
		j = i % columnNum;
		k = i / columnNum;
		InflatRect(&rcControl, &rcBase, j*xOffset, k*yOffset);
		mItemsWnd[i].SetFontSize(size);
		mItemsWnd[i].SetFontColor(color);
		if ( i == 9 )
			mItemsWnd[i].SetTextures(btnSpaceTextures);
		else if ( i == 11 )
			mItemsWnd[i].SetTextures(btnBackTextures);
		else
			mItemsWnd[i].SetTextures(btnTextures);
		mItemsWnd[i].MoveWindow(&rcControl);
	}


}

void CSettingInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();

  DbgOutput(DBG_ERR, "nWndID = <%d>!!\n", nWndID);
  if ( nWndID >= BTN_ITEM_BASE_ID && nWndID < (BTN_ITEM_BASE_ID + inputItem_Count) )
	{
		int nClickID = nWndID - BTN_ITEM_BASE_ID;
		char itemText;
		if ( nClickID == 9 )
			itemText = ' ';
		else if ( nClickID == 11 )
		{
			ReportMsg(INPUTWND_BACKMSG, 0, 0);
			return ;
		}
		else if ( nClickID == 10 )
			itemText = '0';
		else
			itemText = '1' + nClickID;
		ReportMsg(INPUTWND_CHARMSG, (UINT64)itemText,	0);
	}
}

void CSettingInputWnd::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();
	RefreshItemText(nWndID);
}

void CSettingInputWnd::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
}

void CSettingInputWnd::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
}

void CSettingInputWnd::RefreshItemText(int clickIdx)
{
	 if ( clickIdx >= BTN_ITEM_BASE_ID && clickIdx < (BTN_ITEM_BASE_ID + inputItem_Count) )
	{
		int nClickID = clickIdx - BTN_ITEM_BASE_ID;
		mItemsWnd[nClickID].SetWindowVisible(FALSE);
		mItemsWnd[nClickID].SetWindowVisible(TRUE);
	}
}
