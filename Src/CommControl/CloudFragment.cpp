#include "CloudFragment.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../SQL/SearchSQL.h"
#include "../GlobalUIClass.h"

#define BTN_RETURN_ID			0x01
#define BTN_ITEM_BASE_ID		0x02
#define BTN_SHADOW_ID			0x03


CCloudFragment::CCloudFragment()
{
	mpPressedWnd = NULL;
	miXoffset = 0;
	mpCurSearchItemType = NULL;
}

CCloudFragment::~CCloudFragment()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CCloudFragment::Create(CBaseWnd *pParent, SEARCHITEM *pRankTypeList)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(	"CloudFragment", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};

	mReturnBtn.Create(this, rcControl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	mShadowBtn.Create(this, rcControl);
  mReturnBtn.SetWndID(BTN_SHADOW_ID);

	mCloudItemBKWnd.CreateStatic(this, rcControl);

	if ( pRankTypeList )
	{
		mpCurSearchItemType = pRankTypeList;
		for ( int i = 0; i < pRankTypeList->sChildItemList.GetCount(); i++ )
		{
			SEARCHITEM *pSearchItem = (SEARCHITEM *)pRankTypeList->sChildItemList.GetAt(i);
			if ( !pSearchItem )
				continue;

			CBaseButton *pItemWnd = new CBaseButton;
			if ( !pItemWnd )
			{
				DbgOutput(DBG_ERR, "new CBaseButton failed.\n");
				break;
			}

			pItemWnd->Create(&mCloudItemBKWnd, rcControl, BUTTONTYPE_NORMAL, false, 0.9f);
			pItemWnd->SetWndID(BTN_ITEM_BASE_ID+i);
			pItemWnd->SetOnClickListener(this);
			pItemWnd->SetOnTouchListener(this);
			if ( i != 0 )
			{
				//pItemWnd->SetWindowTextA(pSearchItem->cItemName);
			}

			mItemsList.AddData(pItemWnd);
		}
	}
	LoadResource();
}

void CCloudFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "CloudFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcAllControl;
	CTexture bkTexture, btnTextures[4], allBtnTextures[4];
	char imgPath[64] = {0};
	int i;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "CloudList/ReturnBtn"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ShadowBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShadowBtnInfo", "path", "CloudList/Shadow"), sizeof(imgPath));
  CreateBtnImgTextures(imgPath, btnTextures);
	mShadowBtn.SetTextures(btnTextures);
	mShadowBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CloudItemsBkWndInfo", &mrcOrigItemsBKShow);

	XmlLoadRect(&parser, "AllCloudItemWndInfo", &rcAllControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("AllCloudItemWndInfo", "path", "CloudList/0"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, allBtnTextures);

	XmlLoadRect(&parser, "BaseListItemWndInfo", &mrcBaseItem);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("BaseListItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));

	int idx = 0;
	for ( i = 0; i < mItemsList.GetCount(); i++ )
	{
		CBaseButton *pitem = (CBaseButton *)mItemsList.GetAt(i);
		if ( !pitem )
			continue;
		if ( idx == 0 )
		{
			pitem->SetTextures(allBtnTextures);
			pitem->MoveWindow(&rcAllControl);
		}
		else
		{
			char tmpPath[128] = {0};

      switch(i)
      {
        case 1:
          XmlLoadRect(&parser, "JanuaryItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("JanuaryItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 2:
          XmlLoadRect(&parser, "FebruaryItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("FebruaryItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 3:
          XmlLoadRect(&parser, "MarchItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("MarchItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 4:
          XmlLoadRect(&parser, "AprilItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("AprilItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 5:
          XmlLoadRect(&parser, "MayItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("MayItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 6:
          XmlLoadRect(&parser, "JuneItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("JuneItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 7:
          XmlLoadRect(&parser, "JulyItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("JulyItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 8:
          XmlLoadRect(&parser, "AugustItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("AugustItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 9:
          XmlLoadRect(&parser, "SeptemberItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("SeptemberItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 10:
          XmlLoadRect(&parser, "OctoberItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("OctoberItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 11:
          XmlLoadRect(&parser, "NovemberItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("NovemberItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
            snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        case 12:
          XmlLoadRect(&parser, "DecemberItemWndInfo", &rcControl);
          SAFE_STRNCPY(imgPath, parser.GetStrValue("DecemberItemWndInfo", "basePath", "CloudList"), sizeof(imgPath));
          snprintf(tmpPath, sizeof(tmpPath), "%s/%d", imgPath, idx);
          break;
        default:
          break;
          
      }
			CreateBtnImgTextures(tmpPath, btnTextures);

			pitem->SetTextures(btnTextures);
			pitem->MoveWindow(&rcControl);
		}
		idx++;
	}

	mrcOrigItemsBKShow.right = rcControl.right;
	miXoffset = 0;
	mCloudItemBKWnd.MoveWindow(&mrcOrigItemsBKShow);
}

void CCloudFragment::OnDestroy()
{
	CParentClass::OnDestroy();

	DelNormalList(&mItemsList, CBaseButton);
}

void CCloudFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_RETURN_ID )
	{
		gMainCtrlPage->ReturnToLastFragment();
	}
	else if ( nWndID >= BTN_ITEM_BASE_ID && nWndID < (BTN_ITEM_BASE_ID + mItemsList.GetCount()) )
	{
		int idx = nWndID - BTN_ITEM_BASE_ID;
		if ( mpCurSearchItemType )
		{
			SEARCHITEM *pSearchType = (SEARCHITEM *)mpCurSearchItemType->sChildItemList.GetAt(idx);
			if ( pSearchType )
			{
				gSongListFragment->SetSearchBarItem(pSearchType, 0);
				gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
			}
			else
			{
				DbgOutput(DBG_ERR, "Get Month Type:%d failed.\n", idx);
			}
		}
	}
}

void CCloudFragment::OnTouchDown(CBaseWnd *pWnd, POINT pt)
{
	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);
	mpPressedWnd = pWnd;

	miXoffset = mCloudItemBKWnd.mRectRelativeToParent.left;

	pWnd->SetCapture();
}

void CCloudFragment::OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

	ReleaseCapture();

	mpPressedWnd = NULL;
}

void CCloudFragment::OnTouchMove(CBaseWnd *pWnd, POINT pt)
{
	if (pWnd != mpPressedWnd)
	{
		return;
	}

	if ( RECTWIDTH(mrcOrigItemsBKShow) <= RECTWIDTH(mRectRelativeToParent) )
		return ;

	POINT ptDrag;
	pWnd->ConvertWndPointToScreenPoint(&ptDrag, pt);
	ConvertScreenPointToWndPoint(&ptDrag, ptDrag);

	int xDragLen = ptDrag.x-mPtPressedScreen.x;
	int dstLeft = miXoffset + xDragLen;
	RECT rcCurBK;
	rcCurBK.left = mrcOrigItemsBKShow.left + dstLeft;
	rcCurBK.right = rcCurBK.left + RECTWIDTH(mrcOrigItemsBKShow);
	rcCurBK.top = mrcOrigItemsBKShow.top;
	rcCurBK.bottom = mrcOrigItemsBKShow.bottom;

	if ( rcCurBK.left > mrcOrigItemsBKShow.left )
	{
		rcCurBK.left = mrcOrigItemsBKShow.left;
		rcCurBK.right = mrcOrigItemsBKShow.right;
	}
	else if ( rcCurBK.right <= RECTWIDTH(mRectRelativeToParent) )
	{
		rcCurBK.right = RECTWIDTH(mRectRelativeToParent);
		rcCurBK.left = rcCurBK.right - RECTWIDTH(mrcOrigItemsBKShow);
	}

	mCloudItemBKWnd.MoveWindow(&rcCurBK);
}

void CCloudFragment::OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd)
{
	ReleaseCapture();

	mpPressedWnd = NULL;
}
