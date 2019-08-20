#ifndef _DLNATYPEDEF_H
#define _DLNATYPEDEF_H

//request and response
#define HEARTBEAT_REQ		0		//Heartbeat request
#define HEARTBEAT_RESP		1		//Heartbeat response
#define SYNCSONG_REQ		2		//Synchronous request
#define SYNCSONG_RESP		3		//Synchronous response
#define INSERTSONG_REQ		4		//Insert request
#define DELETESONG_REQ		5		//Delete request
#define MOVESONG_REQ		6		//Movint request
#define PAUSE_REQ			7		//Pause request
#define RESUME_REQ			8		//Resume request
#define REPLAY_REQ			9		//Replay request
#define SWITCHSONG_REQ		10		//Switch song request
#define SWITCHCHANNEL_REQ	11		//Switch channel request
#define SETVOLUME_REQ		12		//Set volume request
#define AMBIENCE_REQ		13		//Show ambience request
#define	CHEERS_REQ			14		//Cheers request
#define HOOTING_REQ			15		//Hooting request
#define SCENE_REQ			16     	// scene request
#define MEETING_SCENE	   	1		//Meeting scene request
#define SPLACE_SCENE		2		//Space scene request
#define PARK_SCENE			3		//Park scene request
#define ROAD_SCENE			4		//Road scene request
#define MAJIANG_SCENE		5		//Majiang scene request
#define QUIT_SCENE_REQ		21		//Quit scene request
#define START_SCORE_REQ		22		//Start score request
#define STOP_SCORE_REQ		23		//Stop secore request
#define START_GUIDE_REQ		24		//Start guide to sing request
#define STOP_GUIDE_REQ		25		//Stop guide to sing request
#define START_RECORD_REQ	26		//Start record request
#define STOP_RECORD_REQ		27		//Stop record request
#define STOP_RECORD_RESP	28		//Stop record response
#define PLAY_RECORD_REQ		29		//Play record request
#define ZHENGGU_REQ			30		//整蛊请求
#define HESHENG_REQ			31		//和声请求
#define CHANGJIANG_REQ		32		//唱将请求
#define GAOGUAI_REQ			33		//搞怪请求
#define SET_MICRO_VOL_REQ	34		//Set microphone volume request
#define SET_AMP_VOL_REQ		35		//Set amplifier volume request
#define SET_TONE_VOL_REQ	36		//Set tone volume request
#define UDISK_INFO_REQ		37		//Read u disk information request
#define UDISK_INFO_RESP		38		//Read u disk information response
#define PLAY_UDISK_REQ		39		//Play u disk song request
#define SEARCHSINGEDSONG_REQ	40	//Search the singed song list request
#define SEARCHSINGEDSONG_RESP	41	//Searcg tge singed song list response
#define MOVESELTOTOP_REQ	42		//Move song to top from select list request
#define PLAYINGINFO_REQ		43		//get current playing song info request
#define SINGERTYPE_REQ     	44		//get singer type
#define SONGTYPE_REQ     	45      //get song type
#define LANGUAGETYPE_REQ 	46      //get language type
#define RANKTYPE_REQ      	47      //get rank type
#define MUTE_REQ           	48      // mute
#define UNMUTE_REQ       	49      //unmute
#define SINGER_LIST_REQ    	50      //get singer list
#define SONG_LIST_REQ     	51      //get song list from allsonglist
#define SONG_LIST_BYSINGERID_REQ 52
#define SONG_LIST_BYLANGTYPE_REQ       53
#define SONG_LIST_BYSONGTYPE_REQ       54
#define SONG_LIST_BYRANKTYPE_REQ       55
#define NEWSONG_LIST_REQ       56
#define SONGINFO_REQ       57
#define SONG_LIST_BYLYRIC_REQ       58

#define GETNEXT_SONG_REQ            59
#define TEST_RESPONSE_REQ           60
#define TEST_DOWNLOADURL_REQ        61

#define SCENETYPE_REQ 62
#define AMBIENCETYPE_REQ 63
#define GRAFFITO_REQ 64
#define IMPORT_REQ 65
#define GET_DEVLIST_REQ 66
#define GET_IMPORTLIST_REQ 67
#define GET_CAPACITY_REQ 68

