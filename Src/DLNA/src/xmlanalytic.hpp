#ifndef _XMLANALYTIC_HPP
#define _XMLANALYTIC_HPP

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include "../DlnaTypeDef.h"
#include "Song.h"

using namespace pugi;

class CXmlAnalytic
{
public:
	CXmlAnalytic();
	virtual ~CXmlAnalytic();

public:
	int InitXmlDoc(char *buf,int iLen);
	int FreeXmlDoc();

public:
	int GetCmd(int *pCmd);
	int GetSong(SONGINFO *pSongInfo);
	int GetSongId(char *pId);
	int GetMoveInfo(char *pId,int *pSrcIndex,int *pDescIndex);
	int GetVolue(int *pVolue);
	int GetLen(int *plen);
	int GetUdiskPath(char *pPath);
	int GetUdiskFileName(char *pFileName);
	int GetInsertSongId(char *pId);
	int GetPageNum(int *pPageNum);
	int GetNum(int *pNum);
	int GetSingerType(int *pType);
	int GetKeyword(char *pkeyword);
	int GetSpell(char *pspell);
	int GetStr(const char *name,char *value);
	int GetInt(const char *name,int *value);
	int GetAudioTrack(const char *name, int *track);
private:
	xml_document doc;
};

extern CXmlAnalytic *gpXml;

#endif
