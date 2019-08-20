#include "AddSongWnd.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
//#include "AdvanceSettings.h"
#include "BaseWnd.h"

#include <sys/stat.h>
#include <sys/vfs.h>
#include "CSGetMacAddr.h"

//#include "../SQL/SearchSQL.h"

typedef enum eInfoCount
{
	ID=0,
	Name,
	WordLength,
	Singer,
	Type,
	Lang,
	ServerName,
	FilePath,
	FileName,
	PicPath,
	Spell,
	Vol,
	MaxVol,
	MinVol,
	Track,
	Chos,
	Exist,
	NewSong,
	Stroke,
	InfoCount
}InfoTypeCount;

#define UDISKINFOPATH			"info"
#define UDISKINFO				"info.txt"
#if defined(__arm__)
#define SONGPATH			"/stb/config/media"
#else
#define SONGPATH				"/media"
#endif

#define BUTTON_ID_CHOOSEBASE	0x01
#define BUTTON_ID_SONG			0x11
#define BUTTON_ID_PrevPage		0x30
#define BUTTON_ID_NextPage 		0x31
#define BUTTON_ID_Back			0x32
#define BUTTON_ID_Insert		0x33

static char gAddSongType[TYPE_UDISK_ADD_COUNT][64] =
{
		"批量加歌",
		"手动加歌"
};

static char UDISKMOUNTPATH[64] = {0};

CAddSongWnd::CAddSongWnd()
{
	miCurSongPageNum = 0;
	miSongPageNum = 0;
	mCurAddType = TYPE_MANUAL;
	memset(mTitleIcoBasePath,0,sizeof(mTitleIcoBasePath));
	mLocalSongID = 1;
	mSuccessCopyNum = 0;
	mChooseNum = 0;
}

CAddSongWnd::~CAddSongWnd()
{
	DelNormalList(&mUdiskList, UDSONGINFO);
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CAddSongWnd::Create(
		CBaseWnd *pParent,
		RECT rc)
{
	CParentClass::Create("UdiskAddSong", pParent, rc);

	RECT rcControl;
	//mWaitingWnd.Create(this);

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mTileIcoWnd.CreateStatic(this, rcControl);
	mTileWnd.CreateStatic(this, rcControl);
	mCopyStateWnd.CreateStatic(this,rcControl);

	SetRectXY(&rcControl, 236, 30, 382, 55);
	for(int i = 0; i < ADDSONGWND_ITEM_COUNT; i++)
	{
		mUItem[i].SongWnd.Create(this, rcControl);
		mUItem[i].SongWnd.SetWndID(BUTTON_ID_SONG + i);
		mUItem[i].SongWnd.SetOnClickListener(this);
		mUItem[i].SongWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

		mUItem[i].ChooseBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
		mUItem[i].ChooseBtn.SetWndID(BUTTON_ID_CHOOSEBASE + i);
		mUItem[i].ChooseBtn.SetOnClickListener(this);

		mUItem[i].TiaoWnd.CreateStatic(this, rcControl);

		memset(&mUItem[i].USong,0,sizeof(UDSONGINFO));
	}

	SetRectXY(&rcControl, 236, 30, 382, 55);
	mPageInfoWnd.CreateStatic(this, rcControl);

	SetRectXY(&rcControl, 246, 293, 105, 50);
	mPrevPageBtn.Create(this, rcControl);
	mPrevPageBtn.SetWndID(BUTTON_ID_PrevPage);
	mPrevPageBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 70, 297, 172, 50);
	mNextPageBtn.Create(this, rcControl);
	mNextPageBtn.SetWndID(BUTTON_ID_NextPage);
	mNextPageBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 246, 293, 105, 50);
	BackBtn.Create(this, rcControl);
	BackBtn.SetWndID(BUTTON_ID_Back);
	BackBtn.SetOnClickListener(this);

	SetRectXY(&rcControl, 246, 293, 105, 50);
	InsertBtn.Create(this, rcControl);
	InsertBtn.SetWndID(BUTTON_ID_Insert);
	InsertBtn.SetOnClickListener(this);

	LoadResource();
}

