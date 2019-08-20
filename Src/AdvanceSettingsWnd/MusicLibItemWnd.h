#pragma once

#include "BaseWnd.h"
#include "BaseSlide.h"
#include "Song.h"
//#include "ChooseSongWnd.h"
#include "SetMusicLibWnd.h"

#define MUSICLIBEITEM_SHOW_COUNT 5

typedef enum eMUSICLIBTYPE
{
	TYPE_PUBLIC = 0,
	TYPE_NEWSONG,
	TYPE_DELETE,
	TYPE_MUSIC_COUNT
}MUSICLIBTYPE;

class CMusicLibTypeWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

	typedef struct tagMUSICLIBITEM
	{
		CBaseWnd SongNameWnd;
		CBaseWnd SingerNameWnd;
		CBaseWnd HotNumWnd;
		CBaseButton ChooseBtn;
		CBaseButton TiaoWnd;
	}MUSICLIBITEM;

public:
	CMusicLibTypeWnd();
	~CMusicLibTypeWnd();

public:
	void Create(
		CBaseWnd *pParent,
		RECT rc);

	void LoadResource();

	// interface
public:
	virtual void OnClick(
		CBaseWnd *pWnd,
		POINT pt);

	void SetChooseSongWnd(CSetMusicLibWnd *pChooseSongWnd);
	void SetMusicLibType(MUSICLIBTYPE type);

public:
	void OnChooseBtnClick(int Index);
	void OnConfirmBtnClick();
	void OnBackBtnClick();
	void OnChooseSongBtnClick();

	void RefreshItem();

private:
	void InitArrayList();
	void AddSongItem(
			SONGINFO *pSongInfo);
	void UpdateItem();

private:
	void FreeListData(void);

private:
	CBaseWnd mTitleIcoWnd;
	CBaseWnd mTitlePromptWnd;
	CBaseButton mTiaoWnd;

	MUSICLIBITEM mItem[MUSICLIBEITEM_SHOW_COUNT];

	CBaseWnd   mPageInfoWnd;
	CBaseButton mSongPagePrevBtn;
	CBaseButton mSongPageNextBtn;
	int mSongCurPage;
	int mSongPageCount;

	CBaseButton mConfirmBtn;
	CBaseButton mBackBtn;
	CBaseButton mChooseSongBtn;

	CPtrListCtrl mSongItemList;

	MUSICLIBTYPE mCurType;		//0:公播设置 1:新歌设置  2:批量删除 默认为0:
	char mTitleIcoBasePath[64];
	//CChooseSongWnd 	*mpChooseSongWnd;
	CSetMusicLibWnd 	*mpChooseSongWnd;
};
