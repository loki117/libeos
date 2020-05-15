/****************************************************************
* 文件名  ：eos_cpu_dma_proxy.cpp
* 负责人  ：liuk
* 创建日期：20190724
* 版本号  ：v1.0
* 文件描述：DMA PROXY驱动适配接口函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：
******************************************************************************/
#include "eos_lib.h"

#define EOS_DMA_PROXY_MAX_LEN (1 * 1024 * 1024)

typedef enum
{
    PROXY_NO_ERROR = 0, 
    PROXY_BUSY = 1, 
    PROXY_TIMEOUT = 2, 
    PROXY_ERROR = 3 
} proxy_status;

struct dma_proxy_channel_interface 
{
	unsigned char buffer[EOS_DMA_PROXY_MAX_LEN];
	proxy_status status;
	unsigned int length;//有效字节数
};

#define EOS_DMA_PROXY_MAX_BLOCK 10

struct eos_dma_proxy_sg_arg
{
	unsigned long long addr_size[EOS_DMA_PROXY_MAX_BLOCK][2];//[0]保存物理地址 [1]保存要读取长度,单位字节
	unsigned int num;//上面数组的有效个数
	unsigned int dir;//0--read 1--write
};

struct eos_dma_proxy_memcpy_arg
{
	unsigned long long addr;//保存dma搬运的物理起始地址
	unsigned long long size;//保存dma搬运的数据字节数
	unsigned int dir;//0--read 1--write
};

/*************************************************************************
* 函数名  ：eos_open_dma_proxy
* 编写人  ：liuk
* 创建日期：20190724
* 函数功能：dma proxy打开
* 输入参数：dmaproxyname: 设备名称(/dev/dma_proxy_0 /dev/dma_proxy_1 /dev/dma_proxy_2 等)
* 输出参数：
* 返 回 值：成功返回0, 失败返回-1   
* 调用关系：
* 其    它：
*************************************************************************/
int eos_open_dma_proxy(const char *dmaproxyname)
{
	int fd = 0;
	fd = open(dmaproxyname, O_RDWR);
	if(fd == -1)
	{
		printf("open dma proxy(%s) fail!%s\n", dmaproxyname, strerror(errno));
		return -1;
	}
	return fd;
}

/*************************************************************************
* 函数名  ：eos_close_dma_proxy
* 编写人  ：liuk
* 创建日期：20190724
* 函数功能：dma proxy关闭
* 输入参数：fd: eos_open_dma_proxy 函数返回的文件描述符
* 输出参数：
* 返 回 值：成功返回0, 失败返回-1
* 调用关系：调用 eos_open_dma_proxy 后需调用该函数关闭文件描述符
* 其    它：
*************************************************************************/
int eos_close_dma_proxy(int fd)
{
    int ret;
	ret = close(fd);
    if (ret < 0)
    {
        printf("close fd(%d) error!%s", strerror(errno));
    }
	return ret;
}

/*************************************************************************
* 函数名  ：eos_get_dma_proxy_addr
* 编写人  ：liuk
* 创建日期：20190724
* 函数功能：获取某dam proxy的buffer虚拟地址, 通过该地址可访问到dma读取出来的数据
* 输入参数：fd: eos_open_dma_proxy 函数返回的文件描述符
* 输出参数：addr: dam proxy的buffer虚拟地址
* 返 回 值：成功返回0, 失败返回-1
* 调用关系：
* 其    它：
*************************************************************************/
int eos_get_dma_proxy_addr(unsigned int fd, dma_addr *addr)
{
    struct dma_proxy_channel_interface *proxy_interface_p;
    proxy_interface_p = (struct dma_proxy_channel_interface *)mmap(NULL, 
                                                            sizeof(struct dma_proxy_channel_interface),
                                                            PROT_READ | PROT_WRITE, 
                                                            MAP_SHARED, 
                                                            fd, 
                                                            0);
    if (proxy_interface_p == MAP_FAILED) {
		printf("eos_get_dma_proxy_addr mmap failed!!!%s\n", strerror(errno));
		return -1;
	}
    
	//printf("mmap_addr = 0x%lx\n", proxy_interface_p);
	*addr = (dma_addr)(&proxy_interface_p->buffer[0]);

	return 0;
}