void CAddSongWnd::LoadResource()
{
	/*init position by xml*/
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "AddSongWnd.xml");
	if(false == parser.Load(cFileName))
	{
		printf("failed to load xml file!!\n");
	}

	// read from xml

	RECT rcControl;

	XmlLoadRect(&parser, "ShowWindowLocation", &rcControl);
	MoveWindow(&rcControl);

	RECT rcSongItem, rcChoose,rcTiao;
	int i, xOffset, yOffset;

	XmlLoadRect(&parser, "SongInfoLocation", &rcSongItem);
	xOffset = parser.GetIntValue("SongInfoLocation", "xOffset", 371);
	yOffset = parser.GetIntValue("SongInfoLocation", "yOffset", 371);

	XmlLoadRect(&parser, "ChooseLocation", &rcChoose);

	XmlLoadRect(&parser, "TiaoLocation", &rcTiao);

	for(i = 0; i < ADDSONGWND_ITEM_COUNT; i++)
	{
		int j = i % 2;
		int k = i / 2;
		InflatRect(&rcControl, &rcSongItem, j*xOffset, k*yOffset);
		mUItem[i].SongWnd.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcChoose, j*xOffset, k*yOffset);
		mUItem[i].ChooseBtn.MoveWindow(&rcControl);

		if(i < 6)
		{
			InflatRect(&rcControl, &rcTiao, 0, i*yOffset);
			mUItem[i].TiaoWnd.MoveWindow(&rcControl);
		}
		else
		{
			InflatRect(&rcControl, &rcTiao, 0, 0);
			mUItem[i].TiaoWnd.MoveWindow(&rcControl);
		}
	}

	XmlLoadRect(&parser, "TileIcoLocation", &rcControl);
	mTileIcoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "TileWndLocation", &rcControl);
	mTileWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CopyStateLocation", &rcControl);
	mCopyStateWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PageInfoWndLocation", &rcControl);
	mPageInfoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PrevPageLocation", &rcControl);
	mPrevPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NextPageLocation", &rcControl);
	mNextPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "BackLocation", &rcControl);
	BackBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "InsertLocation", &rcControl);
	InsertBtn.MoveWindow(&rcControl);
	/*end init position by xml*/

	char imgPath[64] = {0};
	CTexture bkTexture;

	strcpy(imgPath, parser.GetStrValue("PageInfoImg","path","AdvanceSettings/BG"));
	CreateImgTexture(imgPath, &bkTexture);
	mPageInfoWnd.SetBackgroundTexture(&bkTexture);

	strcpy(mTitleIcoBasePath,parser.GetStrValue("TitleIcoBaseImg","path","AdvanceSettings/MusicLib/Udisk/Choose"));

	CTexture btnTextures[4];
	strcpy(imgPath,parser.GetStrValue("ChooseBtnImg","path","AdvanceSettings/MusicLib/Choose"));
	CreateBtnImgTextures(imgPath, btnTextures);

	strcpy(imgPath,parser.GetStrValue("TiaoImg","path","AdvanceSettings/MusicLib/Choose"));
	CreateImgTexture(imgPath, &bkTexture);

	for(i=0; i<ADDSONGWND_ITEM_COUNT; i++)
	{
		mUItem[i].ChooseBtn.SetTextures(btnTextures);
		mUItem[i].TiaoWnd.SetBackgroundTexture(&bkTexture);
	}

	strcpy(imgPath,parser.GetStrValue("PrevPageBtnImg","path","AdvanceSettings/MusicLib/Udisk/PrevPage"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPrevPageBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("NextPageBtnImg","path","AdvanceSettings/MusicLib/Udisk/NextPage"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNextPageBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("BackBtnImg","path","AdvanceSettings/MusicLib/Udisk/Back"));
	CreateBtnImgTextures(imgPath, btnTextures);
	BackBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("InsertBtnImg","path","AdvanceSettings/MusicLib/Udisk/Insert"));
	CreateBtnImgTextures(imgPath, btnTextures);
	InsertBtn.SetTextures(btnTextures);

	strcpy(UDISKMOUNTPATH, parser.GetStrValue("usbmount", "path", "/stb/disk/sdb1"));
  DbgOutput(DBG_WARN,"[%s-%s-%d] UDISKMOUNTPATH = %s \n",__FILE__,__FUNCTION__,__LINE__,UDISKMOUNTPATH);
	strcpy(gAddSongType[TYPE_BATCH], parser.GetStrValue("TitlePrompt", "batchmsg", "批量加歌"));
	strcpy(gAddSongType[TYPE_MANUAL], parser.GetStrValue("TitlePrompt", "manualmsg", "手动加歌"));
}

