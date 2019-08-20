
#ifndef _CONTROL_HPP
#include "control.hpp"
#endif

#ifndef _XMLANALYTIC_HPP
#include "xmlanalytic.hpp"
#endif

#ifndef _CMDDEFINE_HPP
#include "../DlnaTypeDef.h"
#endif

#ifndef _PADCONTROL_H
#include "PadControl.h"
#endif

extern "C"
{
#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _ERRNO_H
#include <errno.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _SYS_STAT_H
#include<sys/stat.h>
#endif

#ifndef _DIRENT_H
#include<dirent.h>
#endif


}

#include "../GlobalUIClass.h"
#include "../PlayerApi/PlayerApi.h"
#include "../OSDControl/BCM_Control.h"
#include "../DLNA/DlnaApi.h"

extern int dmr_process_uievent(int value);

#define MAX_SONGLIST	100

#define LISTNAME		"listbak.su"
#define XMLHEADER		"<?xml version=\"1.0\" encoding=\"utf-8\"?>"

typedef enum eUdiskFileType
{
	ISDIR = 0,
	ISFILE
}UTYPE;

CControl *gpControl = NULL;

//--------------------Control class---------------------
CControl::CControl()
{
	mChannelStatus = 1;
	mPlayStatus = 1;
	mVolume = 50;
}

CControl::~CControl()
{

}

int CControl::Init()
{
	mPlayStatus = 1;
	mChannelStatus = gs_SystemSetting.miStartUp_orignal;
	mVolume = gs_SystemSetting.miStartUp_vol;
	return 0;
}

void CControl::HandlePAD(int sockfd,int cmd)
{
#if USE_MODULE_DLNA && USE_DLNA_PAD
	switch(cmd)
	{
		case HEARTBEAT_REQ:
			break;

		case SYNCSONG_REQ:				//同步歌曲请求
			SyncSongReq(sockfd);
			break;

		case INSERTSONG_REQ:			//点歌请求
			InsertSongReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_SELECT_SONG);
			break;

		case DELETESONG_REQ:			//删歌请求
			DeleteSongReq(sockfd);
			break;

		case MOVESONG_REQ:				//移歌请求
			MoveSongReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_SELECT_SONG);
			break;

		case PAUSE_REQ:					//暂停请求
			PauseSongReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_PAUSE);
			break;

		case RESUME_REQ:				//播放请求
			ResumeSongReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_RESUME);
			break;

		case REPLAY_REQ:				//重唱请求
			RePlaySongReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_START);
			break;

		case SWITCHSONG_REQ:			//切歌请求
			SwitchSongReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_SWITCH_SONG);
			break;

		case SWITCHCHANNEL_REQ:			//切换声道请求
			SwitchChannelReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_AUDIO_TRACK);
			break;

		case SETVOLUME_REQ:				//设置音量请求
			SetVolReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_VOLUME);
			break;

		case AMBIENCE_REQ:				//发送气氛图片请求
			AmbienceReq(sockfd);
			break;
		case GRAFFITO_REQ:
			GraffitoReq(sockfd);
			break;

		case CHEERS_REQ:				//喝彩请求
			CheersReq(sockfd);
			break;

		case HOOTING_REQ:				//倒彩请求
			HootingReq(sockfd);
			break;

		case SCENE_REQ:			//情景请求
			SceneReq(sockfd);
			break;
		case QUIT_SCENE_REQ:			//退出情景请求
			gPlayerInfo->StopSituation();
			break;

		case START_SCORE_REQ:			//开始评分请求
			StartScoreReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_START_SCORE);
			break;

		case STOP_SCORE_REQ:			//停止评分请求
			StopScroeReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_STOP_SCORE);
			break;

		case START_GUIDE_REQ:			//开始导唱请求
			StartGuideReq(sockfd);
			break;

		case STOP_GUIDE_REQ:			//停止导唱请求
			StopGuideReq(sockfd);
			break;

		case START_RECORD_REQ:			//开始录音请求
			StartRecordReq(sockfd);
			break;

		case STOP_RECORD_REQ:			//停止录音请求
			StopRecordReq(sockfd);
			break;

		case PLAY_RECORD_REQ:			//回放录音请求
			break;

		case ZHENGGU_REQ:				//整蛊请求
			break;

		case HESHENG_REQ:				//和声请求
			break;

		case CHANGJIANG_REQ:			//唱将请求
			break;

		case GAOGUAI_REQ:				//搞怪请求
			break;

		case SET_MICRO_VOL_REQ:			//设置麦克风请求
			break;

		case SET_AMP_VOL_REQ:			//设置功放请求
			break;

		case SET_TONE_VOL_REQ:			//设置音调请求
			break;

		case UDISK_INFO_REQ:			//获取U盘信息请求
			UdiskInfoReq(sockfd);
			break;

		case PLAY_UDISK_REQ:			//播放U盘信息请求
			PlayUdiskSongReq(sockfd);
			break;

		case SEARCHSINGEDSONG_REQ:		//查询已唱列表
			SearchSingedSongReq(sockfd, SEARCHSINGEDSONG_RESP);
			break;
		case MOVESELTOTOP_REQ:
			MoveSongTopReq(sockfd);
			dmr_process_uievent(DMR_EVENT_UICHANGE_SELECT_SONG);	
			break;
		case PLAYINGINFO_REQ:
			SynPlayingSongInfoReq(sockfd,cmd);	
			break;
		case MUTE_REQ:
			SynSetMute(sockfd,1);
			break;
		case UNMUTE_REQ:
			SynSetMute(sockfd,0);
			break;
		case SINGERTYPE_REQ:
			SynTypeReq(sockfd,cmd);
			break;
		case SONGTYPE_REQ:
			SynTypeReq(sockfd,cmd);
			break;
		case LANGUAGETYPE_REQ:
			SynTypeReq(sockfd,cmd);
			break;
		case RANKTYPE_REQ:
			SynTypeReq(sockfd,cmd);
			break;
		case SCENETYPE_REQ:
		case AMBIENCETYPE_REQ:
			SynTypeReq(sockfd,cmd);
			break;
		case SINGER_LIST_REQ:
			SynSingerList(sockfd,cmd);		
			break;
		case SONG_LIST_REQ:
		case SONG_LIST_BYSINGERID_REQ:
		case SONG_LIST_BYLANGTYPE_REQ:
		case SONG_LIST_BYSONGTYPE_REQ:  
		case SONG_LIST_BYRANKTYPE_REQ:    
		case NEWSONG_LIST_REQ:
		case SONG_LIST_BYLYRIC_REQ:
		case LOCALSONG_LIST_REQ:
		case PUBSONG_LIST_REQ:
		case DOWNLOADING_LIST_REQ:
		case DOWNLOADED_LIST_REQ:
		case SONG_LIST_BYFAV_REQ:
			SynSongList(sockfd,cmd);
			break;       
		case SONGINFO_REQ:
			SynSongInfo(sockfd,cmd);
			break;
		case GETNEXT_SONG_REQ:
			SynNextSong(sockfd);
			break;
    		case SETPUB_REQ:
              		SetPubSongReq(sockfd);
                      break;
		case PUBSONG_DEL_REQ:
			PubSongDelReq(sockfd);
			break;
		case PUBSONG_MOVETOP_REQ:
			PubSongMoveTopReq(sockfd);
			break;
		case DOWNLOADING_MOVETOP_REQ:
			DownloadingSongMoveTopReq(sockfd);
			break;
		case FAV_MOVETOP_REQ:
			FavSongMoveTopReq(sockfd);
			break;
		case FAVSONG_ADD_REQ:
			AddFavSongReq(sockfd);
			break;
		case FAVSONG_DEL_REQ:
			DelFavSongReq(sockfd);
			break;
		case DELDOWNLOADINGSONG_REQ:
			DeleteDownloadingSongReq(sockfd);
      break;
