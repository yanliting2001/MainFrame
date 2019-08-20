#include "AudioCompare_OSD.h"
#include "hisi3719_Control.h"
#include "damd_base_type.h"
#include "BCM_OSD_CTRL.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "yy_compare_ff.h"

using namespace std;

////save all the data ,  disable this 
////static map<int, CAudioCompareOSD::COMPARE_ITEM_S> gDstAudioList;
////save the data which will be showed to UI
static deque<CAudioCompareOSD::COMPARE_ITEM_S> gShowItemList;

static CAudioCompareOSD::COMPARE_ITEM_RET_T gShowItemRet ;

static BOOL gbHasNewItem = FALSE;

extern int GetCurPosition(int *filetime);


#define  AUDIO_DEBUG_PRINTF  //printf  


#define BUFFER_SIZE 1024  
#define PARENT_MEDIA_PATH		"/stb/config"
#define MAIN_AUDIO_READ_AHEAD_TIME (60*30) ////60ms * 30 itemCount

/*
	int pos=0;
	GetCurPosition(&pos)
*/
/////do_callback
static int do_call(PCM_DATA_SRC_TYPE type,PcmPowerItem* ppiArray,int len,float score,unsigned char showflag)
{
	if ( type == PCM_TYPE_MAINAUDIO ) //////for show 
	{
		CAudioCompareOSD::COMPARE_ITEM_S item;
		//printf("		PCM_TYPE_MAINAUDIO,pts = %d,power=%d,showflag = %d \r\n",ppiArray->pts,ppiArray->power,showflag);
		
		item.dstPower = ppiArray->power;
		item.showFlag = showflag ;
		g_OSD_Lock.Lock();
		while ( gShowItemList.size() >= MAX_DOT_IN_LINE )
		{
			gShowItemList.pop_back();
		}
		gShowItemList.push_front(item);
		g_OSD_Lock.Unlock();
		gbHasNewItem = TRUE;
	} 
	else if ( type == PCM_TYPE_MICROPHONE )
	{
		//printf("PCM_TYPE_MICROPHONE,pts = %d,power=%d,showflag = %d score=%.2f item_score=%.2f \r\n",
		//ppiArray->pts,ppiArray->power,showflag,score,ppiArray->itemscore);
		
		g_OSD_Lock.Lock();
		gShowItemRet.showFlag = showflag ;
		gShowItemRet.curPower = ppiArray->power ;
		gShowItemRet.ismatch = ppiArray->ismatch ;
		float newscore = score*1.8 ;
		gShowItemRet.score = ((newscore >= 100.0)?(100):(newscore));
		gShowItemRet.itemscore = ppiArray->itemscore;
		g_OSD_Lock.Unlock();
	}

	return 0;
}
#ifdef  DUMP_AUDIO_PCM
static unsigned  char  getCompareAllowFlag()
{
	int ret= 0  ;
	int fd = open(COMPARE_ALLOW_FILE_NAME,O_RDONLY,0777);
	if (fd <0)  
    {  
        perror("getCompareAllowFlag error ,open COMPARE_ALLOW_FILE_NAME file failed \n");
        return 0;  
    } 
	char buffer[128]={0};	
	read(fd,buffer,sizeof(buffer)) ;
	close(fd);

	if(buffer[0] == 'y' || buffer[0] == 'Y')
		ret = 1 ;
	
	return  ret ;
}

