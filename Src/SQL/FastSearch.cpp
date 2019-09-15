#include "FastSearch.h"
#include "BaseApp.h"
#include "DataFileOperate.h"
#include "../GlobalUIClass.h"
#include "string.h"
#include "SearchSQL.h"

#include "InternetCtrl/NetCtrlApi.h"

#pragma GCC diagnostic ignored "-Waddress"

#ifdef CMD_BY_CASE

#define CMP_SONG_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSongInfo)								\
	( (eInputMethodType == INPUTMETHODTYPE_SPELL && strcasestr(pSongInfo->cSpell, cContent)) ||		\
	(eInputMethodType == INPUTMETHODTYPE_HANDWRITE && strcasestr(pSongInfo->cName, cContent)) ||	\
	(eInputMethodType == INPUTMETHODTYPE_STROKES && strcasestr(pSongInfo->cStroke, cContent)) ) || \
	(eInputMethodType == INPUTMETHODTYPE_LYRIC && strcasestr((const char*)pSongInfo->pLyrics, cContent))

#define CMP_SINGER_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSingerInfo)							\
	( (eInputMethodType == INPUTMETHODTYPE_SPELL && strcasestr(pSingerInfo->cSpell, cContent)) ||		\
	(eInputMethodType == INPUTMETHODTYPE_HANDWRITE && strcasestr(pSingerInfo->cName, cContent)) ||	\
	(eInputMethodType == INPUTMETHODTYPE_STROKES && strcasestr(pSingerInfo->cStroke, cContent)) )

#else

#define CMP_SONG_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSongInfo)                                \
    ( (eInputMethodType == INPUTMETHODTYPE_SPELL && (strcasestr(pSongInfo->cSpell, cContent) == pSongInfo->cSpell)) ||        \
    (eInputMethodType == INPUTMETHODTYPE_HANDWRITE && (strcasestr(pSongInfo->cName, cContent) == pSongInfo->cName)))

#define CMP_SINGER_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSingerInfo)                            \
    ( (eInputMethodType == INPUTMETHODTYPE_SPELL && (strcasestr(pSingerInfo->cSpell, cContent) == pSingerInfo->cSpell)) ||        \
    (eInputMethodType == INPUTMETHODTYPE_HANDWRITE && (strcasestr(pSingerInfo->cName, cContent) == pSingerInfo->cName)) )

#endif

#define CMP_BY_WORDLENGTH(eWordLengthType, wWordLength)				\
	( (eWordLengthType == WordLength_All) ||							\
	(eWordLengthType+1 == wWordLength) )

typedef BOOL (*SINGER_PRECOMPARE_FUNC)(const void *pCompareData, const SINGERINFO *pSongInfo) ;
typedef BOOL (*SONG_PRECOMPARE_FUNC)(const void *pCompareData, const SONGINFO *pSongInfo) ;
typedef BOOL (*SONG_PRECOMPARE_LYRIC_FUNC)(const char* pCompareData, const SONGINFO *pSongInfo);

int g_iMaxSongId = 0;
CPtrArrayCtrl gAllSingerList;
CPtrArrayCtrl gAllSongList;

CPtrArrayCtrl gAllRankSongList;

CPtrArrayCtrl gFavoritSongList;

//add by yq,2012-12-17
CPtrArrayCtrl gPubBroadCastSongList;
CPtrArrayCtrl gDownloadedSongList;
CPtrArrayCtrl gDownloadingSongList;


