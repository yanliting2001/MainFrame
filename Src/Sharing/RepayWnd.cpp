#include "RepayWnd.h"
#include "XMLParser.h"
#include "BaseApp.h"

#define BTN_CLOSE_ID	0x01
#define BTN_PAYITEM_BASE_ID	0x10

void CPayItemsInfo::Create(CRepayWnd *pParent)
{
	RECT rc = {0, 0, 100, 100};
	CreateStatic(pParent,	rc);

	RECT rcControl;
	SetRectXY(&rcControl, 240, 210, 800, 300);

	for ( int i = 0; i < PAYINFO_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].itemBKWnd.CreateStatic(this, rcControl);
		mPayItemsWnd[i].itemRemarkWnd.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].price_nowWnd.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].price_orgWnd.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].itemBtn.CreateStatic(&mPayItemsWnd[i].itemBKWnd, rcControl);
		mPayItemsWnd[i].itemBtn.SetWndID(BTN_PAYITEM_BASE_ID + i);
		mPayItemsWnd[i].itemBtn.SetOnClickListener(pParent);
	}

	LoadResource();
}

void CPayItemsInfo::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "RepayWnd.xml");
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
	xOffset = parser.GetIntValue("PayItemsInfo", "xOffset", 20);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PayItemsInfo", "path", "RepayWnd/"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);

	for ( i = 0; i < PAYINFO_MAX_COUNT; i++ )
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
	for ( i = 0; i < PAYINFO_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].itemRemarkWnd.SetFontSize(size);
		mPayItemsWnd[i].itemRemarkWnd.SetFontColor(color);
		mPayItemsWnd[i].itemRemarkWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PayItemPriceNowInfo", &rcControl);
	size = parser.GetIntValue("PayItemPriceNowInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("PayItemPriceNowInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	for ( i = 0; i < PAYINFO_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].price_nowWnd.SetFontSize(size);
		mPayItemsWnd[i].price_nowWnd.SetFontColor(color);
		mPayItemsWnd[i].price_nowWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PayItemPriceOrgInfo", &rcControl);
	size = parser.GetIntValue("PayItemPriceOrgInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("PayItemPriceOrgInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	for ( i = 0; i < PAYINFO_MAX_COUNT; i++ )
	{
		mPayItemsWnd[i].price_orgWnd.SetFontSize(size);
		mPayItemsWnd[i].price_orgWnd.SetFontColor(color);
		mPayItemsWnd[i].price_orgWnd.MoveWindow(&rcControl);
	}
}

void CPayItemsInfo::RefreshPayInfo(void)
{
	for ( int i = 0; i < PAYINFO_MAX_COUNT; i++ )
	{
		char price[32] = {0};

		mPayItemsWnd[i].itemRemarkWnd.SetWindowTextA(CClientSharing::mPayItemsList[i].remark);
		snprintf(price, sizeof(price), "￥%.1f", CClientSharing::mPayItemsList[i].price_now);
		mPayItemsWnd[i].price_nowWnd.SetWindowTextA(price);
		snprintf(price, sizeof(price), "￥%.1f", CClientSharing::mPayItemsList[i].price_org);
		mPayItemsWnd[i].price_orgWnd.SetWindowTextA(price);
	}
}

void CRepayWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+18);

	CreateStatic(pParent,	rc,  WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP, WINDOWSTATE_INVISIBLE);

	mPayQRCodeWnd.Create(pParent);

	RECT rcControl = {0, 0, 100, 100};
	mPayItemsWnd.Create(this);

	mCloseBtn.Create(this, rcControl);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	LoadResource();
}

void CRepayWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "RepayWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml
	RECT rcShow;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];

	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "RepayWnd/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);
}

void CRepayWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( bVisible )
	{
		mPayItemsWnd.RefreshPayInfo();
		mPayItemsWnd.SetWindowVisible(TRUE);
		mPayQRCodeWnd.SetWindowVisible(FALSE);
	}
}

void CRepayWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd )
	{
		int nWndID = pWnd->GetWndID();
		if ( nWndID == BTN_CLOSE_ID )
		{
			SetWindowVisible(FALSE);
		}
		else if ( nWndID >= BTN_PAYITEM_BASE_ID && nWndID < (BTN_PAYITEM_BASE_ID + PAYINFO_MAX_COUNT) )
		{
			int idx = nWndID - BTN_PAYITEM_BASE_ID;
			mPayQRCodeWnd.ShowPayQRCode(idx);
			SetWindowVisible(FALSE);
		}
	}
}

void CRepayWnd::SetPayQRCodeWnd(BOOL bVisible)
{
	mPayQRCodeWnd.SetWindowVisible(bVisible);
}

void CPayQRCodeWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+10);
	CreateStatic(pParent,	rc,  WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {0, 0, 100, 100};

	mCloseBtn.Create(this, rcControl);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	mQRCodeWnd.CreateStatic(this, rcControl);

	LoadResource();
}

void CPayQRCodeWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "RepayWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml
	RECT rcShow;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];

	XmlLoadRect(&parser, "QRCodeShowWindowInfo", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("QRCodeShowWindowInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

	XmlLoadRect(&parser, "QRCodeCloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("QRCodeCloseBtnInfo", "path", "RepayWnd/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "QRCodeQRCodeWndInfo", &rcControl);
	mQRCodeWnd.MoveWindow(&rcControl);
}

void CPayQRCodeWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( pWnd )
	{
		int nWndID = pWnd->GetWndID();
		if ( nWndID == BTN_CLOSE_ID )
		{
			SetWindowVisible(FALSE);
			remove(LOAD_PAYCODE_JPG);
		}
	}
}

void CPayQRCodeWnd::ShowPayQRCode(int idx)
{
	if ( idx < 0 || idx >= 5 )	return;

	if ( !UrlLoadToFile(LOAD_PAYCODE_JPG, CClientSharing::mPayItemsList[idx].qrcode) )
	{
		printf("load QR code from:%s failed.\n", CClientSharing::mPayItemsList[idx].qrcode);
		return;
	}
	CTexture texture;
	texture.CreateFromImgFile(LOAD_PAYCODE_JPG);
	mQRCodeWnd.SetBackgroundTexture(&texture);

	SetWindowVisible(TRUE);
}
