#pragma once

#include "commonctrl_header.h"
#include "FastSearch.h"
#include "SongInfoDef.h"

#define SEARCHSINGER_DEPTH_LEIXING 1
#define SEARCHSINGER_DEPTH_SINGER 2
// 歌星类型(depth:0)->大陆男，大陆女，港台男,,,(depth:1)->（大陆男）刘德华，张学友，，，(depth:2)

#define SEARCHSONG_DEPTH_TYPE 1
// 歌曲类型(depth:0)->流行歌曲，舞曲，，，(depth:1)->（流行歌曲）忘情水，，，
// 语种类型(depth:0)->国语，粤语，，，(depth:1)->忘情水，，，

//typedef enum eSearchType
//{
//	SearchByPinyin=0,
//	SearchBySinger,
//	SearchBySongType,
//	SearchByLanguageType,
//	SearchByRank,
//	SearchByNewSong,
//	SearchByMovieSong,
//	SearchByFavorit,
//} SearchType;

typedef enum eInputSearchType
{
	InputSearchPinyin=0,
} InputSearchType;

typedef enum eSongLengthSearchType
{
	SongLengthSearch=0,
} SongLengthSearchType;

//typedef struct tagSEARCHITEM
//{
//	SearchType eType;
//	int nDepth;
//	char cItemName[32];
//	tagSEARCHITEM *pParentItem;
//	CPtrListCtrl sChildItemList;
//} SEARCHITEM ;

extern SEARCHITEM gSearchSpell;
extern SEARCHITEM gSearchSingerType;
extern SEARCHITEM gSearchSongType;
extern SEARCHITEM gSearchLanguageType;
extern SEARCHITEM gSearchRankType;
extern SEARCHITEM gSearchAlbumType;
extern SEARCHITEM gSearchMonthType;

typedef void (*SEARCHSQLRECORD_CALLBACK)(int nIndex, void *pRecord, void *param);

void InitSQL();
void DeinitSQL();

void DeinitSQLNotSaveRankList();

void DeinitSQLNotSaveList();

SEARCHITEM * FindSubSearchItem(
	SEARCHITEM *pParent,
	const char *cSubItemName);

int GetSearchItemIndex(
	SEARCHITEM *pSearchItem,
	const char *cItemName);

SEARCHITEM * AddSubSearchItem(
	SEARCHITEM *pParent,
	const char *cSubItemName,
	int itemId);

void DelAllSubSearchItem(
	SEARCHITEM *pParent);

SEARCHITEM* GetSingerTypeSearchItem(
	const char* cSingerName);

int ConnectDb(
		char *pStrDbName);
int DisconnectDb();
int Sqlite3_insert_song(SONGINFO *pSongInfo);

int sqlite3_delete_song(SONGINFO *pSongInfo);

int BeginTransaction();
int Commit();
int DeleteView();
int CreateView();

//更新歌星和歌曲点击率
void Sql_UpdateSingerOrderTimes(const SINGERINFO *pSingerInfo);

void Sql_UpdateSongOrderTimes(const SONGINFO *pSongInfo);

void UpdateOrderTimes();

int GetSongCountBySongInfo(const SONGINFO *pSongInfo);

void SetExistValue(const SONGINFO *pSongInfo, int iValue);

void FreeSingerList(CPtrCtrl *pSingerList);
void FreeSongList(CPtrCtrl *pSongList);
