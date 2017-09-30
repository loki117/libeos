/****************************************************************
* 文件名  ：eos_cpu_dma.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：CPU的DMA接口函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160617 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/
#include "eos_lib.h"

#define EOS_DMA_MAX_LEN 0xa000
#define EOS_DMA_MAX_BLOCK 10

#if !defined(CPU_NAME_LS1021)
#define EOS_SET_DMA_ARG 0
#define EOS_START_DMA_TRANSFER 1
#define EOS_GET_DMA_ADDR 2
#define EOS_READ_BUFFER_DATA 3
#else
#define EOS_SET_DMA_ARG 0
#define EOS_START_DMA_TRANSFER 1
#define EOS_GET_DMA_ADDR 4
#define EOS_READ_BUFFER_DATA 8
#endif

struct eos_dma_arg
{
	unsigned long long addr_size[EOS_DMA_MAX_BLOCK][2];
	unsigned int num;
};

struct eos_print_dma_unit
{
	unsigned int off;
	unsigned int data;
};

int eos_open_dma(const char *devname)
{
	int fd = 0;
	fd = open(devname,O_RDWR);
	if(fd == -1)
	{
		printf("can not open dma device\n");
		return -1;
	}
	return fd;
}

int eos_close_dma(int fd)
{
	close(fd);
	return 0;
}

int eos_init_dma_arg(unsigned int fd,unsigned long long (*p)[2],unsigned int num)
{
	int i = 0;
	int ret = 0;
	unsigned int all_length = 0;
	struct eos_dma_arg dma_arg;
	if(num > EOS_DMA_MAX_BLOCK)
	{
		printf("arg num too large\n");
		return -1;
	}
	for(i = 0;i < num;i++)
	{
		dma_arg.addr_size[i][0] = p[i][0];
		dma_arg.addr_size[i][1] = p[i][1];
	}
	dma_arg.num = num;
	for(i = 0;i < dma_arg.num;i++)
	{
		all_length = all_length + dma_arg.addr_size[i][1];
	}
	if(all_length > EOS_DMA_MAX_LEN)
	{
		printf("size too large\n");
		return -1;
	}
	ret = ioctl(fd,EOS_SET_DMA_ARG,&dma_arg);
	if(ret == -1)
	{
		printf("set dma arg error\n");
		return -1;
	}
	return 0;
}

int eos_start_dma_transfer(unsigned int fd)
{
	int ret = 0;
	ret = ioctl(fd,EOS_START_DMA_TRANSFER,NULL);
	if(ret == -1)
	{
		printf("start dma transfer error\n");
		return -1;
	}
	return 0;
}

int  eos_get_dma_buffer_addr(unsigned int fd,unsigned int *addr)
{
	unsigned int dma_phy_addr = 0;
	int mem_fd = 0;
	int ret = 0;
	unsigned char *mmap_addr;
	ret = ioctl(fd,EOS_GET_DMA_ADDR,&dma_phy_addr);
	if(ret == -1)
	{
		printf("get dma phy addr error\n");
		return -1;
	}
	mem_fd = open("/dev/mem",O_RDWR);
	if(mem_fd == -1)
	{
		printf("open device /dev/mem fail\n");
		return -1;
	}
	mmap_addr = (unsigned char *)mmap(NULL,EOS_DMA_MAX_LEN * 4,PROT_READ,MAP_SHARED,mem_fd,dma_phy_addr);
	if(mmap_addr == MAP_FAILED)
	{
		printf("mmap /dev/mem fail\n");
		close(mem_fd);
		return -1;
	}
	*addr = (unsigned long)mmap_addr;

	return 0;
}

int eos_get_dma_status(unsigned int fd)
{
 	int status;
	read(fd,&status,4);
	return status;
}

/*此函数用于库自己内部调试使用，可以随时访问驱动获取的DMA数据 */
int eos_print_dma_data(const char *devname,unsigned int addr)
{
        struct eos_print_dma_unit dma_data;
        int fd = 0;
        memset(&dma_data,0,sizeof(struct eos_print_dma_unit));

        dma_data.off = addr;
        fd = open(devname,O_RDWR);
        if(-1 == fd)
        {
                printf("open device pciedev fail\n");
                return -1;
        }
        ioctl(fd,EOS_READ_BUFFER_DATA,&dma_data);
        printf("addr =  0x%x,data = 0x%x\n",dma_data.off,dma_data.data);
        close(fd);
        return 0;

}

