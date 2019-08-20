#pragma once

#include "commonctrl_header.h"
#include "Song.h"
#include "CSCloudSongLib.h"

#define SONGITEM_MAX   100

class CPlayerInfo
{
public:
	CPlayerInfo();
	~CPlayerInfo();

	void PlayStart();		//add by yq for start to paly pub broad cast,2013-01-09

// play control
	void Play(
		SONGINFO *pSongInfo,
		BOOL bForceSwitch);
	void PlayNext(
		BOOL bForceSwitch);
	void Pause();
	void Resume();
	void Replay();
	void SeekTo(int msec);

	void StopPlay();

	void SetOriginal(
		int nOriginal);

	void SetVolume(
		int nVolume);
	int GetVolume();
	int GetPlayState();

	void StartPhantom();
	void StopPhantom();

	void StartPreview(
		const char *cServerName,
		const char *cFileName,
		int x,
		int y,
		int w,
		int h);
	void PausePreview();
	void StopPreview();
	void ResumePreview();

	void PlayMTV(
		int x,
		int y,
		int w,
		int h);
	void PlayMTVFullScreen(
		int bFullScreen);
	void StopMTV();

	void StartSituation(
		const char *cServerName,
		const char *cFileName);
	void StopSituation();

	void Cheer(
		int nType);		//add by yq   0:喝彩  1:倒彩
	void Ambience(int nFaceIndex);
	void Attunement(
		int nType);
	//add by yq,2012-12-07,for song mark
	void StartSongMark();
	void StopSongMark();
	//end add by yq,2012-12-07,
	void SongMark(
		int nType);

//add by yq,2012-13
	void SetTVLightness(int iLightness);
	void SetTVContrast(int iContrast);
	void SetTVSaturation(int iStaturation);
	void ChangeBGVideo(int iBGVideoIndex);
	int GetCurBGVideo(void);
//end add by yq,2012-12-13

	void SetTvPutOutMode(int mode);		//0:HDMI  1:AV

	void SetHdmiAudio(int bOpen);		//

	void StartGuide();
	void StopGuide();

	void PreviewMovie(int x, int y, int w, int h);
	void StopPreviewMoview();
	void PauseMovie();
	void ResumeMovie();
	void StartForwardMovie();
	void StartBackwardMovie();

	void StartRecord(const char *recordName);
	void StopRecord();
	void PlayRecord(const char *filename);

	void InScreenSave();

	void OutScreenSave();

//电视水印
	void ShowWatermark();
	void UnShowWatermark();

//播放url
	void PlayUrlStream(const char *url);

//更新跑马灯的信息（正播，和下一首）
	void UpdateScrollMsg();
// song list control
public:
	int GetSelectedSongCount();
	int GetSingedSongCount();
	SONGINFO *GetCurPlaySongInfo();
	SONGINFO *GetSelectedSongInfoByIndex(
		int nIndex);
	SONGINFO *GetSingedSongInfoByIndex(
		int nIndex);
	int GetSelectedSongIndexByID(
		const char *cSongID);
	int GetSingedSongIndexByID(
		const char *cSongID);
	BOOL IsSongInSelectedList(
		const char* cSongID);
	BOOL IsSongInSingedList(
		const char* cSongID);
	BOOL AddSongToSelectedList(
		const SONGINFO *pSongInfo,
		int nIndex,
		BOOL bForceAdd=FALSE);
	BOOL AddSongToSingedList(
		const SONGINFO *pSongInfo,
		int nIndex);
	void DelSongFromSelectedList(
		const char *cSongID);
	void DelSongFromSelectedList(
		int nIndex);
	void DelSongFromSingedList(
		const char *cSongID);
	void DelSongFromSingedList(
		int nIndex);
	void MoveSelectedSongToFirst(
		int nIndex);
	void MoveSelectedSongToFirst(
		const char *cSongID);
	void MoveSelectedListItem(
		int nItemFrom,
		int nItemTo);
	void MoveSingedListItem(
		int nItemFrom,
		int nItemTo);

	void Play_UDiskSong(
			const char *cServerName,
				const char *cFileName);

	//add by yq,2013-03-06
	SONGINFO * GetPlayingSongInfo();

	//end add by yq,2013-03-06
	void SetPadChangeState(BOOL bFlag);

	int GetOriginalState();

	//add by yanliting 2013-10-31
	char* GetIp();

	void SaveSelectSongList();		// 保存已点列表

	void SaveCloudSongList();		// 保存云加歌列表

	void ClearSelectedSongList(void);	// 清空已点列表

	CSongLibrary *GetCloudSongHandle();

private:
	void ReportSongStateChanged(
		const char* cSongID);
	void ReportListStateChanged();

	void ReportOriginalUIStateChanged();		//add by yq,2013-01-09

	void ReportUIStateChangedToPad(int iType);	//add by yq,2013-03-06

public:
	int mVolume;
	int mOriginal;
	int mPlayStatus;
	char mIp[64];

	SONGINFO mPlayingSong;
	SONGINFO mPlayingPublicSong;
	CPtrListCtrl mSelectedList;
	CPtrListCtrl mSingedList;
	CBaseLock mSongInfoListLock;

	BOOL mSituationMode;
	BOOL mSongMarkMode;			//add by yq,2012-12-07

	BOOL mPadChangeState;
	int 		mCurPlayBgVideoIdx;

	// 云加歌部分
public:

	CSongLibrary mCloudSongs;

	void CloudDownNext();

	void CloudFailedDownNext();

	void ClearDowningSongInfo();

	BOOL AddSongToCloudList(
		const SONGINFO *pSongInfo,
		int nIndex);

	void DelSongFromCloudList(
			int nIndex);

	void DelSongFromCloudListByID(const char *cSongID);

	BOOL IsSongInCloudList(
		const char* cSongID);

	int GetCloudSongCount();

	SONGINFO *GetCloudSongInfoByIndex(
		int nIndex);

	void MoveCloudListItem(
			int nItemFrom,
			int nItemTo);

	void MoveCloudSongToFirst(
		int nIndex);

	void MoveCloudSongToFirst(
		const char *cSongID);

	SONGINFO *GetCurDowningInfo();

	void ReportDowningStateChanged();

	// 获取下载进度数据
	int GetDownProgress();

public:
	// 点歌有吧台号的歌曲
	void AddBarNumSongToSelectedList(
			const SONGINFO *pSongInfo,
			int nIndex);


//	CPtrListCtrl mCloudSongList;		// 云歌曲列表
//	CBaseLock	 mCloudSongListLock;
//	SONGINFO     mCloudDownSong;
};

extern CPlayerInfo *gPlayerInfo;

