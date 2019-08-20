#ifndef _AUDIOCOMPARE_OSD_H
#define _AUDIOCOMPARE_OSD_H
#include "BaseThread.h"
#include "commdef.h"
#include "damd_base_type.h"
#include "damd_porting_platform.h"
#include "yy_compare.h"
#include "BCM_OSD_CTRL.h"
#include <deque>
#include <map>

#define MAX_DOT_IN_LINE					60		// 一行存放的最大点数
#define MAX_AUDIO_LEVEL					7			// 等级数，上、中、下３个不同高度的等级
#define INC_STEP										30

// 定义比较条背景图片路径
#define COMPARE_BKGROUND_PNG			"KTV_Data/osdimg/CompareBKGround.png"
#define AUDIO_DEFAULT_PNG						"KTV_Data/osdimg/default.png"
#define AUDIO_MATCHED_PNG1					"KTV_Data/osdimg/matched1.png"
#define AUDIO_MATCHED_PNG2					"KTV_Data/osdimg/matched2.png"
#define AUDIO_MATCHED_PNG3					"KTV_Data/osdimg/matched3.png"
#define AUDIO_MATCHED_PNG4					"KTV_Data/osdimg/matched4.png"



#define AUDIO_EATING_PNG							"KTV_Data/osdimg/eating.png"

class CAudioCompareOSD : public CBaseThread
{
public:
	typedef struct _sCompareItem
	{
		unsigned char showFlag; ////增加srt文件，容许从哪个地方开始显示图，默认是1，全显示
	    unsigned char dstPower; // 0-7 (0 means no power,no need draw)	// 原唱的power
	} COMPARE_ITEM_S;

	typedef struct _sCompareItemRet
	{
		unsigned char showFlag; 
		//below is the result show for 
		unsigned char curPower; // 麦克风的power
		PCM_DATA_MATCH_TYPE ismatch;
		float score;
		float itemscore; /*0.0~100.0*/
	} COMPARE_ITEM_RET_T;

public:
	CAudioCompareOSD();
	virtual ~CAudioCompareOSD(){};

public:
	BOOL Start();
	void Stop();
	void ThreadFunc();
	void ShowPicThreadFunc();
	void DumpAudioTrackCheckThreadFunc();
public:
	void AudioCompareInit(void);
	void AudioCompareDeinit(void);

public:
	// 显示总排行
	void ShowAllRank(float rankID, BOOL bVisible = TRUE);
	// 显示分数
	void ShowSingingScore(float score, BOOL bVisible = TRUE);

	// 开始评分
	void StartAudioCompare(void);
	// 结束评分
	void StopAudioCompare(BOOL delfile=TRUE);

	void PauseAudioCompare(BOOL bPause);

	void ReStartAudioCompare();

	void PlayMainChannel(BOOL flag);
		
	///dump  main audio track
	void AudioCompareCheck(const char* path ,const int track) ;

private:
	// 设置当前时刻的对比图
	void SetCurAudioCompare(int power);

	void ShowCompareProcess(BOOL bVisible);

private:
	CBaseThread mAudioDumpFinish;
	CBaseThread mGetCastData;
	CBaseThread mShowPic;
	unsigned int mhCast;
	BOOL mExitThread;
	BOOL mbStartCompare;
	BOOL playStatus;

	RECT mrcCompareBK;			// 比较图片背景显示的区域
	RECT mrcAudioItemBK;
	RECT mrcAudioEatingBK;
	// 以下是分数的区域和用户头像区域
	RECT mrcUserHeadImg;
	RECT mrcRank;
	RECT mrcScore;
	CFreeType			mFont;

	unsigned int lrt_start_time ;
};
#endif		// _AUDIOCOMPARE_OSD_H