BOOL Singer_PrecompareBySingerType(const void *pCompareData, 	const SINGERINFO *pSingerInfo)
{
	if (!pCompareData || ((const char *)pCompareData)[0] == '\0')
	{
		return TRUE;
	}

	if(strncasecmp("所有",(const char *)pCompareData, strlen("所有")) == 0)
	{
		return TRUE;
	}

	if (strcasecmp(pSingerInfo->cType, (const char *)pCompareData) == 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareBySingerName(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if ( !pCompareData || ( *(int *)pCompareData) == 0 )
	{
		return TRUE;
	}

	if ( pSongInfo->singerID == *(int *)pCompareData )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareByLanguageType(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if ( !pCompareData || ( *(int *)pCompareData) == 0 )
	{
		return TRUE;
	}

	if ( pSongInfo->langID == *(int *)pCompareData )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareBySongType(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if ( !pCompareData || ( *(int *)pCompareData) == 0 )
	{
		return TRUE;
	}

	if ( pSongInfo->typeID == *(int *)pCompareData )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareByCloudType(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if ( !pCompareData || ( *(int *)pCompareData) == 0 )
	{
		return TRUE;
	}

	if ( pSongInfo->monthID == *(int *)pCompareData )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareByRankingType(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if ( !pCompareData || ( *(int *)pCompareData) == 0 )
	{
		return TRUE;
	}

	if ( pSongInfo->rankID == *(int *)pCompareData )
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareByNewSong(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if (pSongInfo->bNewSong)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL Song_PrecompareByGradeSong(const void *pCompareData, const SONGINFO *pSongInfo)
{
	if (pSongInfo->bChos == TRUE)  //此标志位用来检测评分
	{
		return TRUE;
	}

	return FALSE;
}


int FastSearch_FindSongInfoByID(const void *pItem, const void *pData)
{
	SONGINFO *pSongInfo = (SONGINFO *)pItem ;
	char *cID = (char *)pData ;

	if ( strcmp(pSongInfo->cID, cID) == 0 )
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

int FastSearch_FindSingerInfoByID(const void *pItem, const void *pData)
{
	SINGERINFO *pSingerInfo = (SINGERINFO *)pItem ;
	int *cID = (int *)pData ;

	if ( pSingerInfo->id == *cID )
	{
		return 1 ;
	}
	else
	{
		return 0 ;
	}
}

char *FastSearch_GetSingerByID(int id)
{
	SINGERINFO*singer = FastSearch_GetSingerInfoByID(&id);
	return singer ?  singer->cName : NULL;
}

int FastSearch_GetSingerIDByName(const char *singer)
{
	int nCount = gAllSingerList.GetCount();
	int i;
	SINGERINFO *pSingerInfo = NULL;

	for ( i = 0 ; i < nCount ; i++ )
	{
		pSingerInfo = (SINGERINFO *)gAllSingerList.GetAt(i);
		if ( pSingerInfo && (strcmp(pSingerInfo->cName, singer) == 0) )
		{
			return pSingerInfo->id;
		}
	}
	return 0;
}

SINGERINFO* FastSearch_GetSingerInfoByID(int *sID)
{
	int nCount = gAllSingerList.GetCount();
	int i;
	SINGERINFO *pSingerInfo;
	for ( i = 0 ; i < nCount ; i++ )
	{
		pSingerInfo = (SINGERINFO *)gAllSingerList.GetAt(i);

		if (!pSingerInfo)
		{
			continue;
		}
		if( pSingerInfo->id == *sID ) return pSingerInfo;
	}
	return NULL;
}

CPtrArrayCtrl gUserSongList;		// 用户添加的歌曲链表，用于区别云服务器上的链表

void InitUserSongList(
		const char *cFileName)
{
	DelNormalList(&gUserSongList, SONGINFO);

	UINT64 uTime1 = GetTickCount();

	LoadSongsFromFile(&gUserSongList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get Total UserSong Song list %d, used %llu ms\n",
			gUserSongList.GetCount(), uTime2-uTime1);
}

void InitPubBroadCastSongList(
		const char *cFileName)
{
	DelNormalList(&gPubBroadCastSongList, SONGINFO);

	UINT64 uTime1 = GetTickCount();

	LoadSongsFromFile(&gPubBroadCastSongList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get Total PubBroadCast Song list %d, used %llu ms\n",
			gPubBroadCastSongList.GetCount(), uTime2-uTime1);
}

void InitDownloadedSongList(const char *cFileName)
{
	DelNormalList(&gDownloadedSongList, SONGINFO);
	LoadSongsFromFile(&gDownloadedSongList, cFileName);
}
void InitDownloadingSongList(const char *cFileName){
	DelNormalList(&gDownloadingSongList, SONGINFO);
	LoadSongsFromFile(&gDownloadingSongList, cFileName);
}

void InitAllSingerList(
	const char *cFileName)
{
	DelNormalList(&gAllSingerList, SINGERINFO);

	UINT64 uTime1 = GetTickCount();

	LoadSingersFromFile(&gAllSingerList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get Total singer list %d, used %llu ms\n",
		gAllSingerList.GetCount(), uTime2-uTime1);
}

void InitAllSongList(
	const char *cFileName)
{
	DelNormalList(&gAllSongList, SONGINFO);

	UINT64 uTime1 = GetTickCount();

	LoadSongsFromFile(&gAllSongList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get Total song list %d, used %llu ms\n",
		gAllSongList.GetCount(), uTime2-uTime1);
}

void InitRankList(
	const char *cFileName,
	CPtrCtrl *pSongList)
{
	DelNormalList(pSongList, SONGINFO);

	UINT64 uTime1 = GetTickCount();

	LoadSongsFromFile(pSongList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get rank song list %d, used %llu ms\n",
		pSongList->GetCount(), uTime2-uTime1);
}

//add by yq,2013-03-15
int InitGetMaxSongId()
{
	int iMaxId = 0;
	int id = 0;
	int nCount = gAllSongList.GetCount();
	SONGINFO *pSongInfo = NULL;

	for (int i = 0 ; i < nCount ; i++ )
	{
		pSongInfo = (SONGINFO *)gAllSongList.GetAt(i);

		if (!pSongInfo)
		{
			continue;
		}
		id = atoi(pSongInfo->cID);
		iMaxId = (iMaxId > id) ? iMaxId : id;
	}

	return (iMaxId >= 99000001) ? iMaxId : 99000000;		//U盘加歌的ID必须大于99000000
}
//end add by yq,2013-03-15


void InitFastSearch()
{
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, "AllSingerList.dat");
	InitAllSingerList(cFileName);

	CombinePathName(cFileName, cDBFilePath, "AllSongList.dat");
	InitAllSongList(cFileName);

#if USE_NEW_GEN_RANK
	UINT64 uTime1 = GetTickCount();
	GenerateAllRankSongList();
	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_INFO, "GenerateAllRankSongList used %llu ms\n", (uTime2 - uTime1));
#else
	CombinePathName(cFileName, cDBFilePath, "GuoYuRankSongList.dat");
	InitRankList(cFileName, &gGuoYuRankSongList);

	CombinePathName(cFileName, cDBFilePath, "YueYuRankSongList.dat");
	InitRankList(cFileName, &gYueYuRankSongList);

	CombinePathName(cFileName, cDBFilePath, "TaiYuRankSongList.dat");
	InitRankList(cFileName, &gTaiYuRankSongList);

	CombinePathName(cFileName, cDBFilePath, "YingYuRankSongList.dat");
	InitRankList(cFileName, &gYingYuRankSongList);

	CombinePathName(cFileName, cDBFilePath, "RiYuRankSongList.dat");
	InitRankList(cFileName, &gRiYuRankSongList);

	CombinePathName(cFileName, cDBFilePath, "HanYuRankSongList.dat");
	InitRankList(cFileName, &gHanYuRankSongList);

	CombinePathName(cFileName, cDBFilePath, "NewSongSongList.dat");
	InitRankList(cFileName, &gNewSongRankSongList);

	CombinePathName(cFileName, cDBFilePath, "AllRankSongList.dat");
	InitRankList(cFileName, &gAllRankSongList);
#endif

	CombinePathName(cFileName, cDBFilePath, "PubBroadCastSongList.dat");
	InitPubBroadCastSongList(cFileName);

	CombinePathName(cFileName, cDBFilePath, "UserSongList.dat");
	InitUserSongList(cFileName);
                CombinePathName(cFileName, cDBFilePath, "DownloadedSongList.dat");
	InitDownloadedSongList(cFileName);
                CombinePathName(cFileName, cDBFilePath, "DownloadingSongList.dat");
	InitDownloadingSongList(cFileName);
	FastSearch_LoadFavoritSongList("");	

	g_iMaxSongId = InitGetMaxSongId();
}

void DeInitFastSearch()
{
	FastSearch_SaveAllList();
	DbgOutput(DBG_INFO, "FastSearch_SaveAllList.\n");

//	FreeSongListForLyrics(&gAllSongList);
//	FreeSongListForLyrics(&gFavoritSongList);
#if !USE_NEW_GEN_RANK
	FreeSongListForLyrics(&gGuoYuRankSongList);
	FreeSongListForLyrics(&gYueYuRankSongList);
	FreeSongListForLyrics(&gTaiYuRankSongList);
	FreeSongListForLyrics(&gYingYuRankSongList);
	FreeSongListForLyrics(&gRiYuRankSongList);
	FreeSongListForLyrics(&gHanYuRankSongList);
	FreeSongListForLyrics(&gNewSongRankSongList);
	FreeSongListForLyrics(&gAllRankSongList);
#endif
//	FreeSongListForLyrics(&gPubBroadCastSongList);
//	FreeSongListForLyrics(&gDownloadedSongList);
//	FreeSongListForLyrics(&gDownloadingSongList);
	DbgOutput(DBG_INFO, "FreeSongListForLyrics.\n");

	DelNormalList(&gAllSingerList, SINGERINFO);
	DelNormalList(&gAllSongList, SONGINFO);
	DelNormalList(&gFavoritSongList, SONGINFO);
#if USE_NEW_GEN_RANK
//	gGuoYuRankSongList.DeleteAll();
//	gYueYuRankSongList.DeleteAll();
//	gTaiYuRankSongList.DeleteAll();
//	gYingYuRankSongList.DeleteAll();
//	gRiYuRankSongList.DeleteAll();
//	gHanYuRankSongList.DeleteAll();
	//gNewSongRankSongList.DeleteAll();
	gAllRankSongList.DeleteAll();
#else
	DelNormalList(&gGuoYuRankSongList, SONGINFO);
	DelNormalList(&gYueYuRankSongList, SONGINFO);
	DelNormalList(&gTaiYuRankSongList, SONGINFO);
	DelNormalList(&gYingYuRankSongList, SONGINFO);
	DelNormalList(&gRiYuRankSongList, SONGINFO);
	DelNormalList(&gHanYuRankSongList, SONGINFO);
	DelNormalList(&gNewSongRankSongList, SONGINFO);
	DelNormalList(&gAllRankSongList, SONGINFO);
#endif

	DelNormalList(&gPubBroadCastSongList, SONGINFO);		//add by yq,2012-12-17
	DelNormalList(&gUserSongList, SONGINFO);
	DelNormalList(&gDownloadedSongList, SONGINFO);
	DelNormalList(&gDownloadingSongList, SONGINFO);
	DbgOutput(DBG_INFO, "DelNormalList.\n");
}

void DeInitFastSearchNotRankList()
{
	FastSearch_SaveAllListNotRank();
	DbgOutput(DBG_INFO, "FastSearch_SaveAllList.\n");

	//FreeSongList(&gAllSongList);
	//FreeSongList(&gFavoritSongList);
#if !USE_NEW_GEN_RANK
	FreeSongListForLyrics(&gGuoYuRankSongList);
	FreeSongListForLyrics(&gYueYuRankSongList);
	FreeSongListForLyrics(&gTaiYuRankSongList);
	FreeSongListForLyrics(&gYingYuRankSongList);
	FreeSongListForLyrics(&gRiYuRankSongList);
	FreeSongListForLyrics(&gHanYuRankSongList);
	FreeSongListForLyrics(&gNewSongRankSongList);
	FreeSongListForLyrics(&gAllRankSongList);
#endif
	FreeSongList(&gPubBroadCastSongList);
	DbgOutput(DBG_INFO, "FreeSongListForLyrics.\n");

	DelNormalList(&gAllSingerList, SINGERINFO);
	DelNormalList(&gAllSongList, SONGINFO);
	DelNormalList(&gFavoritSongList, SONGINFO);
#if USE_NEW_GEN_RANK
//	gGuoYuRankSongList.DeleteAll();
//	gYueYuRankSongList.DeleteAll();
//	gTaiYuRankSongList.DeleteAll();
//	gYingYuRankSongList.DeleteAll();
//	gRiYuRankSongList.DeleteAll();
//	gHanYuRankSongList.DeleteAll();
	//gNewSongRankSongList.DeleteAll();
	gAllRankSongList.DeleteAll();
#else
	DelNormalList(&gGuoYuRankSongList, SONGINFO);
	DelNormalList(&gYueYuRankSongList, SONGINFO);
	DelNormalList(&gTaiYuRankSongList, SONGINFO);
	DelNormalList(&gYingYuRankSongList, SONGINFO);
	DelNormalList(&gRiYuRankSongList, SONGINFO);
	DelNormalList(&gHanYuRankSongList, SONGINFO);
	DelNormalList(&gNewSongRankSongList, SONGINFO);
	DelNormalList(&gAllRankSongList, SONGINFO);
#endif

	DelNormalList(&gPubBroadCastSongList, SONGINFO);		//add by yq,2012-12-17
	DelNormalList(&gUserSongList, SONGINFO);
	DbgOutput(DBG_INFO, "DelNormalList.\n");
}

void DeInitFastSearchNotSaveList()
{
	DelNormalList(&gAllSingerList, SINGERINFO);
	DelNormalList(&gAllSongList, SONGINFO);
	DelNormalList(&gFavoritSongList, SONGINFO);

	gAllRankSongList.DeleteAll();

	DelNormalList(&gPubBroadCastSongList, SONGINFO);		//add by yq,2012-12-17
	DelNormalList(&gUserSongList, SONGINFO);
}

void FastSearch_SaveAllListNotRank()
{
	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, "AllSingerList.dat");
	SaveSingersToFile(&gAllSingerList, cFileName);

	CombinePathName(cFileName, cDBFilePath, "AllSongList.dat");
	SaveSongsToFile(&gAllSongList, cFileName);

	CombinePathName(cFileName, cDBFilePath, "PubBroadCastSongList.dat");
	SaveSongsToFile(&gPubBroadCastSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "UserSongList.dat");
	SaveSongsToFile(&gUserSongList,cFileName);

	system("sync");

	UINT64 uTime2 = GetTickCount();
	DbgOutput(DBG_DEBUG, "Save All song list %d, used %llu ms\n",
			gAllSongList.GetCount(), uTime2-uTime1);
}

const char* FastSearch_GetSingerType(
	const char * cSingerName)
{
	if (!cSingerName)
	{
		return NULL;
	}

	const char* cSingerType = "";

	UINT64 uTime1 = GetTickCount();

	int nCount = gAllSingerList.GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SINGERINFO *pSingerInfo = (SINGERINFO *)gAllSingerList.GetAt(i);

		if (!pSingerInfo)
		{
			continue;
		}

		if (strcasecmp(pSingerInfo->cName, cSingerName) == 0)
		{
			cSingerType = pSingerInfo->cType;
			break;
		}
	}

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Search singer type by memory(KYO), singer=%s, type=%s, used %d ms\n",
		cSingerName, cSingerType, uTime2-uTime1);

	return cSingerType;
}

void FastSearch_SearchSingerListBySingerType(
	const char *cSingerType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	if (!pResultList)
	{
		return;
	}

	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	if (!pSearchInfo)
	{
		pSearchInfo = &searchinfo;
	}

	const char* cContent = pSearchInfo->cInputContent;
	INPUTMETHODTYPE eInputMethodType = pSearchInfo->eInputMethodType;

	UINT64 uTime1 = GetTickCount();

	int nCount = gAllSingerList.GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SINGERINFO *pSingerInfo = (SINGERINFO *)gAllSingerList.GetAt(i);

		if (!pSingerInfo)
		{
			continue;
		}
	//	printf("%s.\n",pSingerInfo->cType);
		if (Singer_PrecompareBySingerType(cSingerType, pSingerInfo))
		{
			if (CMP_SINGER_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSingerInfo))
			{
				pResultList->AddData(pSingerInfo);
			}
		}
	}

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Search singer by memory(KYO), result count=%d, used %d ms\n",
		pResultList->GetCount(), uTime2-uTime1);
}

static void FastSearch_SearchSongListBySearchInfo(
	SONG_PRECOMPARE_FUNC pfnPrecompare,
	const void *pCompareData,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pSongList,
	CPtrCtrl *pResultList,
	BOOL bCloud)
{
	if (!pSongList || !pResultList)
	{
		return;
	}

	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	if (!pSearchInfo)
	{
		pSearchInfo = &searchinfo;
	}

	const char* cContent = pSearchInfo->cInputContent;
	INPUTMETHODTYPE eInputMethodType = pSearchInfo->eInputMethodType;
	WORDLENGTHTYPE eWordLengthType = pSearchInfo->eWordLengthType;

	UINT64 uTime1 = GetTickCount();

	int nCount = pSongList->GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)pSongList->GetAt(i);

		if (!pSongInfo)
		{
			continue;
		}

		if (!pfnPrecompare || pfnPrecompare(pCompareData, pSongInfo))
		{
			if (CMP_SONG_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSongInfo))
			{
				if (CMP_BY_WORDLENGTH(eWordLengthType, pSongInfo->wWdCt))
				{
					if ( bCloud && !pSongInfo->bExist )
						pResultList->AddData(pSongInfo);
					else if ( !bCloud && pSongInfo->bExist )
						pResultList->AddData(pSongInfo);
          else if ( !bCloud && !pSongInfo->bExist )
						pResultList->AddData(pSongInfo);
				}
			}
		}
	}

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Search song by memory(KYO), result count=%d, used %d ms\n",
		pResultList->GetCount(), uTime2-uTime1);
}


