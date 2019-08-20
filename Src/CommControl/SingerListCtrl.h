#pragma once

#include "BaseListCtrl.h"
#include "Song.h"

typedef enum eSINGERLISTVIEW_ITEM
{
	lviSingerBackground = 0,
	lviSingerItemImage,
	lviSingerName,
	lviSingerSubCount
} SINGERLISTVIEW_ITEM;

class CSingerListCtrl: public CBaseListCtrl,
	public IListCtrlOperateProvider,
	public IMsgRecvWndInterface
{
	typedef CBaseListCtrl CParentClass;

public:
	CSingerListCtrl();
	virtual ~CSingerListCtrl();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

	// interface
public:
	virtual void OnDeleteItem(LISTITEM *pli);
	virtual void OnItemClicked(int nItemIndex, int nSubItemIndex);
	virtual void OnFirstItemIndexChanged(int nNewFirstItemIndex);
	virtual int GetSubItemCount();
	virtual int GetSubItemByPoint(POINT pt);
	virtual void GetFixedSubItemInfo(int nItemIndex);
	virtual void GetUnFixedSubItemInfo(	int nItemIndex);

public:
	void AddSingerItem(const SINGERINFO *pSingerInfo, int nIndex);

private:
	RECT mNameRect;
	RECT mItemImageRect;
	CTexture mItemBkTexture;
};
