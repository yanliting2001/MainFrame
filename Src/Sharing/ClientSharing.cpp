#include "ClientSharing.h"
#include "DbgOutput.h"
#include "commdef.h"
#include "InternetCtrl/NetCtrlApi.h"
#include "SharingErr.h"
#include "BaseApp.h"

#define LOGIN_FILENAME		"LoginUser.dat"

//测试服务器地址
//char CClientSharing::mServerURL[128] = "http://test.ilangbar.com:8000/kbar/api";

//测试服务器
//char CClientSharing::mServerURL[128] = "http://www.ilangbar.com:8000/kbar/api";

//壹浪暂停
//char CClientSharing::mServerURL[128] = "http://ilangbar.com:8000/kbar/api";

//壹浪运营服务器
//char CClientSharing::mServerURL[128] = "http://ktv.ilangbar.com:8000/kbar/api";
//乐唱服务器
//char CClientSharing::mServerURL[128] = "http://ktv.lcktv.net:8000/kbar/api";

//tcp测试服务器
//char CClientSharing::mServerURL[128] = "www.ilangbar.com";
//int CClientSharing::mServerPort = 8282;

#if 0
//测试服务器
char CClientSharing::mServerURL[128] = "www.ilangbar.com";
int CClientSharing::mServerPort = 8282;

#else
//运营服务器
char CClientSharing::mServerURL[128] = "ktv.ilangbar.com";
int CClientSharing::mServerPort = 8282;
#endif

static char mVersionStr[32] = "1.0.01";  //现付后点歌
//static char mVersionStr[32] = "1.0.02";  //先点歌后付费

char CClientSharing::mToken[64] = {0};
char CClientSharing::mStr[32] = {0};
LOGIN_USERINFO_S CClientSharing::mUserInfo;
PAYINFO_S CClientSharing::mPayItemsList[PAYINFO_MAX_COUNT];

ADDRESS_S CClientSharing::mAddressWorkTime;
CLICKINFO_S CClientSharing::mRangItemsList[RANK_MAX_COUNT];

CClientSocket CClientSharing::mClientSocket;

void CClientSharing::clearUserInfo(void)
{
	memset(&mUserInfo, 0x0, sizeof(mUserInfo));
}

void CClientSharing::clearPayInfo(void)
{
	memset(mPayItemsList, 0x0, sizeof(mPayItemsList));
}

void CClientSharing::clearRangClickInfo(void)
{
	memset(mRangItemsList, 0x0, sizeof(mRangItemsList));
}

void CClientSharing::clearAddressInfo(void)
{
	memset(&mAddressWorkTime, 0x0, sizeof(mAddressWorkTime));
}


char *CClientSharing::getDataFolder(char *folder, size_t len)
{
	if ( !folder ) 	return NULL;

	char cProgramFolder[MAX_PATH];
	GetProgramPath(cProgramFolder, MAX_PATH);
	char cDataFolder[MAX_PATH];
	CombinePathName(cDataFolder, cProgramFolder, "KTV_Data/database");

	SAFE_STRNCPY(folder, cDataFolder, len);

	return folder;
}

void CClientSharing::saveLoginUserToDat(void)
{
	char cDataFolder[MAX_PATH] = {0};
	getDataFolder(cDataFolder, sizeof(cDataFolder));
	char loginFile[MAX_PATH] = {0};
	snprintf(loginFile, sizeof(loginFile), "%s/%s", cDataFolder, LOGIN_FILENAME);

	DbgOutput(DBG_INFO, "open login file:%s.\n", loginFile);
	FILE *fp = fopen(loginFile, "wb");
	if ( fp == NULL )
	{
		DbgOutput(DBG_WARN, "open file:%s failed.\n", loginFile);
		return ;
	}

	fwrite(&mUserInfo, sizeof(mUserInfo), 1, fp);
	fflush(fp);
	fflush(fp);
	fclose(fp);
	sync();
}

