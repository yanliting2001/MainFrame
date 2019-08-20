#include "../DlnaApi.h"
#include "DLNADefine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h> /*for struct sockaddr_in*/
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>

#include "../CommControl/MainCtrlPage.h"
#include "../OSDControl/BCM_Control.h"
#include "../PlayerApi/PlayerInfo.h"
#include "../PlayerApi/PlayerApi.h"
#if APP_ID == QIKUN500
//#include "../CommApi/LoadNetMediaInfo.h"
#elif APP_ID == QIKUN510
#if USE_DLNA_DMR
//#include "../NetTv/LoadNetMediaInfo.h"
#endif
#elif APP_ID == OEMDML3 || APP_ID == QIKUN500S
#include "../CommApi/LoadNetMediaInfo.h"
#include "../CommControl/MainCtrlPage-oem-my500.h"
#elif APP_ID == OEMHEB500
#include "../CommApi/LoadNetMediaInfo.h"
#include "../CommControl/MainCtrlPage-oem-yw500.h"
#else
#include "../NetTv/LoadNetMediaInfo.h"
#endif
#include "CommApi.h"
#include "hi_dlna_type.h"
#include "hi_dlna_api.h"


extern int GetDuration(int *filetime); 
extern int GetCurPosition(int *filetime);

#if 0
static int giDLNASock  = -1;
static int gThreadExit = 0;
CBaseThread gDlnaPostionNotifyThread;
extern CMainCtrlPage *gMainCtrlPage;

void PlayFormUrl(char *url);
int dmr_process_stop();
int dmr_process_pause(int pause);
int dmr_process_seek(int pos);
int dmr_process_mute(int mute);
int dmr_process_volume(int volume);

static int AnalyseCmdMsg(char *vCmdMsgInfo, char *vCmdMsg[], int iCmdMaxNum)
{
	if ( !vCmdMsgInfo || !vCmdMsg )
		return -1;

	int vMsgCount = 0;
	int vCmdMessageSize = strlen(vCmdMsgInfo);
	if ( vCmdMessageSize > 0 )
	{
		int vCmdMessageIndex;
		for (vCmdMessageIndex = 0; vCmdMessageIndex < iCmdMaxNum; vCmdMessageIndex ++)
		{
			vCmdMsg[vCmdMessageIndex] = NULL;
		}

		char *p, *q;
		p = vCmdMsgInfo;
		while(p < vCmdMsgInfo + vCmdMessageSize)
		{
			q = strchr(p,',');
			if( q )
			{
				*q=0;
				vCmdMsg[vMsgCount++] = p;
				p = q + 1;
			}
			else
				break;
		}
	}

	return vMsgCount;
}

LPVOID DlnaPositionNofityThreadFunc(LPVOID lpParam)
{
	CloseSock(giDLNASock);

	giDLNASock = CreateServer(UI_PORT);
	if ( giDLNASock <= 0 )
	{
		printf("Create server:%d failed\n", UI_PORT);
		return NULL;
	}

	char msg[MAX_MSG_LEN] = {0};

	while ( !gThreadExit )
	{
		memset(msg, 0, sizeof(msg));
		char *vCmdMsg[20];
	    int vMsgCount = 0;

		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		int msgsocket = accept(giDLNASock, (struct sockaddr*)&client_addr, &length);
		if ( msgsocket < 0)
		{
			if ( (errno == ECONNABORTED) || (errno == EINTR) )
				continue;
		}

		if ( RecvMsg(msgsocket, msg, sizeof(msg)) <= 0 )
		{
			CloseSock(msgsocket);
			break;
		}

		if ( (vMsgCount = AnalyseCmdMsg(msg, vCmdMsg, 20)) <= 0 )
		{
			CloseSock(msgsocket);
			continue ;
		}
		printf("====the msg,%s, CMD[0]:%s, CMD[1]:%s, CMD[2]:%s\n", msg, vCmdMsg[0], vCmdMsg[1], vCmdMsg[2]);
		if ( vCmdMsg[0] && strcmp(vCmdMsg[0], DMR2UICMD) == 0 )
		{
			if ( vCmdMsg[1] )
			{
				int cmd = atoi(vCmdMsg[1]);
				switch(cmd)
				{
				case DMR2UIEvent_StartMoniPosition:
					break;

				case DMR2UIEvent_Play:
					if ( vCmdMsg[2] )
					{
						PlayFormUrl(vCmdMsg[2]);
					}
					break;

				case DMR2UIEvent_Stop:
					dmr_process_stop();
					break;

				case DMR2UIEvent_Pause:
					dmr_process_pause(1);
					break;

				case DMR2UIEvent_Resume:
					dmr_process_pause(0);
					break;

				case DMR2UIEvent_Seek:
					if ( vCmdMsg[2] )
					{
						int pos = atoi(vCmdMsg[2]);
						dmr_process_seek(pos);
					}
					break;

				case DMR2UIEvent_Mute:
					if ( vCmdMsg[2] )
					{
						int mute = atoi(vCmdMsg[2]);
						dmr_process_mute(mute);
					}
					break;

				case DMR2UIEvent_Volue:
					if ( vCmdMsg[2] )
					{
						int vol = atoi(vCmdMsg[2]);
						dmr_process_volume(vol);
					}
					break;

				default:
					break;
				}
			}
		}

		CloseSock(msgsocket);
	}

	CloseSock(giDLNASock);

	return NULL;
}