void CAddSongWnd::SetAddSongType(UDISKADDTYPE type)
{
	mCurAddType = type;

	char ico[64] = {0};

	sprintf(ico,"%s/ico%d",mTitleIcoBasePath,mCurAddType);

	CTexture bkTexture;
	CreateImgTexture(ico, &bkTexture);
	mTileIcoWnd.SetBackgroundTexture(&bkTexture);

	mTileWnd.SetWindowTextA(gAddSongType[mCurAddType]);
	mCopyStateWnd.SetWindowVisible(FALSE);

	miCurSongPageNum = 0;
	mChooseNum = 0;
	mSuccessCopyNum = 0;
	mLocalSongID = 1;						//初始化本地ID
	GetUdiskInfo();
}

void CAddSongWnd::GetUdiskInfo()
{
	DelNormalList(&mUdiskList, UDSONGINFO);
	//最多支持5个分区
	char strUsbMountPath[64] = {0};
	char strUsbMountRootPath[64] = "/stb/usb/sd";
	if(mCurAddType == TYPE_BATCH)
	{
		// 增加对如：/std/usb/sda、 /std/usb/sdb 等目录检查
		for( int j = 'a'; j < ('a'+5); j++)
		{
			sprintf(UDISKMOUNTPATH, "%s%c", strUsbMountRootPath, j);
			for(int i = 0; i < 10; i++)
			{
				if(i == 0)
					strcpy(strUsbMountPath, UDISKMOUNTPATH);
				else
					snprintf(strUsbMountPath, sizeof(strUsbMountPath)-1, "%s%d", UDISKMOUNTPATH, i);
        DbgOutput(DBG_WARN,"[%s-%s-%d] strUsbMountPath = %s \n",__FILE__,__FUNCTION__,__LINE__,strUsbMountPath);
				SearchFromUdiskForBatch(strUsbMountPath);
			}
		}
	}
	else
	{
		for( int j = 'a'; j < ('a'+5); j++)
		{
			sprintf(UDISKMOUNTPATH, "%s%c", strUsbMountRootPath, j);
			for(int i = 0; i < 10; i++)
			{
				if(i == 0)
					strcpy(strUsbMountPath, UDISKMOUNTPATH);
				else
					snprintf(strUsbMountPath, sizeof(strUsbMountPath)-1, "%s%d", UDISKMOUNTPATH, i);
        DbgOutput(DBG_WARN,"[%s-%s-%d] strUsbMountPath = %s \n",__FILE__,__FUNCTION__,__LINE__,strUsbMountPath);
				SearchFromUdiskForManual(strUsbMountPath,0);
			}
		}
	}

	int iSongNum = mUdiskList.GetCount();
	miCurSongPageNum = 0;
	miSongPageNum = (iSongNum + ADDSONGWND_ITEM_COUNT-1) / ADDSONGWND_ITEM_COUNT;
	UpdateUdiskInfo();
}

