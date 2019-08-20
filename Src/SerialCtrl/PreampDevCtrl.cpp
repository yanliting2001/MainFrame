#include "PreampDevCtrl.h"
#include <pthread.h>

#define NEW_STYLE_0525 (1) ///chenbo define this

static pthread_t g_uart_test = 0;

static void* uart_read_thread(void* arg)
{
  CPreampDevCtrl* pdata = (CPreampDevCtrl*) arg ;
  unsigned char buffer[1024];
  while(1)
  {
    memset(buffer,0,1024);
    int len = pdata->mSerialCtrl.ReadChar(buffer, 1024);
    printf("##Err: pthread_create uart_read_thread loop len=%d.\n",len);
    if(len >0)
    {
        printf("uart_read_thread\n");
        for(unsigned char index = 0 ; index < len; index ++)
        {
          printf(" 0x%x ",buffer[index]);
        }
        printf("\n uart_read_thread end \n");
    }
    else
    {
        sleep(1);
    }
  }
}

BOOL CPreampDevCtrl::OpenPreampDev(void)
{
	if ( !mSerialCtrl.Open("/dev/ttyAMA4") )
		return FALSE;

 	if ( pthread_create(&g_uart_test, NULL, uart_read_thread, this) != 0)
	{
		printf("##Err: pthread_create OpenPreampDev failed.\n");
	} 
	return TRUE;
}

void CPreampDevCtrl::ClosePreampDev(void)
{
	mSerialCtrl.Close();
}

///useless
// 体感强度
void CPreampDevCtrl::SetBodyFeel(unsigned char feel)
{
	unsigned char msg[] = {0x0a, 0x08, 0x0b, 0x00, 0x08, 0x0a};
	msg[3] = feel;
	mSerialCtrl.WriteChar(msg, sizeof(msg));
}


// 设置混响类型
void CPreampDevCtrl::SetPCMTuneType(unsigned char type)
{
	//unsigned char msg[] = {0x0a, 0x08, 0x0a, 0x00, 0x08, 0x0a};
	unsigned char msg[] = {0x48, 0x4d, 0x00, 0x02,0x02,0x00, 0x01,0x01,0x05,0xaa};
	type+=1;
	msg[7] = type;
	msg[8] = msg[0]^msg[1]^msg[2]^msg[3]^msg[4]^msg[5]^msg[6]^msg[7];
	mSerialCtrl.WriteChar(msg, sizeof(msg));
}

// 麦克风音量
void CPreampDevCtrl::SetMicroVol(unsigned char vol)
{
	//unsigned char msg[] = {0x0a, 0x08, 0x0c, 0x00, 0x08, 0x0a};
	unsigned char msg[] = {0x48, 0x4d,0x00,0x06, 0x02, 0x00, 0x00,0x02,0x00,0xaa};
	msg[6] = (vol*7)/10;
	msg[8] = msg[0]^msg[1]^msg[2]^msg[3]^msg[4]^msg[5]^msg[6]^msg[7];
	mSerialCtrl.WriteChar(msg, sizeof(msg));
}

// 音量
void CPreampDevCtrl::SetVolume(unsigned char vol)
{
	//unsigned char msg[] = {0x0a, 0x08, 0x0d, 0x00, 0x08, 0x0a};
	unsigned char volumeType[] = {0x03};
	unsigned char msg[] = {0x48, 0x4d,0x00,0x06, 0x02, 0x00, 0x00,0x01,0x00,0xaa};
	msg[6] = (vol*7)/10;
	int len = sizeof(volumeType);
	for(int i=0;i<len;i++){
		msg[7] = volumeType[i];
		msg[8] = msg[0]^msg[1]^msg[2]^msg[3]^msg[4]^msg[5]^msg[6]^msg[7];
		mSerialCtrl.WriteChar(msg, sizeof(msg));
	}
}

// 设置混响类型
void CPreampDevCtrl::MealKtvSetPCMTuneType(unsigned char type)
{
#ifdef  NEW_STYLE_0525
	unsigned char msg[] = {0xaa, 0x55, 0x83, 0x00,0x00};
	msg[3] = type;
	msg[4] = 0xff&(msg[2]+msg[3]) ;	
	{
    printf("MealKtvSetPCMTuneType\n");
    for(unsigned char index = 0 ; index < 5; index ++)
    {
      printf(" 0x%x ",msg[index]);
    }
    printf("\nend \n");
  } 
  mSerialCtrl.WriteChar(msg, sizeof(msg));
#else
	//unsigned char msg[] = {0x0a, 0x08, 0x0a, 0x00, 0x08, 0x0a};
  unsigned char msg[] = {0x55,0x04,0x09,0x01,0x00,0x00};
	type+=1;
	msg[4] = type;
	msg[5] = 0xff^msg[1]^msg[2]^msg[3]^msg[4];
	mSerialCtrl.WriteChar(msg, sizeof(msg));
#endif  
}

// 麦克风音量
void CPreampDevCtrl::MealKtvSetMicroVol(unsigned char vol)
{
#ifdef  NEW_STYLE_0525
	unsigned char msg[] = {0xaa, 0x55, 0x81, 0x00,0x00};
	msg[3] = (vol*79)/100;
	msg[4] = 0xff&(msg[2]+msg[3]);
	{
    printf("MealKtvSetMicroVol\n");
    for(unsigned char index = 0 ; index < 5; index ++)
    {
      printf(" 0x%x ",msg[index]);
    }
    printf("\nend \n");
  }
	mSerialCtrl.WriteChar(msg, sizeof(msg));
#else  
	//unsigned char msg[] = {0x0a, 0x08, 0x0c, 0x00, 0x08, 0x0a};
	unsigned char msg[] = {0x55,0x04,0x09,0x14,0x00,0x00};
	msg[4] = (vol*63)/100;
	msg[5] = 0xff^msg[1]^msg[2]^msg[3]^msg[4];
	mSerialCtrl.WriteChar(msg, sizeof(msg));
#endif  
}

// 音量
void CPreampDevCtrl::MealKtvSetVolume(unsigned char vol)
{
#ifdef  NEW_STYLE_0525
	unsigned char msg[] = {0xaa, 0x55, 0x80, 0x00,0x00};
	msg[3] = (vol*79)/100;
	msg[4] = 0xff&(msg[2]+msg[3]);
	{
    printf("MealKtvSetVolume\n");
    for(unsigned char index = 0 ; index < 5; index ++)
    {
      printf(" 0x%x ",msg[index]);
    }
    printf("\nend \n");
  }  
	mSerialCtrl.WriteChar(msg, sizeof(msg));
#else  
	//unsigned char msg[] = {0x0a, 0x08, 0x0d, 0x00, 0x08, 0x0a};
	unsigned char msg[] = {0x55,0x03,0x09,0x13,0x00,0x00};
	msg[4] = (vol*63)/100;
	msg[5] = 0xff^msg[1]^msg[2]^msg[3]^msg[4];
	mSerialCtrl.WriteChar(msg, sizeof(msg));
#endif
}


