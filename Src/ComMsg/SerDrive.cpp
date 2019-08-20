#include "SerDrive.h"
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


#define COIN_MSG_LENGTH_MAX 1024
static unsigned char g_bExit = 0;
static pthread_t g_MonitorCoreComThreadID = 0;

static int cmd_ctrl_execute_cmd(const char *cmd, char *result, int *plen)   
{   
    char buf_line[1024]={0};
    FILE *ptr; 
    int n=0, max_len;
    
    if((ptr=popen(cmd, "r")) == NULL) 
    {
        printf("popen %s error\n", cmd);
        return -1;
    }

    max_len = *plen;
    while(fgets(buf_line, 1024, ptr)!=NULL)   
    {   
        n += snprintf(result+n, max_len-n, "%s", buf_line); 
    }
    
    *plen = n;
    pclose(ptr);   
    ptr = NULL;   

//	if(n)printf("exc result : %s\n", result);
    return 0;
} 

static void* MonitorCoreComThreadFunc(void* arg)
{
  #if ENABLE_COIN_FUNCTION
  
	char exc_result[COIN_MSG_LENGTH_MAX]={0};
	int exc_len = sizeof(exc_result);
	char cmd[COIN_MSG_LENGTH_MAX]={0};
	snprintf(cmd, sizeof(cmd)-1, "himm 0xF8B31020 0x08 | grep 0x00000000");
	
	system("himm 0xF8A2120C 0");
	system("himm 0xF8B31400 0x00");	
	
	while( !g_bExit )
	{
		memset(exc_result,0x00,COIN_MSG_LENGTH_MAX);
		if(cmd_ctrl_execute_cmd(cmd, exc_result, &exc_len) < 0)
		{
			printf("'%s' fail, %s\n", cmd, strerror(errno));
			
			continue;
		}
		///post msg
		if(NULL == exc_result || exc_len == 0)
		{
			continue ;
		}
    printf(" DISPLAY_INSERT_COIN_MSG \n");
		theBaseApp->BroadcastMsg(DISPLAY_INSERT_COIN_MSG, 0, 0);
    usleep(80*1000);
	}
	
  EXIT:
	g_bExit = 1; 
  
  #endif
}

void StartMinitorCoinThread()
{
	g_bExit = 0;

	if ( pthread_create(&g_MonitorCoreComThreadID, NULL, MonitorCoreComThreadFunc,NULL) != 0)
	{
		printf("##Err: pthread_create MonitorNet failed.%s\n", strerror(errno));
	}
}

void StopMinitorCoinThread(void)
{
	g_bExit = 1;
	pthread_join(g_MonitorCoreComThreadID, NULL);
	printf("===== the MonitorNet is Stop.===\n");
}

