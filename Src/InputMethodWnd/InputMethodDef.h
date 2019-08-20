#pragma once

#include "types.h"

#define INPUTID_SPELL_BASE  0x01
#define INPUTID_SPELL_COUNT 26

#define INPUTID_STROKES_BASE  0x21
#define INPUTID_STROKES_COUNT 5

#define INPUTID_HANDWRITE_BASE  0x31
#define INPUTID_HANDWRITE_COUNT 6

//#define CANDIDATE_TEXT_COUNT 10

#define STROKE_CHAR_1	L'一'
#define STROKE_CHAR_2	L'丨'
#define STROKE_CHAR_3	L'丿'
#define STROKE_CHAR_4	L'丶'
#define STROKE_CHAR_5	L'乙'
#define STROKE_CHAR_6	L'？'

typedef struct tagINPUTITEM
{
	int nItemID;
	wchar_t wszChar;

	RECT rcItem;
	const char *cBtnImageNameBase;
} INPUTITEM ;


typedef enum eINPUTMETHODTYPE
{
	INPUTMETHODTYPE_SPELL=0,
	INPUTMETHODTYPE_HANDWRITE,
	INPUTMETHODTYPE_STROKES,
	INPUTMETHODTYPE_LYRIC
} INPUTMETHODTYPE ;

typedef enum eWORDLENGTHTYPE
{
	WordLength_1=0,
	WordLength_2,
	WordLength_3,
	WordLength_4,
	WordLength_5,
	WordLength_6,
	WordLength_7,
//	WordLength_8,
//	WordLength_Multi,
	WordLength_All,
	WordLength_TypeCount,
} WORDLENGTHTYPE ;

typedef struct tagSEARCHINFO
{
	char cInputContent[64];
	INPUTMETHODTYPE eInputMethodType;
	WORDLENGTHTYPE eWordLengthType;
} SEARCHINFO ;
