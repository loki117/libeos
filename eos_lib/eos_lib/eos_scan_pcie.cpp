#include "eos_lib.h"


#define PCISYSDEV "/dev/pcie"
#define WRI_CMD_SCAN_BUS_USER   0x87 
#define WRI_CMD_DEL_BUS_USER    0x88 

struct usr_info
{
    int bus;
    int dev;
    int func;
    int addr;
    int val;
    int len;
};

int eos_scan_pcidev(int busnum)
{
	int ret;
	int fd;
	struct usr_info info;
	bzero(&info, sizeof(info));
	fd = open(PCISYSDEV, O_RDWR);
	if (-1 == fd)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
	info.bus = busnum;
	ret = ioctl(fd, WRI_CMD_SCAN_BUS_USER , &info);
	close(fd);
	return ret;
}


int eos_remove_pcidev(int busnum)
{
    int ret;
    int fd;
    struct usr_info info;
    bzero(&info, sizeof(info));
    fd = open(PCISYSDEV, O_RDWR);
    if (-1 == fd)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    info.bus = busnum;
    ret = ioctl(fd, WRI_CMD_DEL_BUS_USER, &info);
	close(fd);
	return ret;
}

/*************************************************************************
* 函数名  ：wri_remove_pcidev
* 负责人  ：
* 创建日期：
* 函数功能：针对应用程序兼容的接口
* 输入参数：
* 输出参数：
* 返回值：
* 调用关系：
* 其?   它：
*************************************************************************/
int wri_remove_pcidev(int busnum)
{
	return eos_remove_pcidev(busnum);
}

/*************************************************************************
* 函数名  ：wri_scan_pcidev
* 负责人  ：
* 创建日期：
* 函数功能：针对应用程序兼容的接口
* 输入参数：
* 输出参数：
* 返回值：
* 调用关系：
* 其?   它：
*************************************************************************/
int wri_scan_pcidev(int busnum)
{
	return eos_scan_pcidev(busnum);
}