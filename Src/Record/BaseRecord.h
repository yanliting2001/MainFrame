#ifndef _BASERECORD_H
#define _BASERECORD_H
#include "types.h"

class CBaseRecord
{
public:
	CBaseRecord() {}
	~CBaseRecord() {}

public:
	static void Init_Record(void);
	static void DeInit_Record(void);

	static void SetRecordOn(BOOL bOn);

	static void Start_Record(void);
	static void Stop_Record(const char *cSong);
	static void Pause_Record(void);
	static void Resume_Record(void);
	static void Restart_Record(void);

private:
	static BOOL mbRecordOn;		// 作用于登录后才可以正在录音
	static BOOL mbInRecording;
};

#endif 	// _BASERECORD_H