void FastSearch_SearchSongListByLyricContent(
	SONG_PRECOMPARE_LYRIC_FUNC pfnPrecompare,
	const char *pCompareData,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pSongList,
	CPtrCtrl *pResultList)
{
	if (!pSongList || !pResultList)
	{
		return;
	}

	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_LYRIC, WordLength_All};
	if (!pSearchInfo)
	{
		pSearchInfo = &searchinfo;
	}

//	const char* cContent = pSearchInfo->cInputContent;
	//DbgOutput(DBG_INFO,"cContent is %s \n", cContent);
	//INPUTMETHODTYPE eInputMethodType = pSearchInfo->eInputMethodType;
	WORDLENGTHTYPE eWordLengthType = pSearchInfo->eWordLengthType;

	UINT64 uTime1 = GetTickCount();
	//BOOL bIsNetConnect = CheckNetConnect();
	int nCount = pSongList->GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)pSongList->GetAt(i);

		if (!pSongInfo)
		{
			continue;
		}

		if (!pfnPrecompare || pfnPrecompare(pCompareData, pSongInfo))
		{
			//if (CMP_SONG_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSongInfo))
			{
				if (CMP_BY_WORDLENGTH(eWordLengthType, pSongInfo->wWdCt))
				{
#if HISI_NET_APP
					if ( pSongInfo->bExist )
						pResultList->AddData(pSongInfo);
#else
					if ( g_bIsNetConn )	// 网络存在，需要显示云加歌
						pResultList->AddData(pSongInfo);
					else if( pSongInfo->bExist )  // 网络不存在，不需要显示云加歌，
						pResultList->AddData(pSongInfo);
#endif
				}
			}
		}
	}

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Search song by memory(KYO), result count=%d, used %d ms\n",
		pResultList->GetCount(), uTime2-uTime1);
}