BOOL FifoPushMsg(const char* filepath ,const int track)
{
	static BOOL  FifoInitFinish=FALSE;
	static int pipe_fd = 0 ;
	int bytes = 0;  
	int totalLen = sizeof(url_cmd_info_t);
	url_cmd_info_t buffer = { 0 };
	char path[1024]={0};
	sprintf(path,"%s/%s",PARENT_MEDIA_PATH,filepath);
	printf("%s: (%d) path = %s track = %d \n", __func__, __LINE__,path,track);
	if(!FifoInitFinish)
	{
		FifoInitFinish = TRUE;
		int open_mode = O_WRONLY | O_NONBLOCK; 
		if (access(FIFO_NAME, F_OK) == -1)  
	    {  
	        int res = mkfifo(FIFO_NAME, 0777);  
	        if (res != 0)  
	        {  
	            printf("Could not create fifo %s\n", FIFO_NAME);
	            return  FALSE; 
	        }  
	    } 
		////maybe  block the fifo
		pipe_fd = open(FIFO_NAME, open_mode); 
		if (pipe_fd == -1)  
			return FALSE ;

		////do not close the handle
	}

	if(strlen(path) >= (FILENAME_MAX_LEN -1)) ////means the url is too long , 
		return FALSE ;	
	memcpy(buffer.url,path,strlen(path));
	buffer.audioIndex = track ;
	
	while (bytes < sizeof(url_cmd_info_t))  
	{
		int res = write(pipe_fd, &buffer, totalLen);  
		bytes += res ;
	}

	return TRUE ;
}
static int currentPts = -1 ;
static FILE *audio_pcm_fd=NULL;
static int  audio_pcm_file_open()
{
	currentPts = -1 ;
	if(audio_pcm_fd)
	{
		printf("close the file !\n");
		fclose(audio_pcm_fd);
	}
	audio_pcm_fd = fopen(AUDIO_PCM_FILENAME,"r");

	if(audio_pcm_fd ==NULL)
		return 0 ;
	
	return 1 ;
}
int  get_pcm_data_by_pos(int pts,char* pbuffer,int bufferLen,int* realpts,int* shouldwait)
{
	if(audio_pcm_fd == NULL)
		return -1 ;

	unsigned int offset = 0 ;
	int fsize = 0 ;
	////check the pts position
	if(currentPts < 0)  ////in first time ,it is -1
	{
		currentPts = pts ;
		*shouldwait = 0 ;
		///pts is ms
		offset = getTimeStepDataLen(PCM_TYPE_MAINAUDIO)/getBaseStepInMs() * currentPts ; 
		fseek(audio_pcm_fd,offset,SEEK_SET);
	}
	else if(currentPts <= (pts+MAIN_AUDIO_READ_AHEAD_TIME))//current data is ahead to the stream
	{	//////read the data , and util the diff is MAIN_AUDIO_READ_AHEAD_TIME,and do sleep in wait !
		*shouldwait = 0 ;
//		currentPts = pts+MAIN_AUDIO_READ_AHEAD_TIME;
//		offset = getTimeStepDataLen(PCM_TYPE_MAINAUDIO)/getBaseStepInMs() * currentPts ; 
//		fseek(audio_pcm_fd,offset,SEEK_SET);
	}
	else
	{
//		*shouldwait = 1 ;
	}
	*realpts = currentPts ;
	fsize = (unsigned int)fread(pbuffer,1,bufferLen,audio_pcm_fd);
//	currentPts += fsize*getBaseStepInMs()/getTimeStepDataLen(PCM_TYPE_MAINAUDIO) ;
	currentPts += getBaseStepInMs() ;
	//printf("====[LINE:%d][FUNC:%s] oldpts=%d currentPts=%d size=%d ====\n", 
	//	 __LINE__, __func__,pts,currentPts,fsize);
	return fsize;
}
#endif
LPVOID DumpAudioTrackThreadFunc(LPVOID lpParam)
{
#ifdef  DUMP_AUDIO_PCM
	CAudioCompareOSD *pAudioCompare = (CAudioCompareOSD *)lpParam;

	if ( pAudioCompare )
	{
		pAudioCompare->DumpAudioTrackCheckThreadFunc();
	}
#endif
	return NULL;
}

LPVOID AudioCompareOSDThreadFunc(LPVOID lpParam)
{
	CAudioCompareOSD *pAudioCompare = (CAudioCompareOSD *)lpParam;

	if ( pAudioCompare )
	{
		pAudioCompare->ThreadFunc();
	}

	return NULL;
}