BOOL CClientSharing::loadLoginUserFromDat(void)
{
	char cDataFolder[MAX_PATH] = {0};
	getDataFolder(cDataFolder, sizeof(cDataFolder));
	char loginFile[MAX_PATH] = {0};
	snprintf(loginFile, sizeof(loginFile), "%s/%s", cDataFolder, LOGIN_FILENAME);

	BOOL ret = TRUE;
	FILE *fp = fopen(loginFile, "rb") ;
	if ( fp == NULL )
	{
		DbgOutput(DBG_WARN, "open file:%s failed.\n", loginFile);
		return FALSE;
	}

	memset(&mUserInfo, 0, sizeof(mUserInfo));
	if ( fread(&mUserInfo, sizeof(mUserInfo), 1, fp) != 1 )
	{
		DbgOutput(DBG_WARN, "read userinfo from:%s failed.\n", loginFile);
		ret = FALSE;
	}

	fclose(fp);

	return ret;
}

// 登录命令
int CClientSharing::loginUser(char *errMsg, size_t errSize)
{
	int ret = -1;
	char buffer[1024] = {0};
	char url[256] = {0};
	char postData[128] = {0};
	char upgradeUrl[256] = {0};

	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}

	memset(mToken, 0x0, sizeof(mToken));

	snprintf(url, sizeof(url), "%s", mServerURL);
	snprintf(postData, sizeof(postData), "{\"cmd\":\"login\", \"data\": {\"mac\": \"%s\",\"ver\": \"%s\"}}", g_sIPInfo.cMac, "1.0.02");
	if ( UrlLoadToBuf(buffer, sizeof(buffer), url, postData, TRUE) ) {
		DbgOutput(DBG_INFO, "loginUser postData :%s  result: %s\n",postData,buffer);
		ret = JsonParser_LoginUser(buffer, mToken, sizeof(mToken), upgradeUrl, sizeof(upgradeUrl), errMsg, errSize);
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}

int CClientSharing::logoutUser(char *errMsg, size_t errSize)
{
	int ret = -1;
	char buffer[1024] = {0};
	char url[256] = {0};
	char postData[128] = {0};

	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}

	snprintf(url, sizeof(url), "http://%s/device/api/playerLogout", mServerURL);
	snprintf(postData, sizeof(postData), "mac=%s", g_sIPInfo.cMac);
	if ( UrlGetDataByPostToBuf(buffer, sizeof(buffer), url, postData, TRUE) )
	{
		if ( JsonParser_getLogoutResult(buffer) != JSON_RET_OK )
		{
			//TODO:注销失败
			printf("the logout failed, the buf:%s\n", buffer);
		}
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}

// 就绪　获取登陆二维码接口
int CClientSharing::readyGetQRCode(char *errMsg, size_t errSize)
{
	int ret = -1;
	char buffer[1024] = {0};
	char url[256] = {0};
	char postData[128] = {0};
	char QRCode[256] = {0};

	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}

	snprintf(url, sizeof(url), "%s", mServerURL);
	snprintf(postData, sizeof(postData), "{\"cmd\":\"ready\", \"data\": {\"token\": \"%s\"}}", mToken);
	if ( UrlLoadToBuf(buffer, sizeof(buffer), url, postData, TRUE) )
	{
		if ( (ret = jsonParser_getLoadQRCode(buffer, QRCode, sizeof(QRCode), errMsg, errSize)) == 0 )
		{
			if ( !UrlLoadToFile(LOAD_QRCODE_PNG, QRCode) )
			{
				printf("load QR code from:%s failed.\n", QRCode);
				snprintf(errMsg, errSize, "load QR code failed.\n");
				ret = JSON_RET_ERR_NET;
			}
		}
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}