int dmr_process_stop()
{
    // player to stop
    gPlayerInfo->StopPlay();
    return 1;
}


int dmr_process_pause(int pause)
{
	if(pause)
	{
        // here to pause the play
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
	}
	else
	{
		//BCMP_PlayResume();
	    gPlayerInfo->SetPadChangeState(TRUE);
	    gPlayerInfo->Resume();
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
	    gMainCtrlPage->SetPlayingState(TRUE, TRUE);
#elif NEW_VERB
	    gMainCtrlPage->SetPalyStatus(TRUE, TRUE, FALSE);
#else
	    gMainCtrlPage->SetPalyState(FALSE);
#endif
	    gPlayerInfo->SetPadChangeState(FALSE);
	}

    return 1;
}

int dmr_process_seek(int pos)
{
    //LOGV("seek the stream to: %ds \r\n",pos);
    PlayApi_SeekTo(pos*1000);
    return 1;
}

int dmr_process_mute(int mute)
{
    gPlayerInfo->SetPadChangeState(TRUE);
    if(mute)
    {
    	//LOGV("Player  mute:%d \r\n",mute);
    	gPlayerInfo->SetVolume(0);
#if APP_ID == OEMHEB500
    	gMainCtrlPage->SetMute(TRUE, TRUE);
#elif NEW_VERB
    	gMainCtrlPage->SetMuteState(TRUE, TRUE, FALSE);
#else
    	gMainCtrlPage->SetMute(TRUE);
#endif
    }
    else
    {
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
    	gPlayerInfo->SetVolume(gMainCtrlPage->GetVolume());
#elif NEW_VERB
    	gPlayerInfo->SetVolume(gMainCtrlPage->GetVolume());
#else
    	gPlayerInfo->SetVolume(gMainCtrlPage->GetVolumePos());
#endif

#if APP_ID == OEMHEB500
    	gMainCtrlPage->SetMute(FALSE, TRUE);
#elif NEW_VERB
    	gMainCtrlPage->SetMuteState(FALSE, TRUE, FALSE);
#else
    	gMainCtrlPage->SetMute(FALSE);
#endif
        //LOGV("Player  mute:%d \r\n",mute);
    }
    gPlayerInfo->SetPadChangeState(FALSE);

    return 1;
}

int dmr_process_volume(int volume)
{
    gPlayerInfo->SetPadChangeState(TRUE);
    gPlayerInfo->SetVolume(volume);
#if APP_ID == OEMHEB500 || APP_ID == OEMDML3
    gMainCtrlPage->SetVolume(volume, TRUE);
#elif NEW_VERB
    gMainCtrlPage->SetVolume(volume, TRUE, FALSE);
#else
    gMainCtrlPage->SetVolumePos(volume);
#endif
    gPlayerInfo->SetPadChangeState(FALSE);

    return 1;

}

