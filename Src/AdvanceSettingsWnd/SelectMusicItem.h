#ifndef _SELECTMUSICITEM_H
#define _SELECTMUSICITEM_H
#include "BaseWnd.h"
#include "BaseButton.h"
//#include "RadioButtonGroup.h"
#include "Song.h"
#include "../SQL/SearchSQL.h"
//#include "Input/BKInputPanel.h"

#define SELECTITEM_COUNT	6

typedef struct _sSelectCondition
{
	char  cName[64];  	//歌曲名
	char  cSinger[24];	//歌星名
	char  cType[24];	//歌曲类型
	char  cLang[16];	//语种
	int   wOrderTimes;	//点击率
	int   wWdCt;		//歌名字数
} sSearchCondition;

typedef struct _sItemListInfo
{
	void 	*pData;		// 指向SONGIFNO结构
	BOOL	bSelected;
} sItemListInfo;

class CSelectedItem : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

	typedef struct _sSelectMusicItem
	{
		CBaseWnd	SongNameWnd;
		CBaseWnd	HotNumWnd;
		CBaseButton	ChooseBtn;
		CBaseWnd	LineWnd;
	} sSelectMusicItem;
public:
	CSelectedItem();
	virtual ~CSelectedItem();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void AddSearchListToSelectedList(void);

	// 初始化已选择的链表
	void InitSelectedItemList(CPtrListCtrl *pInitList);

	void FreeAllList();

	void UpdateItemByCondition(const sSearchCondition condition);

private:
	//void AddSearchListToSelectedList(SONGINFO *pSearchSong);

	//void DeleteSelectedListFromSearchList(SONGINFO *pSearchSong);

	BOOL IsInSelectedList(SONGINFO *pSearchSong);

	void UpdateItemList();

	void UpdatePageInfo();

	void OnAllSelectedClick(BOOL bAllSelected);

	void OnItemClick(int idx);

	void FreeSearchItemList();

	void FreeSelectedItemList();

private:
	CBaseWnd		mSelectedPromptWnd;
	CBaseWnd		mTitleLineWnd;
	CBaseWnd		mSelectedAllWnd;
	CBaseButton		mSelectedAllBtn;		// 全选
	CBaseWnd		mUnSelectedAllWnd;
	CBaseButton		mUnSelectedAllBtn;		// 反选
	sSelectMusicItem mItemWnd[SELECTITEM_COUNT];

	CPtrListCtrl	mSearchList;			//
	CPtrListCtrl	*mpInitList;
	int 			miSelectedNum;

	CBaseWnd		mPageInfoWnd;
	CBaseButton		mPrevPageBtn;
	CBaseButton		mNextPageBtn;
	int				miPageCount;
	int				miCurrPage;
};

#define CONDITIONTYPE_COUNT		4

class CTypeConditionWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;
public:
	CTypeConditionWnd();

	virtual ~CTypeConditionWnd();
public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);
public:
	void InitTypeConditonList(CPtrListCtrl *pList);

	char * GetTypeCondition();

private:
	void UpdateConditionItem();

	void OnItemClick(int idx);

private:
	CBaseButton	mPrevPageBtn;
	CBaseButton	mNextPageBtn;

	CBaseButton	mTypeTextBtn[CONDITIONTYPE_COUNT];
	int			miCurrPage;
	int			miPageCount;
	CPtrListCtrl *mpTypeList;
	CBaseStringA	mSelectedTypeName;
};

#endif