//		case DELSINGEDSONG_REQ:
//			DeleteSingedSongReq(sockfd);
//			break;
		case PLAYSONG_REQ:
			//PlaySongReq(sockfd);
			break;
		case VOLUMEUP_REQ:
			VolumeReq(sockfd,1);
			break;
		case VOLUMEDOWN_REQ: 
			VolumeReq(sockfd,0);
			break;
		default:
			printf("the cmd[%d] is not define.\n",cmd);
			break;
	}
#endif
}
int  CControl::DeleteDownloadingSongReq(int sockfd)
{
	int ret = 0;
	char songId[8+1] = {0};

	if ( gpXml->GetSongId(songId) == -1 )
	{
		printf("the GetSongId of Delete singed Song is error.\n");
		ret =  -1;
	}
	else
	{
		gPlayerInfo->DelSongFromCloudListByID(songId);
	}
	return ret;
}

int CControl::VolumeReq(int sockfd,int action)
{
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
	int vol = gMainCtrlPage->GetVolume();
#elif NEW_VERB
	int vol = gMainCtrlPage->GetVolume();
#else
	int vol = gMainCtrlPage->GetVolumePos();
#endif

	if(action==1)
	{
		vol+=5;
		vol = (vol > 100) ? 100 : vol;
	}
	else if(action==0)
	{
		vol-=5;
		vol = (vol < 0) ? 0 : vol;
	}

	gPlayerInfo->SetPadChangeState(TRUE);
        gPlayerInfo->SetVolume(vol);
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
        gMainCtrlPage->SetVolume(vol, TRUE);
#elif NEW_VERB
        gMainCtrlPage->SetVolume(vol, TRUE, FALSE);
 #else
        if(vol == 0)
        {
           gMainCtrlPage->SetMute(TRUE);
        }
        else{
                gMainCtrlPage->SetMute(FALSE);
                gMainCtrlPage->SetVolumePos(vol);
        }
 #endif
        gPlayerInfo->SetPadChangeState(FALSE);

        return 0;

}
int CControl::AddFavSongReq(int sockfd){
	int ret = 0;
        char songId[16]={0};
	char buf[4096] = {0};
        if(gpXml->GetSongId(songId) == -1)
        {
                return -1;
        }
        ret = FastSearch_AddSongToFavorit("",songId);
	if(ret ==0){
		sprintf(buf,"{\"code\":\"0\",\"description\":\"Success\"}");
	}else if(ret ==1){
		sprintf(buf,"{\"code\":\"-1\",\"description\":\"Already exists\"}");
	}else{
		sprintf(buf,"{\"code\":\"-1\",\"description\":\"Error\"}");
	}

	ret = SendMsgToPad(sockfd,buf,strlen(buf));
	return ret;
}

int CControl::DelFavSongReq(int sockfd)
{
        int ret = 0;
        char songId[16]={0};
        if(gpXml->GetSongId(songId) == -1)
        {
                return -1;
        }
        FastSearch_DelSongFromFavorit("",songId);
        return ret;
}
int CControl::PubSongDelReq(int sockfd){
	char songId[8+1] = {0};
        if(gpXml->GetSongId(songId) == -1)
        {
                printf("the GetSongId of DeleteSong is error.\n");
                return -1;
        }
	FastSearch_DelPubSong(songId);
	return 0;
}
int CControl::PubSongMoveTopReq(int sockfd)
{
	char songId[8+1] = {0};

        if(gpXml->GetSongId(songId) == -1)
        {
        	printf("the GetSongId of DeleteSong is error.\n");
                return -1;
        }
        printf("pubsong move top songId=%s\r\n",songId);
	FastSearch_MovePubSongTop(songId);
	return 0;
}

int CControl::DownloadingSongMoveTopReq(int sockfd)
{
        char songId[8+1] = {0};

        if(gpXml->GetSongId(songId) == -1)
        {
        	printf("the GetSongId of DeleteSong is error.\n");
                return -1;
        }
        printf("pubsong move top songId=%s\r\n",songId);
        FastSearch_MoveDownloadingSongTop(songId);

        return 0;
}

int CControl::FavSongMoveTopReq(int sockfd)
{
        char songId[8+1] = {0};

        if(gpXml->GetSongId(songId) == -1)
        {
        	printf("the GetSongId of DeleteSong is error.\n");
                return -1;
        }
        printf("pubsong move top songId=%s\r\n",songId);
        FastSearch_MoveFavSongTop(songId);

        return 0;
}

