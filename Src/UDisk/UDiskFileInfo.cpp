#include "UDiskFileInfo.h"
#include <dirent.h>
#include "DbgOutput.h"
#include "commdef.h"
#include <string.h>
#include <BaseString.h>
#include "../SQL/SearchSQL.h"
#include "CommApi.h"

#define SEPARATOR_SIGN	'_'

static void FilterFileName(const char *fileName, UDISK_FILE_INFO_S *fileInfo)
{
	if ( !fileName || !fileInfo )  return ;
	enum nameType
	{
		type_singer=0,
		type_song,
		type_lang,
		type_type,
		type_track,
		type_spell,
		type_count,
	};

	char *ptrType[type_count] = {0};
	char tmpFileName[256] = {0};

	SAFE_STRNCPY(tmpFileName, fileName, sizeof(tmpFileName));
	char *ptrStart = tmpFileName, *ptrEnd = NULL;
 
  DbgOutput(DBG_WARN, "tmpFileName = %s\n", tmpFileName);
	for ( int i = 0; i < type_count; i++ )
	{
		char ch = (i == type_spell) ? '.' : SEPARATOR_SIGN;
		ptrEnd = strchr(ptrStart, ch);
		if ( ptrEnd == NULL )
			break;

		*ptrEnd = '\0';
		ptrType[i] = ptrStart;
		ptrStart = ptrEnd + 1;
    
	}

	if ( ptrType[type_singer] ) SAFE_STRNCPY(fileInfo->cSinger, ptrType[type_singer], sizeof(fileInfo->cSinger));
	if ( ptrType[type_song] ) SAFE_STRNCPY(fileInfo->cSongName, ptrType[type_song], sizeof(fileInfo->cSongName));
	if ( ptrType[type_lang] ) SAFE_STRNCPY(fileInfo->cLang, ptrType[type_lang], sizeof(fileInfo->cLang));
	if ( ptrType[type_type] ) SAFE_STRNCPY(fileInfo->cType, ptrType[type_type], sizeof(fileInfo->cType));
	fileInfo->wTrack = ptrType[type_track] ? atoi(ptrType[type_track]) : 0;
	if ( ptrType[type_spell] ) SAFE_STRNCPY(fileInfo->cSpell, ptrType[type_spell], sizeof(fileInfo->cSpell));
	CBaseStringW wscSong;
	wscSong.Set(fileInfo->cSongName);
	fileInfo->wWdCt = wscSong.GetLength();
  
  DbgOutput(DBG_WARN, "cSinger = %s\n", fileInfo->cSinger);
  DbgOutput(DBG_WARN, "cSongName = %s\n", fileInfo->cSongName);
  DbgOutput(DBG_WARN, "cType = %s\n", fileInfo->cLang);
  DbgOutput(DBG_WARN, "cLang = %s\n", fileInfo->cType);
  DbgOutput(DBG_WARN, "cSpell = %s\n", fileInfo->cSpell);
  
}

void FindFileByPath(CPtrCtrl *list, const char *path, int depth)
{
	if ( !list || !path )  return ;

	DIR *pDir = NULL;
	struct dirent *file = NULL;
  
	if ( (pDir = opendir(path)) == NULL )
	{
		DbgOutput(DBG_INFO,"open dirent:%s failed\n", path);
		return;
	}

	while( (file = readdir(pDir)) != NULL )
	{
		if ( strncmp(file->d_name,".",1) == 0 )
			continue;
		else if ( file->d_type == DT_DIR )
		{
			if ( depth >= 1 )
				continue;
			char tmpPath[256] = {0};
			snprintf(tmpPath, sizeof(tmpPath), "%s/%s", path, file->d_name);
			FindFileByPath(list, tmpPath, 1);
			continue;
		}

		if( (strcasestr(file->d_name,".mp4") == NULL) 	&&
			(strcasestr(file->d_name,".avi") == NULL) 	&&
			(strcasestr(file->d_name,".mov") == NULL) 	&&
			(strcasestr(file->d_name,".mkv") == NULL) 	&&
			(strcasestr(file->d_name,".asf") == NULL) 	&&
			(strcasestr(file->d_name,".wmv") == NULL) 	&&
			(strcasestr(file->d_name,".mpg") == NULL) 	&&
			(strcasestr(file->d_name,".vob") == NULL) 	&&
			(strcasestr(file->d_name,".ts") == NULL) 	&&
			(strcasestr(file->d_name,".f4v") == NULL) 	&&
			(strcasestr(file->d_name,".flv") == NULL) 	&&
			(strcasestr(file->d_name,".dvd") == NULL) 	&&
			(strcasestr(file->d_name,".svcd") == NULL) 	&&
			(strcasestr(file->d_name,".vcd") == NULL) 	&&
			(strcasestr(file->d_name,".ac3") == NULL) 	&&
			(strcasestr(file->d_name,".m4a") == NULL) 	&&
			(strcasestr(file->d_name,".mka") == NULL) 	&&
			(strcasestr(file->d_name,".mp2") == NULL) 	&&
			(strcasestr(file->d_name,".mp3") == NULL) 	&&
			(strcasestr(file->d_name,".wav") == NULL) 	&&
			(strcasestr(file->d_name,".wma") == NULL) 	&&
			(strcasestr(file->d_name,".m2ts") == NULL) 	&&
			(strcasestr(file->d_name,".aac") == NULL) )
		{
			continue;
		}

		printf("==the file name:%s\n", file->d_name);

		UDISK_FILE_INFO_S *pNewFile = new UDISK_FILE_INFO_S;
		if ( !pNewFile )
		{
			DbgOutput(DBG_INFO, "function %s:%u failed\n", __FUNCTION__, __LINE__);
			break ;
		}

		memset(pNewFile, 0x0, sizeof(UDISK_FILE_INFO_S));

		snprintf(pNewFile->cFileName, sizeof(pNewFile->cFileName), "%s", file->d_name);
		snprintf(pNewFile->cFilePath, sizeof(pNewFile->cFilePath), "%s/%s", path, file->d_name);
		FilterFileName(pNewFile->cFileName, pNewFile);

		list->AddData(pNewFile);
	}
	closedir(pDir);
}

