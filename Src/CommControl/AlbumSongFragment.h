#ifndef _ALBUMSONGFRAGMENT_H
#define _ALBUMSONGFRAGMENT_H
#include "FragmentInterface.h"
#include "BaseButton.h"
#include "Song.h"

class CAlbumSongFragment : public CFragment,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CFragment CParentClass;

	typedef enum _eAlbumType
	{
		albumType_zhongGXGeS = 0x0,		// 中国好声音
		albumType_geMing,					// 革命
		albumType_xiQing,  					// 喜庆
		albumType_wuQu,					// 舞曲
		albumType_woShiGeShou,	// 我是歌手
		albumType_erTong,					// 儿童
		albumType_guangChang,		// 广场
		albumType_xiQu,						// 戏曲
		albumType_mengMianCJ,		// 蒙面唱将
		albumType_heChang,				// 合唱
		albumType_caoYuan,				// 草原
		albumType_minGe,					// 民歌
		albumType_count
	} ALBUM_TYPE_E;

	typedef struct _sAlbumTypeID
	{
		int id;
		char typeName[32];
	} ALBUMTYPE_ID_S;

public:
	CAlbumSongFragment();
	virtual ~CAlbumSongFragment();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, 	POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);
  
  void ShowChidlSong();
  void ShowBirthdaySong();

private:
	CBaseWnd	mItemListBKWnd;
	CBaseButton mItemsBtn[albumType_count];
	CBaseButton mShadowBtn;
	CBaseButton mReturnBtn;
  

	RECT mrcBaseItem;
	int miXoffset;
	RECT mrcOrigItemsBKShow;

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;

private:
	ALBUMTYPE_ID_S mAlbumsID[albumType_count];
};

#endif /* _ALBUMSONGFRAGMENT_H */