int CControl::SetPubSongReq(int sockfd){
	int ret = 0;
	char songId[16]={0};
	char buf[4096] = {0};
        if(gpXml->GetStr(SONG_ID,songId) == 0){

        }
        printf("setpub songid=%s\r\n",songId);
	SONGINFO song;
        ret = FastSearch_GetSongInfoByID(songId, &song);
        if(ret==0){
		int result = -1;
		//if(song.bCloud==0){
		if ( song.bExist ) {
			result = FastSearch_AddPubSong(&song);
			if(result==0){
				FastSearch_SavePubBroadCast();
			}
			
		}

		if(result ==0){
                	sprintf(buf,"{\"code\":\"0\",\"description\":\"Success\"}");
        	}else if(result ==1){
                	sprintf(buf,"{\"code\":\"-1\",\"description\":\"Already exists\"}");
        	}else{
                	sprintf(buf,"{\"code\":\"-1\",\"description\":\"Error\"}");
        	}
		
	}else{
		sprintf(buf,"{\"code\":\"-1\",\"description\":\"Error\"}");
	}
	ret = SendMsgToPad(sockfd,buf,strlen(buf));

	return ret;
}
int CControl::SynNextSong(int sockfd)
{
//	char songInfo[512];
//        char songId[8+1] = {0};
//	char imgpath[64];
//        SONGINFO song;
        int len = 0;
	/*
	SONGINFO* ptr = gPlayerInfo->GetNext();
	if(ptr){
		int idx;
        	memset(imgpath,0,sizeof(imgpath));
        	sprintf(imgpath,"http://%s/song_textures/%s",gPlayerInfo->GetIp(),ptr->cPicPath);
        	memset(songInfo,0,sizeof(songInfo));
        	sprintf(songInfo,"{\"vID\":\"%s\",\"name\":\"%s\",\"singers\":[{\"sID\":0,\"name\":\"%s\"}],\"version\":\"\",\"image\":{\"imageID\":1,\"imageURL\":\"%s\"},\"playURL\":\"http://%s%s\",\"songbURL\":\"http://%s%s\",\"frameURL\":\"http://%s%s\"}",ptr->cID,ptr->cName,ptr->cSinger,imgpath,gPlayerInfo->GetIp(),ptr->cFilePath,gPlayerInfo->GetIp(),ptr->cSongBUrl,gPlayerInfo->GetIp(),ptr->cFrameUrl);
        }else{
		sprintf(songInfo,"{}");
	}
	len = SendMsgToPad(sockfd,songInfo,strlen(songInfo));
       */
	 return len;

}
int CControl::SceneReq(int sockfd)
{
	int type=0;
	if(gpXml->GetInt(SONG_TYPE,&type) == -1)
        {
		type = 0;
        }
	type++;
	gPlayerInfo->SetPadChangeState(TRUE);
	switch(type){
		case MEETING_SCENE: //meeting scene
        		gPlayerInfo->StartSituation("./KTV_Data/osdmpg/Sesson.mpg","./KTV_Data/osdmpg/Sesson.mpg");
			break;
                case SPLACE_SCENE:                  //工地情景请求
                        gPlayerInfo->StartSituation("./KTV_Data/osdmpg/Place.mpg","./KTV_Data/osdmpg/Place.mpg");
			break;
                case PARK_SCENE:                    //停车场情景请求
                        gPlayerInfo->StartSituation("./KTV_Data/osdmpg/Station.mpg","./KTV_Data/osdmpg/Station.mpg");
                        break;

                case ROAD_SCENE:                    //马路情景请求
                        gPlayerInfo->StartSituation("./KTV_Data/osdmpg/Road.mpg","./KTV_Data/osdmpg/Road.mpg");
                        break;

                case MAJIANG_SCENE:                 //麻将情景请求
                        gPlayerInfo->StartSituation("./KTV_Data/osdmpg/Majiang.mpg","./KTV_Data/osdmpg/Majiang.mpg");
			break;
	}
	gPlayerInfo->SetPadChangeState(FALSE);
	return 0;
}
int CControl::SynSongInfo(int sockfd,int cmd)
{
	char songInfo[512];
	char songId[8+1] = {0};
	SONGINFO song;
	int len = 0;

	if(gpXml->GetSongId(songId) == -1)
	{
		printf("the GetSongId of DeleteSong is error.\n");
		return -1;
	}

	FastSearch_GetSongInfoByID(songId, &song);
	sprintf(songInfo,"[{\"linkTypeID\":1,\"quality\":1,\"playURL\":\"http://%s%s\",\"description\":\"\"}]",gPlayerInfo->GetIp(), ((song.bExist) ? song.cFilePath : "") );
	len = SendMsgToPad(sockfd,songInfo,strlen(songInfo));

	return len;
}
int CControl::SynSongList(int sockfd,int cmd)
{
	char buf[20480] = {0};
	char songInfo[512];
	char keyword[128] = {0};
	char spell[128] = {0};
	char lyric[128] = {0};
	char imgpath[64]={0};

	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	int pageNum=1;
	int Num = 20;
	int len=0;
	int tagid = 0;
	char singerid[32]={0};
	int wordcount = 0;
	CPtrListCtrl *pList;
	CPtrArrayCtrl sSongList;
	SONGINFO *ptr;
	SEARCHITEM *pSubItem;
	int singerID = 0;
	if(gpXml->GetPageNum(&pageNum) == -1)
		pageNum = 1;

	if(gpXml->GetNum(&Num) == -1)
		Num = 20;

	if(gpXml->GetStr(KEYWORD,keyword) == 0)
	{
		searchinfo.eInputMethodType = INPUTMETHODTYPE_HANDWRITE;
		strcpy(searchinfo.cInputContent,keyword);
	}

	if(gpXml->GetStr(SPELL,spell) == 0)
	{
		searchinfo.eInputMethodType = INPUTMETHODTYPE_SPELL;
		strcpy(searchinfo.cInputContent,spell);
	}

	if(gpXml->GetStr(LYRIC,lyric) == 0)
	{
		searchinfo.eInputMethodType = INPUTMETHODTYPE_SPELL;
		strcpy(searchinfo.cInputContent,lyric);
	}

	if(gpXml->GetInt(SONG_WORDCOUNT,&wordcount) == 0)
	{
		searchinfo.eWordLengthType = (eWORDLENGTHTYPE)wordcount;
	}
	searchinfo.eWordLengthType = WordLength_All;

	switch(cmd)
	{
	case SONG_LIST_BYSINGERID_REQ:
		SINGERINFO *pSingerInfo;
		gpXml->GetStr(SINGERID,singerid);
		printf("singerid =%s\n",singerid);
		singerID = atoi(singerid);
		FastSearch_SearchSongListBySingerName(
			&singerID,
			&searchinfo,
			&sSongList);
		break;

	case SONG_LIST_BYLANGTYPE_REQ:
		if(gpXml->GetInt(TAGID,&tagid) == -1)
		{
			tagid = 0;
		}
		FastSearch_SearchSongListByLanguageType(
			&tagid,
			&searchinfo,
			&sSongList);
		break;

	case SONG_LIST_BYSONGTYPE_REQ:
		if(gpXml->GetInt(TAGID,&tagid) == -1)
			tagid = 0;

		//printf("cType =%s\n",pSubItem->cItemName);
		FastSearch_SearchSongListBySongType(
                       &tagid,
                       &searchinfo,
                       &sSongList);
		break;

	case SONG_LIST_BYRANKTYPE_REQ:
		if(gpXml->GetInt(TAGID,&tagid) == -1)
			tagid = 0;

		FastSearch_SearchSongListByRanking(
			&tagid,
			&searchinfo,
			&sSongList);
		break;

	case NEWSONG_LIST_REQ:
		FastSearch_SearchNewSongList(
			&searchinfo,
			&sSongList);
			break;

	case SONG_LIST_BYLYRIC_REQ:
		//printf("lyric=%s,searchinfo.cInputContent=%s\n",lyric,searchinfo.cInputContent);
		if (strlen(searchinfo.cInputContent) > 0)
		{
			//printf("search lyric =%s\n",searchinfo.cInputContent);
			FastSearch_SearchSongListByLyric(
				searchinfo.cInputContent,
				&searchinfo,
				&sSongList);
		}
		break;
		case PUBSONG_LIST_REQ:
			FastSearch_SearchPubBroadCastSongList(&searchinfo,&sSongList);
			break;
		case DOWNLOADING_LIST_REQ:
		{
			SONGINFO *pCloudingSong = gPlayerInfo->GetCurDowningInfo();
			if ( pCloudingSong )
				sSongList.AddData(pCloudingSong);

			int iCloudSongCount = gPlayerInfo->GetCloudSongCount();
			int i = 0;
			for ( i = 0; i < iCloudSongCount; ++i )
			{
				SONGINFO *pCloudSong = (SONGINFO *)gPlayerInfo->GetCloudSongInfoByIndex(i);
				if ( !pCloudSong )
					continue;

				sSongList.AddData(pCloudSong);
			}
		}
			break;
		case DOWNLOADED_LIST_REQ:
			FastSearch_SearchDownloadedSongList(&searchinfo,&sSongList);		
			break;
		case SONG_LIST_BYFAV_REQ:
			FastSearch_SearchSongListByFavorit(&searchinfo,&sSongList);
			break;
	}

	int iCount = sSongList.GetCount();
	sprintf(buf,"{\"totalRecordCount\":%d,\"songList\":[",iCount);
	int i =0;
	int count = pageNum*Num<iCount?pageNum*Num:iCount;
	 int cloud = 0;
	int percent = 0;
	for( i=(pageNum-1)*Num; i<count; i++)
	{
		ptr = (SONGINFO *)sSongList.GetAt(i);
		if(ptr == NULL)
			continue;

		memset(imgpath,0,sizeof(imgpath));
//		if(strlen(ptr->cPicPath)>0)
//		{
//			//sprintf(cImgPath,"%s_1.jpg",pSongInfo->cID);
//			sprintf(imgpath,"http://%s/KTV_Data/song_textures/%s_1.jpg",gPlayerInfo->GetIp(),ptr->cID);
//		}
		memset(songInfo,0,sizeof(songInfo));
		
			//cloud = ptr->bCloud;
		if ( ptr->bCloud && !ptr->bExist )
			cloud = 1;
		else
			cloud = 0;

		if ( (pageNum == 1) && (i == 0) )
		{
			percent = gPlayerInfo->GetDownProgress();
		}
		else
		{
			percent = 0;
		}

		char url[128]={0};
			//sprintf(url,"http://%s%s",gPlayerInfo->GetIp(),ptr->cFilePath);
		char *singer = FastSearch_GetSingerByID(ptr->singerID);
		snprintf(url, sizeof(url), "http://%s%s" , gPlayerInfo->GetIp(), ((ptr->bExist) ? ptr->cFilePath : "") );
		 sprintf(songInfo,"{\"vID\":\"%s\",\"name\":\"%s\",\"singers\":[{\"sID\":0,\"name\":\"%s\"}],\"version\":\"\",\"image\":{\"imageID\":1,\"imageURL\":\"%s\"},\"playinfo\":[{\"linkTypeID\":1,\"quality\":1,\"playURL\":\"%s\",\"description\":\"\"}],\"iscloud\":%d,\"percent\":%d,\"size\":%d}",ptr->cID,ptr->cName,singer,imgpath,url,cloud,percent,0);
		strcat(buf,songInfo);
		if(i< count -1)
		{
			strcat(buf,",");
		}

	}

	strcat(buf,"]}");
	//printf("buf len = %d\n",strlen(buf));
	len = SendMsgToPad(sockfd,buf,strlen(buf));

	return len;
}
int CControl::SynSingerList(int sockfd,int cmd)
{
	char buf[20480] = {0};
	char singerInfo[512];
	char keyword[128] = {0};
	char spell[128] = {0};
	char imgpath[256]={0};
	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	int pageNum=1;
	int Num = 20;
	int tag=0;
	int len=0;
	SINGERINFO *ptr;
	CPtrArrayCtrl sSingerList;
	char cItemName[64]={0};

	if(gpXml->GetSingerType(&tag) == -1)
		tag = 0;

	if(gpXml->GetPageNum(&pageNum) == -1)
		pageNum = 1;

	if(gpXml->GetNum(&Num) == -1)
		Num = 20;

	if(gpXml->GetStr(KEYWORD,keyword) == 0)
	{
		searchinfo.eInputMethodType = INPUTMETHODTYPE_HANDWRITE;
		strcpy(searchinfo.cInputContent,keyword);
	}
	if(gpXml->GetStr(SPELL,spell) == 0)
	{
		searchinfo.eInputMethodType = INPUTMETHODTYPE_SPELL;
		strcpy(searchinfo.cInputContent,spell);
	}

	CPtrListCtrl *pList = &gSearchSingerType.sChildItemList;
	if(pList->GetCount()>0)
	{
		if(tag >=pList->GetCount())
			tag = 0;
		if(tag == 0) {
			strcpy(cItemName,"所有");
		}else if(tag == 1) {
			strcpy(cItemName,"大陆男");
		}else if(tag == 2) {
			strcpy(cItemName,"大陆女");
		}else if(tag == 3) {
			strcpy(cItemName,"港台男");
		}else if(tag == 4) {
			strcpy(cItemName,"港台女");
		}else if (tag == 5) {
			strcpy(cItemName,"外国男");
		}else if(tag == 6) {
			strcpy(cItemName,"外国女");
		}else if(tag == 7) {
			strcpy(cItemName,"组合");
		}
		
		SEARCHITEM *pSubItem = (SEARCHITEM *)pList->GetAt(tag);
		if(pSubItem) {
			printf("cType =%s\n",cItemName);
	
			FastSearch_SearchSingerListBySingerType(
				cItemName,
				&searchinfo,
				&sSingerList);
		}
	}
	int iCount = sSingerList.GetCount();
	sprintf(buf,"{\"totalRecordCount\":%d,\"singerList\":[",iCount);
	int i =0;
	int count = pageNum*Num<iCount?pageNum*Num:iCount;
	printf("count =%d\n",count);
	for(i=(pageNum-1)*Num; i<count; i++)
	{
		ptr = (SINGERINFO *)sSingerList.GetAt(i);
		if(ptr == NULL)
			continue;

		int idx;
		//sscanf(ptr->cPicPath,"%d.jpg",&idx);
		memset(imgpath,0,sizeof(imgpath));
		sprintf(imgpath,"http://%s/KTV_Data/singer_textures/%d.jpg",gPlayerInfo->GetIp(),ptr->id);
		memset(singerInfo,0,sizeof(singerInfo));
		sprintf(singerInfo,"{\"sID\":\"%d\",\"name\":\"%s\",\"songsTotal\":%d,\"image\":{\"imageID\":1,\"imageURL\":\"%s\"}}",ptr->id,ptr->cName,ptr->wSongsCount,imgpath);
		strcat(buf,singerInfo);
		if(i< count -1)
		{
			strcat(buf,",");
		}

	}

	strcat(buf,"]}");
	len = SendMsgToPad(sockfd,buf,strlen(buf));

	return len;
}