void FastSearch_SearchSongListBySongName(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		NULL,
		NULL,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchSongListBySingerName(
	const void *cSingerID,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		Song_PrecompareBySingerName,
		cSingerID,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchSongListByLanguageType(
	const void *cLanguage,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		Song_PrecompareByLanguageType,
		cLanguage,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchSongListBySongType(
	const void *cSongType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		Song_PrecompareBySongType,
		cSongType,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchSongListByCloud(
	const void *cCloudType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		Song_PrecompareByCloudType,
		cCloudType,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		TRUE);
}

void FastSearch_SearchSongListByRanking(
	const void *cRankingType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		NULL,
		cRankingType,
		pSearchInfo,
		&gAllRankSongList,
		pResultList,
		FALSE);
//	FastSearch_SearchSongListBySearchInfo(
//		Song_PrecompareByRankingType,
//		cRankingType,
//		pSearchInfo,
//		&gAllSongList,
//		pResultList,
//		FALSE);
}

void FastSearch_SearchSongListByLyric(
	const char *cLyric,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
//	FastSearch_SearchSongListByLyricContent(
//		Song_PrecompareByLyric,
//		cLyric,
//		pSearchInfo,
//		&gAllSongList,
//		pResultList);
}

void FastSearch_SearchNewSongList(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		Song_PrecompareByNewSong,
		NULL,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchGradeSongList(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		Song_PrecompareByGradeSong,
		NULL,
		pSearchInfo,
		&gAllSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchMovieSongList(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySongType(
		"电影",
		pSearchInfo,
		pResultList);
}

void FastSearch_SearchSongListByFavorit(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		NULL,
		NULL,
		pSearchInfo,
		&gFavoritSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchSongListByRank(
	const char *cRankType,
	CPtrCtrl *pResultList)
{
//	if (!pResultList)
//	{
//		return;
//	}
//
//	CPtrCtrl *pSongList = NULL;
//
//	if (strcmp(cRankType, "国语排行") == 0)
//	{
//		pSongList = &gGuoYuRankSongList;
//	}
//	else if (strcmp(cRankType, "粤语排行") == 0)
//	{
//		pSongList = &gYueYuRankSongList;
//	}
//	else if (strcmp(cRankType, "台语排行") == 0)
//	{
//		pSongList = &gTaiYuRankSongList;
//	}
//	else if (strcmp(cRankType, "英语排行") == 0)
//	{
//		pSongList = &gYingYuRankSongList;
//	}
//	else if (strcmp(cRankType, "日语排行") == 0)
//	{
//		pSongList = &gRiYuRankSongList;
//	}
//	else if (strcmp(cRankType, "韩语排行") == 0)
//	{
//		pSongList = &gHanYuRankSongList;
//	}
////	else if (strcmp(cRankType, "新歌排行") == 0)
////	{
////		pSongList = &gNewSongRankSongList;
////	}
//	else if (strcmp(cRankType, "总排行") == 0)
//	{
//		pSongList = &gAllRankSongList;
//	}
//
//	pResultList->CopyPtrCtrl(pSongList);
}

void FastSearch_SearchSongListByRank(
	const char *cRankType,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pReSultList)
{
//	if (!pReSultList)
//	{
//		return;
//	}
//
//	if (strcmp(cRankType, "国语排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gGuoYuRankSongList,
//			pReSultList);
//	}
//	else if (strcmp(cRankType, "粤语排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gYueYuRankSongList,
//			pReSultList);
//	}
//	else if (strcmp(cRankType, "台语排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gTaiYuRankSongList,
//			pReSultList);
//	}
//	else if (strcmp(cRankType, "英语排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gYingYuRankSongList,
//			pReSultList);
//	}
//	else if (strcmp(cRankType, "日语排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gRiYuRankSongList,
//			pReSultList);
//	}
//	else if (strcmp(cRankType, "韩语排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gHanYuRankSongList,
//			pReSultList);
//	}
////	else if (strcmp(cRankType, "新歌排行") == 0)
////	{
////		FastSearch_SearchSongListBySearchInfo(
////			NULL,
////			NULL,
////			pSearchInfo,
////			&gNewSongRankSongList,
////			pReSultList);
////	}
//	else if (strcmp(cRankType, "总排行") == 0)
//	{
//		FastSearch_SearchSongListBySearchInfo(
//			NULL,
//			NULL,
//			pSearchInfo,
//			&gAllRankSongList,
//			pReSultList);
//	}
	//printf("FastSearch_SearchSongListByRank cRankType=%s, the count:%d\n",cRankType, pReSultList->GetCount());
}

int FastSearch_AddSongToFavorit(
	const char *cFavoritID,
	const char *cSongID)
{
	if (!cFavoritID || !cSongID)
	{
		return 0;
	}

	int nIndex = gFavoritSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
	if (nIndex >= 0)
	{
		return 0;
	}

	nIndex = gAllSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
	if (nIndex < 0)
	{
		return 0;
	}

	SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(nIndex);
	if (!pSong)
	{
		return 0;
	}

	SONGINFO *pNewSong = new SONGINFO;
	if (!pNewSong)
	{
		return 0;
	}

	memcpy(pNewSong, pSong, sizeof(SONGINFO));

	UINT64 uTime1 = GetTickCount();

	gFavoritSongList.AddData(pNewSong);

	FastSearch_SaveFavoritSongList(cFavoritID);

	DbgOutput(DBG_DEBUG, "function %s, favoriteID=%s, song id=%s, used %llu ms\n",
		__FUNCTION__, cFavoritID, pSong->cID, GetTickCount()-uTime1);

	return 0;
}

void FastSearch_DelSongFromFavorit(
	const char *cFavoritID,
	const char *cSongID)
{
	if (!cSongID || !cFavoritID)
	{
		return;
	}

	int nIndex = gFavoritSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
	if (nIndex < 0)
	{
		return;
	}

	SONGINFO *pSong = (SONGINFO *)gFavoritSongList.DeleteAt(nIndex);

	if (pSong)
	{
		delete pSong;
	}
}

void FastSearch_LoadFavoritSongList(
	const char *cFavoritID)
{
	DelNormalList(&gFavoritSongList, SONGINFO);

	UINT64 uTime1 = GetTickCount();

	char cFileShortName[MAX_PATH];
	sprintf(cFileShortName, "%s_Favorit.dat", cFavoritID);

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, cFileShortName);
	LoadSongsFromFile(&gFavoritSongList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get favorit song list %d, used %llu ms\n",
		gFavoritSongList.GetCount(), uTime2-uTime1);
}

void FastSearch_SaveFavoritSongList(
	const char *cFavoritID)
{
	UINT64 uTime1 = GetTickCount();

	char cFileShortName[MAX_PATH];
	sprintf(cFileShortName, "%s_Favorit.dat", cFavoritID);

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, cFileShortName);
	SaveSongsToFile(&gFavoritSongList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Save favorit song list %d, used %llu ms\n",
		gFavoritSongList.GetCount(), uTime2-uTime1);
}

//add by yq,2012-12-17
void FastSearch_LoadSelectedList(CPtrListCtrl *pList)
{
	if(!pList)
	{
		return ;
	}

	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, "SelectedList.dat");

	DelNormalList(pList, SONGINFO);
	LoadSongsFromFile(pList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get Total selected Song list %d, used %llu ms\n",
			pList->GetCount(), uTime2-uTime1);
}

void FastSearch_SaveSelectedList(CPtrListCtrl *pList)
{
	if(!pList)
	{
		return ;
	}

	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, "SelectedList.dat");
	SaveSongsToFile(pList, cFileName);

	UINT64 uTime2 = GetTickCount();
	DbgOutput(DBG_DEBUG, "Save selected song list %d, used %llu ms\n",
			pList->GetCount(), uTime2-uTime1);
}

void FastSearch_LoadCloudList(CPtrListCtrl *pList)
{
	if(!pList)
	{
		return ;
	}

	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, "CloudList.dat");

	DelNormalList(pList, SONGINFO);
	LoadSongsFromFile(pList, cFileName);

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Get Total selected Song list %d, used %llu ms\n",
			pList->GetCount(), uTime2-uTime1);
}

void FastSearch_SaveCloudList(CPtrListCtrl *pList)
{
	if(!pList)
	{
		return ;
	}

	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, "CloudList.dat");
	SaveSongsToFile(pList, cFileName);

	UINT64 uTime2 = GetTickCount();
	DbgOutput(DBG_DEBUG, "Save selected song list %d, used %llu ms\n",
			pList->GetCount(), uTime2-uTime1);
}

void FastSearch_SaveAllList()
{
	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, "AllSingerList.dat");
	SaveSingersToFile(&gAllSingerList, cFileName);

	CombinePathName(cFileName, cDBFilePath, "AllSongList.dat");
	SaveSongsToFile(&gAllSongList, cFileName);
#if !USE_NEW_GEN_RANK
	CombinePathName(cFileName, cDBFilePath, "GuoYuRankSongList.dat");
	SaveSongListToFile( &gGuoYuRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "YueYuRankSongList.dat");
	SaveSongListToFile(&gYueYuRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "TaiYuRankSongList.dat");
	SaveSongListToFile(&gTaiYuRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "YingYuRankSongList.dat");
	SaveSongListToFile( &gYingYuRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "RiYuRankSongList.dat");
	SaveSongListToFile( &gRiYuRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "HanYuRankSongList.dat");
	SaveSongListToFile(&gHanYuRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "NewSongSongList.dat");
	SaveSongListToFile( &gNewSongRankSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "AllRankSongList.dat");
	SaveSongListToFile(&gAllRankSongList,cFileName);
