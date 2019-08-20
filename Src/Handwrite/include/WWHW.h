#ifndef __WWHW_H__
#define __WWHW_H__


#define WWHW_RANGE_NUMBER				0x1         /* Ê¶±ð·¶Î§£ºÊý×Ö     */
#define WWHW_RANGE_LOWER_CHAR			0x2         /* Ê¶±ð·¶Î§£ºÐ¡Ð´×ÖÄ¸ */
#define WWHW_RANGE_UPPER_CHAR			0x4         /* Ê¶±ð·¶Î§£º´óÐ´×ÖÄ¸ */
#define WWHW_RANGE_ASC_PUNCTUATION		0x8         /* Ê¶±ð·¶Î§£º°ë½Ç±êµã·ûºÅ */

/* Ê¶±ð·¶Î§£ºASCIIÂë£¬°üº¬Êý×Ö¡¢Ð¡Ð´×ÖÄ¸¡¢´óÐ´×ÖÄ¸ºÍ°ë½Ç±êµã·ûºÅ */
#define WWHW_RANGE_ASCII				(WWHW_RANGE_NUMBER | WWHW_RANGE_LOWER_CHAR | WWHW_RANGE_UPPER_CHAR | WWHW_RANGE_ASC_PUNCTUATION)

#define WWHW_RANGE_GB2312				0x8000      /* Ê¶±ð·¶Î§£ºGB2312ºº×Ö */
#define WWHW_RANGE_BIG5					0x200       /* Ê¶±ð·¶Î§£ºBIG5ºº×Ö   */
#define WWHW_RANGE_GBK					0x400       /* Ê¶±ð·¶Î§£ºGBKºº×Ö    */
#define WWHW_RANGE_GB18030				0x1000      /* Ê¶±ð·¶Î§£ºGB18030-2003Ëù¹æ¶¨µÄºº×Ö */

#define WWHW_RANGE_CHN_PUNCTUATION		0x800       /* Ê¶±ð·¶Î§£ºÖÐÎÄ±êµã·ûºÅ */
#define WWHW_RANGE_PUNCTUATION	        (WWHW_RANGE_ASC_PUNCTUATION | WWHW_RANGE_CHN_PUNCTUATION)
#define WWHW_RANGE_CONTROL_CHAR			0x2000      /* Ê¶±ð·¶Î§£º¿Õ¸ñ¡¢»Ø³µÒÔ¼°É¾³ýµÈÈý¸ö¿ØÖÆ×Ö·û */


/* Ê¶±ð·¶Î§£ºÄ¬ÈÏÊ¶±ð·¶Î§£¬ÎªGB2312ºº×Ö¡¢Êý×Ö¡¢×ÖÄ¸ºÍ±êµã·ûºÅ¡£²»°üÀ¨¿ØÖÆ·ûºÅ */
#define WWHW_RANGE_DEFAULT				(WWHW_RANGE_GB2312 | WWHW_RANGE_ASCII | WWHW_RANGE_PUNCTUATION)

#ifndef WWAPI
#ifdef WWIMPORT
#define WWAPI  __declspec(dllimport)
#else
#define WWAPI	extern
#endif
#endif

#ifdef __cplusplus
extern "C"{
#endif

enum WWHW_ERROR
{
	WWHW_OK             = 0x0,
	WWHW_ERR_NOT_INIT   = 0x100,
	WWHW_ERR_NULL_DATA,       /* ¿ÕÊý¾Ý */
	WWHW_ERR_INVALID_DATA,    /* Êý¾Ý¸ñÊ½´íÎó */
	WWHW_ERR_INVALID_PARAM,   /* ´íÎó²ÎÊý  */
    WWHW_ERR_COUNT
};

/***********************************************************************************************************
// 
//  ³õÊ¼»¯ÒýÇæ
//
//  data - Êý¾Ý¿â¿ªÊ¼µØÖ·£¬ Èç¹ûÊÇÄÚÖÃÁËÊý¾Ý¿â£¬ÕâÀï×ªÈë null ¼´¿É
//  param - ÎÞÓÃ
//
//  ·µ»Ø£º0 ³É¹¦£¬·Ç0Ê§°Ü
//
 ***********************************************************************************************************/
WWAPI int  WWInitRecognition(const void * data,void* param);


/***********************************************************************************************************
// 
// ¹Ø±ÕÒýÇæ
//
// ·µ»Ø£º0³É¹¦£¬·Ç0Ê§°Ü
//
 ***********************************************************************************************************/
WWAPI int WWExitRecognition(void);


/***********************************************************************************************************
//
// Ê¶±ðÊý
//
// pStrokeAry       (in)  ×Ö¹ì¼££¬Ö¸ÏòµÄÄÚ´æ±ØÐë¿ÉÐ´£¬ÒòÎªÒýÇæÓÐ¿ÉÄÜ»áÐÞ¸ÄÀïÃæµÄÄÚÈÝ
// pResultStr       (out) Êä³öµÄ½á¹û (Unicode )
// iCount           (in)  ÒªÇó·µ»ØºòÑ¡¸öÊý
// uMatchRange      (in)  Ê¶±ð·¶Î§
// prob             (out) Ã¿¸ö½á¹ûÓëÊäÈë±Ê»­µÄÏàËÆ¶È£¬Ô½Ð¡¾ÍÔ½ÏàËÆ¡£ÓÐÁ½ÖÖ·½Ê½£¬Çë¿´ºê£º WWHW_PROB_IS_DIST
//
// return ·µ»Ø½á¹ûµÄ¸öÊý
//
 ***********************************************************************************************************/
WWAPI int WWRecognizeChar(short* pStrokeAry, unsigned short * pResultStr, int iCount, unsigned long uMatchRange);

WWAPI int  WWRecognize(short* pStrokeAry, unsigned short * pResultStr, int iCount, unsigned long uMatchRange);

#ifdef __cplusplus
}
#endif

#endif