int  CControl::SynSetMute(int sockfd,int mute)
{
	gPlayerInfo->SetPadChangeState(TRUE);

	if(mute==1)
	{
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
		gMainCtrlPage->SetMute(TRUE, TRUE);
#elif NEW_VERB
		gMainCtrlPage->SetMuteState(TRUE, TRUE, FALSE);
#else
		gMainCtrlPage->SetMute(TRUE);
#endif
		gPlayerInfo->SetVolume(0);
	}
	else if(mute ==0)
	{
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
		gMainCtrlPage->SetMute(FALSE, TRUE);
		gPlayerInfo->SetVolume(gMainCtrlPage->GetVolume());
#elif NEW_VERB
		gMainCtrlPage->SetMuteState(FALSE, TRUE, FALSE);
		gPlayerInfo->SetVolume(gMainCtrlPage->GetVolume());
#else
		gMainCtrlPage->SetMute(FALSE);
		gPlayerInfo->SetVolume(gMainCtrlPage->GetVolumePos());
#endif
	}
	gPlayerInfo->SetPadChangeState(FALSE);

	return 0;
}

int  CControl::SynTypeReq(int sockfd,int cmd)
{
	int ret = 0;
	char buf[4096] = {0};
	char typeinfo[128]={0};
	CPtrListCtrl *pList = NULL;
	switch(cmd)
	{
		case SINGERTYPE_REQ:
			pList = &gSearchSingerType.sChildItemList;
			break;
		case SONGTYPE_REQ:
			pList = &gSearchSongType.sChildItemList;
			break;
		case LANGUAGETYPE_REQ:
			pList = &gSearchLanguageType.sChildItemList;
			break;
		case RANKTYPE_REQ:
			pList = &gSearchRankType.sChildItemList;
			break;
		case SCENETYPE_REQ:
		//	pList = &gSearchSceneType.sChildItemList;
			return 0;
			break;
		case AMBIENCETYPE_REQ:
		//	pList = &gSearchAmbienceType.sChildItemList;
			return 0;
			break;
	}
	int count = pList->GetCount();
	strcat(buf,"[");
	for(int i=0;i<count;i++)
	{
		SEARCHITEM *pSubItem = (SEARCHITEM *)pList->GetAt(i);
		//printf("cType =%s\n",pSubItem->cItemName);
		memset(typeinfo,0,sizeof(typeinfo));
		if(cmd == SCENETYPE_REQ)
		{
			char name[32]={0};
			sprintf(name,"/media/songs/scene%d.wav",i);
			sprintf(typeinfo,"{\"tagID\":%d,\"tagName\":\"%s\",\"url\":\"http://%s/%s\"}",i,pSubItem->cItemName,gPlayerInfo->GetIp(),name);
		}
		else if(cmd == AMBIENCETYPE_REQ)
		{
			char name[32]={0};
			sprintf(name,"/media/songs/ambience%d.wav",i);
			sprintf(typeinfo,"{\"tagID\":%d,\"tagName\":\"%s\",\"url\":\"http://%s/%s\"}",i,pSubItem->cItemName,gPlayerInfo->GetIp(),name);
		}
		else
		{
			sprintf(typeinfo,"{\"tagID\":%d,\"tagName\":\"%s\"}",i,pSubItem->cItemName);
		}
		if(i<count-1)
		{
			strcat(typeinfo,",");
		}
		strcat(buf,typeinfo);
	}

	strcat(buf,"]");
	ret = SendMsgToPad(sockfd,buf,strlen(buf));

	return ret;
}
int  CControl::SynSongTypeReq(int sockfd,int cmd)
{
	return 0;
}
int  CControl::SynLanguageTypeReq(int sockfd,int cmd)
{
	return 0;
}
int  CControl::SynRankTypeReq(int sockfd,int cmd)
{
	return 0;
}