int dmr_process_uievent(int value)
{
	return 1;

	int sock = Connect(LOCAL_ADDR, MDR_PORT);
	if ( sock <= 0 )
	{
		printf("Connect :%d failed\n", MDR_PORT);
		return -1;
	}

    char msg[MAX_MSG_LEN] = {0};
    snprintf(msg, sizeof(msg), "%s,%d,\n", UI2DMRCMD, value);

    SendMsg(sock, msg, strlen(msg)+1);

	CloseSock(sock);

    return 1;
}

static int getValue(char *url, const char *startStr, const char *endStr, char *value)
{
	if ( !url || !startStr || !endStr || !value)
		return -1;

	int iVal 	= -1;

	char *p = strstr(url, startStr);
	if ( p != NULL )
	{
		char *end = strstr(p, endStr);
		if ( end != NULL )
			memcpy(value, p+strlen(startStr)+1, end-(p+strlen(startStr))-1);
		else
			strcpy(value,p+strlen(startStr)+1);
		iVal =  0;
	}

	return iVal;
}

void PlayFormUrl(char *url)
{
#if USE_DLNA_DMR
	if ( !url )
		return ;

	char *p = strstr(url, "media_src=");
	if ( p == NULL )
	{
		gPlayerInfo->PlayUrlStream(url);
	}
	else
	{
		char mediaSrc[16] 	= {0};
		char vid[32]		= {0};
		char type[16]		= {0};
		char startTime[32]	= {0};
		char endTime[32]	= {0};
		char pos[16]  		= {0};
		char playurl[1024]	= {0};

		getValue(p, "media_src", "&", mediaSrc);
		getValue(p, "vid", "&", vid);
		getValue(p, "type", "&", type);
		getValue(p, "starttime", "&", startTime);
		getValue(p, "endtime", "&", endTime);
		getValue(p, "pos", "&", pos);

		//printf("mediaSrc=%s\n",mediaSrc);
		//printf("vid=%s\n",vid);
		//printf("type=%s\n",type);
		//printf("starttime=%s\n",startTime);
		//printf("endtime=%s\n",endTime);
		//printf("pos=%s\n",pos);
		//printf("type=%s,type len =%d\n",type,strlen(type));
		if(strncasecmp(type,"vod",3)==0)
		{/*VOD*/
			//printf("type is VOD \n");
			//getVodPlayUrl(vid, playurl);
		}
		else
		{/*BTV*/
			//printf("type is BTV\n");
			//getBtvPlayUrl(vid, 3, startTime, endTime, playurl);
		}

		if ( strlen(playurl)>0 )
		{
			gPlayerInfo->PlayUrlStream(playurl);
		}
	}
#endif
}

int SendSyncInfoToFromUI(int iType)
{
	return 0;
	switch(iType)
	{
	case SYNCTYPE_SWITCH:
		//SendSyncToPadFromUI(0,SYNCTYPE_SWITCH);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SWITCH_SONG);
		break;
	case SYNCTYPE_REPLAY:
		//SendSyncToPadFromUI(0,SYNCTYPE_REPLAY);
		dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_START);
		break;
	case SYNCTYPE_SONG:
		//SendSyncToPadFromUI(0,SYNCTYPE_SONG);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SELECT_SONG);
		break;
	case SYNCTYPE_PAUSE:
		//SendSyncToPadFromUI(0,SYNCTYPE_PAUSE);
		dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_PAUSE);
		break;
	case SYNCTYPE_RESUME:
		//SendSyncToPadFromUI(0,SYNCTYPE_RESUME);
		dmr_process_uievent(DMR_EVENT_UICHANGE_PLAY_RESUME);
		break;
	case SYNCTYPE_VOLUME:
		//SendSyncToPadFromUI(mVolume,SYNCTYPE_VOLUME);
		dmr_process_uievent(DMR_EVENT_UICHANGE_VOLUME);
		break;
	case SYNCTYPE_CHANNEL:
		//SendSyncToPadFromUI(mOriginal,SYNCTYPE_CHANNEL);
		dmr_process_uievent(DMR_EVENT_UICHANGE_AUDIO_TRACK);
		break;
	case SYNCTYPE_START_SCORE:
		//SendSyncToPadFromUI(0,SYNCTYPE_START_SCORE);
		dmr_process_uievent(DMR_EVENT_UICHANGE_START_SCORE);
		break;
	case SYNCTYPE_STOP_SCORE:
		//SendSyncToPadFromUI(0,SYNCTYPE_STOP_SCORE);
		dmr_process_uievent(DMR_EVENT_UICHANGE_STOP_SCORE);
		break;
	case SYNCTYPE_SITUATION:
		//SendSyncToPadFromUI(0,SYNCTYPE_SITUATION);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SITUATION);
		break;
	case SYNCTYPE_PUBLICSONG:
		//SendSyncToPadFromUI(0,SYNCTYPE_PUBLICSONG);
		break;
	case SYNCTYPE_PLAYUDISK:
		//SendSyncToPadFromUI(0,SYNCTYPE_PLAYUDISK);
		break;
	case SYNCTYPE_SINGED:
		//SendSyncToPadFromUI(0,SYNCTYPE_SINGED);
		dmr_process_uievent(DMR_EVENT_UICHANGE_SWITCH_SONG);
		break;
	default:
		break;
	}
	return 0;
}