#endif

	CombinePathName(cFileName, cDBFilePath, "PubBroadCastSongList.dat");
	SaveSongsToFile(&gPubBroadCastSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "UserSongList.dat");
	SaveSongsToFile(&gUserSongList,cFileName);
	CombinePathName(cFileName, cDBFilePath, "DownloadedSongList.dat");
	SaveSongsToFile(&gDownloadedSongList,cFileName);

	CombinePathName(cFileName, cDBFilePath, "DownloadingSongList.dat");
	SaveSongsToFile(&gDownloadingSongList,cFileName);

	system("sync");

	UINT64 uTime2 = GetTickCount();
	DbgOutput(DBG_DEBUG, "Save All song list %d, used %llu ms\n",
			gAllSongList.GetCount(), uTime2-uTime1);
}

void FastSearch_DeleteListBySongInfo(SONGINFO *pSong)
{
//	CPtrArrayCtrl *pTempList = NULL;
//	if (!pSong)
//	{
//		return;
//	}
//	SONGINFO *pDeleteSong = NULL;
//	int idx = -1;
//
//	idx = gPubBroadCastSongList.FindFirst(pSong->cID, FastSearch_FindSongInfoByID);
//	if(idx > -1)
//	{
//		pDeleteSong = (SONGINFO *)gPubBroadCastSongList.DeleteAt(idx);
//		if ( pDeleteSong )
//			delete pDeleteSong;
//	}
//
//	idx = gAllRankSongList.FindFirst(pSong->cID, FastSearch_FindSongInfoByID);
//	if(idx > -1)
//	{
//		pDeleteSong = (SONGINFO *)gAllRankSongList.DeleteAt(idx);
//#if !USE_NEW_GEN_RANK
//		if ( pDeleteSong )
//			delete pDeleteSong;
//#endif
//	}
//
////	if(pSong->bNewSong)
////	{
////		pTempList = &gNewSongRankSongList;
////		idx = pTempList->FindFirst(pSong->cID, FastSearch_FindSongInfoByID);
////		if(idx > -1)
////		{
////			pDeleteSong = (SONGINFO *)pTempList->DeleteAt(idx);
////#if !USE_NEW_GEN_RANK
////			if ( pDeleteSong )
////				delete pDeleteSong;
////#endif
////		}
////	}
//
//	if (strcmp(pSong->cLang, "国语") == 0)
//	{
//		pTempList = &gGuoYuRankSongList;
//	}
//	else if (strcmp(pSong->cLang, "粤语") == 0)
//	{
//		pTempList = &gYueYuRankSongList;
//	}
//	else if (strcmp(pSong->cLang, "台语") == 0)
//	{
//		pTempList = &gTaiYuRankSongList;
//	}
//	else if (strcmp(pSong->cLang, "英语") == 0)
//	{
//		pTempList = &gYingYuRankSongList;
//	}
//	else if (strcmp(pSong->cLang, "日语") == 0)
//	{
//		pTempList = &gRiYuRankSongList;
//	}
//	else if (strcmp(pSong->cLang, "韩语") == 0)
//	{
//		pTempList = &gHanYuRankSongList;
//	}
//
//	if ( pTempList )
//	{
//		idx = pTempList->FindFirst(pSong->cID, FastSearch_FindSongInfoByID);
//		if(idx > -1)
//		{
//			pDeleteSong = (SONGINFO *)pTempList->DeleteAt(idx);
//#if !USE_NEW_GEN_RANK
//			if ( pDeleteSong )
//				delete pDeleteSong;
//#endif
//		}
//	}
//
//	idx = gAllSongList.FindFirst(pSong->cID, FastSearch_FindSongInfoByID);
//	if(idx > -1)
//	{
//		pDeleteSong = (SONGINFO *)gAllSongList.DeleteAt(idx);
//		if ( pDeleteSong )
//			delete pDeleteSong;
//	}
}

int FastSearch_GetSongCount()
{
	return gAllSongList.GetCount();
}

int FastSearch_GetLocalSongCount()
{
	int cnt = 0;
	for ( int i = 0; i < gAllSongList.GetCount(); i++ )
	{
		SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(i);
		if ( pSong && pSong->bExist )
			cnt++;
	}
	return cnt;
}

int FastSearch_GetCloudSongCount()
{
	int cnt = 0;
	for ( int i = 0; i < gAllSongList.GetCount(); i++ )
	{
		SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(i);
		if ( pSong && !pSong->bExist )
			cnt++;
	}
	return cnt;
}

int FastSearch_GetPubBroadCastSongCount()
{
	return gPubBroadCastSongList.GetCount();
}

int FastSearch_GetNewSongCount()
{
	//return gNewSongRankSongList.GetCount();
	int iNewSongCount = 0;
	int iCount = gAllSongList.GetCount();
	int i = 0;
	for ( i = 0; i < iCount; ++i )
	{
		SONGINFO *pSongInfo = (SONGINFO *)gAllSongList.GetAt(i);
		if ( !pSongInfo )
			continue;
		if ( pSongInfo->bNewSong )
			iNewSongCount++;
	}
	return iNewSongCount;
}

SONGINFO *FastSearch_GetPubBroadCastSongByIdx(int idx)
{
	SONGINFO *pSongInfo = (SONGINFO *)gPubBroadCastSongList.GetAt(idx);
	return pSongInfo;
}

void FastSearch_SearchPubBroadCastSongList(
		const SEARCHINFO *pSearchInfo,
		CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		NULL,
		NULL,
		pSearchInfo,
		&gPubBroadCastSongList,
		pResultList,
		FALSE);
}
void FastSearch_SearchDownloadedSongList(
		const SEARCHINFO *pSearchInfo,
		CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		NULL,
		NULL,
		pSearchInfo,
		&gDownloadedSongList,
		pResultList,
		FALSE);
}

void FastSearch_SearchDownloadingSongList(
		const SEARCHINFO *pSearchInfo,
		CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfo(
		NULL,
		NULL,
		pSearchInfo,
		&gDownloadingSongList,
		pResultList,
		FALSE);
}

void FastSearch_SavePubBroadCast()
{
	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, "PubBroadCastSongList.dat");
	SaveSongsToFile(&gPubBroadCastSongList, cFileName);
	system("sync");

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_INFO, "Save PubBroadCast song list %d, used %llu ms\n",
			gPubBroadCastSongList.GetCount(), uTime2-uTime1);
}
void FastSearch_SaveDownloadedSong()
{
	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
        char cFileName[MAX_PATH];

        CombinePathName(cFileName, cDBFilePath, "DownloadedSongList.dat");
        SaveSongsToFile(&gDownloadedSongList, cFileName);
        system("sync");

        UINT64 uTime2 = GetTickCount();

        DbgOutput(DBG_INFO, "Save PubBroadCast song list %d, used %llu ms\n",
                        gDownloadedSongList.GetCount(), uTime2-uTime1);

}

void FastSearch_SaveDownloadingSong()
{
	UINT64 uTime1 = GetTickCount();

	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
        char cFileName[MAX_PATH];

        CombinePathName(cFileName, cDBFilePath, "DownloadingSongList.dat");
        SaveSongsToFile(&gDownloadingSongList, cFileName);
        system("sync");

        UINT64 uTime2 = GetTickCount();

        DbgOutput(DBG_INFO, "Save PubBroadCast song list %d, used %llu ms\n",
                        gDownloadingSongList.GetCount(), uTime2-uTime1);

}
void FastSearch_SavePubBroadCastSongList(CPtrListCtrl *pResultList)
{
	if (!pResultList)
	{
		return;
	}
	DelNormalList(&gPubBroadCastSongList, SONGINFO);
	int nCount = pResultList->GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)pResultList->GetAt(i);

		if ((!pSongInfo) || !pSongInfo->bPlaying)
		{
			continue;
		}
		SONGINFO *pNewSongInfo = new SONGINFO;
		if (!pNewSongInfo)
		{
			return;
		}
		memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));

		gPubBroadCastSongList.AddData(pNewSongInfo);
	}

	FastSearch_SavePubBroadCast();
}
int FastSearch_AddPubSong(SONGINFO *pSongInfo)
{
	if ( pSongInfo )
	{
		int nIndex = gPubBroadCastSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
		if ( nIndex != -1 )
		{
			// already exist
			return 1;
		}

		SONGINFO *pNewSongInfo = new SONGINFO;
		if ( pNewSongInfo )
		{
			memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));
			gPubBroadCastSongList.AddData(pNewSongInfo);
			return 0;
		}
	}
	return -1;
}

