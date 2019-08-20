#include "SearchSQL.h"
#include "BaseApp.h"
#include "Song.h"
#include "../GlobalUIClass.h"
#include "DataFileOp.h"
#include "CSqlite3.h"

SEARCHITEM gSearchSpell;
SEARCHITEM gSearchSingerType;
SEARCHITEM gSearchSongType;
SEARCHITEM gSearchLanguageType;
SEARCHITEM gSearchRankType;
SEARCHITEM gSearchAlbumType;
SEARCHITEM gSearchMonthType;

#define NEED_SEARCH_TYPE		1

void DelAllSubSearchItem(SEARCHITEM *pParent)
{
	if (!pParent)
	{
		return;
	}

	int i, nCount;

	nCount = pParent->sChildItemList.GetCount();
	for ( i = 0 ; i < nCount ; i++ )
	{
		SEARCHITEM *pSubItem = (SEARCHITEM *)pParent->sChildItemList.GetAt(i);

		if (pSubItem)
		{
			DelAllSubSearchItem(pSubItem);

			delete pSubItem;
		}
	}

	pParent->sChildItemList.DeleteAll();
}

SEARCHITEM * FindSubSearchItem(SEARCHITEM *pParent, const char *cSubItemName)
{
	if (strcmp(pParent->cItemName, cSubItemName) == 0)
	{
		return pParent;
	}

	int nCount = pParent->sChildItemList.GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SEARCHITEM *pSubItem = (SEARCHITEM *)pParent->sChildItemList.GetAt(i);

		if (pSubItem)
		{
			SEARCHITEM *pSubSubItem = FindSubSearchItem(pSubItem, cSubItemName);
			if (pSubSubItem)
			{
				return pSubSubItem;
			}
		}
	}

	return NULL;
}

int GetSearchItemIndex(SEARCHITEM *pSearchItem, const char *cItemName)
{
	int nCount = pSearchItem->sChildItemList.GetCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		SEARCHITEM *pSubItem = (SEARCHITEM *)pSearchItem->sChildItemList.GetAt(i);

		if (pSubItem)
		{
			if (strcmp(pSubItem->cItemName, cItemName) == 0)
			{
				return i;
			}
		}
	}

	return -1;
}

SEARCHITEM * AddSubSearchItem(SEARCHITEM *pParent, const char *cSubItemName, int itemId)
{
	if (!pParent)
	{
		return NULL;
	}

	SEARCHITEM *pSubItem = new SEARCHITEM;

	if (!pSubItem)
	{
		return NULL;
	}

	pSubItem->eType = pParent->eType;
	pSubItem->nDepth = itemId;
	strcpy(pSubItem->cItemName, cSubItemName);
	pSubItem->pParentItem = pParent;

	pParent->sChildItemList.AddData(pSubItem);

	return pSubItem;
}

void InitPinyinType()
{
	gSearchSpell.eType = SearchByName;
	gSearchSpell.nDepth = 0;
	strcpy(gSearchSpell.cItemName, "拼音点歌");
	gSearchSpell.pParentItem = NULL;
#if  NEED_SEARCH_TYPE
	const char *song[] =
	{
			"全部",
			"一字",
			"二字",
			"三字",
			"四字",
			"五字",
			"六字",
	};

	int i ;
	for ( i = 0 ; i < 7 ; i++ )
	{
		char *cType = new char[64];
		if (!cType)
		{
			continue;
		}
		strcpy(cType, song[i]);

		AddSubSearchItem(&gSearchSpell, cType, i);
	}
#endif
}

void InitSingerType()
{
	gSearchSingerType.eType = SearchBySinger;
	gSearchSingerType.nDepth = 0;
	strcpy(gSearchSingerType.cItemName, "歌星点歌");
	gSearchSingerType.pParentItem = NULL;
#if NEED_SEARCH_TYPE
	UINT64 uTime1 = GetTickCount();

	CPtrArrayCtrl sTypesList;
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();

	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, "SingerTypes.dat");
	LoadTypesFromFile(&sTypesList, cFileName);

	int nCount = sTypesList.GetCount();
	int i ;
	for ( i = 0 ; i < nCount ; i++ )
	{
		TYPEINFO * cType = (TYPEINFO *)sTypesList.GetAt(i);
		AddSubSearchItem(&gSearchSingerType, cType->name, cType->id);
	}

	DelArrayList(&sTypesList, TYPEINFO);
	DbgOutput(DBG_DEBUG, "function %s, used %llu ms\n",
		__FUNCTION__, GetTickCount()-uTime1);
