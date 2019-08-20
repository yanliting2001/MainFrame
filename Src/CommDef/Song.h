#pragma once
#include "SongInfoDef.h"
#if 0
#pragma pack(push)
#pragma pack(1)

typedef struct tagSONGINFO
{
	char  cID[8+1];            	//歌曲ID
	char  cName[50+1];         	//歌曲名
	char  cSinger[20+1];       	//歌星名
	char  cType[20+1];          //歌曲类型
	char  cLang[8+1];          	//语种
	char  cServerName[20+1];   	//服务器名(网络版用)
	char  cFilePath[50+1];     	//文件路径
	char  cFileName[20+1];     	//文件名称
	char  cPicPath[20+1];      	//歌曲图片
	char  cSpell[9+1];          //拼音缩写
	char  cStroke[9+1];       	//笔划
	int   wOrderTimes;			//点击率			新添加
	short wVol;                	//默认音量
	short wMaxVol;             	//最大音量
	short wMinVol;             	//最小音量
	short wTrack;              	//伴唱音轨
	short wWdCt;               	//歌名字数
	bool  bChos;               	//是否合唱
	bool  bNewSong;            	//是否为新歌
	bool  bPlaying;
	bool  bExist;               //本地是否存在
	bool  bCloud;				//是否云歌曲；
	int nLyricsLen;
	unsigned long long pLyrics;
} SONGINFO;
#pragma pack(pop)

typedef struct tagSINGERINFO
{
	char  cID[8+1];            	//歌手ID
	char  cName[20+1];         	//歌手名
	char  cType[20+1];         	//歌手类型
	char  cPicPath[20+1];      	//歌手图片
	char  cNationality[10+1];  	//国家
	short wSongsCount;         	//歌曲数量
	int   wOrderTimes;			//点击率     新添加
	char  cSpell[8+1];         	//拼音
	char  cStroke[3+1];        	//笔划
} SINGERINFO;

#endif
