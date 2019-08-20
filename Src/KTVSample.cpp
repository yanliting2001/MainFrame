#include <sys/wait.h>
#include "BaseApp.h"
#include "ImgFileOp.h"
#include "./PlayerApi/PlayerServer.h"
#include "GlobalUIClass.h"
#include <signal.h>
#include "./OSDControl/BCM_Control.h"
#include "CurlApi.h"
#include "InternetCtrl/NetCtrlApi.h"
#include "CommApi.h"
#include "SerDrive.h"
#include "DLNA/DlnaApi.h"
#include <locale.h>
#ifdef SOUND_XFYUN
#include 	"SoundCmd.h"
#endif
#include "Sharing/ClientSharing.h"
#include "Record/BaseRecord.h"
#include "CSqlite3.h"
void SignalHandler(int sig)
{
	DbgOutput(DBG_WARN, "Signal %d caught...\n", sig);
	theBaseApp->AddQuitMsg();
}

void PrintMachineLength()
{
	DbgOutput(DBG_WARN, "sizeof(int) = %d, expect=4\n", sizeof(int));
	DbgOutput(DBG_WARN, "sizeof(size_t) = %d, expect=4(32bit machine), 8(64bit machine)\n", sizeof(size_t));
	DbgOutput(DBG_WARN, "sizeof(WORD) = %d, expect=2\n", sizeof(WORD));
	DbgOutput(DBG_WARN, "sizeof(UINT32) = %d, expect=4\n", sizeof(UINT32));
	DbgOutput(DBG_WARN, "sizeof(UINT64) = %d, expect=8\n", sizeof(UINT64));
	DbgOutput(DBG_WARN, "sizeof(__time_t) = %d, expect=4(32bit machine), 8(64bit machine)\n", sizeof(__time_t));
	DbgOutput(DBG_WARN, "sizeof(__suseconds_t) = %d, expect=4(32bit machine), 8(64bit machine)\n", sizeof(__suseconds_t));
	DbgOutput(DBG_WARN, "sizeof(long) = %d, expect=8\n", sizeof(long));
	DbgOutput(DBG_WARN, "sizeof(wchar_t) = %d, expect=4\n", sizeof(wchar_t));
	DbgOutput(DBG_WARN, "sizeof(SONGINFO) = %d, expect=262\n", sizeof(SONGINFO));
	DbgOutput(DBG_WARN, "sizeof(SINGERINFO) = %d, expect=96\n", sizeof(SINGERINFO));
}

BOOL gMainLoopExit = FALSE;
BOOL gInitUIComplete = FALSE;

void InitUI()
{
	DbgOutput(DBG_WARN, "Start...\n");
	CBaseRecord::Init_Record();

#if USE_MODULE_BCM
	InitSysLibJpeg("libjpeg.so");
	InitSysLibPng("libpng.so");
#else
	InitSysLibJpeg("/usr/lib/i386-linux-gnu/libjpeg.so");
	InitSysLibPng("/usr/lib/i386-linux-gnu/libpng.so");
#endif

	InitSQL();

	PlayerServer_Init();
	
#if USE_MODULE_BCM
	gs_SystemSetting.miTV_lightness = 65;//50   电视机亮度饱和度设置
	gs_SystemSetting.miTV_contrast = 40;//25
  gs_SystemSetting.miTV_Saturation = 90;//90
	DbgOutput(DBG_WARN, "Set system RGB brightness = %d ,Contrast=%d ,Stturation = %d\n",gs_SystemSetting.miTV_lightness,gs_SystemSetting.miTV_contrast,gs_SystemSetting.miTV_Saturation);
	g_pPlayerCtrl->setBrightness(gs_SystemSetting.miTV_lightness);
	g_pPlayerCtrl->setContrast(gs_SystemSetting.miTV_contrast);
	g_pPlayerCtrl->setSaturation(gs_SystemSetting.miTV_Saturation);
#endif

	if ( IsInMonkeyMode() )
		theBaseApp->SetInputType(INPUTTYPE_MONKEY);
	else
		theBaseApp->SetInputType(INPUTTYPE_TOUCHKEY);

	theBaseApp->Run(LAYOUT_WIDTH, LAYOUT_HEIGHT);

#if USE_MODULE_BCM
	theBaseApp->SuspendInput();
	if ( g_pBCM_Ctrl && theBaseApp )
		g_pBCM_Ctrl->Create(theBaseApp->GetMainWindow());
#endif

	InitGlobalUIClass();
	gPageManager->InitPages();

#if USE_MODULE_BCM
	if ( g_PLYCtrl.GetInitVideoState() )
		theBaseApp->SuspendInput();								 // BCM在开机视频播放完后恢复
	else
		theBaseApp->ResumeInput();
#else
	theBaseApp->ResumeInput();
#endif
	//theBaseApp->ResumeInput();
}