#endif
}

void InitSongType()
{
	gSearchSongType.eType = SearchBySongType;
	gSearchSongType.nDepth = 0;
	strcpy(gSearchSongType.cItemName, "曲种点歌");
	gSearchSongType.pParentItem = NULL;
#if NEED_SEARCH_TYPE
	UINT64 uTime1 = GetTickCount();

	CPtrArrayCtrl sTypesList;
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();

	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, "SongTypes.dat");
	LoadTypesFromFile(&sTypesList, cFileName);

	int nCount = sTypesList.GetCount();
	int i ;
	for ( i = 0 ; i < nCount ; i++ )
	{
		TYPEINFO * cType = (TYPEINFO *)sTypesList.GetAt(i);
		if ( strncasecmp(cType->name, "所有", strlen("所有")) == 0 )
			cType->id = 0;
		AddSubSearchItem(&gSearchSongType, cType->name, cType->id);
	}

	DelArrayList(&sTypesList, TYPEINFO);

	DbgOutput(DBG_DEBUG, "function %s, used %llu ms\n",
		__FUNCTION__, GetTickCount()-uTime1);
#endif
}

void InitLanguageType()
{
	gSearchLanguageType.eType = SearchByLanguageType;
	gSearchLanguageType.nDepth = 0;
	strcpy(gSearchLanguageType.cItemName, "语种点歌");
	gSearchLanguageType.pParentItem = NULL;
#if NEED_SEARCH_TYPE
	UINT64 uTime1 = GetTickCount();

	CPtrArrayCtrl sTypesList;
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();

	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, "LanguageTypes.dat");
	LoadTypesFromFile(&sTypesList, cFileName);

	int nCount = sTypesList.GetCount();
	int i ;
	for ( i = 0 ; i < nCount ; i++ )
	{
		TYPEINFO * cType = (TYPEINFO *)sTypesList.GetAt(i);
		if ( strncasecmp(cType->name, "所有", strlen("所有")) == 0 )
			cType->id = 0;
		AddSubSearchItem(&gSearchLanguageType, cType->name, cType->id);
	}

	DelArrayList(&sTypesList, TYPEINFO);

	DbgOutput(DBG_DEBUG, "function %s, used %llu ms\n",
		__FUNCTION__, GetTickCount()-uTime1);
#endif
}

void InitRankType()
{
	gSearchRankType.eType = SearchByRank;
	gSearchRankType.nDepth = 0;
	strcpy(gSearchRankType.cItemName, "排行点歌");
	gSearchRankType.pParentItem = NULL;
#if NEED_SEARCH_TYPE
	UINT64 uTime1 = GetTickCount();

	CPtrArrayCtrl sTypesList;
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();

	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, "RankTypes.dat");
	LoadTypesFromFile(&sTypesList, cFileName);

	int nCount = sTypesList.GetCount();
	int i ;

	AddSubSearchItem(&gSearchRankType, "所有排行", 0);

	for ( i = 0 ; i < nCount ; i++ )
	{
		TYPEINFO* cType = (TYPEINFO *)sTypesList.GetAt(i);
		AddSubSearchItem(&gSearchRankType, cType->name, cType->id);
	}

	DelArrayList(&sTypesList, TYPEINFO);

	DbgOutput(DBG_DEBUG, "function %s, used %llu ms\n",
		__FUNCTION__, GetTickCount()-uTime1);
#endif
}

