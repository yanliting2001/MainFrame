//#include "DataFileOperate.h"
//#include "Song.h"
//#include "FileOperate.h"
//#include "DbgOutput.h"
//#include "types.h"
//#include "commdef.h"
//
//#define QKDAT_FILEHEADER "QIKUNDAT_1.0"
////#define QKDAT_FILEHEADER "SQL2DATA_VER 1.0"
//
//static BOOL ReadSize(void *p, size_t size, size_t nmemb, FILE *stream)
//{
//	if ( fread(p, size, nmemb, stream) != nmemb)
//	{
//		DbgOutput(DBG_ERR, "fread size:%d, nmemb:%d failed.\n", size, nmemb);
//		return FALSE;
//	}
//	return TRUE;
//}
//
//static size_t WriteSize(const void *p, size_t size, size_t nmemb, FILE *stream)
//{
//	if ( fwrite(p, size, nmemb, stream) != nmemb )
//	{
//		DbgOutput(DBG_ERR, "fwrite size:%d, nmemb:%d failed.\n", size, nmemb);
//		return FALSE;
//	}
//	return TRUE;
//}
//
//void SaveTypesListToFile(CPtrCtrl *pTypesList, const char *cFileName)
//{
//	if (!pTypesList || !cFileName)
//		return;
//
//	FILE* fp = fopen(cFileName, "wb");
//	if (!fp)
//	{
//		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
//		return;
//	}
//
//	do
//	{
//		int nWriteSize = strlen(QKDAT_FILEHEADER);
//		int nCount = pTypesList->GetCount();
//		int i=0;
//
//		fwrite(QKDAT_FILEHEADER, nWriteSize, 1, fp);
//		fwrite(&nCount, sizeof(int), 1, fp);
//
//		for ( i = 0 ; i < nCount ; i++ )
//		{
//			const char *cType = (const char *)pTypesList->GetAt(i);
//			if (!cType)
//				continue;
//
//			nWriteSize = strlen(cType)+1;
//
//			fwrite(&nWriteSize, sizeof(int), 1, fp);
//			fwrite(cType, nWriteSize, 1, fp);
//		}
//
//		DbgOutput(DBG_DEBUG, "SQL2DATA: Save types To File %s OK!\n", cFileName);
//	} while (0);
//
//	fclose(fp);
//}
//
//void LoadTypesListFromFile(
//	CPtrCtrl *pTypesList,
//	const char *cFileName)
//{
//	if (!pTypesList || !cFileName)
//	{
//		return;
//	}
//
//	FILE* fp = fopen(cFileName, "rb");
//	if (!fp)
//	{
//		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
//		return;
//	}
//
//	do
//	{
//		char cFileHeader[64]={0};
//		int nReadSize = strlen(QKDAT_FILEHEADER);
//		int nCount = 0;
//		int i=0;
//
//		fread(cFileHeader, nReadSize, 1, fp);
//		cFileHeader[nReadSize] = '\0';
//
//		if (strcasecmp(cFileHeader, QKDAT_FILEHEADER) != 0)
//		{
//			DbgOutput(DBG_ERR, "SQL2DATA: Error, invalid file header\n");
//			break;
//		}
//
//		fread(&nCount, sizeof(int), 1, fp);
//
//		for ( i = 0 ; i < nCount ; i++ )
//		{
//			fread(&nReadSize, sizeof(int), 1, fp);
//
//			char *cType = new char[nReadSize];
//			if (!cType)
//			{
//				continue;
//			}
//
//			fread(cType, nReadSize, 1, fp);
//			cType[nReadSize-1] = '\0';
//
//			pTypesList->AddData(cType);
//		}
//
//		DbgOutput(DBG_DEBUG, "SQL2DATA: Load types From File %s OK!\n", cFileName);
//	} while (0);
//
//	fclose(fp);
//}
//
//void SaveSingerListToFile(
//	CPtrCtrl *pSingerList,
//	const char *cFileName)
//{
//	if (!pSingerList || !cFileName)
//	{
//		return;
//	}
//
//	FILE* fp = fopen(cFileName, "wb");
//
//	if (!fp)
//	{
//		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
//
//		return;
//	}
//
//	do
//	{
//		int nWriteSize = strlen(QKDAT_FILEHEADER);
//		int nCount = pSingerList->GetCount();
//		int i=0;
//
//		fwrite(QKDAT_FILEHEADER, nWriteSize, 1, fp);
//		fwrite(&nCount, sizeof(int), 1, fp);
//		for ( i = 0 ; i < nCount ; i++ )
//		{
//			const SINGERINFO *pSingerInfo = (const SINGERINFO *)pSingerList->GetAt(i);
//
//			if (!pSingerInfo)
//			{
//				continue;
//			}
//
//			fwrite(pSingerInfo, sizeof(SINGERINFO), 1, fp);
//		}
//
//		DbgOutput(DBG_DEBUG, "SQL2DATA: Save Singer To File %s OK!\n", cFileName);
//	} while (0);
//
//	fclose(fp);
//}
//
//void LoadSingerListFromFile(
//	CPtrCtrl *pSingerList,
//	const char *cFileName)
//{
//	if (!pSingerList || !cFileName)
//	{
//		return;
//	}
//
//	FILE* fp = fopen(cFileName, "rb");
//
//	if (!fp)
//	{
//		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
//
//		return;
//	}
//
//	do
//	{
//		char cFileHeader[64]={0};
//		int nReadSize = strlen(QKDAT_FILEHEADER);
//		int nCount = 0;
//		int i=0;
//
//		fread(cFileHeader, nReadSize, 1, fp);
//		cFileHeader[nReadSize] = '\0';
//
//		if (strcasecmp(cFileHeader, QKDAT_FILEHEADER) != 0)
//		{
//			DbgOutput(DBG_ERR, "SQL2DATA: Error, invalid file header\n");
//			break;
//		}
//
//		fread(&nCount, sizeof(int), 1, fp);
//
//		for ( i = 0 ; i < nCount ; i++ )
//		{
//			SINGERINFO *pSingerInfo = new SINGERINFO;
//
//			if (!pSingerInfo)
//			{
//				continue;
//			}
//
//			fread(pSingerInfo, sizeof(SINGERINFO), 1, fp);
//			pSingerList->AddData(pSingerInfo);
//		}
//
//		DbgOutput(DBG_DEBUG, "SQL2DATA: Load Singer From File %s OK!\n", cFileName);
//	} while (0);
//
//	fclose(fp);
//
//}
//
//void SaveSongListToFile(
//	CPtrCtrl *pSongList,
//	const char *cFileName)
//{
//	if ( !pSongList || !cFileName )
//		return;
//
//	FILE* fp = fopen(cFileName, "wb");
//	if (!fp)
//	{
//		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
//		return;
//	}
//
//	do
//	{
//		int nWriteSize = strlen(QKDAT_FILEHEADER);
//		int nCount = pSongList->GetCount();
//		int i=0;
//
//		WriteSize(QKDAT_FILEHEADER, nWriteSize, 1, fp);
//		WriteSize(&nCount, sizeof(int), 1, fp);
//
//		for ( i = 0 ; i < nCount ; i++ )
//		{
//			SONGINFO *pSongInfo = (SONGINFO *)pSongList->GetAt(i);
//			if (!pSongInfo)
//				continue;
//
//			if ( pSongInfo->nLyricsLen > 0 && pSongInfo->pLyrics == 0 )
//			{
//				// 此处有问题，默认设置nLyricsLen = 0;
//				pSongInfo->nLyricsLen = 0;
//			}
//
//			if ( WriteSize(pSongInfo, sizeof(SONGINFO), 1, fp) )
//			{
//				WriteSize(&pSongInfo->nLyricsLen, sizeof(int), 1, fp);
//				if( pSongInfo->nLyricsLen > 0 && pSongInfo->pLyrics > 0 )
//				{
//					if ( WriteSize((BYTE *)pSongInfo->pLyrics, pSongInfo->nLyricsLen, 1, fp) ==FALSE )
//						break;
//				}
//			}
//			else
//				break;
//		}
//
//		DbgOutput(DBG_DEBUG, "SQL2DATA: Save Song To File %s OK!\n", cFileName);
//	} while (0);
//
//	fclose(fp);
//	sync();
//}
//
//void LoadSongListFromFile(
//	CPtrCtrl *pSongList,
//	const char *cFileName)
//{
//	if ( !pSongList || !cFileName )
//		return ;
//
//	FILE *fp = fopen(cFileName, "rb");
//	if ( NULL == fp )
//	{
//		DbgOutput(DBG_ERR, "open file:%s failed\n", cFileName);
//		return ;
//	}
//
//	char cFileHeader[64] = {0};
//	int	iReadSize 		 = strlen(QKDAT_FILEHEADER);
//	int iCount 			 = 0;
//	int i 				 = 0;
//
//	ReadSize(cFileHeader, iReadSize, 1, fp);
//	cFileHeader[iReadSize] = '\0';
//
//	if ( strcasecmp(cFileHeader, QKDAT_FILEHEADER) != 0 )
//	{
//		DbgOutput(DBG_ERR, "invalid file header\n");
//		fclose(fp);
//		return ;
//	}
//
//	ReadSize(&iCount, sizeof(int), 1, fp);
//
//	SONGINFO tempInfo;
//	int singerNameLen = sizeof(tempInfo.cSinger);
//	int songNameLen = sizeof(tempInfo.cName);
//
//	for ( i = 0; i < iCount; ++i )
//	{
//		SONGINFO *pSongInfo = new SONGINFO;
//		if ( NULL == pSongInfo )
//		{
//			DbgOutput(DBG_ERR, "new SONGINFO:%d failed\n", sizeof(SONGINFO));
//			break;
//		}
//		memset(pSongInfo, 0, sizeof(SONGINFO));
//
//		if ( ReadSize(pSongInfo, sizeof(SONGINFO), 1, fp) == FALSE )
//		{
//			delete pSongInfo;
//			break;
//		}
//
//		strcpy(pSongInfo->cServerName, pSongInfo->cFileName);
//
//		if(strlen(pSongInfo->cSinger)>=(singerNameLen-1))
//		{
//			unsigned char *ptr = (unsigned char*)pSongInfo->cSinger;
//			if(((ptr[19]&0xc0)==0xc0) || ((ptr[19]&0xe0)==0xe0)||((ptr[19]&0xf0)==0xf0))
//			{
//				ptr[19]='.';
//			}
//			else if((ptr[19]&0x80)==0x80)
//			{
//				if(((ptr[18]&0xe0)==0xe0) || ((ptr[18]&0xf0)==0xf0))
//				{
//					ptr[19]='.';
//					ptr[18]='.';
//				}
//				else if((ptr[18]&0x80)==0x80)
//				{
//					if((ptr[17]&0xf0)==0xf0)
//					{
//						ptr[19]='.';
//						ptr[18]='.';
//						ptr[17]='.';
//					}
//				}
//			}
//		}
//		if(strlen(pSongInfo->cName)>=(songNameLen-1))
//		{
//			unsigned char *ptr = (unsigned char*)pSongInfo->cName;
//			if(((ptr[49]&0xc0)==0xc0) || ((ptr[49]&0xe0)==0xe0)||((ptr[49]&0xf0)==0xf0))
//			{
//				ptr[49]='.';
//			}
//			else if((ptr[49]&0x80)==0x80)
//			{
//				if(((ptr[48]&0xe0)==0xe0) || ((ptr[48]&0xf0)==0xf0))
//				{
//					ptr[49]='.';
//					ptr[48]='.';
//				}
//				else if((ptr[48]&0x80)==0x80)
//				{
//					if((ptr[47]&0xf0)==0xf0)
//					{
//						ptr[49]='.';
//						ptr[48]='.';
//						ptr[47]='.';
//					}
//				}
//			}
//		}
//
//		int iLyricsLen;
//		ReadSize(&iLyricsLen, sizeof(int), 1, fp);
//
//		if ( iLyricsLen != pSongInfo->nLyricsLen )
//		{
//			pSongInfo->nLyricsLen = 0;
//			pSongInfo->pLyrics    = 0;
//
//			pSongList->AddData(pSongInfo);
//
//			break;
//		}
//
//		if ( pSongInfo->nLyricsLen > 0 )
//		{
//			char *pLyrics = new char[pSongInfo->nLyricsLen+1];
//			if ( NULL == pLyrics )
//			{
//				DbgOutput(DBG_ERR, "new lyrics size :%d failed\n", pSongInfo->nLyricsLen + 1);
//				pSongInfo->nLyricsLen = 0;
//				pSongInfo->pLyrics    = 0;
//
//				pSongList->AddData(pSongInfo);
//
//				break;
//			}
//
//			if ( ReadSize(pLyrics, pSongInfo->nLyricsLen, 1, fp) == FALSE )
//			{
//				delete pLyrics;
//				pSongInfo->nLyricsLen = 0;
//				pSongInfo->pLyrics    = 0;
//				pSongList->AddData(pSongInfo);
//
//				break;
//			}
//
//			pLyrics[pSongInfo->nLyricsLen] = '\0';
//			pSongInfo->pLyrics = (UINT64)pLyrics;
//		}
//
//		pSongList->AddData(pSongInfo);
//
//	}
//
//	DbgOutput(DBG_DEBUG, "Load song from file %s OK!\n", cFileName);
//
//	fclose(fp);
//}
//
//#if 0
//
//void SaveSongListToFile(
//	CPtrCtrl *pSongList,
//	const char *cFileName)
//{
//	if (!pSongList || !cFileName)
//	{
//		return;
//	}
//
//	FILE* fp = fopen(cFileName, "wb");
//
//	if (!fp)
//	{
//		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
//
//		return;
//	}
//
//	do
//	{
//		int nWriteSize = strlen(QKDAT_FILEHEADER);
//		int nCount = pSongList->GetCount();
//		int i=0;
//
//		fwrite(QKDAT_FILEHEADER, nWriteSize, 1, fp);
//		fwrite(&nCount, sizeof(int), 1, fp);
//
//		for ( i = 0 ; i < nCount ; i++ )
//		{
//			SONGINFO *pSongInfo = (SONGINFO *)pSongList->GetAt(i);
//
//			if (!pSongInfo)
//			{
//				continue;
//			}
//
//			if ( pSongInfo->nLyricsLen > 0 && pSongInfo->pLyrics == 0 )
//			{
//				// 此处有问题，默认设置nLyricsLen = 0;
//				pSongInfo->nLyricsLen = 0;
//			}
//			fwrite(pSongInfo, sizeof(SONGINFO), 1, fp);
//			fwrite(&pSongInfo->nLyricsLen, sizeof(int), 1, fp);
//			if( pSongInfo->nLyricsLen > 0 && pSongInfo->pLyrics > 0 )
//			{
//				fwrite((BYTE *)pSongInfo->pLyrics, pSongInfo->nLyricsLen, 1, fp);
//			}
//		}
//
//		DbgOutput(DBG_DEBUG, "SQL2DATA: Save Song To File %s OK!\n", cFileName);
//	} while (0);
//
//	fclose(fp);
//}
//
////void LoadSongListFromFile(
////	CPtrCtrl *pSongList,
////	const char *cFileName)
////{
////	if (!pSongList || !cFileName)
////	{
////		return;
////	}
////
////	FILE* fp = fopen(cFileName, "rb");
////
////	if (!fp)
////	{
////		DbgOutput(DBG_ERR, "SQL2DATA: Error, can not open file %s \n", cFileName);
////
////		return;
////	}
////
////	do
////	{
////		char cFileHeader[64]={0};
////		int nReadSize = strlen(QKDAT_FILEHEADER);
////		int nCount = 0;
////		int i=0;
////
////		fread(cFileHeader, nReadSize, 1, fp);
////		cFileHeader[nReadSize] = '\0';
////
////		if (strcasecmp(cFileHeader, QKDAT_FILEHEADER) != 0)
////		{
////			DbgOutput(DBG_ERR, "SQL2DATA: Error, invalid file header\n");
////			break;
////		}
////
////		fread(&nCount, sizeof(int), 1, fp);
////
////		for ( i = 0 ; i < nCount ; i++ )
////		{
////			SONGINFO *pSongInfo = new SONGINFO;
////			memset(pSongInfo,0,sizeof(SONGINFO));
////			if (!pSongInfo)
////			{
////				continue;
////			}
////
////			fread(pSongInfo, sizeof(SONGINFO), 1, fp);
////			pSongList->AddData(pSongInfo);
////		}
////
////		DbgOutput(DBG_DEBUG, "SQL2DATA: Load Song From File %s OK!\n", cFileName);
////	} while (0);
////
////	fclose(fp);
////}
//
//
//void LoadSongListFromBuffer(
//	CPtrCtrl *pSongList,
//	const BYTE *pDataBuf,
//	int nDataLen)
//{
//	if (!pSongList || !pDataBuf)
//	{
//		return;
//	}
//
//	int nTagSize = strlen(QKDAT_FILEHEADER);
//
//	int nDataPos = 0;
//
//	if (memcmp(pDataBuf, QKDAT_FILEHEADER, nTagSize) != 0)
//	{
//		DbgOutput(DBG_ERR, "DBOperator: Error, invalid file header\n");
//		return;
//	}
//	nDataPos += nTagSize;
//
//	int nCount = 0;
//	memcpy(&nCount, pDataBuf+nDataPos, sizeof(int));
//	nDataPos += sizeof(int);
//
//	int i=0;
//	SONGINFO tempInfo;
//	int singerNameLen = sizeof(tempInfo.cSinger);
//	int songNameLen = sizeof(tempInfo.cName);
//	for ( i = 0 ; i < nCount ; i++ )
//	{
//		SONGINFO *pSongInfo = new SONGINFO;
//		if (!pSongInfo)
//		{
//			break;
//		}
//		memset(pSongInfo, 0, sizeof(pSongInfo));
//
//		// 读取基本信息
//		memcpy(pSongInfo, pDataBuf+nDataPos, sizeof(SONGINFO));
//		nDataPos += sizeof(SONGINFO);
//		if(strlen(pSongInfo->cSinger)>=(singerNameLen-1))
//		{
//			unsigned char *ptr = (unsigned char*)pSongInfo->cSinger;
//			if(((ptr[19]&0xc0)==0xc0) || ((ptr[19]&0xe0)==0xe0)||((ptr[19]&0xf0)==0xf0))
//			{
//				ptr[19]='.';
//			}
//			else if((ptr[19]&0x80)==0x80)
//			{
//				if(((ptr[18]&0xe0)==0xe0) || ((ptr[18]&0xf0)==0xf0))
//				{
//					ptr[19]='.';
//					ptr[18]='.';
//				}
//				else if((ptr[18]&0x80)==0x80)
//				{
//					if((ptr[17]&0xf0)==0xf0)
//					{
//						ptr[19]='.';
//						ptr[18]='.';
//						ptr[17]='.';
//					}
//				}
//			}
//		}
//		if(strlen(pSongInfo->cName)>=(songNameLen-1))
//		{
//			unsigned char *ptr = (unsigned char*)pSongInfo->cName;
//			if(((ptr[19]&0xc0)==0xc0) || ((ptr[19]&0xe0)==0xe0)||((ptr[19]&0xf0)==0xf0))
//			{
//				ptr[19]='.';
//			}
//			else if((ptr[19]&0x80)==0x80)
//			{
//				if(((ptr[18]&0xe0)==0xe0) || ((ptr[18]&0xf0)==0xf0))
//				{
//					ptr[19]='.';
//					ptr[18]='.';
//				}
//				else if((ptr[18]&0x80)==0x80)
//				{
//					if((ptr[17]&0xf0)==0xf0)
//					{
//						ptr[19]='.';
//						ptr[18]='.';
//						ptr[17]='.';
//					}
//				}
//			}
//		}
//
//
//		// 读取歌词信息
//		int nLength = 0;
//		memcpy(&nLength, pDataBuf+nDataPos, sizeof(int));
//		nDataPos += sizeof(int);
//
//		if (nLength > 0)
//		{
//			BYTE *pLyrics = new BYTE[nLength+1];
//			if (pLyrics)
//			{
//				memcpy(pLyrics, pDataBuf+nDataPos, nLength);
//				pLyrics[nLength] = '\0';
//				nDataPos += nLength;
//
//				pSongInfo->nLyricsLen = nLength;
//				pSongInfo->pLyrics = (UINT64)pLyrics;
//			}
//			else
//			{
//				delete pSongInfo;
//				pSongInfo = NULL;
//				break;
//			}
//		}
//
//		pSongList->AddData(pSongInfo);
//	}
//
//	if (nDataPos != nDataLen)
//	{
//		DbgOutput(DBG_ERR, "DBOperator: Load Song From Buffer invalidate!\n");
//	}
//}
//
//void LoadSongListFromFile(
//	CPtrCtrl *pSongList,
//	const char *cFileName)
//{
//	if (!pSongList || !cFileName)
//	{
//		return;
//	}
//
//	BYTE *pBuffer = NULL;
//	int nDataLen =ReadFileData(cFileName, &pBuffer);
//
//	LoadSongListFromBuffer(pSongList, pBuffer, nDataLen);
//
//	ReleaseFileData(pBuffer);
//}
//
//#endif
//
//void FreeSongListForLyrics(CPtrArrayCtrl *pSongInfo)
//{
//	if(NULL == pSongInfo)
//		return;
//
//	SONGINFO *pSong = NULL;
//	int nCount = pSongInfo->GetCount();
//	int i=0;
//
//	for ( i = 0 ; i < nCount ; i++ )
//	{
//		pSong = (SONGINFO *)pSongInfo->GetAt(i);
//		if(pSong && (pSong->nLyricsLen > 0))
//		{
//			delete []((BYTE *)pSong->pLyrics);
//			pSong->nLyricsLen = 0;
//			pSong->pLyrics = 0;
//		}
//	}
//}
//
////void SaveToFile(char *buf,const char* cFileName)
////{
////	if (!buf || !cFileName)
////	{
////		return;
////	}
////	FILE* fp = fopen(cFileName, "wb");
////	if (!fp)
////	{
////		DbgOutput(DBG_ERR, "Save DownloadList Error, can not open file %s \n", cFileName);
////		return;
////	}
////	fwrite(buf, strlen(buf), 1, fp);
////
////	fclose(fp);
////}



