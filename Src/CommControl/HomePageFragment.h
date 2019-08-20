#ifndef _HOMEPAGEFRAGMENT_H
#define _HOMEPAGEFRAGMENT_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "FragmentInterface.h"

class CHomePageFragment : public CFragment,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CFragment CParentClass;

	typedef enum _eHomePageSelectType
	{
		selectType_rank = 0,
		selectType_Food,
		selectType_Shop,
		selectType_songName,
    		selectType_singer ,
		selectType_classify,
		selectType_movie,
		selectType_NewSongs,
		selectType_count,
	} HOMEPAGESELECTTYPE_E;

	enum _eAlbumCount
	{
		album_count = 1,
	};

public:
	CHomePageFragment();
	virtual ~CHomePageFragment();

public:
	void Create(CBaseWnd *pParent);
	virtual void LoadResource();
  virtual void OnTimer(int nTimerID);

public:
	virtual void OnDestroy();
	virtual void OnWindowVisible(	BOOL bVisible);

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchDrag(CBaseWnd *pWnd, int xDragLen, int yDragLen);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);
	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);

private:
	void OnClickItem(HOMEPAGESELECTTYPE_E type);

	void SetAlbumPicByIdx(int idx);

private:
	CBaseButton mHomePageItem[selectType_count];

	RECT mrcBaseItem;
	int miXoffset;
public:
	CBaseWnd	mHomeItemBKWnd;
	RECT mrcOrigItemsBKShow;

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;

private:
	CBaseButton mPrevAlbumBtn;
	CBaseButton mNextAlbumBtn;
	int miCurAlbumIdx;
	CTexture mAlbumTextures[album_count][4];
};

#endif		// _HOMEPAGEFRAGMENT_H