void CAddSongWnd::SearchFromUdiskForManual(const char *UdiskPath,int iLoopDepth)
{
	DIR *pDir = NULL;
	struct dirent *file = NULL;
	char filePath[128] = {0};
	char picName[256] = {0};

	if(UdiskPath == NULL)
	{
		return;
	}

	strcpy(filePath,UdiskPath);
	if((pDir = opendir(UdiskPath)) == NULL)
	{
		DbgOutput(DBG_INFO,"open dirent:%s failed\n",UdiskPath);
		return;
	}

	while((file = readdir(pDir)) != NULL)
	{
		if(strncmp(file->d_name,".",1) == 0)
		{
			continue;
		}
		else if(file->d_type == DT_DIR)//判断是否是目录
		{
			if(iLoopDepth >= 1)
				continue;
			char subfilePath[256] = {0};
			sprintf(subfilePath,"%s/%s",UdiskPath,file->d_name);
			SearchFromUdiskForManual(subfilePath,1);
			continue;
		}

		if( (strcasestr(file->d_name,".mp4") == NULL) 	&&
			(strcasestr(file->d_name,".avi") == NULL) 	&&
			(strcasestr(file->d_name,".mov") == NULL) 	&&
			(strcasestr(file->d_name,".mkv") == NULL) 	&&
			(strcasestr(file->d_name,".asf") == NULL) 	&&
			(strcasestr(file->d_name,".wmv") == NULL) 	&&
			(strcasestr(file->d_name,".mpg") == NULL) 	&&
			(strcasestr(file->d_name,".vob") == NULL) 	&&
			(strcasestr(file->d_name,".ts") == NULL) 	&&
			(strcasestr(file->d_name,".f4v") == NULL) 	&&
			(strcasestr(file->d_name,".flv") == NULL) 	&&
			(strcasestr(file->d_name,".dvd") == NULL) 	&&
			(strcasestr(file->d_name,".svcd") == NULL) 	&&
			(strcasestr(file->d_name,".vcd") == NULL) 	&&
			(strcasestr(file->d_name,".ac3") == NULL) 	&&
			(strcasestr(file->d_name,".m4a") == NULL) 	&&
			(strcasestr(file->d_name,".mka") == NULL) 	&&
			(strcasestr(file->d_name,".mp2") == NULL) 	&&
			(strcasestr(file->d_name,".mp3") == NULL) 	&&
			(strcasestr(file->d_name,".wav") == NULL) 	&&
			(strcasestr(file->d_name,".wma") == NULL) 	&&
			(strcasestr(file->d_name,".m2ts") == NULL) 	&&
			(strcasestr(file->d_name,".aac") == NULL) )
		{
			continue;
		}

		UDSONGINFO *pNewSong = new UDSONGINFO;
		if (!pNewSong)
		{
			return ;
		}
		memset((char *)pNewSong,0,sizeof(UDSONGINFO));
		sprintf(pNewSong->cID,"%08d",mLocalSongID++);
		strcpy(pNewSong->cFileName,file->d_name);
		snprintf(pNewSong->cFilePath,sizeof(pNewSong->cFilePath)-1,"%s/%s",filePath,file->d_name);

		//判断是否存在歌曲缩略图,是歌曲名同名的.jpg文件
		SAFE_STRNCPY(picName, pNewSong->cFilePath, 256);
		char *pDot = strrchr(picName, '.');
		if ( pDot != NULL )
		{
			*pDot = '\0';
			sprintf(pDot, ".jpg");
			if ( IsFileExist(picName) )			//存在歌曲名同名的.jpg文件
				SAFE_STRNCPY(pNewSong->cPicPath, picName, sizeof(pNewSong->cPicPath));
		}

		pNewSong->bIsChoose = FALSE;
		mUdiskList.AddData(pNewSong);
	}

	closedir(pDir);
}

void CAddSongWnd::SearchFromUdiskForBatch(const char *UdiskPath)
{
	DIR *pDir = NULL;
	struct dirent *file = NULL;
	char UdiskInfoPath[128] = {0};

	if(UdiskPath == NULL)
	{
		return;
	}

	snprintf(UdiskInfoPath,sizeof(UdiskInfoPath)-1,"%s/%s",UdiskPath,UDISKINFOPATH);
	if((pDir = opendir(UdiskInfoPath)) == NULL)
	{
		DbgOutput(DBG_INFO,"open dirent:%s failed\n",UdiskPath);
		return;
	}

	while((file = readdir(pDir)) != NULL)
	{
		if(strncmp(file->d_name,".",1) == 0)
		{
			continue;
		}
		if(strcmp(file->d_name,UDISKINFO) == 0)
		{
			GetSongInfoFromTxt(UDISKINFO,UdiskPath);
			break;
		}
	}

	closedir(pDir);
}

