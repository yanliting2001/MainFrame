#pragma once
#include "BaseString.h"

#define BAR_MODE	1				// 吧台模式

typedef struct SAdvanceSettingApi
{
	int miLanguageID;				//语言选择：		0--中文(默认),
	int miStartUp_status;			//设置启动状态:	0--闭房,1--开房
	int miStartUp_orignal;			//设置启动声道: 	0--伴唱,1--原唱
	int miTV_lightness;				//电视亮度:    	0--100
	int miTV_contrast;				//电视对比度:		0--100
	int miTV_Saturation;			//电视饱和度:		0--100
	int miSerialPort_MainPort0;		//主串口0:		0--未启用,1--触摸屏,2--中控盒,3--多串口盒,4--灯控盒,5--音效器
	int miSerialPort_MainPort1;		//主串口1:		0--未启用,1--触摸屏,2--中控盒,3--多串口盒,4--灯控盒,5--音效器
	int miSerialPort_SubPort1;		//子串口1:  		0--未启用,1--触摸屏,2--中控盒,3--灯控盒,4--音效器
	int miSerialPort_SubPort2;		//子串口2:		0--未启用,1--触摸屏,2--中控盒,3--灯控盒,4--音效器
	int miSerialPort_SubPort3;		//子串口1:  		0--未启用,1--触摸屏,2--中控盒,3--灯控盒,4--音效器
	int miSerialPort_SubPort4;		//子串口2:		0--未启用,1--触摸屏,2--中控盒,3--灯控盒,4--音效器
	int miTouchScreenSet_OnOff;		//支持触摸屏:		0--禁用,1--启用
	int miTouchScreenSet_type;		//触摸屏型号:		0--Penmount_x,1--Penmount_y
	int miTouchScreenSet_count;		//触摸屏校准点数:	0--3点,1--8点,2--25点
	int miVolume_microphone;		//麦克风启动音量:	0--100
	int miVolume_microphoneMax;		//麦克风最大音量:	0--100
	int miVolume_toneUp;			//音调音量上限:	0--100
	int miVolume_toneDown;			//音调音量下限:	0--100
	int miVolume_amplifierMax;		//功放最大音量:	0--100
	int miVOlume_amplifierAd;		//功放最大可调音量:0--100
	int miVolume_amplifierStart;	//功放启动音量:	0--100
	int miMusicLib_songCount;		//歌曲总数：
	int miMusicLib_pubBroadCastCount;	//公播歌曲:
	int miMusicLib_newSong;			//新歌歌曲:
	int miMusicLib_diskMemory;		//总磁盘空间:
	int miMusicLib_diskStatus;		//磁盘状况:
	char msSysPwd[12];				//系统密码:
	char msReceptionPwd[12];		//前台管理员密码:
	int miOutputMode;				//输入模式：   0：HDMI(默认)  1：AV
	char mktvserver_ip[32];
	char mktvserver_port[16];
	char mnetmediaserver_ip[32];
	char mnetmediaserver_port[16];
	int	miBarOnOff;					// 是否需要进行台号点歌
	int miSearchSingerOnOff;		// 是否启用歌星反查
	char msDecryptKey[16];			//密钥
	int	mbHdmiAudioClose;			// HDMI音频输出开关，0:开启， 1：关闭
	int	miSkinsId;					// 皮肤ID
	char mTemp[8];					// 备用
	char	md5[32];

	/////////////////////新版本的变量保存这块////////////////////
	// 调音这块
	unsigned char mucPcmTuneType;		// 调音中的混响类型
	unsigned char mucBodyFeelType;		// 调音中的体感强度
	unsigned char mucMicroVol;				// 调音中的麦克风音量
	unsigned char mucAttunementVol;	// 调音中的音量
	// 设置中的播放设置
	int mbPlayVol_keep;
	int miPlayVol_val;
	int miStartUp_stwichOrignal;	//设置切歌声道: 	0--伴唱,1--原唱,2--保持前一声道
	int miScreenSave_type;				//屏保类型:    	0--无屏保,1--图片屏保,2--当前视频屏保
	int miScreenSave_time;				//屏保启动时间: 秒--最少不能低于30S,或者会经常出现屏保，影响操作
	int mbShowSingerByPic;			// 是否显示图片的歌星

	// 歌库设置
	int mbCloud_default;
	int mbCloud_salf;
	int mbOnDelete;
	int mbOnSaveNew;
	int miOrderType;

	// 公播设置
	int mbPublicVol_Keep;
	int miPublicVol_Val;
	int miPublicTune_set;		// 0:跟随上一曲 1:伴唱  2:原唱
	int mbPublicPlay_open;
	int mbPublic_save;

	// 系统设置
	int miStartUp_vol;						//设置启动音量: 	0--100
	int miStartUp_effectVol;
	char mShutdownPwd[16];
	char mSettingPwd[16];
	char mDeletePwd[16];
	int mbOnRemark;					// 评分
}SSystemSetting;

extern const int INIT_START_STATUS;
extern const int INIT_START_VOL;
extern const int INIT_START_ORIGNAL;
extern const int INIT_START_SWITCHORINGANL;
extern const int INIT_SCREENSAVE_TYPE;
extern const int INIT_SCREENSAVE_TIME;
extern const int INIT_TV_LIGHTNESS;
extern const int INIT_TV_CONTRAST;
extern const int INIT_TV_SATURATION;
extern const int INIT_SERIALPORT_MAINPORT0;
extern const int INIT_SERIALPORT_MAINPORT1;
extern const int INIT_SERIALPORT_SUBPORT0;
extern const int INIT_SERIALPORT_SUBPORT1;
extern const int INIT_SERIALPORT_SUBPORT2;
extern const int INIT_SERIALPORT_SUBPORT3;
extern const int INIT_TOUCHSCREENSET_ONOFF;
extern const int INIT_TOUCHSCREENSET_TYPE;
extern const int INIT_TOUCHSCREENSET_COUNT;
extern const int INIT_VOLUME_MICROPHONE;
extern const int INIT_VOLUME_MICROPHONEMAX;
extern const int INIT_VOLUME_TONEUP;
extern const int INIT_VOLUME_TONEDOWN;
extern const int INIT_VOLUME_AMPLIFIERMAX;
extern const int INIT_VOLUME_AMPLIFIERAD;
extern const int INIT_VOLUME_AMPLIFIERSTART;
extern const char INIT_SYSPWD[12];
extern const char INIT_RECEPTIONPWD[12];

extern SSystemSetting gs_SystemSetting;

void ReadSystemSetting();
void SaveSystemSetting();

bool LoadScrollText(CBaseStringA &OutText, UINT32 &color);

bool SaveScroolText(CBaseStringA &InText, UINT32 color);

