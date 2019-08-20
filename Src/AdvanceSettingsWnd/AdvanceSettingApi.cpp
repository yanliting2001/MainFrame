#include "AdvanceSettingApi.h"
#include <stdio.h>
#include <string.h>
#include "BaseApp.h"
#include "FileOperate.h"

const int INIT_START_STATUS			= 1;
const int INIT_START_VOL 			= 50;
const int INIT_START_ORIGNAL 		= 0;
const int INIT_START_SWITCHORINGANL = 0;
const int INIT_SCREENSAVE_TYPE 		= 1;
const int INIT_SCREENSAVE_TIME		= 5;
const int INIT_TV_LIGHTNESS 		= 50;
const int INIT_TV_CONTRAST 			= 50;
const int INIT_TV_SATURATION 		= 50;
const int INIT_SERIALPORT_MAINPORT0 = 1;
const int INIT_SERIALPORT_MAINPORT1 = 0;
const int INIT_SERIALPORT_SUBPORT0 	= 0;
const int INIT_SERIALPORT_SUBPORT1 	= 0;
const int INIT_SERIALPORT_SUBPORT2 	= 0;
const int INIT_SERIALPORT_SUBPORT3 	= 0;
const int INIT_TOUCHSCREENSET_ONOFF = 1;
const int INIT_TOUCHSCREENSET_TYPE 	= 1;
const int INIT_TOUCHSCREENSET_COUNT = 0;
const int INIT_VOLUME_MICROPHONE 	= 50;
const int INIT_VOLUME_MICROPHONEMAX = 100;
const int INIT_VOLUME_TONEUP  		= 100;
const int INIT_VOLUME_TONEDOWN 		= 0;
const int INIT_VOLUME_AMPLIFIERMAX 	= 100;
const int INIT_VOLUME_AMPLIFIERAD 	= 100;
const int INIT_VOLUME_AMPLIFIERSTART = 50;
const char INIT_SYSPWD[12] 			= "888888";
const char INIT_RECEPTIONPWD[12] 	= "888888";

SSystemSetting gs_SystemSetting;

#define SCROLLTEXT_PATH		"./KTV_Data/database/ScrollText.txt"
//#define SCROLLTEXT_PATH			"/tmp/ScrollText.txt"