void DlnaMdr_Init()
{
	gDlnaPostionNotifyThread.StartThread(DlnaPositionNofityThreadFunc, NULL, "DlnaPositionNofityThreadFunc");
}

void DlnaMdr_DeInit()
{
	gThreadExit = 1;
	shutdown(giDLNASock, SHUT_RDWR);
	CloseSock(giDLNASock);
}

#else

#define MAX_URL_LEN (1024)
#define MAX_STR_LEN (32)
HI_STATIC HI_CHAR  gTimeAllTimeSTR[MAX_STR_LEN] = {0};
HI_STATIC HI_CHAR  gTimeCurTimeSTR[MAX_STR_LEN] = {0};
HI_STATIC HI_UCHAR urlinstance[MAX_URL_LEN]={0};

int SendSyncInfoToFromUI(int iType)
{
}

HI_U32 Callback_DlnaDmrSetMediaUri
(
    HI_U32    ulInstanceID,
    HI_UCHAR  *pucUri,
    HI_U32    ulUrlLen,
    HI_U32    ulMediaId,
    HI_UCHAR  *pucMetaData,
    HI_U32    ulMetaDataLen,
    HI_VOID   *pvAuxData
)
{
    printf("%s:%d,ulInstanceID=%d pucUri=%s\n ulUrlLen=%d ulMediaId=%d,ulMetaDataLen=%d\n",
		__PRETTY_FUNCTION__, __LINE__,
		ulInstanceID,pucUri,ulUrlLen,ulMediaId,ulMetaDataLen);
    HI_DLNA_SetDmrCurrPlayState(HI_DLNA_DMR_ACTION_OPEN);
//	pucUri = (HI_UCHAR  *)"http://103.243.182.37/movie/007.mkv"; 
	memset(urlinstance,0x00,sizeof(char)*MAX_URL_LEN);
	memcpy((char*)urlinstance,(char*)pucUri,strlen((char*)pucUri));
	
    return HI_DLNA_RET_SUCCESS;
}

HI_U32 Callback_DlnaDmrPlayInd
(
    HI_U32  ulInstanceID,
    HI_UCHAR  *pucPlaySpeed,  /* string */
    HI_U32  ulStrLen,
    HI_VOID   *pvAuxData
)
{
    printf("%s:%d,ulInstanceID=%d playspeed=%s  ulStrLen=%d \n",
		__PRETTY_FUNCTION__, __LINE__,
		ulInstanceID,pucPlaySpeed);
    HI_DLNA_SetDmrCurrPlayState(HI_DLNA_DMR_ACTION_PLAY);
	gPlayerInfo->PlayUrlStream((char*)urlinstance);	
    return HI_DLNA_RET_SUCCESS;
}

