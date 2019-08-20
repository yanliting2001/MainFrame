#include "BaseRecord.h"
#include "DbgOutput.h"
#include "../Sharing/ClientSharing.h"
#include "InternetCtrl/NetCtrlApi.h"
#include "arec.h"

extern "C" unsigned  int setMicriPhoneAudioData_2(unsigned int status,char *pbuffer,int nbytes); 
BOOL CBaseRecord::mbRecordOn = FALSE;
BOOL CBaseRecord::mbInRecording = FALSE;

 void CBaseRecord::Init_Record(void)
 {
#if defined(__arm__)
	DbgOutput(DBG_INFO, "arec_initialize record start\r\n");
	 if ( arec_initialize() < 0 )
	 {
		 DbgOutput(DBG_WARN, "arec_initialize failed:%s", arec_lasterr());
	 }
	DbgOutput(DBG_INFO, "arec_initialize record end\r\n");
#endif
 }

 void CBaseRecord::DeInit_Record(void)
 {
	 Init_Record();
 }

void CBaseRecord::SetRecordOn(BOOL bOn)
{
	char *s;
	if (NULL != (s = getenv("USE_RECORD"))) {
		mbRecordOn = bOn;
	}else{
		mbRecordOn = false;
	}
	DbgOutput(DBG_INFO, "SetRecordOn mbRecordOn = %d\r\n",mbRecordOn);
}

 void CBaseRecord::Start_Record(void)
 {
#if defined(__arm__)
	DbgOutput(DBG_INFO, "arec_start record start mbRecordOn=%d\r\n",mbRecordOn);
	 if ( mbRecordOn )
	 {
		 if ( arec_start() < 0 )
		 {
			 DbgOutput(DBG_WARN, "arec_start failed:%s", arec_lasterr());
		 }
		 mbInRecording = TRUE;
		DbgOutput(DBG_ERR, "arec_start record end, set_pcm_extern_callback ! \r\n");

		set_pcm_extern_callback(setMicriPhoneAudioData_2);
	 }
#endif
 }

 void CBaseRecord::Stop_Record(const char *cSong)
 {
#if defined(__arm__)
	DbgOutput(DBG_INFO, "arec_stop record mbInRecording = %d\r\n",mbInRecording);
	 if ( !cSong || strlen(cSong) <= 0 ) return ;
   #if 0
   int RecordState = 0; // 0:单机免费登陆录音 1：付费登陆录音
   #else
   int RecordState = 1; // 0:单机免费登陆录音 1：付费登陆录音
   #endif
   
	 if ( mbInRecording )
	 {
	 	 set_pcm_extern_callback(NULL);
		 mbInRecording = FALSE;
		 const char *s = arec_stop();
		 if ( !s )
		 {
			 DbgOutput(DBG_ERR, "arec_stop failed:%s", arec_lasterr());
		 }
		 else
		 {
			 char id [8] = {0};
       char recordstatus [8] = {0};
			 snprintf(id, sizeof(id), "%d&%d", CClientSharing::mUserInfo.userid, RecordState);

       DbgOutput(DBG_ERR, "%s, line = %d RecordState = %d\n", __FUNCTION__, __LINE__, RecordState);
       if(CClientSharing::mUserInfo.userid != 0)    
       {
  			 if ( arec_upload(s, cSong, g_sIPInfo.cMac, id) < 0 )
  				 return ;
       }
       else
       {
        DbgOutput(DBG_ERR, "%s, line = %d userid = %d\n", __FUNCTION__, __LINE__, CClientSharing::mUserInfo.userid);
        return ;
       }
       
			 upinfo *infos;
			 unsigned int n;
			 unsigned int i;
			 if ( arec_upinfos(&infos, &n ) < 0 )
			 {
			 		printf("Failed to retrieve upload infos!\r\n");
			 }
			 else
			 {
			 		for ( i=0; i<n; i++ )
			 		{
			 			printf("item[%d]: file=(%s), music=(%s), percent=(%.2f), state=(%d), errmsg=(%s)\r\n", i, infos[i].path, infos[i].music, infos[i].percent, infos[i].stat, infos[i].errmsg);
			 		}

         arec_upinfos_free(infos);
			 }
       
       
		 }
	 }
#endif
 }

 void CBaseRecord::Pause_Record(void)
 {
#if defined(__arm__)
	 if ( mbRecordOn )
	 {
		 if ( arec_pause() < 0 )
		 {
			 DbgOutput(DBG_WARN, "arec_pause failed:%s", arec_lasterr());
		 }
	 }
#endif
 }
 void CBaseRecord::Resume_Record(void)
 {
#if defined(__arm__)
	 if ( mbRecordOn )
	 {
		 if ( arec_resume() < 0 )
		 {
			 DbgOutput(DBG_WARN, "arec_resume failed:%s", arec_lasterr());
		 }
	 }
#endif
 }
 void CBaseRecord::Restart_Record(void)
 {
#if defined(__arm__)
	 if ( mbRecordOn )
	 {
		 if ( arec_restart() < 0 )
		 {
			 DbgOutput(DBG_WARN, "arec_restart failed:%s", arec_lasterr());
		 }
	 }
#endif
 }
