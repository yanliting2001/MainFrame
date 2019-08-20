#ifndef _XMLANALYTIC_HPP
#include "xmlanalytic.hpp"
#endif
CXmlAnalytic *gpXml = NULL;

CXmlAnalytic::CXmlAnalytic()
{
}

CXmlAnalytic::~CXmlAnalytic()
{	
}

int CXmlAnalytic::InitXmlDoc(char * buf, int iLen)
{
	if(buf == NULL)
	{
		printf("the prarm of InitXmlDoc is null.\n");
		return -1;
	}
	
	if(!doc.load(buf)) 
	{
		printf("#Error: load %s failed.\n",buf);
		return -1;	
	}
	return 0;
}

int CXmlAnalytic::FreeXmlDoc()
{
	return 0;
}

int CXmlAnalytic::GetCmd(int * pCmd)
{
	xml_node cmd;
	
	if(pCmd == NULL)
	{
		printf("Error: the param of GetCmd is null.\n");
		return -1;
	}

	cmd = doc.child(ROOT);
	if(cmd && cmd.child(CMD))
		*pCmd = atoi(cmd.child_value(CMD));
	else
		return -1;
	return 0;
}

int CXmlAnalytic::GetSong(SONGINFO * pSongInfo)
{
 	xml_node song;
 	
 	if(NULL == pSongInfo)
 	{
	 	printf("Error: the param of GetSong is null.\n");
		return -1;
	}
	
	song = doc.child(ROOT).child(SONGS).child(SONG);
	if(song)
	{
//		strcpy(pSongInfo->cID, song.child_value(SONG_ID));
//		strcpy(pSongInfo->cName, song.child_value(SONG_NAME));
//		pSongInfo->wWdCt = atoi(song.child_value(SONG_WORDCOUNT));
//		strcpy(pSongInfo->cSinger, song.child_value(SONG_SINGER));
//		strcpy(pSongInfo->cType, song.child_value(SONG_TYPE));
//		strcpy(pSongInfo->cLang, song.child_value(SONG_LANG));
//		strcpy(pSongInfo->cServerName, song.child_value(SONG_SERVERNAME));
//		strcpy(pSongInfo->cFilePath, song.child_value(SONG_FILEPATH));
//		strcpy(pSongInfo->cPicPath, song.child_value(SONG_PICPATH));
//		strcpy(pSongInfo->cSpell, song.child_value(SONG_SPEL));
//		pSongInfo->wVol = atoi(song.child_value(VOL));
//		pSongInfo->wMaxVol = atoi(song.child_value(SONG_MAXVOL));
//		pSongInfo->wMinVol = atoi(song.child_value(SONG_MINVOL));
//		pSongInfo->wTrack = atoi(song.child_value(SONG_TRACK));
//		pSongInfo->bChos = atoi(song.child_value(SONG_CHOS));
//		pSongInfo->bExist = 1;
//		pSongInfo->bNewSong = atoi(song.child_value(SONG_NEWSONG));
//		strcpy(pSongInfo->cStroke, song.child_value(SONG_STROKE));
//		pSongInfo->bPlaying = atoi(song.child_value(SONG_PLAYING));
	}
	else
		return -1;
	return 0;
}

int CXmlAnalytic::GetSongId(char * pId)
{
	int ret = 0;
	xml_node item;

	if(pId == NULL)
	{
		printf("Error: the param of GetSongId is null.\n");
		return -1;
	}
	item = doc.child(ROOT).child(ITEMS).child(ITEM);
	if(item && item.child(SONGID))
		strcpy(pId, item.child_value(SONGID));
	else
		ret = -1;
	return ret;
}

int CXmlAnalytic::GetMoveInfo(char * pId, int * pSrcIndex, int * pDescIndex)
{
	int ret = 0;
	xml_node item;
	
	if((pId == NULL) || (pSrcIndex == NULL) || (pDescIndex == NULL))
	{
		printf("Error: the param of GetMoveInfo is null.\n");
		return -1;
	}
	item = doc.child(ROOT).child(ITEMS).child(ITEM);
	if(item)
	{
		strcpy(pId, item.child_value(SONGID));
		*pSrcIndex = atoi(item.child_value(SRCINDEX));
		*pDescIndex = atoi(item.child_value(DESCINDEX));
	}
	else
		ret = -1;
	return ret;
}

