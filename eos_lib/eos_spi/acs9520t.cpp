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

eos_spi_acs9520t::eos_spi_acs9520t()
{
	enable = NULL;
	disable = NULL;
}

int eos_spi_acs9520t::cs_init(ACS_ENABLE enable_device , ACS_DISABLE diable_device)

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

int eos_spi_acs9520t::device_read(unsigned char *pucDataBuf,unsigned char ucStartAddr, int ucLen)
{
	unsigned int i = 0;
	unsigned char operation_code = 0;
	int file_description = 0;
	int ret = 0;
	int read_size = 0;
	unsigned char addr = ucStartAddr;
	unsigned int len;
	pthread_mutex_lock(&spi_lock);

	_recvLen = 0;
	file_description = get_fd();
	read_size = SPI_MAX_READ_BUFF_LEN -ACS_ADDR_LEN - ACS_CMD_LEN;
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = ACS_ADDR_LEN + ACS_CMD_LEN;
		_recvBuf[1] = ucStartAddr | 1<<7;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)	
		{
			printf("ACS9520T_Read transfer error!\n");
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
		_recvBuf[0] = ACS_ADDR_LEN + ACS_CMD_LEN;
		_recvBuf[1] = ucStartAddr | 1<<7;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)	
		{
			printf("ACS9520T_Read transfer error!\n");
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

int eos_spi_acs9520t::device_write(unsigned char *pucDataBuf,unsigned char ucStartAddr, int ucLen)
{
	int file_description = 0;
	char operation_code = 0;
	int busy = 0;
	int i = 0;
	int write_size = 0;
	unsigned char addr = 0;
	int ret;
	pthread_mutex_lock(&spi_lock);
	file_description = get_fd();

	_sendLen = 0;
	addr = ucStartAddr;
	write_size = SPI_MAX_WRITE_BUFF_LEN -ACS_ADDR_LEN - ACS_CMD_LEN;

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
			printf("ACS9520T_Write transfer error!\n");
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
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + ACS_ADDR_LEN + ACS_CMD_LEN);
		if(ret < 0)
		{
			printf("ACS9520T_Write transfer error!\n");
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


