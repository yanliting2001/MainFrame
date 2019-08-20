//===========================================
#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <string.h>
#include     <errno.h>
#include     <sys/types.h>
#include     <sys/stat.h>
#include 	 <pthread.h>
#include 	"UserMsgDefine.h"
#include 	"BaseApp.h"
#include 	"qisr.h"
#include 	"msp_cmn.h"
#include 	"msp_errors.h"
#include 	"SoundCmd.h"
#include 	"arec.h"


#ifdef SOUND_XFYUN


#define	BUFFER_SIZE	4096
#define FRAME_LEN	640 
#define HINTS_SIZE  100

#define DUMP_MICRO_PCM_FILE
#undef DUMP_MICRO_PCM_FILE
#ifdef DUMP_MICRO_PCM_FILE
static 	FILE *fpcm=0;
static char pcm2path[1024] = {};
#endif

typedef enum {
	XFYUN_UNDEF=0,
	XFYUN_INIT,
	XFYUN_MAX,
} xfyun_status_t;

static xfyun_status_t xfyun_status = XFYUN_UNDEF ;

typedef struct _soundPcmMsg {
	char* buffer ;
}SoundPcmMsg;

CBaseLock		g_SoundCmd_Lock;
static deque<SoundPcmMsg*> gSoundItemList;

pthread_t soundCmdThread;


// 每次写入200ms音频(16k，16bit)：
// 1帧音频20ms，10帧=200ms。
// 16k采样率的16位音频，一帧的大小为640Byte 
#define  XFYUN_ONEFRAMESIZE (16000*16/8*20*10)

////micro sound info 
//// 48000 , 16, 2 channel 
#define  ONEFRAMESIZE (48000*2*16/8*20*10)

#define SOUND_PCM_BUFFER_MAX_SIZE  10*XFYUN_ONEFRAMESIZE ///save 10 frame to parsr, 200ms items 
static char soundbuffer[SOUND_PCM_BUFFER_MAX_SIZE]={0};
static unsigned int soundbufferStillDataLen  = 0;

static volatile unsigned char threadAlive = 0 ;

///each frame have 200ms data
void* startSoundDetectFunc(unsigned int pcm_size,const char* p_pcm)
{
	const char* session_begin_params	=	"sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = utf8";

	const char*		session_id					=	NULL;
	static char	    rec_result[BUFFER_SIZE]		=	{NULL};	
	char			hints[HINTS_SIZE]			=	{NULL}; //hints为结束本次会话的原因描述，由用户自定义
	unsigned int	total_len					=	0; 
	int				aud_stat					=	MSP_AUDIO_SAMPLE_CONTINUE ;		//音频状态
	int				ep_stat						=	MSP_EP_LOOKING_FOR_SPEECH;		//端点检测
	int				rec_stat					=	MSP_REC_STATUS_SUCCESS ;			//识别状态
	int				errcode						=	MSP_SUCCESS ;

	long			pcm_count					=	0;////data index

	memset(rec_result,0x00,BUFFER_SIZE);

	printf("\n开始语音听写 ...\n");
	session_id = QISRSessionBegin(NULL, session_begin_params, &errcode); //听写不需要语法，第一个参数为NULL
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
		goto iat_exit;
	}
	
	while (1) 
	{
		unsigned int len = 10 * FRAME_LEN; // 每次写入200ms音频(16k，16bit)：1帧音频20ms，10帧=200ms。16k采样率的16位音频，一帧的大小为640Byte
		int ret = 0;

		if (pcm_size < 2 * len) 
			len = pcm_size;  ///set pcm_size = len[10*FRAME_LEN] ;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		printf(">");
		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret)
		{
			printf("\nQISRAudioWrite failed! error code:%d\n", ret);
			goto iat_exit;
		}
			
		pcm_count += (long)len;
		pcm_size  -= (long)len;
		
		if (MSP_REC_STATUS_SUCCESS == rec_stat) //已经有部分听写结果
		{
			const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
			if (MSP_SUCCESS != errcode)
			{
				printf("\nQISRGetResult failed! error code: %d\n", errcode);
				goto iat_exit;
			}
			if (NULL != rslt)
			{
				unsigned int rslt_len = strlen(rslt);
				total_len += rslt_len;
				if (total_len >= BUFFER_SIZE)
				{
					printf("\nno enough buffer for rec_result !\n");
					goto iat_exit;
				}
				strncat(rec_result, rslt, rslt_len);
			}
		}

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		usleep(200*1000); //模拟人说话时间间隙。200ms对应10帧的音频
	}
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRAudioWrite failed! error code:%d \n", errcode);
		goto iat_exit;	
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat) 
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode)
		{
			printf("\nQISRGetResult failed, error code: %d\n", errcode);
			goto iat_exit;
		}
		if (NULL != rslt)
		{
			unsigned int rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE)
			{
				printf("\nno enough buffer for rec_result !\n");
				goto iat_exit;
			}
			strncat(rec_result, rslt, rslt_len);
		}
		usleep(150*1000); //防止频繁占用CPU
	}
	printf("\n语音听写结束\n");
	printf("=============================================================\n");
	printf("strlen(rec_result)=%d , str=%s\n",strlen(rec_result),rec_result);
	
	///for test
	for(unsigned int mm=0 ; mm < strlen(rec_result); mm ++)
	{
		printf(" 0x%x ",rec_result[mm]);
	}
	
	printf("=============================================================\n");

