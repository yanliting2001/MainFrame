#include "YiLang.h"
#include "BaseApp.h"
#include "XMLParser.h"

void CYiLang::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 2, 0, 1276, 74);
	SetRectXY(&rcHide, 2, -74, 1276, 74);
	CParentClass::Create(
		"TopPier",
		pParent,
		WINDOWFLAG_DEFAULT,
		rcShow,
		rcHide,
		FALSE);

	RECT rcControl = {10, 0, 500, 30};

  for (int i = 0; i < Yilang_count; i++ )
	{
		mYilangItem[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
		mYilangItem[i].SetWndID(Shop_item+i);
		mYilangItem[i].SetOnClickListener(this);
	}

	LoadResource();
}

void CYiLang::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;
	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();
  
	CombinePathName(cFileName, cXmlFolder, "YiLang.xml");
	if ( false == parser.Load(cFileName) )
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
	char imgPath[64] = {0};
	CTexture bkTexture[4];
	char colorbuf[16];
	int size;
	UINT32 color;

  XmlLoadRect(&parser, "ShopWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShopWndInfo", "path", "YiLang/Logo"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mYilangItem[Shop_item].SetTextures(bkTexture);
	mYilangItem[Shop_item].MoveWindow(&rcControl);


  XmlLoadRect(&parser, "FoodBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("FoodBntInfo", "path", "YiLang/Homepage"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mYilangItem[Food_item].SetTextures(bkTexture);
	mYilangItem[Food_item].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "KbarBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("KbarBntInfo", "path", "YiLang/Cloud"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mYilangItem[Kbar_item].SetTextures(bkTexture);
	mYilangItem[Kbar_item].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "YiwaBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("YiwaBntInfo", "path", "YiLang/Record"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, bkTexture);
	mYilangItem[Yiwa_item].SetTextures(bkTexture);
	mYilangItem[Yiwa_item].MoveWindow(&rcControl);
  
}

void CYiLang::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;
	int nWndID = pWnd->GetWndID();
	
        
    switch ( nWndID )
		{
   
    case Shop_item:

			break;

		case Food_item:

			break;
		case  Kbar_item:

      break;
      
    case  Yiwa_item:
      break;

		default:
			break;
		}
    
 }