HI_U32 Callback_DlnaDmrPauseInd
(
    HI_U32 ulInstanceID,
    HI_VOID   *pvAuxData
)
{
	printf("%s:%d \n",__PRETTY_FUNCTION__, __LINE__);
	gPlayerInfo->SetPadChangeState(TRUE);
	gPlayerInfo->Pause();
	gMainCtrlPage->SetPalyState(TRUE);
	gPlayerInfo->SetPadChangeState(FALSE);	

    HI_DLNA_SetDmrCurrPlayState(HI_DLNA_DMR_ACTION_PAUSE);
    return HI_DLNA_RET_SUCCESS;
}

/////while do stop , play next cmd
HI_U32 Callback_DlnaDmrStopInd
(
    HI_U32 ulInstanceID,
    HI_VOID   *pvAuxData
)
{
	printf("%s:%d \n",__PRETTY_FUNCTION__, __LINE__);
#if 1
	gPlayerInfo->PlayNext(TRUE);
#else
	gPlayerInfo->StopPlay();
#endif	
    HI_DLNA_SetDmrCurrPlayState(HI_DLNA_DMR_ACTION_STOP);
    return HI_DLNA_RET_SUCCESS;
}

HI_U32 Callback_DlnaDmrSeekInd
(
    HI_U32      ulInstanceID,
    HI_DLNA_SEEKMODE_E enSeekMode,
    HI_DLNA_PARATYPE_E enSeekDataType,
    HI_VOID       *pvSeekTarget,
    HI_VOID       *pvAuxData
)
{
    printf("seekCmd enSeekMode=%d enSeekDataType = %d \n",enSeekMode,enSeekDataType);
	printf("seekCmd str=%s len=%d \n",((HI_DLNA_STRING_S*)pvSeekTarget)->pucBuf,
			((HI_DLNA_STRING_S*)pvSeekTarget)->ulLen);

	HI_CHAR  seekTimeStr[MAX_STR_LEN] = {0};
	int time=0;
	memcpy(seekTimeStr,((HI_DLNA_STRING_S*)pvSeekTarget)->pucBuf,((HI_DLNA_STRING_S*)pvSeekTarget)->ulLen);

	unsigned char h = 0;
	unsigned char m = 0;
	unsigned char s = 0;
#if 1
	char *hstr = strtok(seekTimeStr,":");
	char *mstr = strtok(NULL,":");
	char *sstr = strtok(NULL,":");
	h = atoi(hstr);
	m = atoi(mstr);
	s = atoi(sstr);
#else
	sscanf(seekTimeStr,"%02d:%02d:%02d",&h,&m,&s);
#endif
	time = h*3600 + m*60+ s ;			
	printf("seekTimeStr=%s,h=%d,m=%d,s=%d ,time=%d\n",seekTimeStr,h,m,s,time);
	gPlayerInfo->SeekTo(time*1000);

    return HI_DLNA_RET_SUCCESS;
}

HI_CHAR * Callback_DlnaDmrGetCurrTime(HI_CHAR iEvent)
{
	int pos=0;
	GetCurPosition(&pos) ;	

	printf("%s:%d pos=%d \n",__PRETTY_FUNCTION__, __LINE__,pos );

	unsigned char h = (pos/1000)/3600;
	unsigned char m = ((pos/1000)%3600)/60;
	unsigned char s = ((pos/1000)%3600)%60;
	memset(gTimeCurTimeSTR,0x00,MAX_STR_LEN);	
	sprintf(gTimeCurTimeSTR,"%02d:%02d:%02d",h,m,s);
	
	return gTimeCurTimeSTR;	
}
HI_CHAR * Callback_DlnaDmrGetAllTime(HI_CHAR iEvent)
{
	int alltime=0;
	GetDuration(&alltime);	
	printf("%s:%d alltime=%d \n",__PRETTY_FUNCTION__, __LINE__,alltime );

	unsigned char h = (alltime/1000)/3600;
	unsigned char m = ((alltime/1000)%3600)/60;
	unsigned char s = ((alltime/1000)%3600)%60;
	memset(gTimeAllTimeSTR,0x00,MAX_STR_LEN);	
	sprintf(gTimeAllTimeSTR,"%02d:%02d:%02d",h,m,s);	
	
   	return gTimeAllTimeSTR;

}
HI_CHAR * Callback_DlnaDmrSetVol
(
    HI_S32  ulInstanceID,
    HI_U32 ulDesiredVol,
    HI_CHAR        *pcChannel
)
{
	printf("%s:%d \n",__PRETTY_FUNCTION__, __LINE__);
    gPlayerInfo->SetPadChangeState(TRUE);
    gPlayerInfo->SetVolume(ulDesiredVol);
    gMainCtrlPage->SetVolumePos(ulDesiredVol);
    gPlayerInfo->SetPadChangeState(FALSE);

    return "";
}
HI_CHAR* Callback_DlnaDmrSetMute
(
    HI_S32 ulInstanceID,
    HI_BOOL   bDesiredMute,
    HI_CHAR   *pcChannel
)
{
    printf("%s:%d \n",__PRETTY_FUNCTION__, __LINE__);

	gPlayerInfo->SetPadChangeState(TRUE);
	if(bDesiredMute)
	{
		//LOGV("Player	mute:%d \r\n",mute);
		gPlayerInfo->SetVolume(0);
		gMainCtrlPage->SetMute(TRUE);
	}
	else
	{
		gPlayerInfo->SetVolume(gMainCtrlPage->GetVolumePos());
		gMainCtrlPage->SetMute(FALSE);
	}
	gPlayerInfo->SetPadChangeState(FALSE);

	return "";
}

