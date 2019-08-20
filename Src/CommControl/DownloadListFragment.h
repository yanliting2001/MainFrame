#ifndef _DOWNLOADLISTFRAGMENT_H
#define _DOWNLOADLISTFRAGMENT_H
#include "FragmentInterface.h"
#include "BaseButton.h"
#include "Song.h"

class CSystemInfoWnd: public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	void RefreshSystemInfo(void);

private:
	CBaseWnd mVodIDWnd;		// 机器码
	CBaseWnd mIPAddressWnd;
	CBaseWnd mLocalSongCntWnd;
	CBaseWnd mCloudSongCntWnd;
	CBaseWnd mLeftMemoryWnd;
	CBaseWnd mCanBeAddCntWnd;
	CBaseButton mShowCloudSongBtn;
};

class CDownloadListFragment : public CFragment,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CFragment CParentClass;

	typedef struct _sItemInfo
	{
		CBaseWnd itemBKGroundWnd;
		CBaseWnd idxWnd;
		//CBaseWnd fengeWnd;
		CBaseWnd songNameWnd;
		CBaseWnd singerWnd;
		CBaseWnd precentWnd;
		CBaseButton downloadIcoWnd;
		CBaseButton deleteBtn;
		CBaseWnd fengexianWnd;
	} ITEMINFO_S;

	typedef enum _eItemCount
	{
		item_count = 9,
	} ITEMCOUNT_E;

public:
	CDownloadListFragment() : miCurPage(0), miAllPage(0) {}

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTimer(int nTimerID);

	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	void InitDownloadList(void);

private:
	void UpdateItemList(void);

	void SetSongInfo(int nIndex, const SONGINFO *pSongInfo);

	void SetDownProcess(int process);

	void StartUpdateProcess(BOOL bStart);

private:
	CBaseButton	mReturnBtn;
	CBaseWnd mDownloadListLabel;

	CBaseWnd mLeftBKGroundWnd;
	CSystemInfoWnd mSystemInfoWnd;

	ITEMINFO_S mItemList[item_count];

	CBaseButton mPrevPageBtn;
	CBaseButton mNextPageBtn;
	CBaseWnd mPageWnd;

	int miCurPage;
	int miAllPage;
};

#endif /* _DOWNLOADLISTFRAGMENT_H */