int CControl::SyncSongReq(int sockfd)
{
	int ret = 0;
	if(sockfd < 0)
	{
		printf("\r\nError: the sockfd[%d] of sync song reponse is wrong.\n",sockfd);
		return -1;
	}

	ret = SendSyncSongToPad(sockfd,SYNCSONG_RESP);

	return ret;
}

int CControl::SynPlayingSongInfoReq(int sockfd,int cmd)
{
	int ret = 0;
#if defined(__arm__) || defined(__mips__)
	char buf[4096] = {0};
	char songInfo[512];
	char imgpath[64]={0};
	char url[64]={0};
	SONGINFO *ptr = NULL;

	memset(buf,0,sizeof(buf));
	strcat(buf,"{");
	ptr = gPlayerInfo->GetCurPlaySongInfo();
	if(ptr != NULL)
	{
		int pos=0;
		GetCurPosition(&pos);
		memset(imgpath,0,sizeof(imgpath));
		//if(strlen(ptr->cPicPath)>0)
		{
			//sprintf(imgpath,"http://%s/song_textures/%s",gPlayerInfo->GetIp(),ptr->cPicPath);
		}
		sprintf(url,"http://%s%s",gPlayerInfo->GetIp(),ptr->cFilePath);
		memset(songInfo,0,sizeof(songInfo));
		//sprintf(songInfo,"\"vID\":\"%s\",\"name\":\"%s\",\"singers\":[{\"sID\":0,\"name\":\"%s\"}],\"version\":\"\",\"pos\":%d,\"image\":{\"imageID\":1,\"imageURL\":\"%s\"},\"url\":\"%s\",",ptr->cID,ptr->cName,ptr->cSinger,pos,imgpath,url);
		strcat(buf,songInfo);

		memset(songInfo,0,sizeof(songInfo));
		sprintf(songInfo,"\"playstate\":%d,\"vol\":%d,\"channel\":%d",gPlayerInfo->GetPlayState(),gPlayerInfo->GetVolume(),gPlayerInfo->GetOriginalState());
		strcat(buf,songInfo);
	}
	strcat(buf,"}");
	ret = SendMsgToPad(sockfd,buf,strlen(buf));
#endif
	return ret;
}


int CControl::InsertSongReq(int sockfd)
{
	int ret = 0;
	SONGINFO song;

	char buf[4096] = {0};
	if(gpXml->GetInsertSongId(song.cID) == -1)
	{
		printf("the GetSongId of DeleteSong is error.\n");
		sprintf(buf,"{\"code\":\"-1\",\"description\":\"Error\"}");
		ret = SendMsgToPad(sockfd,buf,strlen(buf));
		return -1;
	}

	FastSearch_GetSongInfoByID(song.cID, &song);
	if ( !song.bExist && song.bCloud ){
		sprintf(buf,"{\"code\":\"0\",\"description\":\"Downloading\"}");

	}else{
		sprintf(buf,"{\"code\":\"0\",\"description\":\"Success\"}");
	}
//	if(gpXml->GetSong(&song) == -1)
//	{
//		printf("the GetSongId of DeleteSong is error.\n");
//		return -1;
//	}
	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->AddSongToSelectedList(&song,-1, TRUE);
	gPlayerInfo->SetPadChangeState(FALSE);

	//TODO:send sync to other pad
	ret = SendMsgToPad(sockfd,buf,strlen(buf));
	return ret;
}