int FastSearch_FindSongInfoByIDFromFavList(char *songid,SONGINFO *pSongInfo){
	if(NULL == songid ||NULL == pSongInfo)
        	return -1;
	int nIndex = gFavoritSongList.FindFirst(songid, FastSearch_FindSongInfoByID);
	SONGINFO *pSong = (SONGINFO *)gFavoritSongList.GetAt(nIndex);
	if(pSong){
                memcpy((char *)pSongInfo, (char *)pSong, sizeof(SONGINFO));
                return nIndex;
        }
        return -1;
}

int FastSearch_FindSongInfoByIDFromPubList(char *songid,SONGINFO *pSongInfo){
	if(NULL == songid ||NULL == pSongInfo)
        	return -1;
	int nIndex = gPubBroadCastSongList.FindFirst(songid, FastSearch_FindSongInfoByID);
	SONGINFO *pSong = (SONGINFO *)gPubBroadCastSongList.GetAt(nIndex);
	if(pSong){
		memcpy((char *)pSongInfo, (char *)pSong, sizeof(SONGINFO));
                return nIndex;
	}
	return -1;
}

void FastSearch_AddDownloadedSong(SONGINFO *pSongInfo){
	if(pSongInfo){
        	int nIndex = gDownloadedSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
        	if (nIndex != -1)
        	{
                	// already exist
                	return ;
        	}
		SONGINFO *pNewSongInfo = new SONGINFO;
                if (pNewSongInfo)
                {
			memcpy(pNewSongInfo,pSongInfo,sizeof(SONGINFO));
			gDownloadedSongList.AddData(pNewSongInfo,0);
			FastSearch_SaveDownloadedSong();
		}
	}
}

void FastSearch_AddDownloadingSong(SONGINFO *pSongInfo){
	if(pSongInfo){
		int nIndex = gDownloadingSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
                if (nIndex != -1)
                {
                        // already exist
                        return ;
                }
		SONGINFO *pNewSongInfo = new SONGINFO;
               if (pNewSongInfo)
               {
			memcpy(pNewSongInfo,pSongInfo,sizeof(SONGINFO));
			gDownloadingSongList.AddData(pNewSongInfo);
			FastSearch_SaveDownloadingSong();
		}
	}
}

void FastSearch_DelPubSong(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){

		int nIndex = gPubBroadCastSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
        	if (nIndex >= 0)
        	{
			SONGINFO *pSong = (SONGINFO *)gPubBroadCastSongList.DeleteAt(nIndex);
                        if (pSong)
                        {
                        	delete pSong;
                            FastSearch_SavePubBroadCast();
                        }
                	return;
        	}

	}
}

void FastSearch_DelDownloadedSong(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){

		int nIndex = gDownloadedSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
        	if (nIndex >= 0)
        	{
			SONGINFO *pSong = (SONGINFO *)gDownloadedSongList.DeleteAt(nIndex);
                        if (pSong)
                        {
                        	delete pSong;
                            	FastSearch_SaveDownloadedSong();
                        }
                	return;
        	}

	}
}

void FastSearch_DelDownloadingSong(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){

		int nIndex = gDownloadingSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
        	if (nIndex >= 0)
        	{
			SONGINFO *pSong = (SONGINFO *)gDownloadingSongList.DeleteAt(nIndex);
                        if (pSong)
                        {
                            delete pSong;
                            FastSearch_SaveDownloadingSong();
                        }
                	return;
        	}

	}
}

void FastSearch_MoveSongTop(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){
                int nIndex = gAllSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
                if(nIndex >= 0){
                        gAllSongList.MoveData(nIndex, 1, 0);
                }

        }
}

void FastSearch_MovePubSongTop(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){
		int nIndex = gPubBroadCastSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
		if(nIndex >= 0){
			gPubBroadCastSongList.MoveData(nIndex, 1, 0);
			FastSearch_SavePubBroadCast();
		}

	}
}

void FastSearch_MoveDownloadedSongTop(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){
		int nIndex = gDownloadedSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
		if(nIndex >= 0){
			gDownloadedSongList.MoveData(nIndex, 1, 0);
			FastSearch_SaveDownloadedSong();
		}

	}
}

void FastSearch_MoveDownloadingSongTop(char *cSongID){
	if(cSongID!=NULL && strlen(cSongID)>0){
		int nIndex = gDownloadingSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
		if(nIndex >= 0){
			gDownloadingSongList.MoveData(nIndex, 1, 1);
			//FastSearch_SaveDownloadingSong();
		}

	}
}

void FastSearch_MoveFavSongTop(char *cSongID){
        if(cSongID!=NULL && strlen(cSongID)>0){
                int nIndex = gFavoritSongList.FindFirst(cSongID, FastSearch_FindSongInfoByID);
                if(nIndex >= 0){
                        gFavoritSongList.MoveData(nIndex, 1, 0);
                        FastSearch_SaveFavoritSongList("");
                }

        }
}



void FastSearch_SaveNewSong()
{
//	UINT64 uTime1 = GetTickCount();
//
//	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
//	char cFileName[MAX_PATH];
//
//	CombinePathName(cFileName, cDBFilePath, "NewSongSongList.dat");
//	SaveSongListToFile(&gNewSongRankSongList, cFileName);
//	system("sync");
//
//	UINT64 uTime2 = GetTickCount();
//
//	DbgOutput(DBG_INFO, "Save New song list %d, used %llu ms\n",
//			gNewSongRankSongList.GetCount(), uTime2-uTime1);
}

void FastSearch_SaveNewSongList(CPtrListCtrl *pResultList)
{
	if (!pResultList)
	{
		return;
	}

	int idx;
	int nCount = gAllSongList.GetCount();
	int i;
	SONGINFO *pSongInfo = NULL;
	SONGINFO *pAllSonInfo = NULL;
#if USE_NEW_GEN_RANK
	//gNewSongRankSongList.DeleteAll();
#else
	DelNormalList(&gNewSongRankSongList, SONGINFO);
#endif
	nCount = pResultList->GetCount();

	for ( i = 0 ; i < nCount ; i++ )
	{
		pSongInfo = (SONGINFO *)pResultList->GetAt(i);
		if (!pSongInfo)
			continue;

		idx = gAllSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
		pAllSonInfo = (SONGINFO *)gAllSongList.GetAt(idx);
		if (!pAllSonInfo)
		{
			continue;
		}

		if(pSongInfo->bPlaying)			//选择该首歌曲为新歌
		{
			pSongInfo->bNewSong = TRUE;
			pAllSonInfo->bNewSong = TRUE;

#if USE_NEW_GEN_RANK
			//gNewSongRankSongList.AddData(pAllSonInfo);
#else
			SONGINFO *pNewSongInfo = new SONGINFO;
			if (!pNewSongInfo)
			{
				return;
			}
			memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));
			pNewSongInfo->nLyricsLen = 0;
			gNewSongRankSongList.AddData(pNewSongInfo);
#endif
		}
		else
		{
			pAllSonInfo->bNewSong = FALSE;
		}
	}
	//FastSearch_SaveNewSong();
	FastSearch_SaveAllList();
}

