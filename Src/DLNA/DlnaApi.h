#ifndef _DLNAAPI_H
#define _DLNAAPI_H
#include "DlnaTypeDef.h"
#include "AppVerb.h"

void Dlna_Init(const char* name);

void Dlna_DeInit();

int SendSyncInfoToFromUI(int iType);

//int dmr_update_playing_info(char *url,char *ext,char *title);

#endif