void CAddSongWnd::UpdateUdiskInfo()
{
	int nFirstIndex = miCurSongPageNum * ADDSONGWND_ITEM_COUNT;
	int i;

	char temp[256] = {0};

	for ( i = 0 ; i < ADDSONGWND_ITEM_COUNT ; i++ )
	{
		UDSONGINFO *pSong = (UDSONGINFO *)mUdiskList.GetAt(i+nFirstIndex);
		if (pSong)
		{
			memcpy(&mUItem[i].USong,pSong,sizeof(UDSONGINFO));
			sprintf(temp,"%d.  %s",i+nFirstIndex,pSong->cFileName);
			if(strlen(pSong->cName) < 1)
			{
				mUItem[i].SongWnd.SetFontColor(0xFFFFFFFF);
			}
			else
			{
				mUItem[i].SongWnd.SetFontColor(0xFFFF00FF);
			}
			mUItem[i].SongWnd.SetWindowTextA(temp);

			if(!pSong->bIsChoose)
			{
				mUItem[i].ChooseBtn.SetRadioBtnPressed(FALSE);
			}
			else
			{
				mUItem[i].ChooseBtn.SetRadioBtnPressed(TRUE);
			}
			mUItem[i].ChooseBtn.SetWindowVisible(TRUE);
		}
		else
		{
			memset(&mUItem[i].USong,0,sizeof(UDSONGINFO));
			mUItem[i].SongWnd.SetWindowTextA(NULL);
			mUItem[i].ChooseBtn.SetWindowVisible(FALSE);
		}
	}

	CBaseStringW sTmp;

	sTmp.Format("%d/%d", miCurSongPageNum+1,miSongPageNum);
	mPageInfoWnd.SetWindowTextW(sTmp.GetString());
}

void CAddSongWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();

	if(nWndID >= BUTTON_ID_CHOOSEBASE &&
		(nWndID < BUTTON_ID_CHOOSEBASE + ADDSONGWND_ITEM_COUNT))
	{
		int nIndex = nWndID-BUTTON_ID_CHOOSEBASE;
		SetChooseIndex(nIndex);
	}
	else if(nWndID >= BUTTON_ID_SONG &&
			(nWndID < BUTTON_ID_SONG + ADDSONGWND_ITEM_COUNT))
	{
		int nIndex = nWndID-BUTTON_ID_SONG;
		SetChooseSong(nIndex);
	}
	else if(nWndID == BUTTON_ID_PrevPage)
	{
		if (miCurSongPageNum > 0)
		{
			miCurSongPageNum --;
			UpdateUdiskInfo();
		}
	}
	else if(nWndID == BUTTON_ID_NextPage)
	{
		if (miCurSongPageNum < (miSongPageNum-1))
		{
			miCurSongPageNum ++;
			UpdateUdiskInfo();
		}
	}
	else if(nWndID == BUTTON_ID_Back)
	{
		SetWindowVisible(FALSE);
		DelNormalList(&mUdiskList, UDSONGINFO);
		gMusicLibWnd->SetWindowVisible(TRUE);
	}
	else if(nWndID == BUTTON_ID_Insert)
	{
		OnInsertClick();
	}
}