void DeInitUI()
{
	DbgOutput(DBG_WARN, "Stop...\n");

	PlayerServer_DeInit();

    int xCalibration = -100 * LAYOUT_WIDTH / theE3DEngine->GetDispWidth();
    int yCalibration = -100 * LAYOUT_HEIGHT / theE3DEngine->GetDispHeight();

    theBaseApp->InsertTouchInput(TRUE, TRUE, xCalibration, yCalibration);
    theBaseApp->InsertTouchInput(TRUE, FALSE, xCalibration, yCalibration);
    theBaseApp->InsertTouchInput(TRUE, TRUE, xCalibration, yCalibration);


	DeInitGlobalUIClass();

	DeinitSQL();

	DeInitSysLibJpeg();
	DeInitSysLibPng();
}

UINT64 gLastRefreshTime = 0;
#define REFRESH_DELAY_TIME	33

void NativeOnStep()
{
	UINT64 uWaitTimeStart = GetTickCount();
	UINT64 uWaitTime = uWaitTimeStart-gLastRefreshTime;
	if (uWaitTime < REFRESH_DELAY_TIME)
	{
		DbgOutput(DBG_DEBUG, "%llu: Wait %llu ms\n",
			uWaitTimeStart, REFRESH_DELAY_TIME-uWaitTime);

		theE3DEngine->WaitForEvent(REFRESH_DELAY_TIME-uWaitTime);
	}

	UINT64 uRefreshStart = GetTickCount();
	if (uRefreshStart-gLastRefreshTime >= REFRESH_DELAY_TIME)
	{
		if (!gInitUIComplete)
		{
			// 减少刷新次数，让UI初始化更快�?					    
			static int nFreshTimes = 0;
			if ( (nFreshTimes % 3) == 0 )
			{
				theE3DEngine->Refresh();
			}

			nFreshTimes ++;
		}
		else
		{
			theE3DEngine->Refresh();
		}

		DbgOutput(DBG_DEBUG, "%llu: Refresh interval %llu ms, refresh used %llu ms\n",
			uRefreshStart, uRefreshStart-gLastRefreshTime, GetTickCount()-uRefreshStart);

		gLastRefreshTime = uRefreshStart;
	}
	else
	{
		theE3DEngine->ProcessCommands();
	}
}

int NativeMainLoop()
{
	DbgOutput(DBG_INFO, "function %s enter\n", __FUNCTION__);

	InitUI();
	gInitUIComplete = TRUE;
	DbgOutput(DBG_INFO, "##############InitUI Completed############\n");
#if !USE_MODULE_BCM
	gMainCtrlPage->mSharingIndexWnd.StartCheckLogin();
#endif

	MESSAGE msg;
	while (theBaseApp->GetMsg(&msg))
	{
		DbgOutput(DBG_DEBUG, "get window message, wnd=0x%08x, type=%u, wparam=%llu, lparam=%llu\n",
			msg.pWnd, msg.uType, msg.wParam, msg.lParam);

		//Process Msg
		theBaseApp->DispatchMsg(&msg);
	}

#define FASTPOWEROFF
#ifdef FASTPOWEROFF
	//gMainCtrlPage->SaveFavorit();
	PlayerServer_DeInit();

    int xCalibration = -100 * LAYOUT_WIDTH / theE3DEngine->GetDispWidth();
    int yCalibration = -100 * LAYOUT_HEIGHT / theE3DEngine->GetDispHeight();

    theBaseApp->InsertTouchInput(TRUE, TRUE, xCalibration, yCalibration);
    theBaseApp->InsertTouchInput(TRUE, FALSE, xCalibration, yCalibration);
    theBaseApp->InsertTouchInput(TRUE, TRUE, xCalibration, yCalibration);

    Dlna_DeInit();

	BCM_DeInit();

	DeInitCurl();
	sync();

#if USE_MODULE_BCM
	system("poweroff");
#endif

#endif
	DeInitUI();
	gInitUIComplete = FALSE;
	gMainLoopExit = TRUE;

	DbgOutput(DBG_INFO, "function %s leave\n", __FUNCTION__);

	return 0;
}

static CBaseThread  gNativeThread;
LPVOID NativeMainThreadFunc(LPVOID lpParam)
{
	//printf("the pthread of NativeThread ID is:%ld.\n",syscall(SYS_gettid));
	NativeMainLoop();

	return NULL;
}

