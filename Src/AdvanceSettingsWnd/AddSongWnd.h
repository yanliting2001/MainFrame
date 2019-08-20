#pragma once

#include "BaseWnd.h"
#include "BaseSlide.h"
#include "../Effect/SlideWnd.h"
//#include "../CommControl/WaitingWnd.h"
#include "../CommDef/Song.h"

typedef enum eUDISKADDTYPE
{
	TYPE_BATCH = 0,
	TYPE_MANUAL,
	TYPE_UDISK_ADD_COUNT
}UDISKADDTYPE;

//U盘歌曲信息
typedef struct tagUDSONGINFO
{
	char  cID[8+1];            //歌曲ID
	char  cName[128+1];         //歌曲名
	short wWdCt;               //歌名字数
	char  cSinger[20+1];       //歌星名
	char  cType[20+1];          //歌曲类型
	char  cLang[8+1];          //语种
	char  cServerName[20+1];   //服务器名(网络版用)
	char  cFilePath[256+1];     //文件路径
	char  cFileName[128+1];     //文件名称
	char  cPicPath[256+1];      //歌曲图片
	char  cSpell[9+1];          //拼音缩写
	short wVol;                //默认音量
	short wMaxVol;             //最大音量
	short wMinVol;             //最小音量
	short wTrack;              //伴唱音轨
	bool  bChos;               //是否合唱
	bool  bExist;              //歌曲是否存在
	bool  bNewSong;            //是否为新歌
	char  cStroke[10+1];       //笔划
	bool  bIsChoose;			//是否选中
}UDSONGINFO;			//重新定义的目的是防止u盘歌曲路径或文件名太长

typedef struct tagUDISKINFOITEM
{
	CBaseButton  SongWnd;
	CBaseButton ChooseBtn;
	CBaseWnd TiaoWnd;
	UDSONGINFO USong;
}UITEM;

#define ADDSONGWND_ITEM_COUNT	10

class CAddSongWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

public:
	CAddSongWnd();
	~CAddSongWnd();

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

public:
	void SetAddSongType(UDISKADDTYPE type);
	void GetUdiskInfo();
	void UpdateUdiskInfo();

private:
	void SearchFromUdiskForBatch(const char *UdiskPath);
	void SearchFromUdiskForManual(const char *UdiskPath,int iLoopDepth);
	void SetChooseIndex(int nIndex);
	void SetChooseSong(int nIndex);
	void OnInsertClick();
	void AddSongToDataAndDisk();
	BOOL WhetherSpaceEnough(const char *srcSongSize);
	int CopyChooseToAddSong(SONGINFO *dstSong, UDSONGINFO *srcSong);

	void GetSongInfoFromTxt(const char *filename,const char *filePath);
	void SetSongInfoFromBuf(char *buf,const char *filePath);
private:
	CBaseWnd mTileIcoWnd;
	CBaseWnd mTileWnd;
	CBaseWnd mCopyStateWnd;

	UITEM mUItem[ADDSONGWND_ITEM_COUNT];

	CBaseWnd mPageInfoWnd;
	CBaseButton mPrevPageBtn;
	CBaseButton mNextPageBtn;

	CBaseButton BackBtn;
	CBaseButton InsertBtn;

	CPtrListCtrl mUdiskList;


private:
	UDISKADDTYPE mCurAddType;
	int miCurSongPageNum;
	int miSongPageNum;
	char mTitleIcoBasePath[64];
	int mLocalSongID;			//本地分配的SongId;
	int mSuccessCopyNum;		//成功拷贝的个数
	int mChooseNum;				//选中的个数
};