void InitMonthType()
{
	gSearchMonthType.eType = SearchByMonth;
	gSearchMonthType.nDepth = 0;
	strcpy(gSearchMonthType.cItemName, "云端");
	gSearchMonthType.pParentItem = NULL;

	UINT64 uTime1 = GetTickCount();

	CPtrArrayCtrl sTypesList;
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();

	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, FILEDATA_MONTHTYPE);
	LoadTypesFromFile(&sTypesList, cFileName);

	int nCount = sTypesList.GetCount();
	int i ;

	AddSubSearchItem(&gSearchMonthType, "所有云端", 0);

	for ( i = 0 ; i < nCount ; i++ )
	{
		TYPEINFO* cType = (TYPEINFO *)sTypesList.GetAt(i);
		AddSubSearchItem(&gSearchMonthType, cType->name, cType->id);
	}

	DelArrayList(&sTypesList, TYPEINFO);

	DbgOutput(DBG_DEBUG, "function %s, used %llu ms\n",
		__FUNCTION__, GetTickCount()-uTime1);
}
void InitAlbumType()
{
	gSearchAlbumType.eType = SearchByAlbum;
	gSearchAlbumType.nDepth = 0;
	strcpy(gSearchAlbumType.cItemName, "专题精选");
	gSearchAlbumType.pParentItem = NULL;
#if NEED_SEARCH_TYPE
	UINT64 uTime1 = GetTickCount();

	CPtrArrayCtrl sTypesList;
	const char* cDBFilePath;
	cDBFilePath = theBaseApp->GetDataBaseFolder();

	char cFileName[MAX_PATH];
	CombinePathName(cFileName, cDBFilePath, FILEDATA_ALBUMTYPE);
	LoadTypesFromFile(&sTypesList, cFileName);

	int nCount = sTypesList.GetCount();
	int i ;
	for ( i = 0 ; i < nCount ; i++ )
	{
		TYPEINFO* cType = (TYPEINFO *)sTypesList.GetAt(i);
		AddSubSearchItem(&gSearchAlbumType, cType->name, cType->id);
	}

	DelArrayList(&sTypesList, TYPEINFO);

	DbgOutput(DBG_DEBUG, "function %s, used %llu ms\n",
		__FUNCTION__, GetTickCount()-uTime1);
#endif
}

void InitSQL()
{		
	InitPinyinType();
	InitSingerType();
	InitSongType();
	InitLanguageType();
	InitRankType();
	InitMonthType();
	InitAlbumType();

	InitFastSearch();

}

void DeinitSQL()
{
	DelAllSubSearchItem(&gSearchSpell);
	DelAllSubSearchItem(&gSearchSingerType);
	DelAllSubSearchItem(&gSearchSongType);
	DelAllSubSearchItem(&gSearchLanguageType);
	DelAllSubSearchItem(&gSearchRankType);
	DelAllSubSearchItem(&gSearchAlbumType);
	DelAllSubSearchItem(&gSearchMonthType);

	DbgOutput(DBG_INFO, "DelAllSubSearchItem.\n");

	//DeInitFastSearch();
	DbgOutput(DBG_INFO, "DeInitFastSearch.\n");

	DbgOutput(DBG_INFO, "function %s leave\n", __FUNCTION__);
}

void DeinitSQLNotSaveList()
{
	DelAllSubSearchItem(&gSearchSpell);
	DelAllSubSearchItem(&gSearchSingerType);
	DelAllSubSearchItem(&gSearchSongType);
	DelAllSubSearchItem(&gSearchLanguageType);
	DelAllSubSearchItem(&gSearchRankType);
	DelAllSubSearchItem(&gSearchAlbumType);
	DelAllSubSearchItem(&gSearchMonthType);

	DeInitFastSearchNotSaveList();

	DbgOutput(DBG_INFO, "function %s leave\n", __FUNCTION__);
}

void DeinitSQLNotSaveRankList()
{
	DelAllSubSearchItem(&gSearchSpell);
	DelAllSubSearchItem(&gSearchSingerType);
	DelAllSubSearchItem(&gSearchSongType);
	DelAllSubSearchItem(&gSearchLanguageType);
	DelAllSubSearchItem(&gSearchRankType);
	DelAllSubSearchItem(&gSearchAlbumType);
	DelAllSubSearchItem(&gSearchMonthType);

	DbgOutput(DBG_INFO, "DelAllSubSearchItem.\n");

	DeInitFastSearchNotRankList();
	DbgOutput(DBG_INFO, "DeInitFastSearch.\n");

	DbgOutput(DBG_INFO, "function %s leave\n", __FUNCTION__);
}

