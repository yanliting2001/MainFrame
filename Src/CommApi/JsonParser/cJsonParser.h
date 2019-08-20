#ifndef _CJSONPARSER_H
#define _CJSONPARSER_H
#include "../SQL/SearchSQL.h"
#include "CurlApi.h"
#include "curl.h"
#include "cJSON.h"
#include "commdef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define JSON_RET_OK						0		//
#define JSON_RET_ERR_NET			1		// 网络访问失败
#define JSON_RET_ERR_CODE		2		// errcode错误
#define JSON_RET_ERR_PARSER	3		// 解析错误
#define JSON_RET_ERR_OTHER		-1		// 其他错误

#define PAYINFO_MAX_COUNT		5		// 套餐最多5条
// 付费方式
typedef enum _ePayType
{
	payType_count = 0,		// 按歌曲数目计算
	payType_time,				// 按分钟计算
} PAYTYPE_E;

typedef struct _sLoginUserInfo
{
	char cmd[64];
	int userid;		// 用户ID
	char username[128];	// 用户名
	char beginTime[32];
	int type; 	// 开启方式（0-按歌曲数目计算，以n_开头；1-按时长计算，以t_开头）
	int n_count;	// 歌曲数量计算
	int t_count;		// 时长计算(s)
	int m_count;
} LOGIN_USERINFO_S;

// 套餐信息
typedef struct _sPayInfo
{
	int id;		// 套餐ID
	char name[64]; 	// 套餐名称
	int type;	// 套餐类型(0:按歌曲数目计算， 1:按分钟计算)
	int count;// 套餐包含数量(依赖type类型)
	double price_now;		// 套餐价格
	double price_org;		// 套餐原价
	char remark[64];			// 套餐说明
	char qrcode[128];
} PAYINFO_S;

// 套餐信息
typedef struct _sAddressInfo
{
	char anotherName[128];			//别名
	char address[64];          //地址
	char shopName[64];         //店名
	char workTime[128];        //营业时间
} ADDRESS_S;

// 歌曲排行信息
typedef struct _sClickInfo
{
	char songid[8+1];            	 //歌曲ID
	int click;	                   // 点击次数
	int RankCount;                 // 获取的热榜歌曲数量

} CLICKINFO_S;


typedef struct _sCommonResp
{
	int rc;
	char msg[64];
	char data[256];
}CommonResp;
// 解析登录接口
int JsonParser_LoginUser(const char *jsonbuf,
		char *token, size_t tokenSize, char *upgradeUrl, size_t upgradeSize,
		char *errMsg, size_t errSize);

// 解析就绪接口，获取二维码
int jsonParser_getLoadQRCode(const char *jsonbuf,
		char *QRCode, size_t QRSize,
		char *errMsg, size_t errSize);

int JsonParser_getLoginUserInfo(const char *jsonbuf,
		char *QRCode, size_t QRSize, LOGIN_USERINFO_S &userInfo,
		char *errMsg, size_t errSize,char *str);

int JsonParser_getPayInfo(const char *jsonbuf, PAYINFO_S *payInfo, char *errMsg, size_t errSize);

//获取机器地址及运营时间
int JsonParser_getAddressInfo(const char *jsonbuf, ADDRESS_S *addressInfo, char *errMsg, size_t errSize);

//解析榜单信息
int JsonParser_getTotalRankingInfo(const char *jsonbuf, CLICKINFO_S *clickInfo, char *errMsg, size_t errSize);

// 解析数据库版本信息
int JsonParser_getServerDBVer(const char *jsonbuf, char *dbver, char *dbid);

// 解析服务器数据库的MD5
int JsonParser_getServerDBMD5(const char *jsonbuf, char *md5, size_t len);

// 解析注销信息
int JsonParser_getLogoutResult(const char *jsonbuf);

int jsonParser_CommonResp(const char *jsonbuf,CommonResp &result);
#endif
