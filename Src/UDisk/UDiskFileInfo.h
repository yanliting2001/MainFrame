#ifndef _UDISKFILEINFO_H
#define _UDISKFILEINFO_H
#include <sys/stat.h>
#include <sys/vfs.h>
#include "PtrControl.h"
#include "types.h"

typedef enum _eTrackType
{
	track_left = 0,		// 左声道	伴唱
	track_rigth,			// 右声道 	原唱
	track_count,
} TRACK_TYPE_E;

typedef struct _sUdiskFileInfo
{
	char  cFileName[256];     //文件名称
	char  cFilePath[256];     	//文件路径
	char  cSongName[128];	//歌曲名
	char  cSinger[64];      		 //歌星名
	char  cSpell[16];          		//拼音缩写
	char  cType[64];          		//歌曲类型
	char  cLang[64];          		//语种
	short wWdCt;               //歌名字数
	short wTrack;              //伴唱音轨
} UDISK_FILE_INFO_S;

void FindFileByPath(CPtrCtrl *list, const char *path, int depth);

// 根据语种名获取id
int GetLangIdxByName(const char *pLangName, BOOL bListPos=TRUE);
const char *GetLangNameByIdx(int idx);

// 根据分类名获取id
int GetTypeIdxByName(const char *pTypeName, BOOL bListPos=TRUE);
const char *GetTypeNameByIdx(int idx);

// 增加歌曲
BOOL AddUDiskFileToVod(UDISK_FILE_INFO_S *pFileInfo, BOOL bNeedSave = TRUE);

#endif // _UDISKFILEINFO_H