LPVOID AudioCompareShowPicThreadFunc(LPVOID lpParam)
{
	CAudioCompareOSD *pAudioCompare = (CAudioCompareOSD *)lpParam;

	if ( pAudioCompare )
	{
		pAudioCompare->ShowPicThreadFunc();
	}

	return NULL;
}

CAudioCompareOSD::CAudioCompareOSD() : mhCast(0), mExitThread(TRUE), mbStartCompare(FALSE),playStatus(FALSE)
{
	// 这个区域需要根据电视去做调整
	mrcCompareBK.left = 80;
	mrcCompareBK.top = 285;
	mrcCompareBK.right = 1800;
	mrcCompareBK.bottom = mrcCompareBK.top + 350;

	mrcAudioItemBK.left = 0;
	mrcAudioItemBK.top = 0;
	mrcAudioItemBK.right = 23;
	mrcAudioItemBK.bottom = 23;

  //eat 显示区域
	mrcAudioEatingBK.left = 930;
	mrcAudioEatingBK.top = 0;
	mrcAudioEatingBK.right = 50;
	mrcAudioEatingBK.bottom = 70;
	// 显示分数和头像的坐标，
	// 需要根据实际情况填写
	mrcUserHeadImg.left = 1000;
	mrcUserHeadImg.top = 200;
	mrcUserHeadImg.right = 1200;
	mrcUserHeadImg.bottom = 600;

  //分数显示区域
	mrcRank.left = 1696;
	mrcRank.top = 492;
	mrcRank.right = mrcRank.left + 10;
	mrcRank.bottom = mrcRank.top + 10;

	mrcScore.left = 600;
	mrcScore.top = 100;
	mrcScore.right = 800;
	mrcScore.bottom = 200;

	mFont.LoadFont(FONTPATH);
}

BOOL CAudioCompareOSD::Start()
{
	mExitThread = FALSE;

	BOOL bStartOK = mGetCastData.StartThread(AudioCompareOSDThreadFunc, this, "AudioCompareOSDThreadFunc");
	if ( !bStartOK )
	{
		mExitThread = TRUE;
	}

	bStartOK = mShowPic.StartThread(AudioCompareShowPicThreadFunc, this, "AudioCompareShowPicThreadFunc");
	if ( !bStartOK )
	{
		mExitThread = TRUE;
	}

	return bStartOK;
}

void CAudioCompareOSD::Stop()
{
	mExitThread = TRUE;
	mAudioDumpFinish.StopThread();
	mGetCastData.StopThread();
	mShowPic.StopThread();
}
void CAudioCompareOSD::DumpAudioTrackCheckThreadFunc()
{
	/////get the msg in 10 seconds , if fail , so can not support for audio compare	
	/////else start thread to get the msg 
#ifdef  DUMP_AUDIO_PCM
	unsigned char looptime = 0 ;
	do
	{
		///get the ret ;
		if(getCompareAllowFlag())
		{
			printf("able dump !!\n");
			break ;
		}
		
		looptime ++;
		sleep(1);
	}while(looptime < lrt_start_time );

	char buffer[128]={0};
	sprintf(buffer,"rm -fr %s",COMPARE_ALLOW_FILE_NAME);
	printf("remove compare file buffer=%s\n",buffer);
	system(buffer);

 ///force to support audio compare yiyuan
	if(0  && looptime >= lrt_start_time)
	{
		///not support 
		printf("%s: (%d)  do not support audio compare !\n", __func__, __LINE__);
	}
	else 
	{
		////support ;
		////start thread , able draw, and get the pcm data by pts 
		////check the pcm file , AUDIO_PCM_FILENAME
		if(audio_pcm_file_open())
		{
			StartAudioCompare();
			////maybe
		}
	}
#endif	
}

