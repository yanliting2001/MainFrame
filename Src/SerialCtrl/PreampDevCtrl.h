#ifndef _PREAMPDEVCTRL_H
#define _PREAMPDEVCTRL_H
#include "SerialCtrl.h"
#include "types.h"

class CPreampDevCtrl
{
public:
	~CPreampDevCtrl() {ClosePreampDev();}

public:
	BOOL OpenPreampDev(void);

	void ClosePreampDev(void);

public:
  /************唱吧效果器接口***********/
	// 设置混响类型
	void SetPCMTuneType(unsigned char type);

	// 体感强度
	void SetBodyFeel(unsigned char feel);

	// 麦克风音量
	void SetMicroVol(unsigned char vol);

	// 音量
	void SetVolume(unsigned char vol);

  
  /*******餐K效果器接口******/
  // 设置混响类型
  void MealKtvSetPCMTuneType(unsigned char type);

  // 麦克风音量
  void MealKtvSetMicroVol(unsigned char vol);
  
  // 音量
  void MealKtvSetVolume(unsigned char vol);

public:
	CSerialCtrl mSerialCtrl;
};

#endif // _PREAMPDEVCTRL_H