void CAddSongWnd::SetChooseIndex(int nIndex)
{
	if((nIndex<0) || (nIndex >=ADDSONGWND_ITEM_COUNT))
	{
		DbgOutput(DBG_VERB, "function %s:%d ...\n", __FILE__,__LINE__);
		return ;
	}

	int nFirstIndex = miCurSongPageNum * ADDSONGWND_ITEM_COUNT;

	UDSONGINFO *pSong = (UDSONGINFO *)mUdiskList.GetAt(nIndex+nFirstIndex);
	if(!pSong)
	{
		DbgOutput(DBG_VERB, "function %s:%d ...\n", __FILE__,__LINE__);
		return ;
	}

	if(mCurAddType == TYPE_MANUAL)
	{
		if(strlen(pSong->cName) < 1)
		{
			mUItem[nIndex].ChooseBtn.SetRadioBtnPressed(FALSE);
			return;
		}
	}

	if(mUItem[nIndex].ChooseBtn.GetRadioBtnPressed())
	{
		pSong->bIsChoose = TRUE;
		mChooseNum++;
	}
	else
	{
		pSong->bIsChoose = FALSE;
		mChooseNum--;
	}

	UpdateUdiskInfo();
}

void CAddSongWnd::SetChooseSong(int nIndex)
{
	if((nIndex<0) || (nIndex >=ADDSONGWND_ITEM_COUNT))
	{
		DbgOutput(DBG_VERB, "function %s:%d ...\n", __FILE__,__LINE__);
		return ;
	}

	if(mCurAddType != TYPE_MANUAL)
		return;

	int nFirstIndex = miCurSongPageNum * ADDSONGWND_ITEM_COUNT;

	UDSONGINFO *pSong = (UDSONGINFO *)mUdiskList.GetAt(nIndex+nFirstIndex);
	if(!pSong)
	{
		DbgOutput(DBG_VERB, "function %s:%d ...\n", __FILE__,__LINE__);
		return ;
	}

	SetWindowVisible(FALSE);
	gUdiskSongInfoWnd->SetSongInfo(pSong);
	gUdiskSongInfoWnd->SetWindowVisible(TRUE);
}

void CAddSongWnd::OnInsertClick()
{
	//mWaitingWnd.StartWaiting();
	gMainCtrlPage->StartWaiting();
	AddSongToDataAndDisk();
	gMainCtrlPage->StopWaiting();
	char temp[256] = {0};
	sprintf(temp,"成功：%d,失败：%d",mSuccessCopyNum,mChooseNum - mSuccessCopyNum);
	mCopyStateWnd.SetWindowTextA(temp);
	mCopyStateWnd.SetWindowVisible(TRUE);
}


void CAddSongWnd::GetSongInfoFromTxt(const char *filename,const char *filePath)
{
	char buffer[256] = {0};
	FILE *fp = NULL;

	if((filename == NULL) || (filePath == NULL))
	{
		return;
	}

	sprintf(buffer,"%s/%s/%s",filePath,UDISKINFOPATH,filename);
	if((fp = fopen(buffer,"r")) == NULL)
	{
		DbgOutput(DBG_VERB,"open file:%s failed\n",filename);
		return;
	}

	while(!feof(fp))
	{
		memset(buffer,0,sizeof(buffer));
		fgets(buffer,sizeof(buffer),fp);
		SetSongInfoFromBuf(buffer,filePath);
	}

	fclose(fp);
}

