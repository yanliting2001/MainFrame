#ifndef _CONTROL_HPP
#define _CONTROL_HPP

//------------Control class ---------------
class CControl
{
public:
	CControl();
	virtual ~CControl();

public:
	int Init();
	void HandlePAD(int sockfd,int cmd);

public:
	int SyncSongReq(int sockfd);
	int InsertSongReq(int sockfd);		//
	int DeleteSongReq(int sockfd);		//
	int MoveSongReq(int sockfd);		//
	int PauseSongReq(int sockfd);		//
	int ResumeSongReq(int sockfd);		//
	int RePlaySongReq(int sockfd);		//
	int SwitchSongReq(int sockfd);		//
	int SwitchChannelReq(int sockfd);	//
	int SetVolReq(int sockfd);			//
	int AmbienceReq(int sockfd);
	int CheersReq(int sockfd);
	int HootingReq(int sockfd);
	int StartScoreReq(int sockfd);
	int StopScroeReq(int sockfd);
	int StartGuideReq(int sockfd);
	int StopGuideReq(int sockfd);
	int StartRecordReq(int sockfd);
	int StopRecordReq(int sockfd);
	int SearchSingedSongReq(int sockfd, int cmd);

	int UdiskInfoReq(int sockfd);
	int PlayUdiskSongReq(int sockfd);
	void ReadIpInfo(char *dstbuf);
	int MoveSongTopReq(int sockfd);

public:
	int GetSyncSongInfo(char *pBuf, int cmd);
	int SendSyncSongToPad(int sockfd,int cmd);
	int SendSyncStatusToPad(int sockfd,int cmd,int vol);
	int SendSyncPublicSongToPad(int sockfd, int cmd);
	int SynPlayingSongInfoReq(int sockfd,int cmd);
	int SynTypeReq(int sockfd,int cmd);
	int SynSongTypeReq(int sockfd,int cmd);
	int SynLanguageTypeReq(int sockfd,int cmd);
	int SynRankTypeReq(int sockfd,int cmd);
	int SynSetMute(int sockfd,int mute);
	int SynSingerList(int sockfd,int cmd);
	int SynSongList(int sockfd,int cmd);
	int SynSongInfo(int sockfd,int cmd);
	int SceneReq(int sockfd);
	int SynNextSong(int sockfd);
	int GraffitoReq(int sockfd);
	int SetPubSongReq(int sockfd);
	int DeleteFileReq(int sockfd);
	int PubSongDelReq(int sockfd);
	int PubSongMoveTopReq(int sockfd);
	int DownloadingSizeReq(int sockfd);
	int AddFavSongReq(int sockfd);
	int DelFavSongReq(int sockfd);
	//int DeleteSingedSongReq(int sockfd);
	int DeleteDownloadedSongReq(int sockfd);
	int DeleteDownloadingSongReq(int sockfd);
	int DownloadingSongMoveTopReq(int sockfd);
	int FavSongMoveTopReq(int sockfd);
	int PlaySongReq(int sockfd);
	int VolumeReq(int sockfd,int action);
private:
	int mChannelStatus;		//原伴唱状态:	0：原唱,1:伴唱
	int mPlayStatus;		//播放状态: 	0：暂停,1:播放
	int mVolume;			//音量值
};

extern CControl *gpControl;
#endif