#if 1

/*xbluo add 20161025*/

#define EOS_DMA_MAX_CHANNEL (2)

/*
channel号不能大于2
如channel 0、1
*/

struct eos_dma_arg_channel
{
	unsigned long long addr_size[EOS_DMA_MAX_BLOCK][2];
	unsigned int num;
	unsigned int ch_num;

};

int eos_open_dma_channel(const char *devname)
{
	int fd = 0;
	fd = open(devname,O_RDWR);
	if(fd == -1)
	{
		printf("can not open dma device\n");
		return -1;
	}
	return fd;
}

int eos_close_dma_channel(int fd)
{
	close(fd);
	return 0;
}


int eos_init_dma_arg_channel(unsigned int fd,unsigned long long (*p)[2],unsigned int num, int channel)
{
	int i = 0;
	int ret = 0;
	unsigned int all_length = 0;
	struct eos_dma_arg_channel dma_arg;
	if(num > EOS_DMA_MAX_BLOCK)
	{
		printf("arg num too large\n");
		return -1;
	}
	for(i = 0;i<num;i++)
	{
		dma_arg.addr_size[i][0] = p[i][0];
		dma_arg.addr_size[i][1] = p[i][1];
	}
	dma_arg.num = num;
	for(i = 0;i<dma_arg.num;i++)
	{
		all_length = all_length + dma_arg.addr_size[i][1];
	}
	if(all_length > EOS_DMA_MAX_LEN)
	{
		printf("size too large\n");
		return -1;
	}
	dma_arg.ch_num = (channel);
	ret = ioctl(fd,EOS_SET_DMA_ARG,&dma_arg);
	if(ret == -1)
	{
		printf("set dma arg error\n");
		return -1;
	}
	return 0;
}


int  eos_get_dma_buffer_addr_channel(unsigned int fd,unsigned int *addr, int channel)
{
	unsigned int dma_phy_addr = 0;
	int mem_fd = 0;
	int ret = 0;
	unsigned char *mmap_addr;
	dma_phy_addr = channel;
	ret = ioctl(fd,EOS_GET_DMA_ADDR,&dma_phy_addr);
	if(ret == -1)
	{
		printf("get dma phy addr error\n");
		return -1;
	}
	mem_fd = open("/dev/mem",O_RDWR);
	if(mem_fd == -1)
	{
		printf("open device /dev/mem fail\n");
		return -1;
	}
	mmap_addr = (unsigned char *)mmap(NULL,EOS_DMA_MAX_LEN * 4,PROT_READ,MAP_SHARED,mem_fd,dma_phy_addr);
	if(mmap_addr == MAP_FAILED)
	{
		printf("mmap /dev/mem fail\n");
		close(mem_fd);
		return -1;
	}
	*addr = (unsigned int )mmap_addr;

	return 0;
}


int eos_get_dma_status_channel(unsigned int fd, int channel)
{
 	int status;
	status = channel;
	read(fd,&status,4);
	return status;
}

int eos_start_dma_transfer_channel(unsigned int fd, int channel)
{
	int ret = 0;
	if ((channel < 0) || (channel >= EOS_DMA_MAX_CHANNEL))
       {
                printf("copy_to_user error err channel %d\n", channel);
                return -1;
       }		
	ret = ioctl(fd,EOS_START_DMA_TRANSFER,&channel);
	if(ret == -1)
	{
		printf("start dma transfer error\n");
		return -1;
	}
	return 0;
}


#endif

