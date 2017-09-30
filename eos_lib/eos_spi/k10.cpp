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

eos_spi_k10::eos_spi_k10()
{
	enable = NULL;
	disable = NULL;
}

int eos_spi_k10::cs_init(K10_ENABLE enable_device , K10_DISABLE diable_device)

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

int eos_spi_k10::device_read(unsigned char *pucDataBuf,int ucLen)
{
	unsigned int i = 0;
	int file_description = 0;
	int ret = 0;
	int read_size = 0;
	unsigned int len;
	pthread_mutex_lock(&spi_lock);

	_recvLen = 0;
	file_description = get_fd();

	/*再从地址读入数据*/
	read_size = SPI_MAX_READ_BUFF_LEN -K10_ADDR_LEN - K10_CMD_LEN;
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = K10_ADDR_LEN + K10_ADDR_LEN;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)	
		{
			printf("K10_Read transfer error!\n");
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
	if((ucLen %read_size) !=0)
	{
		_recvBuf[0] = K10_ADDR_LEN + K10_CMD_LEN;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)	
		{
			printf("K10_Read transfer error!\n");
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

int eos_spi_k10::device_write(unsigned char *pucDataBuf, int ucLen)
{
	int file_description = 0;
	int i = 0;
	int write_size = 0;
	int ret;
	pthread_mutex_lock(&spi_lock);
	file_description = get_fd();

	_sendLen = 0;
	/*为了限定每次写最大值为256字节*/
	write_size = SPI_MAX_WRITE_BUFF_LEN -K10_ADDR_LEN - K10_CMD_LEN -4;


	/*再往地址写入数据*/
	for(i = 0;i < ucLen/write_size;i++)
	{
		memcpy(_sendBuf,pucDataBuf+_sendLen,write_size);
		if(NULL != enable)
		{
			enable();
		}	
		ret = spi_transfer(_sendBuf, 0,write_size);
		if(ret < 0)
		{
			printf("K10_Write transfer error!\n");
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
	}
	if((ucLen % write_size) !=0)
	{
		memcpy(_sendBuf ,pucDataBuf+_sendLen,ucLen % write_size);
		if(NULL != enable)
		{
			enable();
		}	
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + K10_ADDR_LEN + K10_CMD_LEN);
		if(ret < 0)
		{
			printf("K10_Write transfer error!\n");
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