void FastSearch_DeleteSongList(CPtrListCtrl *pResultList)
{
	char delCmd[128];

	if (!pResultList)
	{
		return;
	}

	int nCount;
	int i;
	SONGINFO *pSongInfo = NULL;
	char cSongRealPath[256] = {0};

	nCount = pResultList->GetCount();
	for ( i = 0 ; i < nCount ; i++ )
	{
		pSongInfo = (SONGINFO *)pResultList->GetAt(i);

		if ((!pSongInfo) || !pSongInfo->bPlaying)		//没有选择
		{
			continue;
		}
		ConvertLink2Path(cSongRealPath, pSongInfo->cFilePath, sizeof(cSongRealPath));
		sprintf(delCmd,"rm -rf %s", cSongRealPath);
		//sprintf(delCmd,"rm -rf %s",pSongInfo->cFilePath);
		pid_t status = system(delCmd);
		if((status == -1) || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
		{
			DbgOutput(DBG_INFO,"cp file error, exit code: %d\n", WEXITSTATUS(status));
			continue;
		}

		sqlite3_delete_song(pSongInfo);

		if ( pSongInfo->bCloud )
		{
			// 云加歌的歌曲，不需要删除记录
			//pSongInfo->bExist = 0;
			FastSet_SongInfoExistValue(pSongInfo, FALSE);
		}
		else
		{
			// 用户加的歌曲， 需要删除记录
			FastSearch_DeleteListBySongInfo(pSongInfo);
		}

		//FastDelete_UserSongList(pSongInfo);
	}

	FastSearch_SaveAllList();
}

void FastAdd_SongList(const SONGINFO *pAddSongInfo)
{
	if (!pAddSongInfo)
	{
		return;
	}

	SONGINFO *pNewSongInfo = new SONGINFO;
	if (!pNewSongInfo)
	{
		return;
	}
	memcpy(pNewSongInfo, pAddSongInfo, sizeof(SONGINFO));

	int idx = gAllSongList.FindFirst(pAddSongInfo->cID, FastSearch_FindSongInfoByID);
	if(idx == -1)
	{
		gAllSongList.AddData(pNewSongInfo);
	}

//	idx = gAllRankSongList.FindFirst(pAddSongInfo->cID, FastSearch_FindSongInfoByID);
//	if(idx == -1)
//	{
//		gAllRankSongList.AddData(pNewSongInfo);
//	}

//	if(pAddSongInfo->bNewSong)
//	{
//		gNewSongRankSongList.AddData(pNewSongInfo);
//	}

	//FastAdd_UserSongList(pAddSongInfo);
}
//end add by yq,2012-12-17

int FastSearch_GetFavoritSongCount()
{
	return gFavoritSongList.GetCount();
}

int FastSearch_GetMaxSongID()
{
	return g_iMaxSongId;
}
void FastSearch_IncreaseMaxSongID(int iStep)
{
	g_iMaxSongId += iStep;
}

void FastSearch_GetAllSongList(
		CPtrCtrl *pResultList)
{
	if (!pResultList)
	{
		return;
	}

	CPtrCtrl *pSongList = NULL;
	pSongList = &gAllSongList;
	pResultList->CopyPtrCtrl(pSongList);
}

void FastSearch_GetPublicSongList(
		CPtrCtrl *pResultList)
{
	if (!pResultList)
	{
		return;
	}

	CPtrCtrl *pSongList = NULL;
	pSongList = &gPubBroadCastSongList;
	pResultList->CopyPtrCtrl(pSongList);
}

int FastSearch_GetSongInfoByID(char *id,
		SONGINFO *pSongInfo)
{
	if(NULL == id ||
		NULL == pSongInfo)
		return -1;
	int idx = gAllSongList.FindFirst(id, FastSearch_FindSongInfoByID);
	SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(idx);
	if(pSong)
	{
		memcpy((char *)pSongInfo, (char *)pSong, sizeof(SONGINFO));
		return 0;
	}
	else
		printf("#####Err: the song[%s] not find.", id);
	return -1;
}

//增加歌曲的点击率
void FastAdd_SongOrderTimes(const SONGINFO *pSongInfo)
{
	if(NULL == pSongInfo)
	{
		printf("#####Err: the parameter of FastAdd_SongOrderTimes is null.\n");
		return ;
	}
	FastAdd_SongOrderTimes(pSongInfo->cID);
//	int idx = gAllSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
//	SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(idx);
//	if(pSong)
//		pSong->wOrderTimes++;
}

void FastAdd_SongOrderTimes(const char *cID)
{
	if ( !cID )
		return ;
	int idx = gAllSongList.FindFirst(cID, FastSearch_FindSongInfoByID);
	SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(idx);
	if(pSong)
	{
		pSong->wOrderTimes++;

	//	printf("============Inc Order Timers %s, %s orderTimes:%d\n", pSong->cID, pSong->cName, pSong->wOrderTimes);
	}
}

//增加歌手的点击率
void FastAdd_SingerOrderTimes(const SINGERINFO *pSingerInfo)
{
//	if(NULL == pSingerInfo)
//	{
//		printf("#####Err: the parameter of FastAdd_SingerOrderTimes is null.\n");
//		return ;
//	}
//
//	int idx = gAllSingerList.FindFirst(&pSingerInfo->id, FastSearch_FindSingerInfoByID);
//	SINGERINFO *pSinger = (SINGERINFO *)gAllSingerList.GetAt(idx);
//	if(pSinger)
//		pSinger->wOrderTimes++;
}

void FastSave_AllSongList()
{
	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
	char cFileName[MAX_PATH];

	CombinePathName(cFileName, cDBFilePath, FILEDATA_SONGS);
	SaveSongsToFile(&gAllSongList, cFileName);

	system("sync");
}

void FastAdd_UserSongList(const SONGINFO *pSongInfo)
{
	if (!pSongInfo)
	{
		return;
	}

	SONGINFO *pNewSongInfo = new SONGINFO;
	if (!pNewSongInfo)
	{
		return;
	}
	memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));

	int idx = gUserSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
	if(idx == -1)
	{
		gUserSongList.AddData(pNewSongInfo);
	}
}

void FastDelete_UserSongList(const SONGINFO *pSongInfo)
{
	if ( !pSongInfo )
		return ;

	int idx = gUserSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
	if(idx > -1)
	{
		SONGINFO *pDeleteSong = (SONGINFO *)gUserSongList.DeleteAt(idx);
		if ( pDeleteSong )
			delete pDeleteSong;
	}
}

void FastModify_CloudSongExist(SONGINFO *pSongInfo)
{
	if ( !pSongInfo )
		return ;
	SetExistValue(pSongInfo, 1);

	FastSet_SongInfoExistValue(pSongInfo, TRUE);

	FastSave_AllSongList();

//	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
//	char cFileName[MAX_PATH];
//
//	CombinePathName(cFileName, cDBFilePath, "AllSongList.dat");
//	SaveSongsToFile(&gAllSongList, cFileName);
}


void FastSet_SongInfoExistValue(SONGINFO *pSongInfo, BOOL bExist)
{
	if ( !pSongInfo )
		return;

	BOOL exist = bExist;
	pSongInfo->bExist = exist;
	int idx = gAllSongList.FindFirst(pSongInfo->cID, FastSearch_FindSongInfoByID);
	SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(idx);
	if( pSong )
	{
		strcpy(pSong->cFilePath, pSongInfo->cFilePath);
		pSong->bExist = exist;
	}
}

void FastSearch_SearchSongListBySearchInfoNoCloud(
	SONG_PRECOMPARE_FUNC pfnPrecompare,
	const void *pCompareData,
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pSongList,
	CPtrCtrl *pResultList)
{
	if (!pSongList || !pResultList)
	{
		return;
	}

	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	if (!pSearchInfo)
	{
		pSearchInfo = &searchinfo;
	}

	const char* cContent = pSearchInfo->cInputContent;
	INPUTMETHODTYPE eInputMethodType = pSearchInfo->eInputMethodType;
	WORDLENGTHTYPE eWordLengthType = pSearchInfo->eWordLengthType;

	UINT64 uTime1 = GetTickCount();

	int nCount = pSongList->GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)pSongList->GetAt(i);

		if (!pSongInfo)
		{
			continue;
		}

		if (!pfnPrecompare || pfnPrecompare(pCompareData, pSongInfo))
		{
			if (CMP_SONG_BY_INPUTMETHODTYPE(cContent, eInputMethodType, pSongInfo))
			{
				if (CMP_BY_WORDLENGTH(eWordLengthType, pSongInfo->wWdCt))
				{
					if( pSongInfo->bExist )
						pResultList->AddData(pSongInfo);
				}
			}
		}
	}

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Search song by memory(KYO), result count=%d, used %d ms\n",
		pResultList->GetCount(), uTime2-uTime1);
}

