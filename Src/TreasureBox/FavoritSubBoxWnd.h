#ifndef _FAVORITSUBBOXWND_H
#define _FAVORITSUBBOXWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "Song.h"
#include "../Effect/PopupSlideWnd.h"

//#include "TreasureBoxDef.h"

#define MYDOCSUBBOXWND_COUNT		5

typedef struct _sFavoritItem
{
	CBaseWnd  SongName;
	CBaseButton Insert;
	CBaseButton Rush;
	CBaseButton Del;
	CBaseWnd Tiao;
} sFavoritItem;

class CFavoritSubBoxWnd : public CPopupSlideWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
  
typedef CPopupSlideWnd CParentClass;

public:
	CFavoritSubBoxWnd();
	~CFavoritSubBoxWnd();

public:
  
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnWindowVisible(BOOL bVisible);

private:
	void InitFavoritList();

	void FreeFavoritList();

	void UpdateFavoritItem();

	void SetPageInfo();

	void AddFavoritItem(SONGINFO *pFavoritSong);

private:
	sFavoritItem 	mMyDocItem[MYDOCSUBBOXWND_COUNT];
	CBaseWnd		mPageInfoWnd;
	CBaseButton 	mPrevPageBtn;
	CBaseButton 	mNextPageBtn;
	CBaseButton 	mLoginExit;
  CBaseWnd 	mTitleIcoWnd;
	CBaseButton mColseBtn;
  
	CPtrArrayCtrl 	mFavoritList;
	int 			mCurPage;
	int 			mPageCount;
	unsigned long mSelectColor;

private:
#if APP_ID == QIKUN500
	void LoadResource_v500();
#elif APP_ID == QIKUN510
	void LoadResource_v510();
#elif APP_ID == QIKUN501
	void LoadResource_v501();
#elif APP_ID == QIKUN520
	void LoadResource_v520();
#endif
};

#endif
