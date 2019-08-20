#ifndef _SHARINGERR_H
#define _SHARINGERR_H
//#include "LoadRecordApi.h"

typedef enum _eRecordErrType
{
	usbhid_err = 0x0,		// 前级初始化失败
	initUART_err,				// 录音串口初始化失败
	startRecord_err,			// 开始录音失败
	stopRecord_err,			// 停止录音失败
	mount_err,					// 挂载sd卡失败
	umount_err,				// 卸载sd卡失败
	musicFolder_err,		// music录音目录未找到
	copyRecord_err,		// 拷贝文件失败
	upload_err,					// 上传失败
	registervod_err,			// 注册vod失败
	authorvod_err,			// vod验证失败
	errcode_err,					// errcode
	recordErrType_count,
} RECORDERRTYPE_E;

typedef struct _sRecordErr
{
	char errCode[8];		// 错误代码
	char errMsg[64];		// 错误信息
} RECORDERR_S;

extern RECORDERR_S	gRecordErr[recordErrType_count] ;

#endif	// _SHARINGERR_H
