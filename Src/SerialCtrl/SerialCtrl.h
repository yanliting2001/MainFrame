#ifndef _SERIALCTRL_H
#define _SERIALCTRL_H

class CSerialCtrl
{
public:
	CSerialCtrl();
	virtual ~CSerialCtrl();

public:
	bool Open(const char *Dev);
	void Close();

    //串口发送N个字节
    int WriteChar(unsigned char *cMsg, int size); 
    int ReadChar(unsigned char *cMsg, int size);    

private:
	void SetPort();
	bool SetParity(
		int databits,
		int stopbits,
		int parity);

private:
	int mComfd;
};

#endif 	// SERIALCTRL_H
