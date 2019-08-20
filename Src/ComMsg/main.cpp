#include "SerDrive.h"
#include <fcntl.h>      /*文件控制定义*/
#include <stdio.h>
//======================================
#if 0
int SerFd = -1;
void ProcessInit(void)
{

    SerFd = open("/dev/ttymxc3", O_RDWR|O_NOCTTY|O_NDELAY);
	if (0 < SerFd)
	{
        printf("begin set opt\n");
		set_opt(SerFd, BAUD_115200, DATA_BIT_8, PARITY_NONE, STOP_BIT_1);//设置串口参数
        printf("end set opt\n");
	}
	else
	{
		printf("open_port ERROR !\n");
	}
}
#endif

int main(int argc, char *argv[])
{
	int nTmp = 0;
	char Buf[100];
    printf("begin\n");
	int SerFd =  0 ;
	if(open_port("/dev/ttymxc3")==0)
		return 0;
	SerFd = get_com_handle() ;
	send_data_tty(SerFd, "hello series\n",sizeof("hello series\n"));
	while (1)
	{
	//	send_data_tty(SerFd, "hello series\n",sizeof("hello series\n"));
		nTmp = read_datas_tty(SerFd, Buf, 100, 100);
		if(nTmp)printf("%s",Buf);
		if (0 < nTmp)
		{
            printf("rcv len=%d,data:%s\n",nTmp,Buf);
			//向串口传数据
			send_data_tty(SerFd, Buf, nTmp);
		}
	}
}

