#ifndef _BCM_CONTROL_H
#define _BCM_CONTROL_H

#include "BCM_PLT_CTRL.h"
#include "BCM_OSD_CTRL.h"
#include"AudioCompare_OSD.h"

void BCM_Init();

void BCM_DeInit();

//void InitBCM();
//
//void DeInitBCM();
//
//void StartApp();
#if WORKMODE == NET_WORKMODE
void BCM_RoomScrollInfo(int iTime);
#endif

#if USE_MODULE_BCM

typedef struct _sBCM_CTRL_IMG_INFO
{
	int				miPLTID;			// 新老板号，810，还是830
	sScrollTextInfo	mMainScrollText;	// 主跑马灯信息
	sScrollTextInfo	mSubScrollText;		// 祝福语跑马灯信息
	sSwitchScrollMsg mSwitchInfo;
	sWartermarkInfo	mWartermarkInfo;
	sVolumeInfo		mVolumeInfo;
	sAmbienceInfo	mAmbienceInfo;		// 气氛
	CtrlImgInfo		mCtrlImgInfo;
	sImgInfo 		mLogo;				// Logo
	sImgInfo		mMovie;				// 电影的
	sImgInfo		mGuide;
	sImgInfo		mRecord;
	RECT			mBarCodeRect;		// 二维码显示区域
	RECT			mSubTitleRect;		// 外挂字幕显示区域
	CBaseStringA	mBGVideoPath[BGVideoType_Count];
}sBCM_Ctrl_ImgInfo;

#define PLAYSTATUS_STOP		0
#define PLAYSTATUS_PAUSE	1
#define PLAYSTATUS_PLAY		2

#define DELAYNUM			5*1000	// 5秒

class CBCM_Ctrl : public CBaseWnd
{
	typedef CBaseWnd CParentClass;
public:
	CBCM_Ctrl();

	virtual ~CBCM_Ctrl();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	virtual void OnTimer(int nTimerID);

	void InitBCM();

	void DeInitBCM();

	void MinitorUIThreadFunc();

public:
	void StartShutDownTimer();

private:
	void StartMonitorUIState();

	void StopMonitorUIState();

	void StartMonitorGuideState();

	void StopMonitorGuideState();

private:
	int RecvCmdMsg(int &sock, char *msg, int iLen);

	int AnalyseCmdMsg(char *vCmdMsgInfo, char *vCmdMsg[], int iCmdMaxNum);

	void ProcessCmdMsg();

public:
	void SetVolume(int iVol);

	void SelectAudio(int iAudio);

	void StartBCM();	  // 开机视频播放完成，启动

	void StartPlaySong(); // 解码完成，开始播放

	void UpdateMainScrollInfo();

	void SetMainScrollInfo(CBaseStringA &Text, UINT32 color);

	const char * GetMainScrollInfo();

	void UpdateSubScrollInfo(const wchar_t *Msg);

	void UpdateWartermark();


  void StartAudioCompareInit() ;
	void AudioCompareStop();
	void AudioComparePause(BOOL bPause);
	void AudioCompareStart();
	void AudioCompareCheck(const char* path ,const int track);
	void AudioCompareReStart();
private:
	void PlayNext(const char *FileName,
				  const char *SongName,
				  const char *PicPath,
				  BOOL bAuto);
	void Pause();
	void Resume();
	void Replay();

	void PlayPreview(const char *FileName, RECT rc);
	void RePreview();

	void PlaySituation(const char *FileName);
	void PlayCheer(int idx);		// 播放气氛
	void PlayUSBMovie(char *FileName);
	void PlayNetMovie(const char *Url);

private:
	void ShowVolume(BOOL bShow);
	void ShowAmbience(BOOL bShow);
	void ShowCtrlImg(int iCmdType, BOOL bShow);
	void ShowGuide(BOOL bShow);
	void ShowRecord(BOOL bShow);

	void ShowSongmark(int iScore);

	void ChangeBGVideo(int iNum);

	void ShowSubtitle(const char *pcSubtitle);

private:
	void WrapMsg(const wchar_t *Msg, wchar_t *pOut);		// 对Msg进行裁减,最后5个汉字
	void SwicthScrollMsg(const char *SongName, const char *PicPath);

	void GetNewMainScrollInfo(CBaseStringW *pOut);
public:
	CBaseThread MonitorUIThread;
	CBaseThread mMonitorGuideThread;
	int 		miServerSock;
	BOOL		mbMinitorGuideStatus;

private:
	int			miVol;
	int 		miCurUITrack;			// UI页面，
	int			miCurSongTrack;			// 数据库数据
	int 		miCurBGVideoIdx;		//
	char 		mPlayingFile[MAX_BUFFER_LEN];
	char 		mPreviewFile[MAX_BUFFER_LEN];
	int			miPlayStatus;

	RECT		mRcPreview;
	RECT		mRcMTV;
	int 		miInNetMode;

private:
	CFreeType			mFont;
	sBCM_Ctrl_ImgInfo	mImgInfo;

	BOOL	mbOpenMultiChannelFlag;	//

private:
	CAudioCompareOSD mAudioCompareOSD;
};

extern CBCM_Ctrl   *g_pBCM_Ctrl;

extern CBaseStringA	gLogoText;

void OSD_ShowMtvWatermark();
void OSD_SetUpdatePlayInfoFlag(int iFlag);
void OSD_ScrollGreetingMsg(const wchar_t *GreetingMsg, int iTimer);

int GetDuration(int *filetime);
int GetCurPosition(int *filetime);
int GetPlayState();

#endif

#endif
