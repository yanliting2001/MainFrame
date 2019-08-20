#include "BKSpellInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define SPELLBTN_CLICK_BASEID		0x00
#define NUMBERBTN_CLICK_BASEID		0x20
#define SPELLFLAGBTN_CLICK_ID		0x30
#define SPACEBTN_CLICK_ID			0x31
#define SPECIALSYMBOLBTN_CLICK_ID	0x32
#define BACKBTN_CLICK_ID			0x33

CBKSpellInputWnd::CBKSpellInputWnd()
{
	mbInSymbol 	= FALSE;
	mbInUpper	= FALSE;
}

CBKSpellInputWnd::~CBKSpellInputWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CBKSpellInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("BKSpellInput", pParent, rc);

	int i ;
	RECT rcControl = {0, 0, 57, 57};
	for ( i = 0; i < BKSPELLITEM_COUNT; ++i )
	{
		mSpellItemBtn[i].Create(this, rcControl);
		mSpellItemBtn[i].SetWndID(SPELLBTN_CLICK_BASEID + i);
		mSpellItemBtn[i].SetOnClickListener(this);
	}

	for ( i = 0; i < BKNUMBERITEM_COUNT; ++i )
	{
		mNumberItemBtn[i].Create(this, rcControl);
		mNumberItemBtn[i].SetWndID(NUMBERBTN_CLICK_BASEID + i);
		mNumberItemBtn[i].SetOnClickListener(this);
	}

	mSpellFlagBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mSpellFlagBtn.SetWndID(SPELLFLAGBTN_CLICK_ID);
	mSpellFlagBtn.SetOnClickListener(this);

	mSpaceBtn.Create(this, rcControl);
	mSpaceBtn.SetWndID(SPACEBTN_CLICK_ID);
	mSpaceBtn.SetOnClickListener(this);

	mSpecialSymbolBtn.Create(this, rcControl);
	mSpecialSymbolBtn.SetWndID(SPECIALSYMBOLBTN_CLICK_ID);
	mSpecialSymbolBtn.SetOnClickListener(this);

	mBackBtn.Create(this, rcControl);
	mBackBtn.SetWndID(BACKBTN_CLICK_ID);
	mBackBtn.SetOnClickListener(this);

	LoadResource();

}

void CBKSpellInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "BKSpellInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl, rcItem;
	char strImgPath[64];
	int column = 0, xOffset = 0, yOffset = 0;
	int i, x, y;

	XmlLoadRect(&parser, "ShowWindowInfo", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	CTexture bkTexture;
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("ShowWindowInfo","ImgPath","AdvanceSettings/Input/SpellBG"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4];

	// number location
	XmlLoadRect(&parser, "NumberBtnInfo", &rcControl);
	xOffset = parser.GetIntValue("NumberBtnInfo", "xOffset", 57);
	yOffset = parser.GetIntValue("NumberBtnInfo", "yOffset", 57);
	column 	= parser.GetIntValue("NumberBtnInfo", "colNums", 8);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("NumberBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);

	for( i = 0; i < BKNUMBERITEM_COUNT; ++i )
	{
		x = i % column;
		y = i / column;
		InflatRect(&rcItem, &rcControl, x*xOffset, y*yOffset);
		mNumberItemBtn[i].MoveWindow(&rcItem);
		mNumberItemBtn[i].SetTextures(btnTextures);
	}

	// spell location
	XmlLoadRect(&parser, "SpellBtnInfo", &rcControl);
	xOffset	= parser.GetIntValue("SpellBtnInfo", "xOffset", 57);
	yOffset = parser.GetIntValue("SpellBtnInfo", "yOffset", 57);
	column 	= parser.GetIntValue("SpellBtnInfo", "colNums", 8);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SpellBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);

	for ( i = 0; i < BKSPELLITEM_COUNT; ++i )
	{
		x = (i+4) % column;
		y = (i+4) / column;
		InflatRect(&rcItem, &rcControl, x*xOffset, y*yOffset);
		mSpellItemBtn[i].MoveWindow(&rcItem);
		mSpellItemBtn[i].SetTextures(btnTextures);
	}

	XmlLoadRect(&parser, "UppderFlagBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("UppderFlagBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSpellFlagBtn.MoveWindow(&rcControl);
	mSpellFlagBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "SpaceBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SpaceBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSpaceBtn.MoveWindow(&rcControl);
	mSpaceBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "SpecialSymbolBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SpecialSymbolBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSpecialSymbolBtn.MoveWindow(&rcControl);
	mSpecialSymbolBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "BackBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("BackBtnInfo", "ImgPath", "AdvanceSettings/Input/Item"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mBackBtn.MoveWindow(&rcControl);
	mBackBtn.SetTextures(btnTextures);
}

void CBKSpellInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )
		return ;

	int iWndID = pWnd->GetWndID();
	int iClickID;
	wchar_t c;

	if ( iWndID == SPECIALSYMBOLBTN_CLICK_ID )
	{
		OnSpecialSymbolClick();
		return ;
	}

	mbInSymbol = FALSE;
	if ( (iWndID >= SPELLBTN_CLICK_BASEID) && (iWndID < (SPELLBTN_CLICK_BASEID + BKSPELLITEM_COUNT)) )
	{
		// Spell
		iClickID = iWndID - SPELLBTN_CLICK_BASEID;

		if ( mbInUpper )
			c = L'A';
		else
			c = L'a';

		ReportMsg(INPUTWND_CHARMSG, (UINT64)(c+iClickID), 0);
	}
	else if ( (iWndID >= NUMBERBTN_CLICK_BASEID) && (iWndID < (NUMBERBTN_CLICK_BASEID + BKNUMBERITEM_COUNT)) )
	{
		// Number
		iClickID = iWndID - NUMBERBTN_CLICK_BASEID;
		c = L'0' + iClickID;
		ReportMsg(INPUTWND_WORDLENGTHMSG, (UINT64)(c), 0);
	}
	else if ( iWndID == SPELLFLAGBTN_CLICK_ID )
	{
		if ( mSpellFlagBtn.GetRadioBtnPressed() )
			mbInUpper = TRUE;
		else
			mbInUpper = FALSE;
	}
	else if ( iWndID == SPACEBTN_CLICK_ID )
	{
		c = L' ';
		ReportMsg(INPUTWND_CHARMSG, (UINT64)c, 0);
	}
	else if ( iWndID == BACKBTN_CLICK_ID )
	{
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
	}
}

void CBKSpellInputWnd::OnSpecialSymbolClick()
{
	static int iClickNum = 0;
	if ( mbInSymbol )
	{
		iClickNum++;
		iClickNum %= 2;
	}
	else
	{
		mbInSymbol = TRUE;
		iClickNum = 0;
	}

	if ( iClickNum == 0 )
		ReportMsg(INPUTWND_SPECIALCHARMSG, (UINT64)L'[', 0);
	else
		ReportMsg(INPUTWND_SPECIALCHARMSG, (UINT64)L']', 1);
}