int CClientSharing::GetQRCode(char *errMsg, size_t errSize)
{
	int ret = -1;
	char buffer[1024] = {0};
	char url[256] = {0};
	char postData[128] = {0};
  #if USE_FREE_MODE
  int loginStatus = 1;    //1：免费登陆模式
  #else
  int loginStatus = 0;    //0：付费登陆模式
  #endif
	CommonResp result;
	memset(&result, 0x00, sizeof(result));
	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}

	snprintf(url, sizeof(url), "http://%s/device/api/getQr", mServerURL);
  
	snprintf(postData, sizeof(postData), "mac=%s&loginStatus=%d", g_sIPInfo.cMac, loginStatus);
  //snprintf(postData, sizeof(postData), "mac=%s", g_sIPInfo.cMac);
  
	printf("load QR code url:%s postData:%s\r\n", url,postData);
	if (UrlGetDataByPostToBuf(buffer, sizeof(buffer), url, postData, TRUE))
	{
		printf("load QR code result:%s\n",buffer);
		if ( (ret = jsonParser_CommonResp(buffer, result)) == 0 )
		{
			if ( !UrlLoadToFile(LOAD_QRCODE_PNG, result.data) )
			{
				printf("load QR code from:%s failed.\n", result.data);
				snprintf(errMsg, errSize, "load QR code failed,can not get qrcode from %s\n",result.data);
				ret = JSON_RET_ERR_NET;
			}
		}else {
			printf("load QR code from:%s failed.\n", result.data);
			snprintf(errMsg, errSize, "load QR code failed: %s\n",result.msg);
		}
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}
int CClientSharing::keepAlive(LOGIN_USERINFO_S &user, char *loginUserImg, size_t imgSize, char *errMsg, size_t errSize,BOOL islogined)
{
	int ret = -1;
	char buffer[1024] = {0};
	char url[256] = {0};
	char postData[128] = {0};
	int stat = 1;

	if ( errMsg == NULL || errSize <= 0 ||
		 loginUserImg == NULL || imgSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}

	if ( strlen(mToken) <= 0 )
		return -1;
	if(islogined) stat = 2;
	else stat = 1;
	snprintf(url, sizeof(url), "%s", mServerURL);
	snprintf(postData, sizeof(postData), "{\"cmd\":\"kalive\", \"data\": {\"token\": \"%s\",\"stat\":\"%d\",\"str\":\"%s\"}}", mToken,stat,mStr);
	DbgOutput(DBG_INFO, "keepAlive postData:%s \n",postData);
	if ( UrlLoadToBuf(buffer, sizeof(buffer), url, postData, TRUE) )
	{
		DbgOutput(DBG_INFO, "keepAlive result: %s\n",buffer);
		memset(&user, 0x0, sizeof(user));
		ret = JsonParser_getLoginUserInfo(buffer, loginUserImg, imgSize, user, errMsg, errSize,mStr);
		if ( ret == 100 )
		{
			// 未登录，需要重新登入
			memset(mToken, 0x0, sizeof(mToken));	// 设置token无效
		}
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}

// 获取机器套餐信息
int CClientSharing::getPayInfo(char *errMsg, size_t errSize)
{
	int ret = -1;
	char buffer[2056] = {0};
	char url[256] = {0};
	char postData[128] = {0};

	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}
	snprintf(url, sizeof(url), "http://%s/device/api/packages", mServerURL);
	snprintf(postData, sizeof(postData), "mac=%s", g_sIPInfo.cMac);
	if (UrlGetDataByPostToBuf(buffer, sizeof(buffer), url, postData, TRUE) )
	{
		DbgOutput(DBG_INFO, "getPayInfo postData:%s ,result: %s\n",postData,buffer);
		ret = JsonParser_getPayInfo(buffer, mPayItemsList, errMsg, errSize);
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}

// 获取总榜单信息
int CClientSharing::getTotalRankingInfo(char *errMsg, size_t errSize)
{
	int ret = -1;
	//char buffer[2500] = {0};
	char url[205] = {0};
	char postData[128] = {0};

  char *buffer = new char[45000]();
	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}
	snprintf(url, sizeof(url), "http://%s/device/songClick/getWeekRank", mServerURL);
	snprintf(postData, sizeof(postData), "mac=%s", g_sIPInfo.cMac);
  
  DbgOutput(DBG_ERR, "getTotalRankingInfo.\n");
	if (UrlGetDataByPostToBuf(buffer, 45000, url, postData, TRUE) )
	{
		//DbgOutput(DBG_INFO, "getTotalRankingInfo postData:%s ,result: %s\n",postData,buffer);
		ret = JsonParser_getTotalRankingInfo(buffer, mRangItemsList, errMsg, errSize);
	}
	else
	{
    
    DbgOutput(DBG_ERR, "getTotalRankingInfo failed\n");
		ret = JSON_RET_ERR_NET;
	}
  delete []buffer;
  AfreshAllRankSongListforClick();
	return ret;
}

