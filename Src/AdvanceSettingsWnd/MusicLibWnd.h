//#pragma once
#ifndef _MUSICLIBWND_H
#define _MUSICLIBWND_H

#include "BaseWnd.h"
#include "BaseSlide.h"
#include "../Effect/SlideWnd.h"

typedef enum eMUSICLIB_ITEM
{
	SONG_DELETE=0,
	NEWSONG_SET,
	PUBLIC_SET,
	//BATCH_ADD,
	//SONG_SEARCH,
	//SONG_ENCRYPT,
  MUSIC_ITEM_COUNT,
}MUSICLIB_ITEM;

class CMusicLibWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CMusicLibWnd();
	~CMusicLibWnd();

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
	void SetMusicLibInfo();

private:
	void OnU_DiskClick();
	void OnBatchDeleteClick();
	void OnPubBroadcastClick();
	void OnNewSonSetClick();
	void OnUserClick();

	void OnSongSearchClick();
	void OnSongEncryptClick();

	int GetAllSongsCount();
	int GetPubBroadCastCount();
	int GetNewSongsCount();
	int GetDiskMemory();

public:
	CBaseButton mItemBtn[MUSIC_ITEM_COUNT];

	CBaseWnd mSongCountWnd;
	CBaseWnd mPubBroadCastCountWnd;
	CBaseWnd mNewSongCountWnd;
	CBaseWnd mDiskMemoryWnd;
	CBaseWnd mDiskStatusWnd;
private:
	int miSongCount;
	int miPubBroadCastCount;
	int miNewSongCount;
	unsigned int miDiskMemory;
	int miDiskStatus;
};

#endif


