/*
	edit by eva , 
	
	1) set the callback for arec to get the valid audio buffer; 
	2) callback function send the data to a queue , it act as a producter 
	3) a consumer  thread used to get the data from the queue , and send it to the msc Model to get the string , 
	4) the string used to get the keyword for next step cmd	
*/

#ifndef SOUND_CMD_H
#define SOUND_CMD_H


#ifdef SOUND_XFYUN

unsigned char SoundCmdLogin(void);

void* startSoundDetectFunc(long pcm_size,const char* p_pcm) ;

unsigned char SoundCmdInit();

#endif

//==============
#endif // SERDRIVE_H