SEARCHITEM* GetSingerTypeSearchItem(const char* cSingerName)
{
	SEARCHITEM* pSearchItem = NULL;

	const char* cSingerType = FastSearch_GetSingerType(cSingerName);
	if (cSingerType && cSingerType[0] != '\0')
	{
		pSearchItem = FindSubSearchItem(&gSearchSingerType, cSingerType);
	}

	return pSearchItem;
}

void FreeSingerList(CPtrCtrl *pSingerList)
{
	DelNormalList(pSingerList, SINGERINFO);
}

void FreeSongList(CPtrCtrl *pSongList)
{
	DelNormalList(pSongList, SONGINFO);
}


//char* gvErrMsg = NULL;
//sqlite3* gvdb = NULL;
//#define SAFESET(x) (((x) == NULL) ? " " : (x))

int ConnectDb(
		char *pStrDbName)
{
	int ret = 0;
	return ret;
//
//	if(NULL == pStrDbName)
//	{
//		DbgOutput(DBG_DEBUG, "function %s failed\n", __FUNCTION__);
//		return -1;
//	}
//
//	ret = sqlite3_open(pStrDbName, &gvdb);
//	if(ret != SQLITE_OK)
//	{
//		DbgOutput(DBG_DEBUG, "open %s database failed\n", pStrDbName);
//		ret = -1;
//	}
//	return ret;
}

int DisconnectDb()
{
//	if(gvdb != NULL)
//	{
//		sqlite3_close(gvdb);
//		gvdb = NULL;
//	}
	return 0;
}

int Sqlite3_insert_song(SONGINFO *pSongInfo)
{
	if ( pSongInfo )
		CCSqlite3::Sqlite3_InsertSong(pSongInfo);
	return 0;
//	int ret = 0;
//	char buf[1024];
//
//	if(pSongInfo == NULL)
//	{
//		DbgOutput(DBG_DEBUG, "function %s failed\n", __FUNCTION__);
//		return -1;
//	}
//
//	memset(buf,0,sizeof(buf));
//	sprintf(buf,"insert into songs (songbm, songname, zs, singer, songtype, lang, servername,path, filename, spell, volume, maxvol, minvol, musictrack, chours, newsong, stroke, brightness,saturation,contrast, ordertimes,mediainfo, songnamelang,singernamelang,cloud, exist)"
//	                       "values (\"%s\",  \"%s\",  %d,  \"%s\",  \"%s\", \"%s\",  \"%s\", \"%s\", \"%s\", \"%s\",  %d,     %d,     %d,        %d,      %d,      %d,    \"%s\",    500,      500,      500,        %d,       \"3\",     \"国语\",      \"国语\",     %d,   %d)",
//								SAFESET(pSongInfo->cID),
//								SAFESET(pSongInfo->cName),
//								pSongInfo->wWdCt,
//								SAFESET(pSongInfo->cSinger),
//								SAFESET(pSongInfo->cType),
//								SAFESET(pSongInfo->cLang),
//								SAFESET(pSongInfo->cServerName),
//								SAFESET(pSongInfo->cFilePath),
//								SAFESET(pSongInfo->cFileName),
//								SAFESET(pSongInfo->cSpell),
//								pSongInfo->wVol,
//								pSongInfo->wMaxVol,
//								pSongInfo->wMinVol,
//								pSongInfo->wTrack,
//								pSongInfo->bChos,
//								pSongInfo->bNewSong,
//								SAFESET(pSongInfo->cStroke),
//								pSongInfo->wOrderTimes,
//								pSongInfo->bCloud,
//								pSongInfo->bExist);
//	printf("%s\n",buf);
//	ret = sqlite3_exec(gvdb,buf,NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//	{
//		ret = -1;
//		printf("\t\t###Err:insert into database failed: %s\n", gvErrMsg);
//	}
//	else
//		printf("\t\t###:insert into database success\n");
//	return ret;
}

