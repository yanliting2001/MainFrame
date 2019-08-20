#ifndef _BCM_PLT_CTRL_H
#define _BCM_PLT_CTRL_H
#include "BCM_Type.h"

#if USE_MODULE_BCM
#if defined(__mips__)
#include "BCM7231_Control.h"
#define MAX_BUFFER_LEN				1024
#elif defined(__arm__)
#include "hisi3719_Control.h"
#endif

void SetAppVerbToEncryptDev(const char *cAppVerb, int iBufLen);

char* GetAppVerbFormEncryptDev(char *cAppVerb, int iBufLen);

bool CheckVerbFromFlash();

class CBCM_PLTCtrl
{
public:
	CBCM_PLTCtrl();

	virtual ~CBCM_PLTCtrl();
public:
	int Init(int iVerb);

	void DeInit();

public:
//	int Play(int idx,				// PLAYER_MAIN   PLAYER_PIP
//			PLY_MODE_E mode, 			// 播放模式
//			const char *pFileName,	// 文件路径
//			RECT rc);

	int Stop(int idx);

	int Pause(int idx);

	int Resume(int idx);

	int Replay(int idx);

	int GetDuration(int *iFileTime);

	int GetCurPosition(int *iFileTime);

	void SeekTo(int iFileTime);

	int	GetPlayState(int idx);

public:
	int ForWard();					// 快进

	int BackWard();					// 后退

	int PlayBG(char *pBGFileName);	// 背景视频

	int StopBG();

	int PlayPCM(char *pcFileName, int iVol=16);	// 播放混音 如气氛，

	int StartRecord(char *pcFileName, int iVol);// 开始录音

	void StopRecord();				// 停止录音

	int PlayRecord(char *pcFileName);// 播放录音

	int StartPhantom(RECT rc);		// 播放幻影，MTV等

	int StopPhantom();				// 停止幻影

public:
	void SetHdmi(BOOL bOpen);

	void SetOutputMode(int iMode);

	void SetVolume(int iVol);

	void SetMute(BOOL bMute);

	int	GetAllAudioChannel(int &iNum);

	void SetAudioTrack(int iTrack);

	void SetAudioChannel(int iChannel);

public:
	void SetInPlayingInitVideo(BOOL bPlaying);

	BOOL GetInitVideoState();

	void SetGuideState(BOOL bState);

	BOOL GetGuideState();

public:
	int GetSubtitle(int idx, char* pSubtitle, unsigned long *pInterval);

	void SetSubtitleParse(char *pSubtitle);

public:
	void Standby();

public:
	BOOL	mbPlayingInitVideo;		// 播放开机视频

public:
	void RePlayBkgroundView();

private:
	char 		mBkgroundViewFile[MAX_BUFFER_LEN];
};

extern CBCM_PLTCtrl g_PLYCtrl;

int BCMP_SendNextRequest();

#endif 	//
#endif	//
