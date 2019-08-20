#ifndef _BCM_TYPE_H
#define _BCM_TYPE_H
#include "AppVerb.h"

#if USE_MODULE_BCM

#include <stdlib.h>
#include <wchar.h>
#include "types.h"
#include "commdef.h"
#include "damd_7231api.h"

#include "FileOperate.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "../GlobalUIClass.h"
#include "../DLNA/DlnaApi.h"
#include "../PlayerApi/PlayerServer.h"
//#include "../PlayerApi/PlayerInfo.h"

typedef unsigned char	UINT8;
typedef short			INT16;
typedef unsigned short	UINT16;
typedef int 			INT32;
typedef unsigned int	UINT32;

#define BUFLEN			1024

#define CMDLEAD 				"RMC"
#define PLYTYPE					"PLY"
#define OSDTYPE					"OSD"

// PLY CMD
#define PLY_AUTO_PLAYNEXT		"NEXTA"			// 自动切哥
#define PLY_MANUAL_PLAYNEXT		"NEXTM"			// 手动切歌
#define PLY_MAIN_PAUSE			"PAUSE"			// 暂停
#define PLY_MAIN_RESUME			"PAUSERESUME"	// 恢复播放
#define PLY_MAIN_REPEAT			"REPEAT"		// 重唱
#define PLY_MAIN_STOP			"STOPPLAY"		// 停止
#define PLY_SWITCH_AUDIO		"SWITCHAUDIO"	// 切换音频
#define PLY_MAIN_SETVOL			"VOLSET"		// 设置音量
#define PLY_MAIN_MUTE			"VOLMUTE"		// 静音
#define PLY_MAIN_NETPLAY		"URLLINK"		// HTTP/M3U8方式播放
#define PLY_PIP_PLAY			"STARTPREVIEW"	// 播放预览
#define PLY_PIP_PAUSE			"PAUSEPREVIEW"	// 暂停预览
#define PLY_PIP_RESUME			"RESUMEPREVIEW"	// 恢复预览
#define PLY_PIP_STOP			"STOPPREVIEW"	// 停止预览
#define PLY_PLAY_PHANTOM		"PLAYPHANTOM"	// 播放幻影
#define PLY_STOP_PHANTOM		"STOPPHANTOM"	// 停止幻影
#define PLY_PLAY_MTV			"PLAYMTV"		// MTV幻影
#define PLY_STOP_MTV			"STOPMTV"		// 停止MTV幻影
#define PLY_FULL_MTV			"PLAYMTVFULL"	// 全屏幻影
#define PLY_SET_FULLMTV			"FULLSCREEN"	// ????有待完善
#define PLY_PLAY_SITUATION		"SITUATION"		// 播放情景
#define PLY_PLAY_CHEER			"CHEER"			// 播放气氛
#define PLY_PLAY_MOVIE			"MOVIE"			// 播放本地电影
#define PLY_PLAY_FORWARD		"FORWARD"		// 快进
#define PLY_PLAY_BACKWARD		"BACKWARD"		// 后退
#define PLY_SET_HDMIAUDIO		"HdmiAudio"		// 设置HDMI音频输出
#define PLY_SEEKTO				"SEEKTO"		// 跳转

#define PLY_PLAY_MULTCHANNEL	"MULTCHANNEL"	// 5.1多音轨透传