int CControl::DeleteSongReq(int sockfd)
{
	int ret = 0;
	char songId[8+1] = {0};

	if(gpXml->GetSongId(songId) == -1)
	{
		printf("the GetSongId of DeleteSong is error.\n");
		return -1;
	}

	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->DelSongFromSelectedList(songId);
	gPlayerInfo->SetPadChangeState(FALSE);

	return ret;
}

int CControl::MoveSongReq(int sockfd)
{
	int ret = 0;
	char id[8+1] = {0};
	int srcIdx;
	int descIdx;

	if(gpXml->GetMoveInfo(id,&srcIdx,&descIdx) == -1)
	{
		printf("the GetSongId of MoveSong is error.\n");
		return -1;
	}

	gPlayerInfo->SetPadChangeState(TRUE);
	if(srcIdx > descIdx)
	{
		//向上移
		gPlayerInfo->MoveSelectedListItem(srcIdx-1,descIdx-2);
	}
	else if(srcIdx < descIdx)
	{
		//向下移
		gPlayerInfo->MoveSelectedListItem(srcIdx-1,descIdx-1);
	}
	gPlayerInfo->SetPadChangeState(FALSE);
	
	return ret;
}

int CControl::MoveSongTopReq(int sockfd)
{
        int ret = 0;

	char songId[8+1] = {0};

	if(gpXml->GetSongId(songId) == -1)
	{
		printf("the GetSongId of DeleteSong is error.\n");
		return -1;
	}

        gPlayerInfo->SetPadChangeState(TRUE);
	
	if (gPlayerInfo->IsSongInSelectedList(songId))
                gPlayerInfo->MoveSelectedSongToFirst(songId);
	else {
		SONGINFO song;
		FastSearch_GetSongInfoByID(songId, &song);
		gPlayerInfo->AddSongToSelectedList(&song, 0, TRUE);
	}

	gPlayerInfo->SetPadChangeState(FALSE);

        return ret;
}

int CControl::PauseSongReq(int sockfd)
{
	int ret = 0;

	mPlayStatus = 0;
	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->Pause();
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
	gMainCtrlPage->SetPlayingState(FALSE, TRUE);
#elif NEW_VERB
	gMainCtrlPage->SetPalyStatus(FALSE, TRUE, FALSE);
#else
	gMainCtrlPage->SetPalyState(TRUE);
#endif
	gPlayerInfo->SetPadChangeState(FALSE);

	return ret;
}

int CControl::ResumeSongReq(int sockfd)
{
	int ret = 0;

	mPlayStatus = 1;
	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->Resume();
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
	gMainCtrlPage->SetPlayingState(TRUE);
#elif NEW_VERB
	gMainCtrlPage->SetPalyStatus(TRUE, TRUE, FALSE);
#else
	gMainCtrlPage->SetPalyState(FALSE);
#endif
	gPlayerInfo->SetPadChangeState(FALSE);
	
	return ret;
}


int CControl::RePlaySongReq(int sockfd)
{
	int ret = 0;

	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->Replay();
	gPlayerInfo->SetPadChangeState(FALSE);

	return ret;
}

int CControl::SwitchSongReq(int sockfd)
{
	int ret = 0;

	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->PlayNext(TRUE);
	gPlayerInfo->SetPadChangeState(FALSE);

	return ret;
}

int CControl::SetVolReq(int sockfd)
{
	int ret = 0;
	int vol;

	if(gpXml->GetVolue(&vol) == -1)
	{
		printf("the GetVolue of SetVol is error.\n");
		return -1;
	}

	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->SetVolume(vol);

	if ( vol > 100 || vol < 0)
	{
		return -1;
	}
	else if(vol == 0)
	{
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
		gMainCtrlPage->SetMute(TRUE, TRUE);
#elif NEW_VERB
		gMainCtrlPage->SetVolume(vol, TRUE, FALSE);
#else
		gMainCtrlPage->SetMute(TRUE);
#endif
	}
	else{
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
		gMainCtrlPage->SetMute(FALSE, TRUE);
		gMainCtrlPage->SetVolume(vol, TRUE);
#elif NEW_VERB
		gMainCtrlPage->SetVolume(vol, TRUE, FALSE);
#else
		gMainCtrlPage->SetMute(FALSE);
		gMainCtrlPage->SetVolumePos(vol);
#endif
	}
	gPlayerInfo->SetPadChangeState(FALSE);

	return ret;
}

int CControl::SwitchChannelReq(int sockfd)
{
	int ret = 0;
//	mChannelStatus = !mChannelStatus;
	if(gpXml->GetAudioTrack(AUDIOTRACK, &mChannelStatus) == -1)
	{
		printf("the GetAudioTrack of SwitchChannelReq is error.\n");
		return -1;
	}

	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->SetOriginal(mChannelStatus);
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
	gMainCtrlPage->SetAccompanyState((mChannelStatus) ? FALSE : TRUE, TRUE);
#elif NEW_VERB
	gMainCtrlPage->SetChannelStatus(mChannelStatus, TRUE, FALSE);
#else
	gMainCtrlPage->SetChannelState((mChannelStatus) ? FALSE : TRUE);
#endif
	gPlayerInfo->SetPadChangeState(FALSE);
	
	return ret;
}

int CControl::GraffitoReq(int sockfd)
{
	int ret = 0;
	int type = 1;
	char url[256]={0};

	if(gpXml->GetInt(SONG_TYPE,&type) == -1)
		type =1;

	gpXml->GetStr(URL,url);
	//LOGV("ktv ambience type = %d ,url=%s \r\n",type,url);
#if USE_MODULE_DLNA && USE_DLNA_PAD
	dmr_process_uievent(DMR_EVENT_UICHANGE_GRAFFITO);
#endif

	return ret;
}

int CControl::AmbienceReq(int sockfd)
{
	int ret = 0;

	/*
	if(gpXml->GetLen(&len) == -1)
	{
		printf("the GetVolue of SetVol is error.\n");
		return -1;
	}

	ReciveMsgForLen(sockfd,len);
	*/
	gPlayerInfo->Ambience(0);
	return ret;
}

int CControl::CheersReq(int sockfd)
{
	int ret  = 0;
	gPlayerInfo->Cheer(0);
	return ret;
}

int CControl::HootingReq(int sockfd)
{
	int ret  = 0;
	gPlayerInfo->Cheer(1);
	return ret;
}

int CControl::StartScoreReq(int sockfd)
{
	int ret  = 0;
	gPlayerInfo->SetPadChangeState(TRUE);
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
#elif NEW_VERB

#else
	//gMainCtrlPage->SetScoreState(TRUE);
#endif
	gPlayerInfo->StartSongMark();
	gPlayerInfo->SetPadChangeState(FALSE);
	return ret;
}

int CControl::StopScroeReq(int sockfd)
{
	int ret  = 0;
	gPlayerInfo->SetPadChangeState(TRUE);
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
#elif NEW_VERB
#else
	//gMainCtrlPage->SetScoreState(FALSE);
#endif
	gPlayerInfo->StopSongMark();
	gPlayerInfo->SetPadChangeState(FALSE);
	return ret;
}