int sqlite3_delete_song(SONGINFO *pSongInfo)
{
	return 0;
//	int ret = -1;
//	if ( pSongInfo != NULL )
//	{
//		char strDeleteCmd[512] = {0};
//		sprintf(strDeleteCmd,"update songs set EXIST=0 where songbm='%s'", pSongInfo->cID);
//		DbgOutput(DBG_DEBUG, "%s.\n",strDeleteCmd);
//		ret = sqlite3_exec(gvdb, strDeleteCmd, NULL, NULL, &gvErrMsg);
//		if ( ret == SQLITE_OK )
//			ret = 0;
//		else
//			DbgOutput(DBG_ERR, "the delete song failed: %s.\n", gvErrMsg);
//	}
//	return ret;
}

int Commit()
{
	return 0;
//	int ret = 0;
//	if(gvdb == NULL)
//	{
//		DbgOutput(DBG_DEBUG, "Commit %s failed\n", __FUNCTION__);
//		return -1;
//	}
//
//	ret = sqlite3_exec(gvdb,"commit transaction",NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//	{
//		DbgOutput(DBG_DEBUG, "Commit %s failed\n", __FUNCTION__);
//		ret = -1;
//	}
//	return ret;
}

int BeginTransaction()
{
	return 0;
//	if(gvdb == NULL)
//	{
//		printf("the db is not open.\n");
//		return -1;
//	}
//
//	int ret = sqlite3_exec(gvdb,"begin transaction",NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//	{
//		printf("begin transaction failed: %s\n",gvErrMsg);
//		return -1;
//	}
//	return 0;
}

int DeleteView()
{
	return 0;
//	if(gvdb == NULL)
//	{
//		printf("the db is not open.\n");
//		return -1;
//	}
//
//	int ret = sqlite3_exec(gvdb,"drop view if exists RegSongs",NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//	{
//		printf("\t\t##Err: delete regsong view failed: %s\n",gvErrMsg);
//		return -1;
//	}
	return 0;
}

int CreateView()
{
	return 0;
//	if(gvdb == NULL)
//	{
//		printf("the db is not open.\n");
//		return -1;
//	}
//
//	int ret = sqlite3_exec(gvdb,"create view RegSongs as select * from songs where exist=1",NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//	{
//		printf("\t\t##Err: begin transaction failed: %s\n",gvErrMsg);
//		return -1;
//	}
}

//更新歌星的点击率
int Sql_UpdateSingerOrderTimes()
{
	return 0;
//	char sqlcmd[128] = {0};
//
//	int count = gAllSingerList.GetCount();
//	for(int i = 0; i < count; i++)
//	{
//		SINGERINFO *pSinger = (SINGERINFO *)gAllSingerList.GetAt(i);
//		if(!pSinger)
//			continue;
//
//		snprintf(sqlcmd, sizeof(sqlcmd)-1, "update singers set ordertimes=%d where id=\'%d\'", pSinger->wOrderTimes, atoi(pSinger->cID));
//		int ret = sqlite3_exec(gvdb,sqlcmd,NULL,NULL,&gvErrMsg);
//		if(ret != SQLITE_OK)
//		{
//			printf("Sql_UpdateSingerOrderTimes failed: %s\n",gvErrMsg);
//			return -1;
//		}
//	}
//	return 0;
}

//更新歌曲的点击率
int Sql_UpdateSongOrderTimes()
{
	return 0;
//	char sqlcmd[128] = {0};
//
//	int count = gAllSongList.GetCount();
//	for(int i = 0; i < count; i++)
//	{
//		SONGINFO *pSong = (SONGINFO *)gAllSongList.GetAt(i);
//		if(!pSong)
//			continue;
//
//		snprintf(sqlcmd, sizeof(sqlcmd)-1, "update songs set ordertimes=%d where songbm=\'%s\'", pSong->wOrderTimes, pSong->cID);
//		int ret = sqlite3_exec(gvdb,sqlcmd,NULL,NULL,&gvErrMsg);
//		if(ret != SQLITE_OK)
//		{
//			printf("Sql_UpdateSongOrderTimes failed: %s\n",gvErrMsg);
//			return -1;
//		}
//	}
//	return 0;
}

