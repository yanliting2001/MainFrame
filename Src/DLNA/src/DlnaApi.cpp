#include "../DlnaApi.h"
#include "DLNADefine.h"
#include "CommApi.h"

extern int DlnaMdr_Init(const char* name);
extern void DlnaMdr_DeInit();
extern void DlnaPad_Init();
extern void DlnaPad_DeInit();

void Dlna_Init(const char* name)
{
//	DlnaPad_Init();
	DlnaMdr_Init(name);
}

void Dlna_DeInit()
{
//	DlnaPad_DeInit();
	DlnaMdr_DeInit();
}
