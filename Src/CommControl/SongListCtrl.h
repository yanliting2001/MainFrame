#pragma once

#include "BaseListCtrl.h"
#include "Song.h"

typedef enum eSONGLISTVIEW_ITEM
{
	lviSongBackground = 0,
	//lviSongItemImage,
	lviSongSongName,
	lviSongSingerName,
	lviSongFavoritBtn,
	lviSongRushBtn,
	lviSongDelBtn,
	lviSongCloudBtn,
	lviSongGradeBtn,
	lviSongSubCount
} SONGLISTVIEW_ITEM;

typedef enum eSONGBKIMAGETYPE
{
	BKIMAGE_CHINESE = 0,
	BKIMAGE_GLOD,
	BKIMAGE_SILVER,
	BKIMAGE_BRONZE,
	//BKIMAGE_CANTONESE,
	//BKIMAGE_MINNAN,
	//BKIMAGE_KOREAN,
	//BKIMAGE_JAPNESE,
	//BKIMAGE_ENGLISH,
	BKIMAGE_COUNT
} SONGBKIMAGETYPE;

class CSongListCtrl: public CBaseListCtrl,
	public IListCtrlOperateProvider,
	public IMsgRecvWndInterface
{
	typedef CBaseListCtrl CParentClass;

public:
	CSongListCtrl();
	virtual ~CSongListCtrl();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

	// interface
public:
	virtual void OnDeleteItem(LISTITEM *pli);
	virtual void OnItemClicked(int nItemIndex, int nSubItemIndex);
	virtual void OnItemPressed(int nItemIndex, int nSubItemIndex);
	virtual void ClearItemPressed();
	virtual void OnFirstItemIndexChanged(int nNewFirstItemIndex);
	virtual int GetSubItemCount();
	virtual int GetSubItemByPoint(POINT pt);
	virtual void GetFixedSubItemInfo(int nItemIndex);
	virtual void GetUnFixedSubItemInfo(	int nItemIndex);

private:
	void SetSubItemBtnState(int nItemIndex, int nSubItemIndex, int nBtnState);

public:
	void AddSongItem(const SONGINFO *pSongInfo, int nIndex);

	int FindItemByID(const char *cID);

	CTexture *GetSongBkImageTexture(SONGINFO * pSongInfo, int pos = -1);

private:
	SONGBKIMAGETYPE GetSongBkImageType(SONGINFO *pSongInfo);

private:
	//RECT mItemImageRect;
	RECT mSongNameRect;
	RECT mSingerNameRect;
	RECT mFavoritBtnRect;
	RECT mRushBtnRect;
	RECT mDelBtnRect;
  RECT mCloudBtnRect;
  RECT mGradeBtnRect;
	//RECT mPreviewRect;

	CTexture mSongBkTextures[BKIMAGE_COUNT];
	CTexture mFavoritBtnTextures[4];
	CTexture mRushBtnTextures[4];
	CTexture mDelBtnTextures[4];
  CTexture mCloudBtnTextures[4];
  CTexture mGradeBtnTextures[4];
	//CTexture mPreviewTextures[4];

	int mLastPressedItemIndex;
	int mLastPressedSubitemIndex;

	uint32 mSongNameColor;
	uint32 mSingerNameColor;
	uint32 mSelectedColor;
	int mSongSize;
	int mSingerSize;
};