//更新歌星和歌曲点击率
void UpdateOrderTimes()
{
//	char dbName[] = "./KTV_Data/database/vod.db";
//	if(ConnectDb(dbName) == -1)
//	{
//		DbgOutput(DBG_VERB,"open database :%s failed\n", dbName);
//		return ;
//	}
//	BeginTransaction();
//	Sql_UpdateSingerOrderTimes();
//	Sql_UpdateSongOrderTimes();
//	Commit();
//	DisconnectDb();
}

void Sql_UpdateSingerOrderTimes(const SINGERINFO *pSingerInfo)
{
//	char sqlcmd[128] = {0};
//	if(NULL == pSingerInfo)
//	{
//		DbgOutput(DBG_VERB,"Sql_UpdateSingerOrderTimes failed\n");
//		return ;
//	}
//	snprintf(sqlcmd, sizeof(sqlcmd)-1, "update singers set ordertimes=ordertimes+1 where id=\'%d\'", atoi(pSingerInfo->cID));
//	int ret = sqlite3_exec(gvdb,sqlcmd,NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//		printf("Sql_UpdateSingerOrderTimes failed: %s\n",gvErrMsg);
}

void Sql_UpdateSongOrderTimes(const SONGINFO *pSongInfo)
{
//	char sqlcmd[128] = {0};
//	if(NULL == pSongInfo)
//	{
//		DbgOutput(DBG_VERB,"Sql_UpdateSingerOrderTimes failed\n");
//		return ;
//	}
//	snprintf(sqlcmd, sizeof(sqlcmd)-1, "update songs set ordertimes=ordertimes+1 where songbm=\'%s\'", pSongInfo->cID);
//	DbgOutput(DBG_INFO, "the sql cmd:%s. the ordertimes:%d.\n", sqlcmd, pSongInfo->wOrderTimes);
//	int ret = sqlite3_exec(gvdb,sqlcmd,NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//		printf("Sql_UpdateSongOrderTimes failed: %s\n",gvErrMsg);
}

int GetSongCount_callback(void *arg, int argc, char **argv, char **cname)
{
	if(arg == NULL)
		return 0;

	int *count = (int *)arg;
	if(argc == 1)
		*count = atoi(argv[0]);
	else
		*count = 0;
	return 0;
}

int GetSongCountBySongInfo(const SONGINFO *pSongInfo)
{
	return 0;
//	int ret = 0;
//	int count = 0;
//	char cmd[256] = {0};
//	if(NULL == pSongInfo)
//	{
//		fprintf(stderr, "the parameter of GerSongCountBySingerName is null.\n");
//		return -1;
//	}
//
//	snprintf(cmd, sizeof(cmd) - 1, "select count(*) from regsongs where songbm=\"%s\"", pSongInfo->cID);
//	ret = sqlite3_exec(gvdb, cmd, GetSongCount_callback, &count, &gvErrMsg);
//	if(ret != SQLITE_OK)
//	{
//		DbgOutput(DBG_INFO, "GetSongCountBySongInfo: %s",gvErrMsg);
//		return -1;
//	}
//	return count;
}

void SetExistValue(const SONGINFO *pSongInfo, int iValue)
{
//	char sqlcmd[128] = {0};
//	if(NULL == pSongInfo)
//	{
//		DbgOutput(DBG_VERB,"SetExistValue failed\n");
//		return ;
//	}
//	snprintf(sqlcmd, sizeof(sqlcmd)-1, "update songs set exist = %d where songbm=\'%s\'",  iValue, pSongInfo->cID);
//	DbgOutput(DBG_INFO, "the sql cmd:%s.\n", sqlcmd);
//	if ( !gvdb )
//	{
//		char strDbName[MAX_PATH] = {0};
//		char cAppFolder[MAX_PATH];
//		GetProgramPath(cAppFolder, MAX_PATH);
//
//		sprintf(strDbName, "%sKTV_Data/database/vod.db", cAppFolder);
//		if(ConnectDb(strDbName) == -1)
//		{
//			DbgOutput(DBG_ERR,"open database :%s failed\n", strDbName);
//			return ;
//		}
//	}
//	int ret = sqlite3_exec(gvdb,sqlcmd,NULL,NULL,&gvErrMsg);
//	if(ret != SQLITE_OK)
//		printf("SetExistValue failed: %s\n",gvErrMsg);
}