// OSD
#define OSD_SHOW_MARK			"SONGMARK"		// 歌曲评分
#define OSD_SHOW_AMBIENCE		"AMBIENCE"		// 显示气氛
#define OSD_SHOW_GUIDE			"STARTGUIDE"	// 开始导唱
#define OSD_STOP_GUIDE			"STOPGUIDE"		// 停止导唱
#define OSD_SHOW_RECORD			"STARTRECORD"	// 开始录音
#define OSD_STOP_RECORD			"STOPRECORD"	// 停止录音
#define OSD_PLAY_RECORD			"PLAYRECORD"	// 播放录音
#define OSD_OPEN_WATERMARK		"SHOWWATERMARK"	// 打开水印
#define OSD_CLOSE_WATERMARK		"UNSHOWWATERMARK"//关闭水印
#define OSD_UPDATE_WATERMARK	"UPDATEWATERMARK"//更新水印
#define OSD_SET_OUTPUTFORMAT	"TVFORMAT"		// 设置输出模式
#define OSD_SET_SATURATION		"SATURATION"	// 饱和度
#define OSD_SET_CONTRAST		"CONTRAST"		// 对比度
#define OSD_SET_LIGHT			"LIGHTNESS"		// 亮度
#define OSD_UPDATE_SCROLL		"UPDATEMSGINFO"	// 更新跑马灯
#define OSD_SWITCH_BGVIDEO		"CHANGEBGVEDIO" // 切换背景视频
#define OSD_SHOW_BARCODE		"BARCODE"		// 显示二维码

// Other
#define OTHER_START_BCM			"StartBCM"		// 启动BCM
#define OTHER_DECODE_OK			"ShowPic"		// 解码完成，开始播放

#define CMD_PAUSE				0x01
#define CMD_RESUME				0x02
#define CMD_REPLAY				0x03
#define CMD_MUTE				0x04
#define CMD_ACCOMPANY 			0x05			// 伴唱
#define CMD_ORIGINAL			0x06			//原唱

typedef enum _eTRACKTYPE
{
	Track_L		= 0,		// 0音轨原唱
	Track_R,				// 1音轨伴唱
	Channel_L 	= 0,		// 0声道原唱
	Channel_R,				// 1声道原唱
} eAudio_Track;

typedef enum _eBGVideoType
{
	BGVideoType_Home=0,		// 首页
	BGVideoType_Singer,
	BGVideoType_Song,
	BGVideoType_Rank,
	BGVideoType_Movie,
	BGVideoType_Language,
	BGVideoType_Pinyin,
	BGVideoType_NewSong,
	BGVideoType_KGO,
	BGVideoType_Count,
} eBGVideoType;

typedef enum _eTimeHandle
{
	TimeHandle_SwitchScroll=0,// 切歌滚动信息定时句柄
	Timehandle_Volume,		  // 音量
	Timehandle_Ambience,	  // 气氛
	TimeHandle_CtrlImg,		  // 暂停等
	TimeHandle_SRTFile,		  // 外挂字幕文件
	TimeHandle_ShutDown,	  // 关机时间
	TimeHandle_SaveSong,
}eTimeHandle;

typedef struct _sImgInfo
{
	RECT 			mRc;
	CBaseStringA	mPath;
} sImgInfo;

typedef struct _sCtrlImgInfo
{
	RECT 			mRc;
	CBaseStringA	mPausePath;
	CBaseStringA	mResumePath;
	CBaseStringA	mReplayPath;
	CBaseStringA	mMutePath;
	CBaseStringA	mOriginalPath;
	CBaseStringA	mAccompanyPath;
} CtrlImgInfo;

// 气氛
typedef struct _sAmbienceInfo
{
	CBaseStringA 	mPath;
	RECT			mRcet;
	CBaseStringA	mCheerPath;	// 喝彩
	CBaseStringA	mHootPath;	// 倒彩
} sAmbienceInfo;

// 切歌滚动信息
typedef struct _sSwitchScrollMsg		// 目前这个是组合图片来实现
{
	RECT			mScrollRect;		// 滚动区域，不包含图片本身的大小区域
	RECT			mBGRect;			// 背景图片的大小
	RECT			mSongPicRect;		// 相对于背景图片,歌星图片的区域
	RECT			mPlayingScrollRect; // 相对于背景图片,正播信息的区域
	RECT			mNextScrollRect;
	CBaseStringA 	mBGPath;
} sSwitchScrollMsg;

// 音量
typedef struct _sVolumeInfo
{
	CBaseStringA 	mBGPath;
	RECT			mBGRect;
	CBaseStringA	mThumbPath;
	RECT			mScrollRect;		// 滑动的范围,相对于BGRect的
} sVolumeInfo;

#endif

#endif