////get the audio main track msg  thread !
void CAudioCompareOSD::ThreadFunc()
{
#ifdef  DUMP_AUDIO_PCM
	FILE *fpcm2=NULL;
	char pcm2path[1024]={0};
	while ( !mExitThread )
	{
		if(mbStartCompare && (playStatus == TRUE)) 
		{
			////get a basic length 
			////48000*16/8*1*200/1000 = 192000
			////set the default  step 
			char pbuffer[19200]={0};			
			int pos=0;
			GetCurPosition(&pos) ;
			/////check the last pos
			int readDatapos = 0 ;	
			int shouldWait=1;
			int fsize = get_pcm_data_by_pos(pos,pbuffer,getTimeStepDataLen(PCM_TYPE_MAINAUDIO),&readDatapos,&shouldWait);
			if ( fsize >= 0 )
			{
				if(fsize != getTimeStepDataLen(PCM_TYPE_MAINAUDIO))
				{
					printf("maybe in eof , reset the fszie to %d !!\n",getTimeStepDataLen(PCM_TYPE_MAINAUDIO));
					fsize = getTimeStepDataLen(PCM_TYPE_MAINAUDIO) ;
				}
				setMainSrcAudioData_2(readDatapos, pbuffer, fsize);
				if(shouldWait)
				{
					////sleep 60 ms 				
					unsigned time_use=0;
					struct timeval start;
					struct timeval end; 			
					gettimeofday(&start,NULL);
					do
					{
						gettimeofday(&end,NULL);
						time_use=(end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
						if(time_use >= 60)
						{		
		//						printf("start.time:%d_%d\n",start.tv_sec,start.tv_usec);
		//						printf("end.tv_sec:%d_%d\n",end.tv_sec,end.tv_usec);
							//printf("start next time , time_use is %f us\n",time_use);
							break;
						}
					}while(1);
				}	
			}
			else{
				usleep(60000);				
			}
		}else {
			usleep(60000);
		}
	}
#else
	//UINT64 uSeed = GetTickCount();
	//srand(uSeed);
	//for(int i=0;i<8;i++)
	FILE *fpcm2=NULL;
	char pcm2path[1024]={0};
	while ( !mExitThread )
	{
		if(mbStartCompare) 
		{ 
			unsigned int pts;
			char pbuffer[8192]={0};

			int fsize = damd_porting_p_get_audio_frame(mhCast, &pts, pbuffer);
			//printf("audio frame pcmbuffer pts  = %d\n", pts);
			//printf("audio frame pcmbuffer size  = %d\n", fsize);
			if ( fsize > 0 )
			{
			#if 0
				if(NULL == fpcm2)
				{
					memset(pcm2path,0x0,1024);
					sprintf(pcm2path,"/tmp/org_main_0121_%d.pcm",111);
					fpcm2 = fopen(pcm2path, "wb+"); 
					if(fpcm2 ==NULL)
					{
						printf("open file:%s fail !\n",pcm2path);
					}
					else
					{
						printf("open file:%s success!!\n",pcm2path);
					}
				}
				if(fpcm2)
				{
					fwrite(pbuffer,fsize,1,fpcm2);
				}
#endif
				setMainSrcAudioData_2(pts, pbuffer, fsize);
				//setMicriPhoneAudioData_2(pts, pbuffer, fsize);
			}
			usleep(20000);	// 0.2s
		}else {
			usleep(200000);
		}
	}
#endif	
}

void CAudioCompareOSD::ShowPicThreadFunc()
{
	while ( !mExitThread )
	{
		if(mbStartCompare) { 
			if (gbHasNewItem )
			{
				SetCurAudioCompare(0);
				gbHasNewItem = FALSE;
			}
			usleep(20000);	// 0.2s
		}else {
			usleep(200000);
		}
	}
}


void CAudioCompareOSD::AudioCompareInit(void)
{
#ifndef  DUMP_AUDIO_PCM
	damd_porting_p_audiocast_init(&mhCast);
#endif
	initAudioCompare();
	set_PcmCallBack(do_call,GetCurPosition);

	///just keep left channel msg	
	setAudioInfo(PCM_TYPE_MAINAUDIO, 1, 16, 48000);
	setAudioInfo(PCM_TYPE_MICROPHONE, 2, 16, 48000);
	
	unsigned int stepms = getBaseStepInMs();
	printf("default stepms = %d \r\n",stepms);
	setBaseStepInMs(60);
	stepms = getBaseStepInMs();
	printf("stepms = %d \r\n",stepms);
	Start();
}

void CAudioCompareOSD::AudioCompareDeinit(void)
{
	uninitAudioCompare();
	Stop();
}

// 显示总排行
void CAudioCompareOSD::ShowAllRank(float rankID, BOOL bVisible)
{
	D_RECT rc;
	memcpy(&rc, &mrcRank, sizeof(D_RECT));

	g_pPlayerCtrl->EraseImg(rc);

	if ( bVisible )
	{
		CBaseStringW strScore;
                strScore.Format("%.1f", rankID);

                Font_Bitmap_Data score_Font;
                score_Font.text_alpha = NULL;
		            RGBQUAD sFontColor              = {0xFF, 0xFF, 0xFF};
                RGBQUAD sBGColor = {0x99, 0x32, 0xCC};
                

                mFont.SetFontSize(38);
                if ( !GenTextBitmapData(&mFont, strScore.GetString(), sFontColor, sBGColor, &score_Font) )
                {
                        printf("GenTextBitmapData:%ls failed.\n", strScore.GetString());
                        return ;
                }
                if ( score_Font.text_alpha )
                {
                        g_pPlayerCtrl->ShowBmpBuffer(rc.Left, rc.Top, (char *)score_Font.text_alpha);
                        free(score_Font.text_alpha);
                }
	}
}

// 显示分数
void CAudioCompareOSD::ShowSingingScore(float score, BOOL bVisible)
{
	D_RECT rc;
	memcpy(&rc, &mrcScore, sizeof(D_RECT));

	g_pPlayerCtrl->EraseImg(rc);
	if ( bVisible )
	{
		CBaseStringW strScore;
		strScore.Format("%.1f", score);

		Font_Bitmap_Data score_Font;
		score_Font.text_alpha = NULL;

		RGBQUAD sFontColor		= {0xFF, 0xFF, 0xFF};
    RGBQUAD sBGColor = {0x99, 0x32, 0xCC};

		mFont.SetFontSize(50);
		if ( !GenTextBitmapData(&mFont, strScore.GetString(), sFontColor, sBGColor, &score_Font) )
		{
			printf("GenTextBitmapData:%ls failed.\n", strScore.GetString());
			return ;
		}
		if ( score_Font.text_alpha )
		{
			g_pPlayerCtrl->ShowBmpBuffer(rc.Left, rc.Top, (char *)score_Font.text_alpha);
			free(score_Font.text_alpha);
		}else {
			printf("show score error,text_alpha is NULl\r\n");
		}
	}
}
void CAudioCompareOSD::AudioCompareCheck(const char* path ,const int track)
{  
	///pust data to fifo 
	//start a thread to check the result 
#ifdef  DUMP_AUDIO_PCM	

	char lrcpath[128]={0};
	sprintf(lrcpath,"%s/%s",PARENT_MEDIA_PATH,path);
	char* pend=strrchr(lrcpath,'.');
	*(pend+1)='\0';
	strcat(lrcpath,"lrc");

	///the file is not exist	
	if (access(lrcpath, F_OK) == -1)
	{
		printf("====[%s:%d:%s] path=%s is not exist====\n", __FILE__, __LINE__, __func__,lrcpath);
		return ;
	}
	printf("===path=%s ====\n", lrcpath);		
	/////track is second audio
	FifoPushMsg(path,((track==1)?(2):(1)));	

	g_OSD_Lock.Lock();
	ShowCompareProcess(TRUE);
	g_OSD_Lock.Unlock();


	lrt_start_time = initSrcValidFileInfo(lrcpath,PCM_DATA_VALID_STEPS)/1000;	
	if(lrt_start_time < AUDIO_COMPARE_SUPPORT_LOOPTIME)
		lrt_start_time = AUDIO_COMPARE_SUPPORT_LOOPTIME ;
	printf("AudioCompareCheck lrt_start_time=%d\n",lrt_start_time);
	BOOL bStartOK = mAudioDumpFinish.StartThread(DumpAudioTrackThreadFunc, this, "DumpAudioTrackThreadFunc");
#else 
	adfsas
#endif
}
void CAudioCompareOSD::PlayMainChannel(BOOL  flag)
{
#ifdef  DUMP_AUDIO_PCM
	///eva remove this ,  for bug 84
	//doPlayMainChannel((unsigned char)flag);
#endif
}

void CAudioCompareOSD::ReStartAudioCompare(void)
{
#ifdef  DUMP_AUDIO_PCM
	printf("%s,%d\r\n",__FUNCTION__,__LINE__);
	StopAudioCompare(FALSE);

	if(audio_pcm_file_open())
	{
		StartAudioCompare();
		////maybe
	}
#else 
	dsafasd
#endif
}

// 开始评分
void CAudioCompareOSD::StartAudioCompare(void)
{
	startNewAudioCompare();

	g_OSD_Lock.Lock();
	ShowCompareProcess(TRUE);
	g_OSD_Lock.Unlock();

	mbStartCompare = TRUE;
	playStatus = TRUE;
	gbHasNewItem = TRUE;
	printf("%s,%d mbStartCompare = %d, gbHasNewItem=%d\r\n",__FUNCTION__,__LINE__,mbStartCompare,gbHasNewItem);
}

// 结束评分
void CAudioCompareOSD::StopAudioCompare(BOOL delfile)
{
	printf("%s,%d\r\n",__FUNCTION__,__LINE__);
	mbStartCompare = FALSE;
#ifdef  DUMP_AUDIO_PCM	
	if(audio_pcm_fd)
	{
		printf("close the file !\n");
		fclose(audio_pcm_fd);
		audio_pcm_fd = NULL ;
	}
	char buffer[128]={0};	
	if(TRUE == delfile)	
	{
		sprintf(buffer,"rm -fr %s",AUDIO_PCM_FILENAME);
		printf(" remove old file buffer=%s\n",buffer);
		system(buffer);
	}		
	sprintf(buffer,"rm -fr %s",COMPARE_ALLOW_FILE_NAME);
	printf("remove compare file buffer=%s\n",buffer);
	system(buffer);	
#endif
	finishAudioCompare();
	printf("%s,%d\r\n",__FUNCTION__,__LINE__);

	g_OSD_Lock.Lock();
	gShowItemList.clear();
	memset(&gShowItemRet,0x00,sizeof(COMPARE_ITEM_RET_T));

	ShowCompareProcess(FALSE);
	ShowAllRank(0.0f, FALSE);
	ShowSingingScore(0.0f, FALSE);

  /////
  D_RECT rc;
  rc.Left = 0;
  rc.Right = 1920;
  
  rc.Bottom = mrcCompareBK.bottom;
  rc.Top = mrcCompareBK.top;

  g_pPlayerCtrl->EraseImg(rc);
	g_OSD_Lock.Unlock();

	system("echo 3 > /proc/sys/vm/drop_caches");
	
	printf("%s,%d\r\n",__FUNCTION__,__LINE__);
}

////pause = 0 pause
////pause = 1 resume
void CAudioCompareOSD::PauseAudioCompare(BOOL bPause)
{
  printf("%s,%d bPause = %d \r\n",__FUNCTION__,__LINE__,bPause);
	playStatus = bPause ;
	pauseAudioCompare(bPause);
}

void CAudioCompareOSD::SetCurAudioCompare(int power)
{
	D_RECT rc;
  int score;
	//yiyuan for test
//	return ;
	
	g_OSD_Lock.Lock();
	{
		// 已显示一行
		ShowCompareProcess(FALSE);	// 清空
		ShowCompareProcess(TRUE);		// 重画背景图片
	}
	//printf("power  = %d\n", power);

	// 画方格
	int i = 0;
	int middlePosPower = 0;
	for ( deque<COMPARE_ITEM_S>::iterator iter = gShowItemList.begin(); 
		iter != gShowItemList.end() ; 
		++iter,++i )
	{
		if(! iter->showFlag)
			continue ;
		
		mrcAudioItemBK.left = 1530 - (MAX_DOT_IN_LINE - i) * 23;	// 计算x坐标
		mrcAudioItemBK.top = 520 - (iter->dstPower % MAX_AUDIO_LEVEL) * 10;
		//mrcAudioItemBK.top = 590 - (power % MAX_AUDIO_LEVEL) * 17;
		mrcAudioItemBK.right = mrcAudioItemBK.left + 8;
		mrcAudioItemBK.bottom = mrcAudioItemBK.top + 7;
		memcpy(&rc, &mrcAudioItemBK, sizeof(D_RECT));

		if(33 == i)
		{
			middlePosPower = iter->dstPower ;
		}

    score = gShowItemRet.score;
    
		if ( (gShowItemRet.ismatch == MATCHING_STATUS) && (0 < score && score <= 25) && (i > 33))
			g_pPlayerCtrl->ShowPicFile(AUDIO_MATCHED_PNG1, rc);
    else if ( (gShowItemRet.ismatch == MATCHING_STATUS) && (25 < score && score <= 50) && (i > 33))
      g_pPlayerCtrl->ShowPicFile(AUDIO_MATCHED_PNG2, rc);
    else if ( (gShowItemRet.ismatch == MATCHING_STATUS) && (50 < score && score <= 75) && (i > 33))
      g_pPlayerCtrl->ShowPicFile(AUDIO_MATCHED_PNG3, rc);
    else if ( (gShowItemRet.ismatch == MATCHING_STATUS) && (75 < score && score <= 100) && (i > 33))
      g_pPlayerCtrl->ShowPicFile(AUDIO_MATCHED_PNG4, rc);
		else
			g_pPlayerCtrl->ShowPicFile(AUDIO_DEFAULT_PNG, rc);
	}

	// 画蛇头
//	mrcAudioEatingBK.top = 445 - (gShowItemList.front().curPower % MAX_AUDIO_LEVEL) * 10 ;
	mrcAudioEatingBK.top = 510 - (middlePosPower % MAX_AUDIO_LEVEL) * 10 ;	//eat的y坐标
	//mrcAudioEatingBK.top = 580 - (power % MAX_AUDIO_LEVEL) * 15;
	mrcAudioEatingBK.bottom = mrcAudioEatingBK.top + 20;
	memcpy(&rc, &mrcAudioEatingBK, sizeof(D_RECT));
	g_pPlayerCtrl->ShowPicFile(AUDIO_EATING_PNG, rc);

	// 剩下就是画成绩
	ShowAllRank(gShowItemRet.score, TRUE);
#if 0
	if(gShowItemRet.showFlag && gShowItemRet.itemscore  >= 30.0)
	{
		ShowSingingScore(gShowItemRet.itemscore, TRUE);
	}
#endif

	// 画用户头像
	/*
	if ( IsFileExist(LOAD_USER_JPG) )
	{
		D_RECT rc;
		memcpy(&rc, &mrcUserHeadImg, sizeof(D_RECT));
		g_pPlayerCtrl->ShowPicFile(LOAD_USER_JPG, rc);
	}
	*/
	g_OSD_Lock.Unlock();

}

void CAudioCompareOSD::ShowCompareProcess(BOOL bVisible)
{
	D_RECT rc;
	memcpy(&rc, &mrcCompareBK, sizeof(D_RECT));

	if ( bVisible )
	{
		g_pPlayerCtrl->ShowPicFile(COMPARE_BKGROUND_PNG, rc);
	}
	else
	{
		g_pPlayerCtrl->EraseImg(rc);
	}
}