/*************************************************************************
* 函数名  ：eos_start_dma_proxy_sg_transfer
* 编写人  ：liuk
* 创建日期：20190724
* 函数功能：使用sglist方式进行dma操作
* 输入参数：fd: eos_open_dma_proxy 函数返回的文件描述符
*           unsigned long long (*p)[2]: 要做dma操作的地址和长度集合, [0]保存物理地址 [1]保存要读取长度,单位字节
*           unsigned int num: 做dma操作的地址和长度集合的有效个数
*           unsigned int direction: dma操作方向(0-read 1-write)
* 输出参数：
* 返 回 值：成功返回0, 失败返回-1
* 调用关系：
* 其    它：
*************************************************************************/
int eos_start_dma_proxy_sg_transfer(unsigned int fd, unsigned long long (*p)[2], unsigned int num, unsigned int direction)
{
	int i;
	int ret = -1;
	unsigned int all_length = 0;
	struct eos_dma_proxy_sg_arg dma_sg_arg;
    
	if(num > EOS_DMA_PROXY_MAX_BLOCK)
	{
		printf("arg num too large: %d\n", num);
		return -1;
	}
    for(i = 0; i < num; i++)
	{
		all_length += p[i][1];
	}
	if(all_length > EOS_DMA_PROXY_MAX_LEN)
	{
		printf("size too large: %dBytes\n", all_length);
		return -1;
	}
    if ((direction != 0) && (direction != 1))
    {
		printf("direction wrong: %d\n", direction);
		return -1;
	}
    
	for(i = 0; i < num; i++)
	{
		dma_sg_arg.addr_size[i][0] = p[i][0];
		dma_sg_arg.addr_size[i][1] = p[i][1];
	}
	dma_sg_arg.num = num;
    dma_sg_arg.dir = direction;

    struct dma_proxy_channel_interface *proxy_interface_p;
    proxy_interface_p = (struct dma_proxy_channel_interface *)mmap(NULL, 
																sizeof(struct dma_proxy_channel_interface),
																PROT_READ | PROT_WRITE, 
																MAP_SHARED, 
																fd, 
																0);
	if (proxy_interface_p == MAP_FAILED) {
		printf("mmap failed!!!%s\n", strerror(errno));
		return -1;
	}
    
    ioctl(fd, 0, &dma_sg_arg);
    if (proxy_interface_p->status != PROXY_NO_ERROR)
    {
        printf("eos_start_dma_proxy_sg_transfer proxy transfer error\n");
        ret = -1;
        goto end;
    }

end:
    munmap(proxy_interface_p, sizeof(struct dma_proxy_channel_interface));
	return ret;
}

/*************************************************************************
* 函数名  ：eos_start_dma_proxy_memcpy_transfer
* 编写人  ：liuk
* 创建日期：20190724
* 函数功能：使用memcpy方式进行dma操作, ls208x/ls1046使用该方式
* 输入参数：fd: eos_open_dma_proxy 函数返回的文件描述符
*           unsigned long long addr: 要做dma操作的物理地址
*           unsigned long long size: 要做dma操作的长度,单位字节
*           unsigned int direction: dma操作方向(0-read 1-write)
* 输出参数：
* 返 回 值：成功返回0, 失败返回-1
* 调用关系：
* 其    它：
*************************************************************************/
int eos_start_dma_proxy_memcpy_transfer(unsigned int fd, unsigned long long addr, unsigned long long size, unsigned int direction)
{
    int ret = -1;
    struct eos_dma_proxy_memcpy_arg dma_memcpy_arg;
    
    if(size > EOS_DMA_PROXY_MAX_LEN)
	{
		printf("size too large: %dBytes\n", size);
		return -1;
	}
    if ((direction != 0) && (direction != 1))
    {
		printf("direction wrong: %d\n", direction);
		return -1;
	}
    
    dma_memcpy_arg.addr = addr;
    dma_memcpy_arg.size = size;
    dma_memcpy_arg.dir = direction;
    
    struct dma_proxy_channel_interface *proxy_interface_p;
    proxy_interface_p = (struct dma_proxy_channel_interface *)mmap(NULL, 
																sizeof(struct dma_proxy_channel_interface),
																PROT_READ | PROT_WRITE, 
																MAP_SHARED, 
																fd, 
																0);
	if (proxy_interface_p == MAP_FAILED) {
		printf("mmap failed!!!%s\n", strerror(errno));
		return -1;
	}
    
    ioctl(fd, 0, &dma_memcpy_arg);
    if (proxy_interface_p->status != PROXY_NO_ERROR)
    {
        printf("eos_start_dma_proxy_memcpy_transfer proxy transfer error\n");
        ret = -1;
        goto end;
    }

end:
    munmap(proxy_interface_p, sizeof(struct dma_proxy_channel_interface));
	return ret;
}