static HI_U32 SampleCode_DlnaApiRegisterDmrCallback()
{
    HI_U32 uiRet;

    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_SET_MEDIA_URI,
                                (HI_VOID *)Callback_DlnaDmrSetMediaUri);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }

    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_PLAY,
                                (HI_VOID *)Callback_DlnaDmrPlayInd);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }

    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_PAUSE,
                                (HI_VOID *)Callback_DlnaDmrPauseInd);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }

    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_STOP,
                                (HI_VOID *)Callback_DlnaDmrStopInd);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }

    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_SEEK,
                                (HI_VOID *)Callback_DlnaDmrSeekInd);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }

    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_CURTIME,
                                (HI_VOID *)Callback_DlnaDmrGetCurrTime);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }
    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_ALLTIME,
                                (HI_VOID *)Callback_DlnaDmrGetAllTime);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }
    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_SETVOL,
                                (HI_VOID *)Callback_DlnaDmrSetVol);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
    return HI_DLNA_RET_FAILURE;
    }
    uiRet = HI_DLNA_RegisterDmrCallback(HI_DLNA_DMR_CBK_SETMUTE,
                                (HI_VOID *)Callback_DlnaDmrSetMute);
    if(uiRet != HI_DLNA_RET_SUCCESS)
    {
        printf("\n\r###SampleCode_DlnaApiRegisterDmpCallback: "
               "DMR Callback registration Failed ###\r\n");
        return HI_DLNA_RET_FAILURE;
    }
    return (HI_S32)uiRet;
}


int DlnaMdr_Init(const char* name)
{
	HI_U32 uiRet;
	HI_DLNA_INITMODE_E enInitMode = HI_DLNA_INIT_MODE_DMR;
	//HI_CHAR *chVersion = HI_NULL_PTR;

	/* Intializing the stack */
	uiRet= HI_DLNA_InitStack(enInitMode);

	if(uiRet == HI_DLNA_RET_SUCCESS)
	{
		printf("\n\r###Rpc_Test_DlnaApiStackInit: "
			   "Init DLNA SUCCESS, Init Mode %d###\r\n",enInitMode);
	}

	/* Register the callbacks for DMR */
	uiRet = SampleCode_DlnaApiRegisterDmrCallback();
	if (uiRet != HI_DLNA_RET_SUCCESS)
	{
		printf("Failed To Register Callbacks For DMR\r\n");
		HI_DLNA_DestroyStack();
		return 1;
	}


	/* The version of the DLNA stack */
	const HI_CHAR *chVersion = DlnaApiGetVersion();
	if(!chVersion)
	{
		printf("NO VERSION STRING\n");
	}

	printf("Version:%s\n",chVersion);

	HI_DLNA_SetDeviceName(name);
	
	return (HI_S32)uiRet;
}


void DlnaMdr_DeInit()
{
    printf("De-Initializing the DLNA stack \n");

    HI_DLNA_DestroyStack();
}


#endif


