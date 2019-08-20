#pragma once

#include "PlayerServer.h"

#define ATTUNEMENT_MUTE			6
#define ATTUNEMENT_RECORD		7
#define ATTUNEMENT_PLAYBACK		8
#define ATTUNEMENT_TRICKY		9
#define ATTUNEMENT_HARMONY		10
#define ATTUNEMENT_STOPRECORD	11
#define ATTUNEMENT_RECORDLIST	12
#define ATTUNEMENT_SINGERKING	13
#define ATTUNEMENT_FUNNY		14

// song contrl
// nOriginal: 0 disable Original music, 1 enable Original music
int PlayerApi_Play(
	const char *cID,
	const char *cServerName,
	const char *cFileName,
	int nVol,
	int nTrack,
	int nOriginal,
	int bForceSwitch,
	const char *cSongName,
	const char *cSinger,
	const char *cPicPath);

int PlayerApi_Pause();
int PlayerApi_Resume();
int PlayerApi_Stop();
int PlayerApi_Replay();

// volume control
int PlayerApi_SetVolume(
	int nVol);

// audio control
int PlayerApi_SetOriginal(
	int nTrack,
	int nOriginal);

//add by yq
int PlayerApi_StartPreview(
	const char *cServerName,
	const char *cFileName,
	int x,
	int y,
	int w,
	int h);
int PlayerApi_PausePreview();
int PlayerApi_ResumePreview();
int PlayerApi_StopPreview();

int PlayerApi_StartPhantom();
int PlayerApi_StopPhantom();

int PlayerApi_PlayMTV(
	int x,
	int y,
	int w,
	int h);
int PlayerApi_PlayMTVFullScreen(
	int bFullScreen);
int PlayerApi_StopMTV();

int PlayerApi_Cheer(
	int nType);		//add by yq   0:喝彩  1:倒彩
int PlayerApi_Ambience(int nFaceIndex);
int PlayerApi_Situation(
	const char *cServerName,
	const char *cFileName,
	int nVol,
	int nTrack,
	int nOriginal);
int PlayerApi_Attunement(
	int nType);
int PlayerApi_SongMark(
	int nType);

int PlayerApi_Quit();

int PlayerApi_UpdateMsgInfo(
	const char *playingName,
	const char *nextPlayName);

//add by yq,2012-13
int PlayerApi_SetTVLightness(
		int iLightness);

int PlayerApi_SetTVContrast(
		int iContrast);

int PlayerApi_SetTVSaturation(
		int iStaturation);
//end add by yq,2012-12-13

int PlayerApi_ChangeBGVideo(int iBGVedioIndex);

int PlayApi_SetTvPutOutMode(int mode);

int PlayApi_StartGuide();

int PlayApi_StopGuide();

int PlayerApi_PlayMovie(
	const char *cServerName,
	const char *cFileName,
	int nVol,
	int nTrack,
	int nOriginal);

int PlayApi_ForwardMovie();

int PlayApi_BackwardMovie();

int PlayApi_StartRecord(const char *recordName);

int PlayApi_StopRecord();

int PlayApi_PlayRecord(const char *filename,
		int value,
		int nOriginal);
//电视水印
int PlayApi_ShowWatermark();

int PlayApi_UnShowWatermark();

int PlayApi_UpdateWatermark();

//播放url
int PlayApi_PlayUrlStream(const char *url);

int PlayApi_StopPlay();

int PlayApi_SetHdmiAudio(int bOpen);

int OSDApi_ShowBarCode(const char *pFilePath, int bShow);


int PlayApi_SeekTo(int iPos);