iat_exit:

	QISRSessionEnd(session_id, hints);
	
	return rec_result ;
}

unsigned char SoundCmdLogin(void)
{
	int			ret						=	MSP_SUCCESS;
	const char* login_params			=	"appid = 5b8a4f0f, work_dir = ."; // 登录参数，appid与msc库绑定,请勿随意改动

	/*
	* sub:				请求业务类型
	* domain:			领域
	* language:			语言
	* accent:			方言
	* sample_rate:		音频采样率
	* result_type:		识别结果格式
	* result_encoding:	结果编码格式
	*
	*/

	///first force to logout , then  login	
	MSPLogout(); 
	
	/* 用户登录 */
	ret = MSPLogin(NULL, NULL, login_params); //第一个参数是用户名，第二个参数是密码，均传NULL即可，第三个参数是登录参数	
	if (MSP_SUCCESS != ret)
	{
		printf("MSPLogin failed , Error code %d.\n",ret);
		return 0 ; //登录失败，退出登录
	}
	
	return 1 ;
}


#ifdef  DUMP_MICRO_PCM_FILE
unsigned int timeOffset()
{
        struct timeval tv;
        gettimeofday(&tv,NULL);
        return (tv.tv_sec*1000 + tv.tv_usec/1000) ;
}
#endif



