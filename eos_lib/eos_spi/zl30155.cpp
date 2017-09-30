/****************************************************************
* 文件名  ：zl30155.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：时钟芯片接口
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include "eos_spi_chip.h"

eos_spi_zl30155::eos_spi_zl30155()
{
	enable = NULL;
	disable = NULL;
}

int eos_spi_zl30155::cs_init(ZL30155_ENABLE enable_device , ZL30155_DISABLE diable_device)

{
	enable = enable_device;
	disable = diable_device;

	if((NULL == enable) && (NULL != disable))
	{
		printf("init function fail!\n");
		return -1;
	}
	if((NULL != enable) && (NULL == disable))
	{
		printf("init function fail!\n");
		return -1;
	}
	return 0;
}

int eos_spi_zl30155::device_read(unsigned char *pucDataBuf,unsigned char page_addr,unsigned char ucStartAddr, int ucLen)
{
	unsigned int i = 0;
	unsigned char operation_code = 0;
	int file_description = 0;
	int ret = 0;
	int read_size = 0;
	unsigned char addr = ucStartAddr;
	unsigned int len;
	pthread_mutex_lock(&spi_lock);
#if 0
	if(page_addr > 1)
	{
		printf("ZL30155 Read input args error!\n");
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
#endif
	_recvLen = 0;
	file_description = get_fd();
	/*先往0x7f寄存器写值，选择页地址*/
	_recvBuf[0] = 0x7f;
	_recvBuf[1] = page_addr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_recvBuf, 0 , 2);
	if(ret < 0)
	{
		printf("ZL30155_Read write error!\n");
		if(NULL != disable)
		{
			disable();
		}
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
	if(NULL != disable)
	{
		disable();
	}
	/*再从地址读入数据*/
	read_size = SPI_MAX_READ_BUFF_LEN - ZL30155_ADDR_LEN - ZL30155_CMD_LEN;
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = ZL30155_ADDR_LEN + ZL30155_CMD_LEN;
		_recvBuf[1] = ucStartAddr | 1<<7;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)
		{
			printf("ZL30155_Read transfer error!\n");
			if(NULL != disable)
			{
				disable();
			}
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != disable)
		{
			disable();
		}
		memcpy(pucDataBuf+_recvLen,_recvBuf,ret);
		addr = addr + read_size;
		_recvLen = _recvLen + ret;
	}
	if((ucLen %read_size) !=0)
	{
		_recvBuf[0] = ZL30155_ADDR_LEN + ZL30155_CMD_LEN;
		_recvBuf[1] = ucStartAddr | 1<<7;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)
		{
			printf("ZL30155_Read transfer error!\n");
			if(NULL != disable)
			{
				disable();
			}
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != disable)
		{
			disable();
		}
		memcpy(pucDataBuf+_recvLen,_recvBuf,ret);
		_recvLen = _recvLen + ret;
	}
	len = _recvLen;
	pthread_mutex_unlock(&spi_lock);
	return len;
}

int eos_spi_zl30155::device_write(unsigned char *pucDataBuf,unsigned char page_addr,unsigned char ucStartAddr, int ucLen)
{
	int file_description = 0;
	char operation_code = 0;
	int busy = 0;
	int i = 0;
	int write_size = 0;
	unsigned char addr = 0;
	int ret;
	pthread_mutex_lock(&spi_lock);
#if 0
	if(page_addr > 1)
	{
		printf("ZL30155 Write input args error!\n");
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
#endif
	file_description = get_fd();

	_sendLen = 0;
	addr = ucStartAddr;
	write_size = SPI_MAX_WRITE_BUFF_LEN - ZL30155_ADDR_LEN - ZL30155_CMD_LEN;
	/*先往0x7f寄存器写值，选择页地址*/
	_recvBuf[0] = 0x7f;
	_recvBuf[1] = page_addr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_recvBuf, 0 , 2);
	if(ret < 0)
	{
		printf("ZL30155_write write error!\n");
		if(NULL != disable)
		{
			disable();
		}
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
	if(NULL != disable)
	{
		disable();
	}
	/*再往地址写入数据*/
	for(i = 0;i < ucLen/write_size;i++)
	{
		_sendBuf[0] = ucStartAddr & (~( 1<<7));
		memcpy(_sendBuf + 1,pucDataBuf+_sendLen,write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,SPI_MAX_WRITE_BUFF_LEN);
		if(ret < 0)
		{
			printf("ZL30155_Write transfer error!\n");
			if(NULL != disable)
			{
				disable();
			}
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != disable)
		{
			disable();
		}
		_sendLen= _sendLen+ write_size;
		addr = addr + write_size;
	}
	if((ucLen % write_size) !=0)
	{
		_sendBuf[0] = ucStartAddr &(~( 1<<7));
		memcpy(_sendBuf + 1,pucDataBuf+_sendLen,ucLen % write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + ZL30155_ADDR_LEN + ZL30155_CMD_LEN);
		if(ret < 0)
		{
			printf("ZL30155_Write transfer error!\n");
			if(NULL != disable)
			{
				disable();
			}
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != disable)
		{
			disable();
		}
		_sendLen= _sendLen+ ucLen %write_size;
	}

	pthread_mutex_unlock(&spi_lock);
	return 0;
}