// 根据语种名获取id
int GetLangIdxByName(const char *pLangName, BOOL bListPos)
{
	if ( !pLangName ) return -1;

	int count = gSearchLanguageType.sChildItemList.GetCount();
	for ( int i = 0; i < count;i++ )
	{
		SEARCHITEM *pTypeInfo = (SEARCHITEM *)gSearchLanguageType.sChildItemList.GetAt(i);
		if ( pTypeInfo && (strcmp(pLangName, pTypeInfo->cItemName) == 0) )
		{
			return bListPos ? i : pTypeInfo->nDepth;
		}
	}
	return -1;
}

const char *GetLangNameByIdx(int idx)
{
	if ( idx < 0 ) return NULL;

	int count = gSearchLanguageType.sChildItemList.GetCount();
	if ( idx >= count )	return NULL;
	SEARCHITEM *pTypeInfo = (SEARCHITEM *)gSearchLanguageType.sChildItemList.GetAt(idx);
	return pTypeInfo ? pTypeInfo->cItemName : NULL;
}

// 根据分类名获取id
int GetTypeIdxByName(const char *pTypeName, BOOL bListPos)
{
	if ( !pTypeName ) return -1;

	int count = gSearchSongType.sChildItemList.GetCount();
	for ( int i = 0; i < count;i++ )
	{
		SEARCHITEM *pTypeInfo = (SEARCHITEM *)gSearchSongType.sChildItemList.GetAt(i);
		if ( pTypeInfo && (strcmp(pTypeName, pTypeInfo->cItemName) == 0) )
		{
			return bListPos ? i : pTypeInfo->nDepth;
		}
	}
	return -1;
}

const char *GetTypeNameByIdx(int idx)
{
	if ( idx < 0 ) return NULL;

	int count = gSearchSongType.sChildItemList.GetCount();
	if ( idx >= count )	return NULL;
	SEARCHITEM *pTypeInfo = (SEARCHITEM *)gSearchSongType.sChildItemList.GetAt(idx);
	return pTypeInfo ? pTypeInfo->cItemName : NULL;
}

BOOL AddUDiskFileToVod(UDISK_FILE_INFO_S *pFileInfo, BOOL bNeedSave)
{
	if ( !pFileInfo ) return FALSE;

	SONGINFO newSong;

	memset(&newSong, 0x0, sizeof(newSong));

	int curMaxSongID = FastSearch_GetMaxSongID();
	int singerID = FastSearch_GetSingerIDByName(pFileInfo->cSinger);
	int langID = GetLangIdxByName(pFileInfo->cLang, FALSE);
	int typeID = GetTypeIdxByName(pFileInfo->cType, FALSE);

	sprintf(newSong.cID, "%08d", curMaxSongID+1);
	memcpy(newSong.cName, pFileInfo->cSongName, sizeof(newSong.cName));
	memcpy(newSong.cSpell, pFileInfo->cSpell, sizeof(newSong.cSpell));
	newSong.langID = langID;
	newSong.wWdCt = pFileInfo->wWdCt;
	newSong.singerID = singerID;
	newSong.typeID = typeID;
	newSong.wTrack = pFileInfo->wTrack;
	newSong.wOrderTimes = 100;
	newSong.bCloud = false;
	newSong.bExist = true;

	snprintf(newSong.cFileName, sizeof(newSong.cFileName), "%s.mpg", newSong.cID);
	snprintf(newSong.cFilePath, sizeof(newSong.cFilePath), "%s/%s", g_cMainSongParentPath, newSong.cFileName);

	char copyCmd[512] = {0};
	snprintf(copyCmd, sizeof(copyCmd), "cp \"%s\" %s", pFileInfo->cFilePath, newSong.cFilePath);
	if ( !SystemCmd(copyCmd) )
	{
		DbgOutput(DBG_ERR, "%s failed\n", copyCmd);
		return FALSE;
	}

	FastAdd_SongList(&newSong);
	Sqlite3_insert_song(&newSong);
	FastSearch_IncreaseMaxSongID(1);

	if ( bNeedSave )
		FastSearch_SaveAllList();

	return TRUE;
}