#define DOWNLOAD_REQ 69
#define SETPUB_REQ 70
#define DELETEFILE_REQ 71
#define LOCALSONG_LIST_REQ 72
#define PUBSONG_LIST_REQ 73
#define PUBSONG_DEL_REQ 74
#define PUBSONG_MOVETOP_REQ 75
#define DOWNLOADING_LIST_REQ 76
#define DOWNLOADED_LIST_REQ 77
#define DOWNLOADING_SIZE_REQ 78
#define SONG_LIST_BYFAV_REQ 79
#define FAVSONG_ADD_REQ 80
#define FAVSONG_DEL_REQ 81
#define DELSINGEDSONG_REQ 82
#define DELDOWNLOADEDSONG_REQ 83
#define PLAYSONG_REQ 84
#define DELDOWNLOADINGSONG_REQ 85
#define DOWNLOADING_MOVETOP_REQ 86
#define FAV_MOVETOP_REQ 87
#define DelExtDiskFile_REQ 88
#define GetFavSingerList_REQ 89
#define AddFavSinger_REQ 90
#define DelFavSinger_REQ 91
#define TopFavSinger_REQ 92
#define SETALLLIST_REQ 93
#define VOLUMEUP_REQ 94
#define VOLUMEDOWN_REQ 95
#define PLAYURL_REQ 96
#define DELALLLIST_REQ 97
#define SENDKEY_REQ 98

//notice
#define SYNCSONG_NOTICE		100		//Synchronous song notice
#define PAUSE_NOTICE		101		//Pause notice
#define RESUME_NOTICE		102		//Resume notice
#define REPLAY_NOTICE		103		//Replay notice
#define SWITCHSONG_NOTICE	104		//Switch song notice
#define SWITCHCHANNEL_NOTICE	105	//Switch channel notice
#define SETVOLUME_NOTICE	106		//Set volume notice
#define START_SCORE_NOTICE	107		//Start score notice
#define STOP_SCORE_NOTICE	108		//Stop score notice
#define START_GUIDE_NOTICE	109		//Start guide notice
#define STOP_GUIDE_NOTICE	110		//Stop guide notice
#define START_RECORD_NOTICE	111		//Start record notice
#define STOP_RECORD_NOTICE	112		//Stop record notice
#define SET_MICRO_VOL_NOTICE	113	//Set microphone volume notice
#define SET_AMP_VOL_NOTICE	114		//Set amplifier volume notice
#define SET_TONE_VOL_NOTICE	115		//Set tone volume notice
#define SET_SITUATION_NOTICE 116	//Set Situation mode notice
#define SET_PUBLICSONG_NOTICE 117	//Set play public song notice
#define PLAY_UDISK_NOTICE	118		//Play the U disk notice
#define SYNC_SINGED_NOTICE	119		//Synchronous the singed song notice

#define ROOT			"head"
#define CMD             "cmd"
#define SONGS			"songs"
#define SONG			"song"
#define ITEMS			"items"
#define ITEM			"item"
#define SONGID			"songId"
#define SRCINDEX		"srcIndex"
#define DESCINDEX		"descIndex"
#define VOL				"vol"
#define LEN				"len"
#define PATH			"path"
#define FILENAME	"filename"

//�������Ϣ
#define SONG_ID				"id"
#define SONG_NAME			"name"
#define SONG_WORDCOUNT		"wordcount"
#define SONG_SINGER			"singer"
#define SONG_TYPE			"type"
#define SONG_LANG			"lang"
#define SONG_SERVERNAME		"servername"
#define SONG_FILEPATH		"filepath"
#define SONG_PICPATH		"picpath"
#define SONG_SPEL			"spel"
#define SONG_MAXVOL			"maxvol"
#define SONG_MINVOL  		"minvol"
#define SONG_TRACK			"track"
#define SONG_CHOS			"chos"
#define SONG_EXIST			"exist"
#define SONG_NEWSONG		"newsong"
#define SONG_STROKE			"stroke"
#define SONG_PLAYING		"playing"
#define PAGE_NUM		"pagenum"
#define NUM			"num"
#define KEYWORD		"keyword"
#define SPELL		"spell"
#define LYRIC		"lyric"
#define SINGERTYPE	"singertype"
#define SINGERID	"singerid"
#define TAGID	"tagid"
#define STAGE   "stage"
#define RESPSIZE        "size"
#define SRC     "src"
#define WEBSERVER       "webserver"
#define URL     "url"
#define TERMID  "termid"
#define AUDIOTRACK      "audiotrack"

