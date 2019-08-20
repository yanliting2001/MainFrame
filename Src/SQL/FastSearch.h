#pragma once

#include "commonctrl_header.h"
#include "../InputMethodWnd/InputMethodDef.h"
#include "Song.h"

#define USE_NEW_GEN_RANK		1

void LoadTypesListFromFile(CPtrCtrl *pTypesList, const char *cFileName);

void InitFastSearch();
void DeInitFastSearch();

void DeInitFastSearchNotSaveList();

void DeInitFastSearchNotRankList();

void FastSearch_SaveAllListNotRank();

const char* FastSearch_GetSingerType(
	const char * cSingerName);

void FastSearch_SearchSingerListBySingerType(
	const char *cSingerType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListBySongName(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListBySingerName(
	const void *cSingerID,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListByLanguageType(
	const void *cLanguage,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListBySongType(
	const void *cSongType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListByCloud(
	const void *cCloudType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListByRanking(
	const void *cRankingType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchNewSongList(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchGradeSongList(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);


void FastSearch_SearchMovieSongList(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListByRank(
	const char *cRankType,
	CPtrCtrl *pResultList);

void FastSearch_SearchSongListByRank(
	const char *cRankType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pReSultList);

void FastSearch_SearchSongListByFavorit(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

int FastSearch_AddSongToFavorit(
	const char *cFavoritID,
	const char *cSongID);

void FastSearch_DelSongFromFavorit(
	const char *cFavoritID,
	const char *cSongID);

void FastSearch_LoadFavoritSongList(
	const char *cFavoritID);

void FastSearch_SaveFavoritSongList(
	const char *cFavoritID);

void FastSearch_SearchSongListByLyric(
	const char *cLyric,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);
void FastSearch_SearchLocalSongList(
        const SEARCHINFO *pSearchInfo,
        CPtrCtrl *pResultList);
void FastSearch_SavePubBroadCast();
//add by yq,2012-12-17
void FastSearch_SaveDownloadedSong();
void FastSearch_SaveDownloadingDong();
void FastSearch_LoadSelectedList(CPtrListCtrl *pList);
void FastSearch_SaveSelectedList(CPtrListCtrl *pList);

void FastSearch_SaveAllList();

int FastSearch_GetSongCount();
int FastSearch_GetLocalSongCount();
int FastSearch_GetCloudSongCount();
int FastSearch_GetPubBroadCastSongCount();
int FastSearch_GetNewSongCount();

void FastSearch_SearchPubBroadCastSongList(
		const SEARCHINFO *pSearchInfo,
		CPtrCtrl *pResultList);

void FastSearch_SearchDownloadedSongList(
		const SEARCHINFO *pSearchInfo,
		CPtrCtrl *pResultList);

void FastSearch_SearchDownloadingSongList(
		const SEARCHINFO *pSearchInfo,
		CPtrCtrl *pResultList);


SONGINFO *FastSearch_GetPubBroadCastSongByIdx(int idx);
SONGINFO *FastSearch_GetDownloadedSongByIdx(int idx);
SONGINFO *FastSearch_GetDownloadingSongByIdx(int idx);

void FastSearch_SavePubBroadCastSongList(CPtrListCtrl *pResultList);

void FastSearch_SaveNewSongList(CPtrListCtrl *pList);

void FastSearch_DeleteSongList(CPtrListCtrl *pList);

void FastAdd_SongList(const SONGINFO *pAddSongInfo);

int FastSearch_GetFavoritSongCount();
//end add by yq,2012-12-17

int FastSearch_GetMaxSongID();
void FastSearch_IncreaseMaxSongID(int iStep);

void FastSearch_GetAllSongList(
		CPtrCtrl *pResultList);

void FastSearch_GetPublicSongList(
		CPtrCtrl *pResultList);

int FastSearch_GetSongInfoByID(char *id,
		SONGINFO *pSongInfo);
int FastSearch_SetSongCloudByID(char *id,bool isCloud);
int FastSearch_AddPubSong(SONGINFO *pSongInfo);
void FastSearch_DelPubSong(char *songid);
void FastSearch_MovePubSongTop(char *cSongID);

void FastSearch_AddDownloadedSong(SONGINFO *pSongInfo);
void FastSearch_DelDownloadedSong(char *songid);
void FastSearch_MoveDownloadedTop(char *cSongID);
void FastSearch_MoveDownloadingSongTop(char *cSongID);
void FastSearch_MoveFavSongTop(char *cSongID);

void FastSearch_AddDownloadingSong(SONGINFO *pSongInfo);
void FastSearch_DelDownloadingSong(char *songid);
void FastSearch_MoveDownloadingTop(char *cSongID);
//增加歌曲的点击率
void FastAdd_SongOrderTimes(const SONGINFO *pSongInfo);
void FastAdd_SongOrderTimes(const char *cID);
void FastSave_AllSongList();

//增加歌手的点击率
void FastAdd_SingerOrderTimes(const SINGERINFO *pSingerInfo);

void FastDelete_UserSongList(const SONGINFO *pSongInfo);

void FastAdd_UserSongList(const SONGINFO *pSongInfo);

void FastModify_CloudSongExist(SONGINFO *pSongInfo);

void FastSearch_LoadCloudList(CPtrListCtrl *pList);

void FastSearch_SaveCloudList(CPtrListCtrl *pList);

void FastSet_SongInfoExistValue(SONGINFO *pSongInfo, BOOL bExist);

void FastSearch_SearchSongListBySongNameNoCloud(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList);

extern CPtrArrayCtrl gAllSingerList;
extern CPtrArrayCtrl gAllSongList;
//void FastSearch_LoadUDiskSongList(
//	const SONGINFO *pAddSongInfo);

//save add u disk song info for sync pad
//void FastSearch_AddUDiskSongList(
//		const SONGINFO *pAddSongInfo);
char *FastSearch_GetSingerByID(int id);
int FastSearch_GetSingerIDByName(const char *singer);
SINGERINFO* FastSearch_GetSingerInfoByID(int *sID); //yanliting add 2013-11-02
SONGINFO *FastSearch_FindSongInfoByHashInfo(const unsigned char *hashinfo);
SONGINFO *FastSearch_FindSongInfoFromDownloadingByHashInfo(const unsigned char *hashinfo);
int FastSearch_FindSongInfoByHashInfo(CPtrCtrl *pSongList,const unsigned char *hashinfo,SONGINFO *pSongInfo);
void FastSearch_LoadSingedList(CPtrListCtrl *pList);
void FastSearch_SaveSingedList(CPtrListCtrl *pList);
void FastSearch_DeleteListBySongInfo(SONGINFO *pSong);
int FastSearch_FindSongInfoByIDFromFavList(char *songid,SONGINFO *pSongInfo);
int FastSearch_FindSongInfoByIDFromPubList(char *songid,SONGINFO *pSongInfo);
SONGINFO *FastSearch_GetFavoritSongByIdx(int idx);
void FastSearch_MoveSongTop(char *cSongID);
void FastSearch_SaveAllSongList();
void FastSearch_GetFavSingerList(CPtrCtrl *pResultList);
void FastSearch_AddSingerToFavorit(const char *cSingerID);
void FastSearch_DelSongFromFavorit(const char *cSingerID);
void FastSearch_MoveFavSingerTop(char *cSingerID);
void FastSearch_LoadFavoritSingerList();
void FastSearch_SaveFavoritSingerList();
int FastSearch_GetPubBroadCastLocalSongCount();
int FastSearch_GetFavoritLocalSongCount();
void FastSearch_DelAllSongFromList(char *listtype);
void FastSearch_DeleteAllSongList(CPtrArrayCtrl *pResultList);


//#define DELETE_SONGS_COUNT		(200)
void deleteLeastOrdersSongs(int cnt=200);

// 保存歌曲列表为新歌
void SaveSongListToNewSong(CPtrListCtrl *pSongList);

// 保存歌曲列表为公播
void SaveSongListToPublicSong(CPtrListCtrl *pSongList);

#if USE_NEW_GEN_RANK
 void GenerateAllRankSongList();
 void AfreshAllRankSongListforClick();

#endif