void ReadSystemSetting()
{
	int ret = 0;
	char cAppFolder[MAX_PATH] = {0};
	char cSystemFileName[256] = "./KTV_Data/database/SystemInfo.dat";

	GetProgramPath(cAppFolder, MAX_PATH);
	snprintf(cSystemFileName, sizeof(cSystemFileName), "%sKTV_Data/database/SystemInfo.dat", cAppFolder);

	FILE *fp = fopen(cSystemFileName, "rb") ;

	if (fp)
	{
		memset(&gs_SystemSetting,0,sizeof(gs_SystemSetting));
		ret = fread(&gs_SystemSetting,sizeof(gs_SystemSetting),1,fp);

		if(ret != 1)
		{
		   DbgOutput(DBG_INFO, "read the file %s error.\n", cSystemFileName);
			gs_SystemSetting.miLanguageID = LanguageID_Chinese;
			gs_SystemSetting.miStartUp_status = INIT_START_STATUS;
			gs_SystemSetting.miStartUp_vol = INIT_START_VOL;
			gs_SystemSetting.miStartUp_orignal = INIT_START_ORIGNAL;
			gs_SystemSetting.miStartUp_stwichOrignal = INIT_START_SWITCHORINGANL;
			strcpy(gs_SystemSetting.msSysPwd,INIT_SYSPWD);
			strcpy(gs_SystemSetting.msReceptionPwd,INIT_RECEPTIONPWD);
			gs_SystemSetting.miOutputMode = 0;
			DbgOutput(DBG_INFO, "use the default settings\n");
			SaveSystemSetting();
		}
		else
		{
			DbgOutput(DBG_INFO, "read the file %s successed.\n", cSystemFileName);
		}
		fclose(fp);
	}
	else
	{
		DbgOutput(DBG_INFO, "open the file %s error.\n", cSystemFileName);
	}

	strcpy(gs_SystemSetting.mShutdownPwd, "10086");
	strcpy(gs_SystemSetting.mSettingPwd, "10086");
	strcpy(gs_SystemSetting.mDeletePwd, "10086");
	if ( gs_SystemSetting.miScreenSave_time < 30 )
		gs_SystemSetting.miScreenSave_time = 30;

	printf("==the light:%d, contrast:%d, Saturation:%d\n",
			gs_SystemSetting.miTV_lightness, gs_SystemSetting.miTV_contrast, gs_SystemSetting.miTV_Saturation);
	printf("---------Language ID:%d---------------the passwd:%s, rePwd:%s.\n",gs_SystemSetting.miLanguageID, gs_SystemSetting.msSysPwd, gs_SystemSetting.msReceptionPwd);

}
void SaveSystemSetting()
{
	int ret = 0;
	char cAppFolder[MAX_PATH] = {0};
	char cSystemFileName[256] = "./KTV_Data/database/SystemInfo.dat";

	GetProgramPath(cAppFolder, MAX_PATH);
	snprintf(cSystemFileName, sizeof(cSystemFileName), "%sKTV_Data/database/SystemInfo.dat", cAppFolder);

	FILE *fp = fopen(cSystemFileName, "wb") ;

	if (fp)
	{
		ret = fwrite(&gs_SystemSetting, sizeof(gs_SystemSetting), 1, fp);
		fflush(fp);
		fflush(fp);
		fclose(fp);
		if(ret != 1)
		{
			 DbgOutput(DBG_INFO, "write the file %s error.\n", cSystemFileName);
		}
		else
		{
			DbgOutput(DBG_INFO, "write the file %s successed.\n", cSystemFileName);
		}
	}
	else
	{
		DbgOutput(DBG_INFO, "open the file %s error.\n", cSystemFileName);
	}
	sync();
}

static char *GetLine(char *s, int size, FILE *fp)
{
	if ( !s || size <= 0 || !fp )
		return NULL;

	char *chptr = NULL;
	if ( (chptr = fgets(s, size, fp)) == NULL )
		return chptr;

	int len = strlen(s);
	while( len > 0 )
	{
		//Del the CR and NL character at the end of line.
		if ( s[len-1] == 0x0A || s[len-1] == 0x0D )
			s[len-1] = '\0';
		--len;
	}
	return s;
}

bool LoadScrollText(CBaseStringA &OutText, UINT32 &color)
{
	bool bLoadOK = false;
	char cScrollFileName[256] = SCROLLTEXT_PATH;
	FILE *fp = fopen(cScrollFileName, "r") ;

	if (fp)
	{
		char txt[1024] = {0};

		if ( GetLine(txt, sizeof(txt), fp) == NULL )
		{
			DbgOutput(DBG_INFO, "get text color error.\n");
		}
		else
		{
			sscanf(txt, "TextColor:%X", &color);
		}

		if ( GetLine(txt, sizeof(txt), fp) == NULL )
			 DbgOutput(DBG_INFO, "get scroll text error.\n");
		else
		{
			OutText.Set(txt);
			bLoadOK = true;
		}

		fclose(fp);

	}
	else
		DbgOutput(DBG_INFO, "open the file %s error.\n", cScrollFileName);

	return bLoadOK;
}

bool SaveScroolText(CBaseStringA &InText, UINT32 color)
{
	bool bSaveOK = false;
	char cScrollFileName[256] = SCROLLTEXT_PATH;
	FILE *fp = fopen(cScrollFileName, "w") ;
	if (fp)
	{
		CBaseStringA Text;

		Text.Format("TextColor:%X\n", color);
		fputs(Text.GetString(), fp);

		Text.Set(InText.GetString());
		fputs(Text.GetString(), fp);

		bSaveOK = true;
		fclose(fp);
		sync();
	}
	else
		DbgOutput(DBG_INFO, "open the file %s error.\n", cScrollFileName);

	return bSaveOK;
}