int CControl::StartGuideReq(int sockfd)
{
	return 0;
}

int CControl::StopGuideReq(int sockfd)
{
	return 0;
}

int CControl::StartRecordReq(int sockfd)
{
	return 0;
}

int CControl::StopRecordReq(int sockfd)
{
	return 0;
}

int CControl::UdiskInfoReq(int sockfd)
{
	int ret = 0;
	char path[256] = {0};
	DIR *pDir = NULL;
	struct dirent *file = NULL;
	char buf[5000] = {0};
	char filePath[128] = {0};
	char fileInfo[256] = {0};

	memset(buf,0,sizeof(buf));
	if(gpXml->GetUdiskPath(path) == -1)
	{
		printf("the UdiskInfoReq of GetUdiskPath is error.\n");
		return -1;
	}
#if defined(__arm__)
	char cDirPath[256] = "/stb/config/media/";
#else
	char cDirPath[256] = "/media/";
#endif
	strcat(cDirPath, path);
	if((pDir = opendir(cDirPath)) == NULL)
	{
		DbgOutput(DBG_VERB,"open dirent:%s failed\n",path);
		char failed[64] = "{\"code\":0}";
		SendMsgToPad(sockfd,failed,strlen(failed));
		return -1;
	}

	//sprintf(buf,"<head><cmd>%d</cmd><items>",UDISK_INFO_RESP);
	sprintf(buf,"{\"code\":1, \"data\":[");

	while((file = readdir(pDir)) != NULL)
	{
		memset(fileInfo,0,sizeof(fileInfo));
		memset(filePath,0,sizeof(filePath));

		if(strncmp(file->d_name,".",1) == 0)
			continue;
		else if(file->d_type == DT_DIR)
		{
			sprintf(filePath,"%s/%s",path,file->d_name);
			//sprintf(fileInfo,"<filename>%s</filename><path>%s</path></item>",ISDIR,file->d_name,filePath);
			strcat(buf,fileInfo);
		}
		else if((strcasestr(file->d_name,".mpg") != NULL) 	||
				(strcasestr(file->d_name,".mkv") != NULL) 	||
				(strcasestr(file->d_name,".flv") != NULL) 	||
				(strcasestr(file->d_name,".mp4") != NULL) 	||
				(strcasestr(file->d_name,".mp3") != NULL) 	||
				(strcasestr(file->d_name,".avi") != NULL) 	||
				(strcasestr(file->d_name,".rm") != NULL) 	||
				(strcasestr(file->d_name,".rmvb") != NULL) 	||
				(strcasestr(file->d_name,".mov") != NULL) 	||
				(strcasestr(file->d_name,".ts") != NULL) 	||
				(strcasestr(file->d_name,".vob") != NULL) 	||
				(strcasestr(file->d_name,".wmv") != NULL) 	||
				(strcasestr(file->d_name,".asf") != NULL) 	||
				(strcasestr(file->d_name,".mpeg") != NULL))
		{
			sprintf(filePath,"%s/%s",path,file->d_name);
			//sprintf(fileInfo,"<item><type>%d</type><filename>%s</filename><path>%s</path></item>",ISFILE,file->d_name,filePath);
			sprintf(fileInfo,"{\"filename\":\"%s\", \"path\":\"%s\"},", file->d_name, path);
			strcat(buf,fileInfo);
		}
	}

	memset(fileInfo,0,sizeof(fileInfo));
	int itemLen = strlen(buf);
	if ( buf[itemLen-1] == ',' )
	{
		buf[itemLen-1] = ' ';
	}
	sprintf(fileInfo,"]}");
	strcat(buf,fileInfo);
	ret = SendMsgToPad(sockfd,buf,strlen(buf));

	closedir(pDir);

	return ret;
}

int CControl::PlayUdiskSongReq(int sockfd)
{
	int ret = 0;
	char path[256] = {0};
	char fileName[256] = {0};
#if defined(__arm__)
	char absFileName[256] = "/stb/config/media/";
#else
	char absFileName[256] = "/media/";
#endif

	if(gpXml->GetUdiskPath(path) == -1)
	{
		printf("the PlayUdiskSongReq of GetUdiskPath is error.\n");
		return -1;
	}

	if ( gpXml->GetUdiskFileName(fileName) == -1 )
	{
		printf("the PlayUdiskSongReq of GetUdiskFileName is error.\n");
		return -1;
	}

	strcat(absFileName, path);
	strcat(absFileName, "/");
	strcat(absFileName, fileName);

	//PlayerApi_Play(path,path,mVolume,1,mChannelStatus,1);
	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->Play_UDiskSong(absFileName,absFileName);
	gPlayerInfo->SetPadChangeState(FALSE);

	return ret;
}

int CControl::SearchSingedSongReq(int sockfd, int cmd)
{
	int len = 0;
	char buf[20480] = {0};
	char songInfo[512];
	char imgpath[64];
	char url[128];
	SONGINFO *ptr = NULL;

	if(sockfd < 0)
	{
		printf("the sockfd[%d] of SearchSingedSongReq is wrong.\n",sockfd);
		return -1;
	}
	int pageNum=1;
	int Num = 20;

	if(gpXml->GetPageNum(&pageNum) == -1)
		pageNum = 1;

	if(gpXml->GetNum(&Num) == -1)
		Num = 20;

	//printf("pageNum=%d,Num=%d\n",pageNum,Num);
	memset(buf,0,sizeof(buf));
	int iSelectedSongCount = gPlayerInfo->GetSingedSongCount();
	sprintf(buf,"{\"totalRecordCount\":%d,\"songList\":[",iSelectedSongCount);
	int i =0;
	int count = pageNum*Num<iSelectedSongCount?pageNum*Num:iSelectedSongCount;
	//printf("count =%d\n",count);
	for(i=(pageNum-1)*Num; i<count; i++)
	{
		ptr = gPlayerInfo->GetSingedSongInfoByIndex(i);
		if(ptr == NULL)
			continue;

		memset(imgpath,0,sizeof(imgpath));
//		if(strlen(ptr->cPicPath)>0)
//		{
//			sprintf(imgpath,"http://%s/song_textures/%s",gPlayerInfo->GetIp(),ptr->cPicPath);
//		}
                	//sprintf(url,"http://%s%s",gPlayerInfo->GetIp(),ptr->cFilePath);
		snprintf(url, sizeof(url), "http://%s%s",gPlayerInfo->GetIp(), ((ptr->bExist) ? ptr->cFilePath : "") );
		memset(songInfo,0,sizeof(songInfo));
              //  sprintf(songInfo,"{\"vID\":\"%s\",\"name\":\"%s\",\"singers\":[{\"sID\":0,\"name\":\"%s\"}],\"version\":\"\",\"image\":{\"imageID\":1,\"imageURL\":\"%s\"},\"playinfo\":[{\"linkTypeID\":1,\"quality\":1,\"playURL\":\"%s\",\"description\":\"\"}],\"iscloud\":%d,\"percent\":0,\"size\":%d}",ptr->cID,ptr->cName,ptr->cSinger,imgpath,url,ptr->bCloud, 0);
		strcat(buf,songInfo);
		if(i< count -1)
		{
			strcat(buf,",");
		}

	}

	strcat(buf,"]}");
	len = SendMsgToPad(sockfd,buf,strlen(buf));

	return len;
}