void NativeStart(const char* cDataFolder, int nDispWidth, int nDispHeight)
{
	DbgOutput(DBG_INFO, "function %s ...\n", __FUNCTION__);

	gMainLoopExit = FALSE;

	PrintMachineLength();

	DbgOutput(DBG_INFO, "cDataFolder = %s\n", cDataFolder);
	InitBaseApp(cDataFolder);
	InitE3DEngine(nDispWidth, nDispHeight);
	
	DbgOutput(DBG_INFO, "cDataFolder = %s,DBG_TO_FILE=%d\n", cDataFolder,DBG_TO_FILE);

	#ifdef DBG_TO_FILE
	char cDbgFileName[MAX_PATH];
	GetFileNameByDateA(cDbgFileName, "log");
	CombinePathName(cDbgFileName, theBaseApp->GetLogFolder(), cDbgFileName);
	DbgOutputOpenFile(cDbgFileName, FALSE);
	DbgOutput(DBG_INFO, "cDbgFileName, = %s\n", cDbgFileName);
	#endif


	gLastRefreshTime = GetTickCount();

	gNativeThread.StartThread(NativeMainThreadFunc, NULL, "NativeMainThreadFunc");
}

void NativeStop()
{
	DbgOutput(DBG_INFO, "Native Thread stopping...\n");
	gNativeThread.StopThread();
	DbgOutput(DBG_INFO, "Native Thread stopped OK...\n");

	DeInitE3DEngine();
	DeInitBaseApp();

#ifdef DETECT_TEXTURE_LEAK
	DbgOutput(DBG_INFO, "Dump Unfree textures...\n");
	DumpUnFreeTexture();
	DeleteAllTexture();
#endif

	#ifdef DBG_TO_FILE
	DbgOutputCloseFile();
	#endif
}


int main2(int argc ,char *argv[])
{
	setlocale(LC_CTYPE, "zh_CN.UTF-8");
	signal(SIGABRT, &SignalHandler);
	signal(SIGTERM, &SignalHandler);
	signal(SIGINT, &SignalHandler);

	//testRecordeApi();
	printf("\n\n==========Release date : %s %s==========\n\n", __DATE__, __TIME__);
	StartMinitorNet();
	StartMinitorCoinThread();

#ifdef SOUND_XFYUN
	SoundCmdInit();
#endif

	DetectSongParentPath();
	CCSqlite3::Sqlite3_InitDBPath();
	CCSqlite3::Sqlite3_OpenDB(CCSqlite3::mcDBFile);

	char cProgramFolder[MAX_PATH];
	GetProgramPath(cProgramFolder, MAX_PATH);
	char cDataFolder[MAX_PATH];
	CombinePathName(cDataFolder, cProgramFolder, "KTV_Data");

	ReadSystemSetting();

	BCM_Init();
	{
		NativeStart(cDataFolder, 1920, 1080);

		while ( !gMainLoopExit )
		{
			NativeOnStep();
		}

		SaveSystemSetting();
		NativeStop();
	}

	Dlna_DeInit();
	DeInitCurl();
	BCM_DeInit();
	StopMinitorNet();

	DbgOutput(DBG_INFO, "Main Function return...\n");
#if USE_MODULE_BCM
	DbgOutput(DBG_INFO, "poweroff...\n");

	system("poweroff");
#endif

	return 0;
}

///add by EVA
#define  ABLE_WATCHDOG_USE  1
//#undef ABLE_WATCHDOG_USE

int main(int argc ,char *argv[])
{
#ifdef  ABLE_WATCHDOG_USE
    printf("[%s]====start start ======\n", __FUNCTION__);
  
    pid_t pid;
    int status;

    if ( (pid = fork()) < 0)
    {
      printf("[%s]fork error error !!!!!!!!!!!!!\n",__func__);
      printf("[%s] fork fail , will reboot in %d seconds ,by watch dog ! \n",__func__,3);
      sleep(3);
      system("reboot") ;
      while(1)
      {
          sleep(1) ;
      }
      return 1 ;
    }
    else if (pid == 0)
    { 
      printf("[%s] after fork, i am a  child process!!!!!!!!!!!!\n",__func__);
      //////////////////child process end end!!!!!!!!!!!!!!!!!!!!!!
#endif
      return main2(argc,argv);

#ifdef  ABLE_WATCHDOG_USE
    }
    else
    {
      printf("here is parent/main process !!!!!!!!!!!!!!!!!!!!!!!!!\n");
      
      wait(&status); // block myself and wait dead child process !!!!!!!!!!!!!!!!!!!!
    
      if (WIFEXITED(status)) 
      {
          printf("normal termination, exitstatus = %d\n", WEXITSTATUS(status));
      }
      else if (WIFSIGNALED(status))
      {
          printf("abnormal termination, signalstatus = %d\n", WTERMSIG(status), 
#ifdef WCOREDUMP
          WCOREDUMP(status)?"(core file generated)":""); 
#else 
          "");
#endif
      }
      else if (WIFSTOPPED(status))
      {
          printf("child stopped, signal number = %d\n", WSTOPSIG(status));
      }
      
      printf("[%s]  will reboot in %d seconds ,by watch dog ! \n",__func__,3);
      sleep(3);
      system("reboot") ;
      while(1)
      {
          sleep(1) ;
      }
      return 0; 
    }
#endif
}

