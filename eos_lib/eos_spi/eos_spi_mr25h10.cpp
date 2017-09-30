/****************************************************************
* 文件名  ：eos_spi_mr25h10.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：库的初始化函数
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


eos_spi_mr25h10::eos_spi_mr25h10()
{
	enable = NULL;
	disable = NULL;
}

/*****************************************************
函数名：cs_init()

功 能：	将MR25H10存储器的使能和非使能函数挂载

输 入：	使能和非使能函数。必须同时存在或同时为NULL；

描 述：	这个函数初始化处理器的SPI接口为master模式的SPI设备。
	例：
	enable()
	{

	}
	disable()
	{

	}
	MR25H10 fm
	fm.Init(enable,disable);

返回值　0	成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
int eos_spi_mr25h10::cs_init(eos_spi_mr25h10_enable enable_device , eos_spi_mr25h10_disable diable_device)

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

/*****************************************************
函数名：device_read()

功 能：	读取MR25H10存储器的存储数据

输 入：	pucDataBuf	存储数据的BUFFER
	ucStartAddr	读取的数据地址
	ucLen	　　　	读取数据的长度
描 述：	这个函数用于获取MR25H10存储器的数据。
返回值　>=0　成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/

int eos_spi_mr25h10::device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen)
{
	unsigned int i = 0;
	unsigned char operation_code = 0;
	int ret = 0;
	int read_size = 0;
	unsigned long addr = ucStartAddr;
	pthread_mutex_lock(&spi_lock);
	_recvLen = 0;
	read_size = SPI_MAX_READ_BUFF_LEN - EOS_SPI_MR25H10_ADDR_LEN - EOS_SPI_MR25H10_CMD_LEN;
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = EOS_SPI_MR25H10_ADDR_LEN + EOS_SPI_MR25H10_CMD_LEN;
		_recvBuf[1] = 0x03; //读操作码
		_recvBuf[2] = (addr >> 0x10) & 0xff;//地址
		_recvBuf[3] = (addr >> 0x08) & 0xff;//地址
		_recvBuf[4] = (addr) & 0xff;//地址

		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)
		{
			printf("MR25H10_Read transfer error!\n");
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
		memcpy(pucDataBuf + _recvLen,_recvBuf,ret);
		addr = addr + read_size;
		_recvLen = _recvLen + ret;
	}
	if((ucLen % read_size) != 0)
	{
		_recvBuf[0] = EOS_SPI_MR25H10_ADDR_LEN + EOS_SPI_MR25H10_CMD_LEN;
		_recvBuf[1] = 0x03;//读操作码
		_recvBuf[2] = (addr >> 0x10) & 0xff;//地址
		_recvBuf[3] = (addr >> 0x08) & 0xff;//地址
		_recvBuf[4] = addr & 0xff;//地址
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)
		{
			printf("MR25H10_Read transfer error!\n");
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
		memcpy(pucDataBuf + _recvLen,_recvBuf,ret);
		_recvLen = _recvLen + ret;
	}
	pthread_mutex_unlock(&spi_lock);
	return _recvLen;
}

/*****************************************************
函数名：device_write

功 能：	读取MR25H10存储器的存储数据

输 入：	pucDataBuf	存储数据的BUFFER
	ucStartAddr	发送的数据起始地址
	ucLen	　　　	读取数据的长度
描 述：	这个函数用于往MR25H10存储器写数据。
返回值　0　成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/

int eos_spi_mr25h10::device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen)
{
	char operation_code = 0;
	int busy = 0;
	int i = 0;
	int write_size = 0;
	unsigned long addr = 0;
	int ret;
	unsigned int delay_count = 0;
	pthread_mutex_lock(&spi_lock);

	_sendLen = 0;
	addr = ucStartAddr;
	write_size = SPI_MAX_WRITE_BUFF_LEN - EOS_SPI_MR25H10_ADDR_LEN - EOS_SPI_MR25H10_CMD_LEN;
	for(i = 0;i < ucLen/write_size;i++)
	{
		operation_code = 0x06;       //使能写
		_sendBuf[0] = operation_code;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf,0, 1);
		if(ret < 0)
		{
			printf("MR25H10_Write transfer error!\n");
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
		_sendBuf[0] = 0x02;//写操作码
		_sendBuf[1] = (addr >> 0x10) & 0xff;//地址
		_sendBuf[2] = (addr >> 0x08) & 0xff;//地址
		_sendBuf[3] = addr & 0xff;//地址
		memcpy(_sendBuf + 4,pucDataBuf + _sendLen,write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,SPI_MAX_WRITE_BUFF_LEN);
		if(ret < 0)
		{
			printf("MR25H10_Write transfer error!\n");
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
		_sendLen = _sendLen + write_size;
		addr = addr + write_size;
	}
	if((ucLen % write_size) != 0)
	{
		operation_code = 0x06;       //使能写
		_sendBuf[0] = operation_code;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf,0, 1);
		if(ret < 0)
		{
			printf("MR25H10_Write transfer error!\n");
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

		_sendBuf[0] = 0x02;//写操作码
		_sendBuf[1] = (addr >> 0x10) & 0xff;//地址
		_sendBuf[2] = (addr >> 0x08) & 0xff;//地址
		_sendBuf[3] = addr & 0xff;//地址
		memcpy(_sendBuf + 4,pucDataBuf + _sendLen,ucLen % write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + EOS_SPI_MR25H10_CMD_LEN + EOS_SPI_MR25H10_ADDR_LEN);
		if(ret < 0)
		{
			printf("MR25H10_Write transfer error!\n");
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
		_sendLen = _sendLen + ucLen % write_size;
	}
	pthread_mutex_unlock(&spi_lock);
	return 0;
}