void FastSearch_SearchSongListBySongNameNoCloud(
	const SEARCHINFO *pSearchInfo,
	CPtrCtrl *pResultList)
{
	FastSearch_SearchSongListBySearchInfoNoCloud(
		NULL,
		NULL,
		pSearchInfo,
		&gAllSongList,
		pResultList);
}


#if USE_NEW_GEN_RANK
static void AddSongToRankList(const SONGINFO *pSongInfo, CPtrArrayCtrl *pRankSongList)
{
	if ( !pSongInfo || !pRankSongList )
		return;

	int iCount = pRankSongList->GetCount();
	int i = 0;

	for ( i = 0; i < iCount; ++i )
	{
		SONGINFO *pRankSong = (SONGINFO *)pRankSongList->GetAt(i);
		if ( !pRankSong )
			continue;

		if ( pSongInfo->wOrderTimes > pRankSong->wOrderTimes )
		{
			pRankSongList->AddData(pSongInfo, i);

			if ( iCount >= RANK_MAX_COUNT )
			{
				// 需要把最后一个也是点击率最小的删除，确保该链表只有 RANK_MAX_COUNT 个
				pRankSongList->DeleteTail();
			}

			break;
		}
	}

	if ( iCount == 0 || ((i == iCount) && (i < RANK_MAX_COUNT)) )
	{
		pRankSongList->AddData(pSongInfo);
	}
}

void GenerateAllRankSongList()
{
	int iCount = gAllSongList.GetCount();
	for ( int  i = 0; i < iCount; ++i )
	{
		const SONGINFO *pSongInfo = (const SONGINFO *)gAllSongList.GetAt(i);
		if ( !pSongInfo )
			continue;

    
		AddSongToRankList(pSongInfo, &gAllRankSongList);
	}
}

void AfreshAllRankSongListforClick()
{
  int RankCount = 0;
  int j = 0;
  RankCount = CClientSharing::mRangItemsList[0].RankCount;
  DbgOutput(DBG_ERR, "RankCount = %d \n", RankCount);
	for ( int  i = 0; i < RankCount; ++i )
	{
    
    int nIndex = gAllSongList.FindFirst(CClientSharing::mRangItemsList[i].songid, FastSearch_FindSongInfoByID);
    
  	if (nIndex >= 0)
  	{
            
  		SONGINFO *pSongInfo = (SONGINFO *)gAllSongList.GetAt(nIndex);
  		if ( !pSongInfo)
  		{
       
      DbgOutput(DBG_ERR, "pSongInfo = NUll \n");
			continue;
  		}
      pSongInfo->wOrderTimes = CClientSharing::mRangItemsList[i].click;

    // DbgOutput(DBG_ERR, "pSongInfo = %s time =%d id =%s\n", pSongInfo->cName, pSongInfo->wOrderTimes, pSongInfo->cID);

    gAllRankSongList.AddData(pSongInfo, j);
    j++;
    
  	}
	}
}

#endif

void deleteLeastOrdersSongs(int cnt)
{
	CPtrArrayCtrl delSongsList;
	int iCount = gAllSongList.GetCount();
	int iDelCount = 0;
	UINT64 uTime1 = GetTickCount();

	for ( int i = (iCount-1); i >= 0; i-- )
	{
		const SONGINFO *pSongInfo = (const SONGINFO *)gAllSongList.GetAt(i);
		if ( !pSongInfo || !pSongInfo->bExist )
			continue;
		iDelCount = delSongsList.GetCount();
		int j = 0;
		for ( j = 0; j < iDelCount; j++ )
		{
			SONGINFO *pDelSong = (SONGINFO *)delSongsList.GetAt(j);
			if ( !pDelSong )
				continue;

			if ( pSongInfo->wOrderTimes < pDelSong->wOrderTimes )
			{
				delSongsList.AddData(pSongInfo, j);

				if ( iDelCount >= cnt )
				{
					// 需要把最后一个也是点击率最小的删除，确保该链表只有 RANK_MAX_COUNT 个
					delSongsList.DeleteTail();
				}

				break;
			}
		}
		if ( j == 0 || ((j == iDelCount) && (j < cnt)) )
		{
			delSongsList.AddData(pSongInfo);
		}
	}

	char delCmd[256] = {0};
	char cSongRealPath[256] = {0};
	iDelCount = delSongsList.GetCount();
	for ( int i = 0; i < iDelCount; i++ )
	{
		SONGINFO *pDelSong = (SONGINFO *)delSongsList.GetAt(i);
		if ( !pDelSong )
			continue;

		pid_t status = 0;
		ConvertLink2Path(cSongRealPath, pDelSong->cFilePath, sizeof(cSongRealPath));
		snprintf(delCmd, sizeof(delCmd), "rm -rf %s", cSongRealPath);
		//snprintf(delCmd, sizeof(delCmd), "rm -rf %s", pDelSong->cFilePath);
		status = system(delCmd);
		if ( (status == -1) || !WIFEXITED(status) || WEXITSTATUS(status) != 0 )
		{
			DbgOutput(DBG_INFO,"cmd:%s error, exit code: %d\n", delCmd, WEXITSTATUS(status));
			continue;
		}
		//DbgOutput(DBG_INFO, "id=%s, name=%s\n", pDelSong->cID, pDelSong->cName);
		sqlite3_delete_song(pDelSong);
		if ( pDelSong->bCloud )
			FastSet_SongInfoExistValue(pDelSong, FALSE);
		else
		{
			FastSearch_DeleteListBySongInfo(pDelSong);
		}
	}

	UINT64 uTime2 = GetTickCount();
	DbgOutput(DBG_INFO, "Delete songs result count=%d, used %d ms\n", delSongsList.GetCount(), uTime2-uTime1);
}

// 保存歌曲列表为新歌
void SaveSongListToNewSong(CPtrListCtrl *pSongList)
{
	if ( !pSongList ) return;

	SONGINFO *pSrcSong = NULL, *pDstSong = NULL;
	int songCnt = pSongList->GetCount();

	for ( int i = 0; i < songCnt; i++ )
	{
		pSrcSong = (SONGINFO *)(pSongList->GetAt(i));
		if ( !pSrcSong ) continue;

		int nIndex = gAllSongList.FindFirst(pSrcSong->cID, FastSearch_FindSongInfoByID);
		if ( nIndex < 0 )
		{
			DbgOutput(DBG_WARN, "the song id:%s is not in all songs\n", pSrcSong->cID);
			continue;
		}

		pDstSong = (SONGINFO *)gAllSongList.GetAt(nIndex);
		if ( pDstSong )
		{
			pDstSong->bNewSong = true;
		}
	}

	FastSave_AllSongList();
}

// 保存歌曲列表为公播
void SaveSongListToPublicSong(CPtrListCtrl *pSongList)
{
	if ( !pSongList ) return ;

	SONGINFO *pSrcSong = NULL;
	int songCnt = pSongList->GetCount();

	DelNormalList(&gPubBroadCastSongList, SONGINFO);
	for ( int i = 0; i < songCnt; i++ )
	{
		pSrcSong = (SONGINFO *)(pSongList->GetAt(i));
		if ( !pSrcSong ) continue;

		int nIndex = gPubBroadCastSongList.FindFirst(pSrcSong->cID, FastSearch_FindSongInfoByID);
		if ( nIndex < 0 )
		{
			SONGINFO *pNewSong = new SONGINFO;
			if ( pNewSong )
			{
				memcpy(pNewSong, pSrcSong, sizeof(SONGINFO));
				gPubBroadCastSongList.AddData(pNewSong);
			}
		}
	}

	FastSearch_SavePubBroadCast();
}
char *FastSearch_GetLanguageTypeNameByID(int id)
{
	CPtrListCtrl *pList = NULL;
	pList = &gSearchLanguageType.sChildItemList;
	int count = pList->GetCount();
	for(int i=0;i<count;i++)
	{
		SEARCHITEM *pSubItem = (SEARCHITEM *)pList->GetAt(i);
		//printf("cType =%s\n",pSubItem->cItemName);
		if(pSubItem->nDepth==id){
			return pSubItem->cItemName;
		}
	}
	return NULL;
}