int CXmlAnalytic::GetVolue(int * pVolue)
{
	int ret = 0;
	xml_node vol;
	if(pVolue == NULL)
	{
		printf("Error: the param of GetVolue is null.\n");
		return -1;
	}
	vol = doc.child(ROOT);
	if(vol && vol.child(VOL))
		*pVolue = atoi(vol.child_value(VOL));
	else
		ret = -1;
	return ret;
}

int CXmlAnalytic::GetLen(int *plen)
{
	int ret = 0;
	xml_node len;

	if(plen == NULL)
	{
		printf("Error: the param of GetVolue is null.\n");
		return -1;
	}

	len = doc.child(ROOT);
	if(len && len.child(LEN))
		*plen = atoi(len.child_value(LEN));
	else
		ret = -1;
	return ret;
}

int CXmlAnalytic::GetUdiskPath(char *pPath)
{
	int ret = 0;
	xml_node path;

	if(pPath == NULL)
	{
		printf("Error: the param of GetVolue is null.\n");
		return -1;
	}

	path = doc.child(ROOT);
	if(path)
		strcpy(pPath, path.child_value(PATH));
	else
		ret = -1;
	return ret;
}

int CXmlAnalytic::GetUdiskFileName(char *pFileName)
{
	int ret = 0;
	xml_node path;

	if(pFileName == NULL)
	{
		printf("Error: the param of GetVolue is null.\n");
		return -1;
	}

	path = doc.child(ROOT);
	if(path)
		strcpy(pFileName, path.child_value(FILENAME));
	else
		ret = -1;
	return ret;
}

int CXmlAnalytic::GetInsertSongId(char *pId)
{
    int ret = 0;
    xml_node item;

    if(pId == NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT).child(SONGS).child(SONG);
    if(item && item.child(SONG_ID))
        strcpy(pId, item.child_value(SONG_ID));
    else
        ret = -1;
    return ret;
}
int CXmlAnalytic::GetPageNum(int *pPageNum)
{
    int ret = 0;
    xml_node item;

    if(pPageNum == NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(PAGE_NUM))
	*pPageNum = atoi(item.child_value(PAGE_NUM));
    else
        ret = -1;
    return ret;
}
int CXmlAnalytic::GetNum(int *pNum)
{
    int ret = 0;
    xml_node item;

    if(pNum == NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(NUM))
	*pNum = atoi(item.child_value(NUM));
    else
        ret = -1;
    return ret;
}

int CXmlAnalytic::GetSingerType(int *pType)
{
    int ret = 0;
    xml_node item;

    if(pType == NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(SINGERTYPE))
        *pType = atoi(item.child_value(SINGERTYPE));
    else
        ret = -1;
    return ret;
}
int CXmlAnalytic::GetKeyword(char *pkeyword)
{
    int ret = 0;
    xml_node item;

    if(pkeyword == NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(KEYWORD))
        strcpy(pkeyword,item.child_value(KEYWORD));
    else
        ret = -1;
    return ret;
}

int CXmlAnalytic::GetSpell(char *pspell)
{
    int ret = 0;
    xml_node item;

    if(pspell == NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(SPELL))
        strcpy(pspell,item.child_value(SPELL));
    else
        ret = -1;
    return ret;
}

int CXmlAnalytic::GetStr(const char *name,char *value)
{
    int ret = 0;
    xml_node item;

    if(name == NULL || value==NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(name)){
	printf("item value =%s \n",item.child_value(name));
        strcpy(value,item.child_value(name));
    }
    else
        ret = -1;
    return ret;
}
int CXmlAnalytic::GetInt(const char *name,int *value)
{
    int ret = 0;
    xml_node item;

    if(name == NULL || value==NULL)
    {
        printf("Error: the param of GetSongId is null.\n");
        return -1;
    }
    item = doc.child(ROOT);
    if(item && item.child(name))
        *value = atoi(item.child_value(name));
    else
        ret = -1;
    return ret;
}

int CXmlAnalytic::GetAudioTrack(const char *name, int *track)
{
	   int ret = 0;
	    xml_node item;

	    if(name == NULL || track==NULL)
	    {
	        printf("Error: the param of GetSongId is null.\n");
	        return -1;
	    }
	    item = doc.child(ROOT);
	    if(item && item.child(name))
	        *track = atoi(item.child_value(name));
	    else
	        ret = -1;
	    return ret;
}

