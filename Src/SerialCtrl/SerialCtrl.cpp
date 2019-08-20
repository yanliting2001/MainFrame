#include "SerialCtrl.h"
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

CSerialCtrl::CSerialCtrl()
{
	mComfd = -1;
}

CSerialCtrl::~CSerialCtrl()
{
	Close();
}

/**
*@breif 打开串口
*/
bool CSerialCtrl::Open(const char *Dev)
{
	mComfd= open( Dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if ( -1 == mComfd )
	{
		/*设置数据位数*/
		printf("open Serial ERROR=%s\n", strerror(errno));
		return false;
	}
	else
	{
		printf("open device :%s success \n", Dev);
		SetPort();
		return true;
	}
}

void CSerialCtrl::Close()
{
	if ( mComfd >= 0 )
	{
		close(mComfd);
		mComfd = -1;
	}
}

int CSerialCtrl::ReadChar(unsigned char *cMsg, int size)
{
	if (mComfd < 0)
	{
		return -1;
	}
	int writeLen = 0;
  memset(cMsg,0,size);
	writeLen = read(mComfd,cMsg,size);
	return writeLen;
}

//串口发送N个字节
int CSerialCtrl::WriteChar(unsigned char *cMsg, int size)
{
  printf("WriteChar mComfd/size = %d/%d !\r\n",mComfd,size);
	if (mComfd < 0)
	{
		return -1;
	}
	int writeLen = 0;
	
	for(int i=0;i<size;i++){
		printf("%02x ",cMsg[i]);
	}
	printf("\nWriteChar finish !\r\n");
	writeLen = write(mComfd,cMsg,size);
	usleep(200000);  //sleep 200ms wait for write ok;
	if(writeLen!=size){
		printf("set Serial ERROR writeLen = %d\n", writeLen);
	}
	return writeLen;	
}

void CSerialCtrl::SetPort()
{
	struct termios termios_p;
	termios_p.c_cflag =  B19200 ;			// 波特率 B19200 B115200
	termios_p.c_cflag |= CS8;				// ８个数据位
	termios_p.c_cflag &= ~PARENB; /* Clear parity enable */
	termios_p.c_iflag &= ~INPCK;  /* Enable parity checking */
	termios_p.c_cflag &= ~CSTOPB;
//	termios_p.c_cflag |= CREAD;

  termios_p.c_cflag &= ~PARENB;  
  termios_p.c_cflag &= ~CSTOPB;  
  termios_p.c_cflag &= ~CSIZE;  
  termios_p.c_cflag |= CS8;

  
	termios_p.c_iflag = IGNPAR | IGNBRK;
	//termios_p.c_cflag |= CLOCAL;
	termios_p.c_oflag = 0;
	termios_p.c_lflag = 0;
	termios_p.c_cc[VTIME] = 0;
	termios_p.c_cc[VMIN] = 1;

	cfsetispeed(&termios_p, B19200);
	cfsetospeed(&termios_p, B19200);

	tcsetattr(mComfd, TCSANOW, &termios_p);
	tcflush(mComfd, TCOFLUSH);
	tcflush(mComfd, TCIFLUSH);
}

/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄*
*@param  databits 类型  int 数据位   取值 为 7 或者8*
*@param  stopbits 类型  int 停止位   取值为 1 或者2*
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/

bool CSerialCtrl::SetParity(
	int databits,
	int stopbits,
	int parity)
{
	struct termios options;
	if (tcgetattr(mComfd, &options) != 0)
	{
		printf("get Serial attr  ERROR=%s\n", strerror(errno));
		return false;
	}

	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
		options.c_cflag |= CS8;
		break;

	default:
		printf("Unsupported data size\n");
		return false;
	}

	switch (parity)
	{
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB; /* Clear parity enable */
		options.c_iflag &= ~INPCK; /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
		options.c_iflag |= INPCK; /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB; /* Enable parity */
		options.c_cflag &= ~PARODD; /* 转换为偶效验*/
		options.c_iflag |= INPCK; /* Disnable parity checking */
		break;
	case 'S':
	case 's': /*as no parity*/
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;

	default:
		printf("Unsupported parity\n");
		return false;
	}

	/* 设置停止位*/
	switch (stopbits)
	{
	case 1:
		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;

	default:
		printf("Unsupported stop bits\n");
		return false;
	}

	/* Set input parity option */
	if (parity != 'n')
	{
		options.c_iflag |= INPCK;
	}
	options.c_cc[VTIME] = 0; // 15 seconds
	options.c_cc[VMIN] = 1;

	tcflush(mComfd, TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(mComfd, TCSANOW, &options) != 0)
	{
		printf("set Serial TCSANOW ERROR=%s\n", strerror(errno));
		return false;
	}

	return true;
 }
