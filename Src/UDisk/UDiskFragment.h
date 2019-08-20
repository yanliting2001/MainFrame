#ifndef _UDISKFRAGMENT_H
#define _UDISKFRAGMENT_H
#include "../CommControl/FragmentInterface.h"
#include "BaseButton.h"
#include "UDiskFileInfo.h"
#include "AddUDiskFileWnd.h"

class CUDiskFragment : public CFragment,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CFragment CParentClass;

	typedef struct _sItemInfo
	{
		CBaseWnd itemBKGroundWnd;
		CBaseWnd idxWnd;
		CBaseWnd songNameWnd;
		CBaseWnd singerWnd;
		CBaseButton playBtn;;
		CBaseButton addBtn;
		CBaseWnd fengexianWnd;
	} ITEMINFO_S;

	typedef enum _eItemCount
	{
		item_count = 9,
	} ITEMCOUNT_E;

	typedef enum _ePageType
	{
		page_uDisk = 0,
		page_item,
		page_count,
	} PAGE_TYPE_E;

public:
	CUDiskFragment() : miCurPage(0), miAllPage(0), mPageType(page_uDisk) {}
	~CUDiskFragment() { ClearFileList(); }

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnWindowVisible(BOOL bVisible);

	virtual void OnTimer(int nTimerID);

	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	void InitUDiskFileList(void);

private:
	void UpdateItemList(void);

	void SetUDiskFileInfo(int nIndex, const UDISK_FILE_INFO_S *pFileInfo);

	void ClearFileList(void) { DelNormalList(&mUDiskFileLists, UDISK_FILE_INFO_S); }

	void ShowPageType(void);

private:
	CBaseButton	mReturnBtn;
	CBaseWnd mDownloadListLabel;

	CBaseWnd mLeftBKGroundWnd;

	CBaseWnd mUDiskFileBKGroundWnd;
	ITEMINFO_S mItemList[item_count];

	CAddUDiskFileWnd mAddUDiskFileWnd;

	CBaseButton mPrevPageBtn;
	CBaseButton mNextPageBtn;
	CBaseWnd mPageWnd;

	CBaseButton mAddFileBtn;
	CBaseButton mAddAllFileBtn;

	int miCurPage;
	int miAllPage;

	CPtrListCtrl mUDiskFileLists;

	PAGE_TYPE_E mPageType;
	char mPromptText[page_count][64];
};

#endif /* _DOWNLOADLISTFRAGMENT_H */