enum
{
	DMR_EVENT_UICHANGE_PLAY_START		= 100, 		/* 开始播放事件  */
  	DMR_EVENT_UICHANGE_PLAY_STOP		= 101,  	/* 停止播放事件 */
	DMR_EVENT_UICHANGE_PLAY_PAUSE		= 102, 		/* 暂停播放事件	*/
	DMR_EVENT_UICHANGE_PLAY_RESUME		= 103,		/* 恢复播放事件	*/
 	DMR_EVENT_UICHANGE_VOLUME			= 104,   	/* 音量变化事件	*/
  	DMR_EVENT_UICHANGE_MUTE				= 105,    	/* 静音事件	*/
  	DMR_EVENT_UICHANGE_UNMUTE			= 106,   	/* 恢复静音事件	*/
  	DMR_EVENT_UICHANGE_AUDIO_TRACK		= 107, 		/* 音轨切换事件	*/
	DMR_EVENT_UICHANGE_START_SCORE		= 108, 		/* 开始评分事件	*/
	DMR_EVENT_UICHANGE_STOP_SCORE		= 109,  	/* 停止评分事件	*/
	DMR_EVENT_UICHANGE_START_GUIDE_SING	= 110,		/* 开始导唱事件	*/
	DMR_EVENT_UICHANGE_STOP_GUIDE_SING	= 111,		/* 停止导唱事件	*/
	DMR_EVENT_UICHANGE_START_RECODE		= 112, 		/* 开始录音事件	*/
	DMR_EVENT_UICHANGE_STOP_RECODE		= 113, 		/* 停止录音事件	*/
	DMR_EVENT_UICHANGE_MIC_VOLUME		= 114,   	/* 麦克风音量变化事件 */
	DMR_EVENT_UICHANGE_AMPLIFIER_VOLUME	= 115,		/* 功放音量变化事件	*/
	DMR_EVENT_UICHANGE_SELECT_SONG		= 116, 		/* 点歌事件  */
	DMR_EVENT_UICHANGE_SWITCH_SONG		= 117,		/* 切歌事件 */
	DMR_EVENT_UICHANGE_SITUATION		= 118,		/* 情景模式变化事件 */
	DMR_EVENT_UICHANGE_AMBIENCE			= 119,
	DMR_EVENT_UICHANGE_SCENE			= 120,
	DMR_EVENT_UICHANGE_SCENE_STOP		= 121,
	DMR_EVENT_UICHANGE_GRAFFITO 		= 122,
	DMR_EVENT_UICHANGE_IMPORT_START		= 123,
	DMR_EVENT_UICHANGE_IMPORT_OK		= 124,
	DMR_EVENT_UICHANGE_DEVADDED 		= 201,
	DMR_EVENT_UICHANGE_DEVREMOVED		= 202
};

typedef enum eSyncType
{
	SYNCTYPE_SONG=0,		//同步歌曲
	SYNCTYPE_PAUSE,			//暂停
	SYNCTYPE_RESUME,		//播放
	SYNCTYPE_VOLUME,		//音量
	SYNCTYPE_CHANNEL,		//声道
	SYNCTYPE_SWITCH,		//切歌
	SYNCTYPE_REPLAY,		//重唱
	SYNCTYPE_START_SCORE,	//开始评分
	SYNCTYPE_STOP_SCORE,	//停止评分
	SYNCTYPE_START_GUIDE,	//开始导唱
	SYNCTYPE_STOP_GUIDE,	//停止导唱
	SYNCTYPE_START_RECORD,	//开始录音
	SYNCTYPE_STOP_RECORD,	//停止录音
	SYNCTYPE_MRIRO_VOL,		//设置麦克风音量
	SYNCTYPE_AMP_VOL,		//设置功放音量
	SYNCTYPE_TONE_VOL,		//设置音调音量
	SYNCTYPE_SITUATION,		//情景模式
	SYNCTYPE_PUBLICSONG,	//公播模式
	SYNCTYPE_PLAYUDISK,		//播放u盘通知
	SYNCTYPE_SINGED,		//同步已唱歌曲
	SYNCTYPE_COUNT
}SyncType;

#endif

