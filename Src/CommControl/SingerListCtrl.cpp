#include "SingerListCtrl.h"
#include "UserMsgDefine.h"
#include "../GlobalUIClass.h"
#include "XMLParser.h"

CSingerListCtrl::CSingerListCtrl()
{
	SetRectXY(&mNameRect, 0, 174, 177, 18);
	SetRectXY(&mItemImageRect, 18, 16, 142, 142);
}

CSingerListCtrl::~CSingerListCtrl()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CSingerListCtrl::Create(CBaseWnd *pParent, RECT rc)
{
	SetOperateProvider(this);

	LoadResource();

	CParentClass::Create(pParent, rc);
}

void CSingerListCtrl::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SingerListCtrl.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	XmlLoadRect(&parser, "NameRectLocation", &mNameRect);
	XmlLoadRect(&parser, "ItemImageRectLocation", &mItemImageRect);

	char imgPath[64] = {0};

	//strcpy(imgPath, parser.GetStrValue("BkGroundImg","path","SingerList/BkGround"));
	//CreateImgTexture(imgPath, &mItemBkTexture);
}

void CSingerListCtrl::OnDeleteItem(LISTITEM *pli)
{
}

void CSingerListCtrl::OnItemClicked(int nItemIndex, int nSubItemIndex)
{
	if (nItemIndex < 0)
	{
		return;
	}

	ReportMsg(LISTCTRL_ITEM_CLICKMSG, nItemIndex, nSubItemIndex);
}

void CSingerListCtrl::OnFirstItemIndexChanged(int nNewFirstItemIndex)
{
	ReportMsg(LISTCTRL_FIRSTITEM_CHANGEMSG, nNewFirstItemIndex, 0);
}

int CSingerListCtrl::GetSubItemCount()
{
	return lviSingerSubCount;
}

int CSingerListCtrl::GetSubItemByPoint(	POINT pt)
{
	return lviSingerBackground;
}

void CSingerListCtrl::GetFixedSubItemInfo(	int nItemIndex)
{
	LISTITEM *pli = GetListItem(nItemIndex);
	if (!pli || !pli->pSubItem)
	{
		return;
	}
	/*
	pli->pSubItem[lviSingerBackground].texture.SetTexture(&mItemBkTexture);
	SetRectXY(&(pli->pSubItem[lviSingerBackground].rect), 0, 0, GetItemWidth(), GetItemHeight());
	*/
	pli->pSubItem[lviSingerBackground].font = FALSE;
	pli->pSubItem[lviSingerBackground].color = 0xFFFFFFFF;
	pli->pSubItem[lviSingerBackground].drawCircle = FALSE;

	pli->pSubItem[lviSingerItemImage].drawCircle = TRUE;
	pli->pSubItem[lviSingerItemImage].font = FALSE;
	pli->pSubItem[lviSingerName].font = TRUE;
	pli->pSubItem[lviSingerName].drawCircle = FALSE;
}

void CSingerListCtrl::GetUnFixedSubItemInfo(int nItemIndex)
{
	LISTITEM *pli = GetListItem(nItemIndex);
	if (!pli || !pli->pSubItem)
	{
		return;
	}

	SINGERINFO *pSingerInfo = (SINGERINFO *) (pli->uItemData);

	int nDirect;
	SIZE szTexture;

	const char *cSingerPath = theBaseApp->GetSingerTextureFolder();
	char cTemp[MAX_PATH];
	memset(cTemp,0,MAX_PATH);
	char cImgPath[MAX_PATH];
	if (pSingerInfo && !pli->pSubItem[lviSingerItemImage].texture.HasTexture())
	{
		sprintf(cImgPath,"%d.jpg", pSingerInfo->id);
		CombinePathName(cTemp, cSingerPath, cImgPath);

		if(cTemp && strlen(cTemp)>0)
		{
			if (!IsFileExist(cTemp))
			{
				sprintf(cTemp, "%s/0.png", cSingerPath);
			}
			pli->pSubItem[lviSingerItemImage].texture.CreateFromImgFile(cTemp);
		}
	}
	CopyRect(&(pli->pSubItem[lviSingerItemImage].rect), &mItemImageRect);
	pli->pSubItem[lviSingerItemImage].font = FALSE;
	pli->pSubItem[lviSingerItemImage].color = 0xFFFFFFFF;

	if (pSingerInfo && strlen(pSingerInfo->cName))
	{
		if (!pli->pSubItem[lviSingerName].texture.HasTexture())
		{
			CBaseStringW in;
			in.Set(pSingerInfo->cName);

			pli->pSubItem[lviSingerName].texture.CreateFromText(
				in.GetString(),
				26,
				FALSE,
				RECTWIDTH(mNameRect),
				RECTHEIGHT(mNameRect));
		}

		szTexture.cx = pli->pSubItem[lviSingerName].texture.GetImageWidth();
		szTexture.cy = pli->pSubItem[lviSingerName].texture.GetImageHeight();
	}
	else
	{
		szTexture.cx = RECTWIDTH(mNameRect);
		szTexture.cy = RECTHEIGHT(mNameRect);
	}
	nDirect = DRAWTEXT_DIRECT_RIGHT|DRAWTEXT_DIRECT_VCENTER;
	GetTextRectByDirect(mNameRect, szTexture.cx, szTexture.cy,
		nDirect, &(pli->pSubItem[lviSingerName].rect));
	pli->pSubItem[lviSingerName].font = TRUE;
	pli->pSubItem[lviSingerName].color = 0xFF637CFF;
}

void CSingerListCtrl::AddSingerItem(const SINGERINFO *pSingerInfo, int nIndex)
{
	if ( !pSingerInfo )
	{
		return;
	}

	SetItemData(nIndex, (UINT64)pSingerInfo);
}