// 获取机器地址信息
int CClientSharing::getAddressInfo(char *errMsg, size_t errSize)
{
	int ret = -1;
	char buffer[2056] = {0};
	char url[256] = {0};
	char postData[128] = {0};

	if ( errMsg == NULL || errSize <= 0 )
	{
		DbgOutput(DBG_ERR, "the parameters is null.\n");
		return -1;
	}
	snprintf(url, sizeof(url), "http://%s/device/api/getAddress", mServerURL);
	snprintf(postData, sizeof(postData), "mac=%s", g_sIPInfo.cMac);
  DbgOutput(DBG_INFO, "###  getAddressInfo:%s \n",postData);
	if (UrlGetDataByPostToBuf(buffer, sizeof(buffer), url, postData, TRUE) )
	{
		DbgOutput(DBG_INFO, "getaddress:%s ,result: %s\n",postData,buffer);
		ret = JsonParser_getAddressInfo(buffer, &mAddressWorkTime, errMsg, errSize);
	}
	else
		ret = JSON_RET_ERR_NET;
	return ret;
}


int CClientSharing::ConnectHeartServer()
{
	int ret = -1;
	char serverIp[16];
	char *ip = mClientSocket.GetIpByName(mServerURL);
	if(ip==NULL) return -1;
	strcpy(serverIp,ip);
	printf("tcp sockt serverIp = %d\r\n",serverIp);
	if(!mClientSocket.Connect(serverIp,mServerPort)) {
		printf("tcp socket connect to %s:%d error.\n",serverIp,mServerPort);
		
	}else {
		printf("tcp socket connect to %s:%d sucess!.\n",serverIp,mServerPort);
		ret = 0;
	}	
	return ret;
}
int CClientSharing::ReconnectHeartServer()
{
	char serverIp[16];
	mClientSocket.Close();
	char *ip = mClientSocket.GetIpByName(mServerURL);
	if(ip){
		strcpy(serverIp,ip);
	}else return false;
	return mClientSocket.Connect(serverIp,mServerPort);
}

int CClientSharing::RecvHeartPackage(LOGIN_USERINFO_S &user, char *loginUserImg, size_t imgSize, char *errMsg, size_t errSize,BOOL islogined)
{
	int ret = -1;
	char recvbuf[512];
	memset(recvbuf,0,512);
	int status = mClientSocket.Recv(recvbuf, sizeof(recvbuf));
	if(status >0)
	{
		DbgOutput(DBG_INFO, "keepAlive result: %s\n",recvbuf);
		memset(&user, 0x0, sizeof(user));
		ret = JsonParser_getLoginUserInfo(recvbuf, loginUserImg, imgSize, user, errMsg, errSize,mStr);
    if(strcmp(user.cmd,"reboot")==0) 
    {
    	system("reboot");	
    }
		if ( ret == 100 )
		{
			// 未登录，需要重新登入
			memset(mToken, 0x0, sizeof(mToken));	// 设置token无效
		}
	}else if(status ==0) 
		ret = JSON_RET_ERR_NET;
	else ret = JSON_RET_ERR_OTHER;
	return ret;
}
int CClientSharing::SendHeartPackage(BOOL isLogined)
{
	int ret = -1;
	char data[128];
  int stat = 1;
  
  if(isLogined)
  stat = 2;
  else 
  stat = 1;
	sprintf(data,"{\"type\":\"ping\",\"data\":{\"mac\":\"%s\",\"status\":%d}}",g_sIPInfo.cMac,stat);
	//sprintf(data,"{\"type\":\"ping\",\"data\":{\"mac\":\"00:25:e1:02:15:3a\",\"status\":%d}}",isLogined);
	 ret = mClientSocket.Send(data,strlen(data));
	 printf("send heart package result = %d\r\n",ret);
	return ret;
}
void testRecordeApi(void)
{
	char errMsg[32];

	if ( CClientSharing::loginUser(errMsg, sizeof(errMsg)) == JSON_RET_OK )
	{
		CClientSharing::getPayInfo( errMsg, sizeof(errMsg));
		//CClientSharing::readyGetQRCode(errMsg, sizeof(errMsg));
		//CClientSharing::keepAlive(errMsg, sizeof(errMsg));
	}
}
