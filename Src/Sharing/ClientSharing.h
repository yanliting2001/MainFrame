#ifndef _CLIENTSHARING_H
#define _CLIENTSHARING_H
#include "CurlApi.h"
#include "JsonParser/cJsonParser.h"
#include "CClientSocket.h"
#include "DataFileDef.h"

#define LOAD_QRCODE_PNG		"/tmp/loadQRCode.png"		//　登陆二维码图片路径
#define LOAD_USER_JPG				"/tmp/userImg.jpg"
#define LOAD_PAYCODE_JPG		"/tmp/payQRCode.png"

class CClientSharing
{
public:
	static void clearUserInfo(void);

	static void clearPayInfo(void);
    static void clearRangClickInfo(void);
  static void clearAddressInfo(void);

	static void saveLoginUserToDat(void);

	static BOOL loadLoginUserFromDat(void);

	static char *getDataFolder(char *folder, size_t len);

public:
	// 登录命令
	static int loginUser(char *errMsg, size_t errSize);

	static int logoutUser(char *errMsg, size_t errSize);
	// 就绪　获取登陆二维码接口
	static int readyGetQRCode(char *errMsg, size_t errSize);

	// 心跳
	static int keepAlive(LOGIN_USERINFO_S &user,  char *loginUserImg, size_t imgSize, char *errMsg, size_t errSize,BOOL islogined);

	// 获取机器套餐信息
	static int getPayInfo(char *errMsg, size_t errSize);
	static int getTotalRankingInfo(char *errMsg, size_t errSize);
  static int getAddressInfo(char *errMsg, size_t errSize);
	static int GetQRCode(char *errMsg, size_t errSize);
	static int ConnectHeartServer();
	static int ReconnectHeartServer();
	static int RecvHeartPackage(LOGIN_USERINFO_S &user, char *loginUserImg, size_t imgSize, char *errMsg, size_t errSize,BOOL islogined);
	static int SendHeartPackage(BOOL isLogined);

	static char mServerURL[128];
	static int mServerPort ;
	static char mToken[64];
	static char mStr[32];
	static LOGIN_USERINFO_S mUserInfo;
  static ADDRESS_S mAddressWorkTime;
	static  PAYINFO_S mPayItemsList[PAYINFO_MAX_COUNT];
  static  CLICKINFO_S mRangItemsList[RANK_MAX_COUNT];
	static CClientSocket mClientSocket;
};

void testRecordeApi(void);

#endif	// _CLIENTSHARING_H
