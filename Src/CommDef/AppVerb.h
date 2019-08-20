#ifndef _APPVERB_H
#define _APPVERB_H

////////////////////////////////////////
// 板卡类型定义
////////////////////////////////////////
#define BCM_BOARD_810		810			// 上海老板
#define BCM_BOARD_830		830			// 上海新版
#define BCM_BOARD_840		840			// 自生产的主板

#define BOARD_TYPE			BCM_BOARD_850

////////////////////////////////////////


////////////////////////////////////////
// 应用类型定义
////////////////////////////////////////
#define QIKUN500		500

#define APP_ID			QIKUN500		// need to modify this to match application verb
#define APP_VERB		"qikun500"

// 定义网络版还是单机版
#define LOCAL_WORKMODE		0	// 单机版
#define NET_WORKMODE		1	// 网络版

#define WORKMODE			LOCAL_WORKMODE		// 定义网络版

#define ON					1
#define OFF					0

#define HISI_NET_APP				OFF

// BCM7231板子底层控制
#if defined(__mips__) || defined(__arm__)
#define USE_MODULE_BCM		ON
#define USE_MODULE_DLNA		ON
#else
#define USE_MODULE_BCM		OFF
#define USE_MODULE_DLNA		OFF
#endif

#define USE_COMBINE_LIBAL	ON		// 启用合并Libal.so库
#define USE_DECRYPT 		ON		// 启用加密歌曲方式播放，，明文密钥
#define USE_SUBTITLE 		ON		// 启用外挂字幕

// DLNA 控制
#define USE_DLNA_PAD		ON		// 启用手机平板点歌
#define USE_DLNA_DMR		ON		// 启用手机推送

#define DBG_TO_FILE OFF
#define USE_ACTIVITY_TITLE		OFF		// 启用促销标题功能

#define USE_FREE_MODE		OFF		// ON:开启免费登陆模式，OFF：开启支付登陆模式

#define ENABLE_COIN_FUNCTION		OFF		// 开启投币功能

#define USB_ATTUNE_TYPE    		  0     //0:餐k效果器  1：唱效果器  2： 越南效果器


#endif