void CAddSongWnd::SetSongInfoFromBuf(char *buf,const char *filePath)
{
	char CmdMsg[InfoCount][128] = {{0}};
	int num = 0;
	char fileName[256] = {0};
	char picName[256] = {0};

	if((buf == NULL) || (filePath == NULL))
	{
		return;
	}
	if(strncmp(buf,"##",2) == 0)
	{
		//注释内容,忽略
		return;
	}

	char *p=NULL,*q=NULL;
	p = buf;

	while((q = strchr(p,'@')) != NULL )
	{
	  	*q = '\0';
	   	strcpy(CmdMsg[num],p);

//	   	printf(" %s,",CmdMsg[num]);
	   	num++;
	   	if(num >= InfoCount)
	   	{
			break;
		}
	   	p = q + 1;
	}

	sprintf(picName, "%s/%s", filePath, CmdMsg[PicPath]);
	sprintf(fileName,"%s/%s",filePath,CmdMsg[FileName]);
	if((num == 0) || (access(fileName, R_OK|W_OK) != 0))
	{
		return;
	}

	UDSONGINFO *pNewSongInfo = new UDSONGINFO;
	if (!pNewSongInfo)
	{
		return;
	}
	memcpy(pNewSongInfo->cID,CmdMsg[ID],sizeof(pNewSongInfo->cID));
	memcpy(pNewSongInfo->cName,CmdMsg[Name],sizeof(pNewSongInfo->cName));
	pNewSongInfo->wWdCt = atoi(CmdMsg[WordLength]);
	memcpy(pNewSongInfo->cSinger,CmdMsg[Singer],sizeof(pNewSongInfo->cSinger));
	memcpy(pNewSongInfo->cType,CmdMsg[Type],sizeof(pNewSongInfo->cType));
	memcpy(pNewSongInfo->cLang,CmdMsg[Lang],sizeof(pNewSongInfo->cLang));
	memcpy(pNewSongInfo->cServerName,CmdMsg[ServerName],sizeof(pNewSongInfo->cServerName));
	//memcpy(pNewSongInfo->cFilePath,CmdMsg[FilePath],sizeof(pNewSongInfo->cFilePath));
	memcpy(pNewSongInfo->cFilePath, fileName,sizeof(pNewSongInfo->cFilePath));			//需要全路径
	memcpy(pNewSongInfo->cFileName,CmdMsg[FileName],sizeof(pNewSongInfo->cFileName));
	//memcpy(pNewSongInfo->cPicPath,CmdMsg[PicPath],sizeof(pNewSongInfo->cPicPath));
	memcpy(pNewSongInfo->cPicPath, picName,sizeof(pNewSongInfo->cPicPath));		//需要全路径
	memcpy(pNewSongInfo->cSpell,CmdMsg[Spell],sizeof(pNewSongInfo->cSpell));
	pNewSongInfo->wVol = atoi(CmdMsg[Vol]);
	pNewSongInfo->wMaxVol = atoi(CmdMsg[MaxVol]);
	pNewSongInfo->wMinVol = atoi(CmdMsg[MinVol]);
	pNewSongInfo->wTrack = atoi(CmdMsg[Track]);
	pNewSongInfo->bChos = atoi(CmdMsg[Chos]);
	pNewSongInfo->bExist = atoi(CmdMsg[Exist]);
	pNewSongInfo->bNewSong = atoi(CmdMsg[NewSong]);
	memcpy(pNewSongInfo->cStroke,CmdMsg[Stroke],sizeof(pNewSongInfo->cStroke));
	pNewSongInfo->bIsChoose = FALSE;
	mUdiskList.AddData(pNewSongInfo);
}