unsigned  int getUserSoundCmd(char *pbuffer,int nbytes)
{
	if(xfyun_status != XFYUN_INIT || nbytes <= 0 )
	{
		return 0 ;
	}

	unsigned int wantDataLenPerStep = ONEFRAMESIZE ;	
	memcpy(soundbuffer+soundbufferStillDataLen,pbuffer,nbytes);
	soundbufferStillDataLen += nbytes ;	

	printf("\n%s : %s : %d in lastdatasize= %d nbytes=%d wantDataLenPerStep=%d \n",
		__FILE__,__func__,__LINE__,soundbufferStillDataLen,nbytes,wantDataLenPerStep);

#ifdef  DUMP_MICRO_PCM_FILE
		if(fpcm)
		{
			printf("close the file !\n");
			fclose(fpcm);
		}
		memset(pcm2path,0x0,1024);
		sprintf(pcm2path,"/tmp/sound_%d.pcm",timeOffset());
		fpcm = fopen(pcm2path, "wb+");	
		if(fpcm ==NULL)
		{
			printf("open file:%s fail !\n",pcm2path);
		}
		else
		{
			printf("open file:%s success!!\n",pcm2path);
		}
		
	if(fpcm)
	{
		fwrite(pbuffer,nbytes, 1, fpcm);
	}
#endif	
	if(soundbufferStillDataLen < wantDataLenPerStep )
	{
		return 0 ;
	}

	unsigned int offset = 0 ;
	char* retData = (char*)malloc(sizeof(char) * (wantDataLenPerStep+1));
	memset(retData,0x00,(wantDataLenPerStep+1));
	memcpy(retData,soundbuffer,wantDataLenPerStep);
	SoundPcmMsg*  pSoundData = (SoundPcmMsg*)malloc(sizeof(SoundPcmMsg));;
	pSoundData->buffer = retData ;
	g_SoundCmd_Lock.Lock();
	gSoundItemList.push_front(pSoundData);
	g_SoundCmd_Lock.Unlock();

	offset+= wantDataLenPerStep ;
	soundbufferStillDataLen -= offset ;
	memcpy(soundbuffer,soundbuffer+offset,soundbufferStillDataLen);

	printf("%s : %s : %d in still datalen=%d wantDataLenPerStep=%d nbytes=%d\n",
		__FILE__,__func__,__LINE__,soundbufferStillDataLen,wantDataLenPerStep,nbytes);

	return 1 ;	
}
static int soundPcmStructFree(void* param)
{
	SoundPcmMsg* phandle = (SoundPcmMsg*)param ; 
	if(phandle)
	{
		if(phandle->buffer)
			free(phandle->buffer);
		free(phandle);
	}

	return 0 ;
}
///input 16bits 48000  2 channels 
///output 16bits, 16000 1 chanels 
static void* simplest_pcm_data(void* pPcmData,unsigned int dataLen,unsigned int* newDataLen)
{
    if(NULL == pPcmData || NULL == newDataLen)
        return NULL;

	///default is 2 channel , and 16 bits 
	unsigned char channelNum = 2 ;
	unsigned char bitSampleStep = 48/16*2 ; /////sampleRate , channel
	
	char *olddata = (char *)pPcmData ;
	*newDataLen = dataLen/bitSampleStep+1 ; 
	unsigned int newLoopLen = dataLen/bitSampleStep ;
	char *newdata=(char *)malloc(*newDataLen);

	////just  get the left channel,
	for(unsigned int index = 0 ;index < newLoopLen ; index ++)
	{
		memcpy(newdata+index*bitSampleStep,olddata+index*(bitSampleStep*channelNum),bitSampleStep);
	}

    return (void*)newdata ;
}
void *sound_deal_thread_func(void *arg)
{
	printf("\n%s : %s : %d in \n",__FILE__,__func__,__LINE__);	
	threadAlive =1 ;
	do
	{
		if(0 ==threadAlive)
		{
			break;
		}
		g_SoundCmd_Lock.Lock();
		SoundPcmMsg*  retData = gSoundItemList.back(); 
		gSoundItemList.pop_back();	
		g_SoundCmd_Lock.Unlock();
		
		if(retData == NULL || xfyun_status != XFYUN_INIT)
		{
			usleep(10*1000);
			soundPcmStructFree(retData);
			continue ;
		}

		printf(" bufferLen=%d\n",ONEFRAMESIZE);

		///1) simple the data 
		unsigned int newLen = 0;
		char* pNewData = (char *)simplest_pcm_data(retData->buffer,ONEFRAMESIZE,&newLen);
		///2) deal the data
		startSoundDetectFunc(newLen,pNewData); 

		soundPcmStructFree(retData);
	}while(threadAlive);

	return NULL ;
}


unsigned char SoundCmdInit()
{
	g_SoundCmd_Lock.Lock();
	gSoundItemList.clear();
	g_SoundCmd_Lock.Lock();

	if ( pthread_create(&soundCmdThread,NULL,sound_deal_thread_func,NULL) != 0)
	{
		printf("##Err: pthread_create MonitorNet failed.%s\n", strerror(errno));
		threadAlive=0;
		return 0 ;
	}

	set_pcm_sound_callback(getUserSoundCmd);
	
	if(SoundCmdLogin())
	{
		xfyun_status = XFYUN_INIT ;
		return 1 ;
	}

	return 0 ;
}
unsigned char SoundCmdDeInit()
{
	threadAlive=0;

	pthread_join(soundCmdThread,NULL);

	g_SoundCmd_Lock.Lock();
	gSoundItemList.clear();
	g_SoundCmd_Lock.Lock();

	return 1 ;
}


#endif