int CControl::GetSyncSongInfo(char *buf,int cmd)
{
	int ret = 0;
	char songInfo[512];
	char imgpath[64]={0};
	//char url[128]={0};
	SONGINFO *ptr = NULL;
	SONGINFO *playingptr = NULL;
	if(NULL == buf)
	{
		printf("\r\nError: the param of GetSyncSongInfo is NULL.\n");
		return -1;
	}
	int pageNum=1;
	int Num = 20;

	if(gpXml->GetPageNum(&pageNum) == -1)
		pageNum = 1;

	if(gpXml->GetNum(&Num) == -1)
		Num = 20;
	
	playingptr = gPlayerInfo->GetCurPlaySongInfo();
	int iSelectedSongCount = gPlayerInfo->GetSelectedSongCount();
	if(playingptr!=NULL)
	{
		sprintf(buf,"{\"totalRecordCount\":%d,\"songList\":[",iSelectedSongCount+1);
	}
	else
	{
		sprintf(buf,"{\"totalRecordCount\":%d,\"songList\":[",iSelectedSongCount);
	}

	if(playingptr != NULL)
	{
		memset(imgpath,0,sizeof(imgpath));
		//if(strlen(playingptr->cPicPath)>0)
		{
			sprintf(imgpath,"http://%s/song_textures/%s",gPlayerInfo->GetIp(),"");
		}
		memset(songInfo,0,sizeof(songInfo));
		sprintf(songInfo,"{\"vID\":\"%d\",\"name\":\"%s\",\"singers\":[{\"sID\":0,\"name\":\"%d\"}],\"version\":\"\",\"image\":{\"imageID\":1,\"imageURL\":\"%s\"},\"playinfo\":[{\"linkTypeID\":1,\"quality\":1,\"playURL\":\"http://%s%s\",\"description\":\"\"}]}",playingptr->cID,playingptr->cName,playingptr->singerID,imgpath,gPlayerInfo->GetIp(), "" );
		strcat(buf,songInfo);
		if(iSelectedSongCount>0)
		{
			strcat(buf,",");
		}
	}

	int i =0;
	int count = pageNum*Num<iSelectedSongCount?pageNum*Num:iSelectedSongCount;
	for(i=(pageNum-1)*Num; i<count; i++)
	{
		ptr = gPlayerInfo->GetSelectedSongInfoByIndex(i);
		if(ptr == NULL)
		{
			continue;
		}
		memset(imgpath,0,sizeof(imgpath));
		//if(strlen(ptr->cPicPath)>0)
		{
			sprintf(imgpath,"http://%s/song_textures/%s",gPlayerInfo->GetIp(),"");
		}
		memset(songInfo,0,sizeof(songInfo));
		char *singer = FastSearch_GetSingerByID(playingptr->singerID);
	//	sprintf(songInfo,"{\"vID\":\"%d\",\"name\":\"%s\",\"singers\":[{\"sID\":0,\"name\":\"%s\"}],\"version\":\"\",\"image\":{\"imageID\":1,\"imageURL\":\"%s\"},\"playinfo\":[{\"linkTypeID\":1,\"quality\":1,\"playURL\":\"http://%s%s\",\"description\":\"\"}]}",ptr->id,ptr->cName,singer ? singer : "",imgpath,gPlayerInfo->GetIp(), "" );
		strcat(buf,songInfo);
		if(i< count -1)
		{
			strcat(buf,",");
		}
	}
	strcat(buf,"],");
	strcat(buf,"\"selectedIds\":[");

	if(playingptr!=NULL)
	{
		memset(songInfo,0,sizeof(songInfo));
		sprintf(songInfo,"{\"vID\":\"%s\"}",playingptr->cID);
		strcat(buf,songInfo);
		if(iSelectedSongCount>0)
		{
			strcat(buf,",");
		}
	}

	for(i=0;i<iSelectedSongCount;i++)
	{
		ptr = gPlayerInfo->GetSelectedSongInfoByIndex(i);
		if(ptr == NULL)
			continue;

		memset(songInfo,0,sizeof(songInfo));
		sprintf(songInfo,"{\"vID\":\"%s\"}",ptr->cID);
		strcat(buf,songInfo);
		if(i< iSelectedSongCount -1)
		{
			strcat(buf,",");
		}
	}
	
	strcat(buf,"]}");

	return ret;
}

int CControl::SendSyncSongToPad(int sockfd, int cmd)
{
	int len = 0;
	char buf[20480] = {0};

	if(sockfd < 0)
	{
		printf("the sockfd[%d] of SendSyncSongToPad is wrong.\n",sockfd);
		return -1;
	}
	memset(buf,0,sizeof(buf));

	GetSyncSongInfo(buf,cmd);
	len = SendMsgToPad(sockfd,buf,strlen(buf));

	return len;
}

int CControl::SendSyncStatusToPad(int sockfd, int cmd,int vol)
{
	int ret = 0;
	char buf[MAX_BUFFER] = {0};

	if(cmd == PAUSE_NOTICE)
		mPlayStatus = 0;
	else if(cmd == RESUME_NOTICE)
		mPlayStatus = 1;

	if(sockfd < 0)
	{
		printf("the sockfd[%d] of SendSyncStatusToPad is wrong.\n",sockfd);
		return -1;
	}
	if(vol >= 0)
	{
		mVolume = vol;
		sprintf(buf,"<head><cmd>%d</cmd><vol>%d</vol><bank></bank></head>\n",cmd,vol);
	}
	else
	{
		sprintf(buf,"<head><cmd>%d</cmd><bank></bank></head>\n",cmd);
	}
	ret = SendMsgToPad(sockfd,buf,strlen(buf));

	return ret;
}

int CControl::SendSyncPublicSongToPad(int sockfd, int cmd)
{
	int ret = 0;
	char buf[MAX_BUFFER] = {0};
//	char cID[8] = {0};
	SONGINFO *ptr = NULL;

	if(sockfd < 0)
	{
		printf("the sockfd[%d] of SendSyncPublicSongToPad is wrong.\n",sockfd);
		return -1;
	}

	ptr = gPlayerInfo->GetCurPlaySongInfo();
	if((ptr != NULL) && strlen(ptr->cID) > 0)
	{
		sprintf(buf,"<head><cmd>%d</cmd><id>%s</id><bank></bank></head>\n",cmd,ptr->cID);
		ret = SendMsgToPad(sockfd,buf,strlen(buf));
	}

	return ret;
}

