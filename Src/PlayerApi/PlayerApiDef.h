#pragma once

// 特别提示： linux上，非root用户，只能使用1024以上的端口创建服务器！！

#define PLAYERAPI_SOCKET_ADDR		"127.0.0.1"
#define PLAYERAPI_SOCKET_CLIENTPORT 919
#define PLAYERAPI_SOCKET_SERVERPORT 1231

#define PLAYERAPI_ERROR_NONE		0
#define PLAYERAPI_ERROR_SOCKET		1
#define PLAYERAPI_ERROR_OTHER		2


//"CODENUM,TYPECODE,REQCODE<,MOREDATA...>\n"