void CAddSongWnd::AddSongToDataAndDisk()
{
	char CopyCmd[256];
	SONGINFO SongInfo;
	UDSONGINFO *pChooseSong = NULL;

//	const char* cDBFilePath = theBaseApp->GetDataBaseFolder();
//	char cDBName[MAX_PATH];
//	CombinePathName(cDBName, cDBFilePath, "vod.db");

//	if(ConnectDb(cDBName) == -1)
//	{
//		DbgOutput(DBG_VERB,"open database :%s failed\n",cDBName);
//		return ;
//	}
	BeginTransaction();

	int count = mUdiskList.GetCount();

	for(int i=0; i<count; i++)
	{
		pChooseSong = (UDSONGINFO *)mUdiskList.GetAt(i);
		if ((!pChooseSong) || (!pChooseSong->bIsChoose))
		{
			continue;
		}
		CopyChooseToAddSong(&SongInfo,pChooseSong);
		//if(WhetherSpaceEnough(pChooseSong->cFilePath))
		if ( CheckDiskFreeSpaceEnough(SONGPATH) == 0 )
		{
			//if(strlen(pChooseSong->cPicPath) > 0)
			if ( strstr(pChooseSong->cPicPath, "jpg") != NULL )
			{
				//存在歌曲缩略图
//				sprintf(CopyCmd,"cp \"%s\" KTV_Data/song_textures/%s",pChooseSong->cPicPath, SongInfo.cPicPath);
//				printf("##########cp pic cmd:%s\n", CopyCmd);
//				system(CopyCmd);
			}

			pid_t status = 0;
			sprintf(CopyCmd,"cp \"%s\" %s",pChooseSong->cFilePath,SongInfo.cFilePath);
			printf("########cp:%s.\n", CopyCmd);
			status = system(CopyCmd);
			if((status == -1) || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
			{
				DbgOutput(DBG_VERB,"%s, exit code: %d\n", CopyCmd, WEXITSTATUS(status));
				continue;
			}
			FastAdd_SongList(&SongInfo);
			Sqlite3_insert_song(&SongInfo);
			mSuccessCopyNum++;
			FastSearch_IncreaseMaxSongID(1);
		}
		else
		{
			DbgOutput(DBG_VERB,"The %s space is not enough\n",SONGPATH);
			return ;
		}
	}

	FastSearch_SaveAllList();
	Commit();
	DeleteView();
	CreateView();
	//DisconnectDb();
}

BOOL CAddSongWnd::WhetherSpaceEnough(const char *srcSongSize)
{
	if(srcSongSize == NULL)
	{
		DbgOutput(DBG_VERB,"The error in file:%s, function:%s, line:%d\n",__FILE__,__FUNCTION__,__LINE__);
		return FALSE;
	}
	struct statfs diskInfo;
	struct stat buf;

	statfs(SONGPATH,&diskInfo);
	unsigned long long freeSize = (unsigned long long)(diskInfo.f_bavail * (diskInfo.f_bsize /1024))/1024;			// 1024*1024 =1MB  换算成MB单位
	freeSize -= (5 * 1024 );			//需要预留5G的空闲空间
	stat(srcSongSize,&buf);
	unsigned long long needSize = (unsigned long long)(buf.st_size)/1024/1024;

	return (freeSize > needSize) ? TRUE : FALSE;
}

int CAddSongWnd::CopyChooseToAddSong(SONGINFO *dstSong, UDSONGINFO *srcSong)
{
	if((NULL == dstSong) || (NULL == srcSong))
		return -1;

//	int CurMaxSongID = FastSearch_GetMaxSongID();
//	sprintf(dstSong->cID,"%08d",CurMaxSongID+1);
//	memcpy(dstSong->cName,srcSong->cName,sizeof(dstSong->cName));
//	dstSong->wWdCt = srcSong->wWdCt;
//	memcpy(dstSong->cSinger,srcSong->cSinger,sizeof(dstSong->cSinger));
//	memcpy(dstSong->cType,srcSong->cType,sizeof(dstSong->cType));
//	memcpy(dstSong->cLang,srcSong->cLang,sizeof(dstSong->cLang));
//	memcpy(dstSong->cServerName,srcSong->cServerName,sizeof(dstSong->cServerName));
//	sprintf(dstSong->cFileName,"dvd%s.mpg",dstSong->cID);
//	sprintf(dstSong->cFilePath,"%s/songs/%s",SONGPATH,dstSong->cFileName);
//	sprintf(dstSong->cPicPath,"%s_1.jpg",dstSong->cID);
//	memcpy(dstSong->cSpell,srcSong->cSpell,sizeof(dstSong->cSpell));
//	dstSong->wVol = srcSong->wVol;
//	dstSong->wMaxVol = srcSong->wMaxVol;
//	dstSong->wMinVol = srcSong->wMinVol;
//	dstSong->wTrack = srcSong->wTrack;
//	dstSong->bChos = srcSong->bChos;
//	dstSong->bExist = 1;
//	dstSong->bCloud = 0;
//	dstSong->bNewSong = srcSong->bNewSong;
//	memcpy(dstSong->cStroke,srcSong->cStroke,sizeof(dstSong->cStroke));
//	dstSong->bPlaying = 0;
//	dstSong->wOrderTimes = 0;
//	dstSong->nLyricsLen = 0;
//	dstSong->pLyrics = 0;

	return 0;
}

