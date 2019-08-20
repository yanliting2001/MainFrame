#include "cJsonParser.h"

int JsonParser_LoginUser(const char *jsonbuf,
		char *token, size_t tokenSize, char *upgradeUrl, size_t upgradeSize,
		char *errMsg, size_t errSize)
{
	int parserOK = -1;

	if ( jsonbuf == NULL || token == NULL || tokenSize <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL;
	item = cJSON_GetObjectItem(root, "rc");
	if ( item == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = item->valueint;
	if ( parserOK )
	{
		//printf("the errorCode:%d\n", parserOK);
		if ( errMsg && ((item = cJSON_GetObjectItem(root, "msg")) != NULL) )
			snprintf(errMsg, errSize, "%s", item->valuestring);
		cJSON_Delete(root);
		return JSON_RET_ERR_CODE;
	}

	dataItem = cJSON_GetObjectItem(root, "data");
	if ( dataItem == NULL )
	{
		printf("#Err: parser data failed,\n");
		goto failed;
	}

	item = cJSON_GetObjectItem(dataItem, "token");
	if ( item == NULL )
	{
		printf("#Err: parser token failed.\n");
		goto failed;
	}
	snprintf(token, tokenSize, "%s", item->valuestring);

	item = cJSON_GetObjectItem(dataItem, "upgurl");
	if ( item == NULL )
	{
		printf("#Err: parser upgurl failed.\n");
		goto failed;
	}
	if ( upgradeUrl )
		snprintf(upgradeUrl, upgradeSize, "%s", item->valuestring);

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}

int jsonParser_getLoadQRCode(const char *jsonbuf,
		char *QRCode, size_t QRSize,
		char *errMsg, size_t errSize)
{
	int parserOK = -1;

	if ( jsonbuf == NULL ||
		 QRCode == NULL || QRSize <= 0 ||
		 errMsg == NULL || errSize <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL;
	item = cJSON_GetObjectItem(root, "rc");
	if ( item == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = item->valueint;
	if ( parserOK )
	{
		//printf("the errorCode:%d\n", parserOK);
		if ( errMsg && ((item = cJSON_GetObjectItem(root, "msg")) != NULL) )
			snprintf(errMsg, errSize, "%s", item->valuestring);
		cJSON_Delete(root);
		return JSON_RET_ERR_CODE;
	}

	dataItem = cJSON_GetObjectItem(root, "data");
	if ( dataItem == NULL )
	{
		printf("#Err: parser data failed,\n");
		goto failed;
	}

	item = cJSON_GetObjectItem(dataItem, "qrcode");
	if ( item == NULL )
	{
		printf("#Err: parser qrcode failed,\n");
		goto failed;
	}
	snprintf(QRCode, QRSize, "%s", item->valuestring);

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}

int jsonParser_CommonResp(const char *jsonbuf,CommonResp &result)
{
	int parserOK = 0;

	if ( jsonbuf == NULL)
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL,*msgItem=NULL;
	item = cJSON_GetObjectItem(root, "rc");
	if ( item != NULL )
	{
		result.rc = item->valueint;
	}

	dataItem = cJSON_GetObjectItem(root, "data");
	if ( dataItem != NULL )
	{
		if(dataItem->valuestring!=NULL)
			snprintf(result.data, sizeof(result.data), "%s", dataItem->valuestring);
	}
	msgItem = cJSON_GetObjectItem(root, "msg");
	if(msgItem != NULL)
	{
		snprintf(result.msg, sizeof(result.msg), "%s", msgItem->valuestring);
	}
	cJSON_Delete(root);
	return JSON_RET_OK;

}
int JsonParser_getLoginUserInfo(const char *jsonbuf,
		char *QRCode, size_t QRSize, LOGIN_USERINFO_S &userInfo,
		char *errMsg, size_t errSize,char *str)
{
	int parserOK = -1;

	if ( jsonbuf == NULL ||
		 QRCode == NULL || QRSize <= 0 ||
		 errMsg == NULL || errSize <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL;
	cJSON *cmdItem = NULL, *dItem = NULL;
	cJSON *userid = NULL, *username=NULL, *userImg=NULL;
	cJSON *begin = NULL, *type=NULL, *n_count=NULL, *t_count=NULL,*m_count=NULL;
	cJSON *stritem = NULL;

	cmdItem = cJSON_GetObjectItem(root, "cmd");
	if ( cmdItem == NULL )
	{
		//printf("#Err: parser cmd failed,\n");
		goto failed;
	}

	dItem = cJSON_GetObjectItem(root, "data");
	if ( dItem == NULL )
	{
		printf("#Err: parser data failed,\n");
		goto failed;
	}

	userid = cJSON_GetObjectItem(dItem, "user_id");
	if ( userid == NULL )
	{
		printf("#Err: parser user_id failed,\n");
		goto failed;
	}
	if(strcmp(cmdItem->valuestring,"open") == 0){
		username = cJSON_GetObjectItem(dItem, "name");
		if ( username == NULL )
		{
			printf("#Err: parser name failed,\n");
			goto failed;
		}

		userImg = cJSON_GetObjectItem(dItem, "image");
		if ( userImg == NULL )
		{
			printf("#Err: parser image failed,\n");
			goto failed;
		}

		begin = cJSON_GetObjectItem(dItem, "begin");
		if ( begin == NULL )
		{
			printf("#Err: parser begin failed,\n");
			goto failed;
		}

		type = cJSON_GetObjectItem(dItem, "type");
		if ( type == NULL )
		{
			printf("#Err: parser type failed,\n");
			goto failed;
		}

		n_count = cJSON_GetObjectItem(dItem, "n_count");
		if ( n_count == NULL )
		{
			printf("#Err: parser n_count failed,\n");
			goto failed;
		}

		t_count = cJSON_GetObjectItem(dItem, "t_count");
		if ( t_count == NULL )
		{
			printf("#Err: parser t_count failed,\n");
			goto failed;
		}
		m_count = cJSON_GetObjectItem(dItem, "m_count");
		if(m_count==NULL)
		{
			printf("#Err: parser m_count failed,\n");
			goto failed;
		}
	}

	snprintf(userInfo.cmd, sizeof(userInfo.cmd), "%s", cmdItem->valuestring);
	if(userid!=NULL)
		userInfo.userid = userid->valueint;
	if(username!=NULL)
		snprintf(userInfo.username, sizeof(userInfo.username), "%s", username->valuestring);
	if(userImg!=NULL)
		snprintf(QRCode, QRSize, "%s", userImg->valuestring);
	if(begin!=NULL)
		snprintf(userInfo.beginTime, sizeof(userInfo.beginTime), "%s", begin->valuestring);
	if(type!=NULL)
		userInfo.type = type->valueint;
	if(n_count!=NULL)
		userInfo.n_count = n_count->valueint;
	if(t_count!=NULL)
		userInfo.t_count = (t_count->valueint * 60);	// 转换成S
	if(m_count!=NULL)
		userInfo.m_count = m_count->valueint;
	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}


int JsonParser_getTotalRankingInfo(const char *jsonbuf, CLICKINFO_S *clickInfo, char *errMsg, size_t errSize)
{
	int parserOK = -1;

	if ( jsonbuf == NULL || clickInfo == NULL  ||
		 errMsg == NULL || errSize <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL;
	cJSON /**countItem = NULL,*/ *itemsItem = NULL, *dItem = NULL;
	cJSON *id = NULL, *SongId=NULL, *type=NULL, *n_count=NULL, *ClickNumber=NULL;
	cJSON *remark = NULL, *qrcode=NULL;
	int count = 0;

	item = cJSON_GetObjectItem(root, "rc");
	if ( item == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}
  
	parserOK = item->valueint;
	if ( parserOK )
	{
		if ( errMsg && ((item = cJSON_GetObjectItem(root, "msg")) != NULL) )
			snprintf(errMsg, errSize, "%s", item->valuestring);
		cJSON_Delete(root);
    
		return parserOK;
    
	}
 

	itemsItem = cJSON_GetObjectItem(root, "data");
	if ( itemsItem == NULL )
	{
		printf("#Err: parser itemsItem failed,\n");
		goto failed;
	}
  
	count = cJSON_GetArraySize(itemsItem);
  
  DbgOutput(DBG_ERR, "==>   count%d\n", count);
	if ( count < 0 ) count = 0;
	if ( count > 1000 ) count = 1000;


	for ( int i = 0; i < count; i++ )
	{
		dItem = cJSON_GetArrayItem(itemsItem, i);
		if ( !dItem )
			continue;
		
		SongId = cJSON_GetObjectItem(dItem, "sid");
    
		ClickNumber = cJSON_GetObjectItem(dItem, "number");


		 if ( !SongId || !ClickNumber)
		 {
			 printf("==>Err:%s, get array item:%d failed.\n", dItem->string, i);
			 continue;
		 }
     
		 snprintf(clickInfo[i].songid, sizeof(clickInfo[i].songid), "%s", SongId->valuestring);
		 clickInfo[i].click = ClickNumber->valuedouble;
     clickInfo[i].RankCount = count;
     

	}

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}
int JsonParser_getAddressInfo(const char *jsonbuf, ADDRESS_S *addressInfo, char *errMsg, size_t errSize)
{
	int parserOK = -1;

	if ( jsonbuf == NULL || addressInfo == NULL  ||
		 errMsg == NULL || errSize <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}
  
	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL;
	cJSON *itemsItem = NULL;
	cJSON *anotherName=NULL, *worktime=NULL;


	item = cJSON_GetObjectItem(root, "rc");
	if ( item == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = item->valueint;
	if ( parserOK )
	{
		if ( errMsg && ((item = cJSON_GetObjectItem(root, "msg")) != NULL) )
			snprintf(errMsg, errSize, "%s", item->valuestring);
		cJSON_Delete(root);
		return parserOK;
	}
    
	itemsItem = cJSON_GetObjectItem(root, "data");
	if ( itemsItem == NULL )
	{
		DbgOutput(DBG_INFO, "#Err: parser itemsItem failed,\n");
		goto failed;
	}
  
	anotherName = cJSON_GetObjectItem(itemsItem, "anotherName");
	worktime = cJSON_GetObjectItem(itemsItem, "workTime");

  snprintf(addressInfo->anotherName, sizeof(addressInfo->anotherName), "机器名:%s", anotherName->valuestring);
  snprintf(addressInfo->workTime, sizeof(addressInfo->workTime), "营业时间:%s", worktime->valuestring);
      
	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}

int JsonParser_getPayInfo(const char *jsonbuf, PAYINFO_S *payInfo, char *errMsg, size_t errSize)
{
	int parserOK = -1;

	if ( jsonbuf == NULL || payInfo == NULL  ||
		 errMsg == NULL || errSize <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL, *dataItem = NULL;
	cJSON /**countItem = NULL,*/ *itemsItem = NULL, *dItem = NULL;
	cJSON *id = NULL, *name=NULL, *type=NULL, *n_count=NULL, *price_now=NULL, *price_org;
	cJSON *remark = NULL, *qrcode=NULL;
	int count = 0;

	item = cJSON_GetObjectItem(root, "rc");
	if ( item == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = item->valueint;
	if ( parserOK )
	{
		if ( errMsg && ((item = cJSON_GetObjectItem(root, "msg")) != NULL) )
			snprintf(errMsg, errSize, "%s", item->valuestring);
		cJSON_Delete(root);
		return parserOK;
	}

//	countItem = cJSON_GetObjectItem(dataItem, "count");
//	if ( item == NULL )
//	{
//		printf("#Err: parser count failed,\n");
//		goto failed;
//	}

	itemsItem = cJSON_GetObjectItem(root, "data");
	if ( itemsItem == NULL )
	{
		printf("#Err: parser itemsItem failed,\n");
		goto failed;
	}

	count = cJSON_GetArraySize(itemsItem);
	if ( count < 0 ) count = 0;
	if ( count > PAYINFO_MAX_COUNT ) count = PAYINFO_MAX_COUNT;

	for ( int i = 0; i < count; i++ )
	{
		dItem = cJSON_GetArrayItem(itemsItem, i);
		if ( !dItem )
			continue;
		
		name = cJSON_GetObjectItem(dItem, "name");
		price_now = cJSON_GetObjectItem(dItem, "price_now");
		price_org = cJSON_GetObjectItem(dItem, "price_org");
		remark = cJSON_GetObjectItem(dItem, "remark");
		qrcode = cJSON_GetObjectItem(dItem, "qrcode");

		 if ( !name || !price_now || !price_org || !remark || !qrcode  )
		 {
			 printf("==>Err:%s, get array item:%d failed.\n", dItem->string, i);
			 continue;
		 }

		 payInfo[i].id = 0;
		 snprintf(payInfo[i].name, sizeof(payInfo[i].name), "%s", name->valuestring);
		 payInfo[i].type = 0;
		 payInfo[i].count = 0;
		payInfo[i].price_now = price_now->valuedouble;
		payInfo[i].price_org = price_org->valuedouble;
		snprintf(payInfo[i].remark, sizeof(payInfo[i].remark), "%s", remark->valuestring);
		snprintf(payInfo[i].qrcode, sizeof(payInfo[i].qrcode), "%s", qrcode->valuestring);
	}

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}

int JsonParser_getServerDBVer(const char *jsonbuf, char *dbver, char *dbid)
{
	int parserOK = -1;

	if ( jsonbuf == NULL || dbver == NULL || dbid <= 0 )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *status = NULL, *msgItem = NULL, *dbidItem = NULL;

	status = cJSON_GetObjectItem(root, "status");
	if ( status == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = status->valueint;
	if ( parserOK )
	{
		if ( ((msgItem = cJSON_GetObjectItem(root, "msg")) != NULL) )
			printf("==>Err: the server db verb error msg:%s", msgItem->valuestring);
		cJSON_Delete(root);
		return parserOK;
	}

	msgItem = cJSON_GetObjectItem(root, "msg");
	if ( msgItem == NULL )
	{
		printf("#Err: parser msg failed,\n");
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
	}

	dbidItem = cJSON_GetObjectItem(root, "dbid");
	if ( dbidItem == NULL )
	{
		printf("#Err: parser dbid failed,\n");
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
	}

	snprintf(dbver, 10, "%s", msgItem->valuestring);
	snprintf(dbid, 10, "%s", dbidItem->valuestring);

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}

// 解析服务器数据库的MD5
int JsonParser_getServerDBMD5(const char *jsonbuf, char *md5, size_t len)
{
	int parserOK = -1;

	if ( jsonbuf == NULL || md5 == NULL )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *status = NULL, *md5Item = NULL;

	status = cJSON_GetObjectItem(root, "status");
	if ( status == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = status->valueint;
	if ( parserOK )
	{
		printf("the errorCode:%d\n", parserOK);
		cJSON_Delete(root);
		return parserOK;
	}

	md5Item = cJSON_GetObjectItem(root, "md5");
	if ( md5Item == NULL )
	{
		printf("#Err: parser md5 failed,\n");
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
	}

	snprintf(md5, len, "%s", md5Item->valuestring);

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}

// 解析注销信息
int JsonParser_getLogoutResult(const char *jsonbuf)
{
	int parserOK = -1;

	if ( jsonbuf == NULL )
	{
		printf("#Err: the parameter of %s  is null.\n", __FUNCTION__);
		return JSON_RET_ERR_OTHER;
	}

	cJSON *root = cJSON_Parse(jsonbuf);
	if ( !root )
	{
		printf("#Err: parser root failed,\n");
		return JSON_RET_ERR_PARSER;
	}
	cJSON *item = NULL;
	item = cJSON_GetObjectItem(root, "rc");
	if ( item == NULL )
	{
		printf("#Err: parser rc failed.\n");
		goto failed;
	}

	parserOK = item->valueint;
	if ( parserOK )
	{
		printf("the errorCode:%d\n", parserOK);
		cJSON_Delete(root);
		return JSON_RET_ERR_CODE;
	}

	cJSON_Delete(root);
	return JSON_RET_OK;

failed:
		cJSON_Delete(root);
		return JSON_RET_ERR_PARSER;
}
