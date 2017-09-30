#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include "eos_spi_chip.h"



eos_spi_fm25v02::eos_spi_fm25v02()
{
	enable = NULL;
	disable = NULL;
}

int eos_spi_fm25v02::cs_init(FM_ENABLE enable_device , FM_DISABLE diable_device)

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

int eos_spi_fm25v02::device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen)
{
	unsigned int i = 0;
	unsigned char operation_code = 0;
	int file_description = 0;
	int ret = 0;
	int read_size = 0;
	unsigned long addr = ucStartAddr;
	pthread_mutex_lock(&spi_lock);
	_recvLen = 0;
	file_description = get_fd();
	read_size = SPI_MAX_READ_BUFF_LEN -FRAM_ADDR_LEN - FRAM_CMD_LEN;
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = FRAM_ADDR_LEN + FRAM_CMD_LEN;
		_recvBuf[1] = 0x03;
		_recvBuf[2] = (addr >> 0x08) & 0xff;
		_recvBuf[3] = addr & 0xff;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)	
		{
			printf("FM25V02_Read transfer error!\n");
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
		_recvBuf[0] = FRAM_ADDR_LEN + FRAM_CMD_LEN;
		_recvBuf[1] = 0x03;
		_recvBuf[2] = (addr >> 0x08) & 0xff;
		_recvBuf[3] = addr & 0xff;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)	
		{
			printf("FM25V02_Read transfer error!\n");
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
	pthread_mutex_unlock(&spi_lock);
	return _recvLen;
}

int eos_spi_fm25v02::device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen)
{
	int file_description = 0;
	char operation_code = 0;
	int busy = 0;
	int i = 0;
	int write_size = 0;
	unsigned long addr = 0;
	int ret;
	unsigned int delay_count = 0;
	pthread_mutex_lock(&spi_lock);
	file_description = get_fd();

	_sendLen = 0;
	addr = ucStartAddr;
	write_size = SPI_MAX_WRITE_BUFF_LEN -FRAM_ADDR_LEN - FRAM_CMD_LEN;
	for(i = 0;i < ucLen/write_size;i++)
	{
		operation_code = 0x06;       //ʹ��д
		_sendBuf[0] = operation_code;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf,0, 1);
		if(ret < 0)
		{
			printf("FM25V02_Write transfer error!\n");
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
		_sendBuf[0] = 0x02;
		_sendBuf[1] = (addr >> 0x08) & 0xff;
		_sendBuf[2] = addr & 0xff;
		memcpy(_sendBuf + 3,pucDataBuf+_sendLen,write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,SPI_MAX_WRITE_BUFF_LEN);
		if(ret < 0)
		{
			printf("FM25V02_Write transfer error!\n");
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
		delay_count = 0;
		busy = read_status();
		if(busy < 0)
		{
			printf("FM25V02_Write read_status error!\n");
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		while(busy == 1)
		{
			usleep(100);
			delay_count++;
			if(delay_count > 0xff)
			{
				printf("FM25V02 read status error\n");
				pthread_mutex_unlock(&spi_lock);
				return -1;
			}
			busy = read_status();	
			if(busy < 0)
			{
				printf("FM25V02_Write read_status error!\n");
				pthread_mutex_unlock(&spi_lock);
				return -1;
			}
		}
		_sendLen= _sendLen+ write_size;
		addr = addr + write_size;
	}
	if((ucLen % write_size) !=0)
	{
		operation_code = 0x06;       //ʹ��д
		_sendBuf[0] = operation_code;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf,0, 1);
		if(ret < 0)
		{
			printf("FM25V02_Write transfer error!\n");
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

		_sendBuf[0] = 0x02;
		_sendBuf[1] = (addr >> 0x08) & 0xff;
		_sendBuf[2] = addr & 0xff;
		memcpy(_sendBuf + 3,pucDataBuf+_sendLen,ucLen % write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + FRAM_ADDR_LEN + FRAM_CMD_LEN);
		if(ret < 0)
		{
			printf("FM25V02_Write transfer error!\n");
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
		delay_count = 0;

		busy = read_status();
		if(busy < 0)
		{
			printf("FM25V02_Write read_status error!\n");
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		while(busy == 1)
		{
			usleep(100);
			delay_count++;
			if(delay_count > 0xff)
			{
				printf("FM25V02 read status error\n");
				pthread_mutex_unlock(&spi_lock);
				return -1;
			}

			busy = read_status();	
			if(busy < 0)
			{
				printf("FM25V02_Write read_status error!\n");
				pthread_mutex_unlock(&spi_lock);
				return -1;
			}
		}
		_sendLen= _sendLen+ ucLen % write_size;
	}

	pthread_mutex_unlock(&spi_lock);
	return 0;
}

int eos_spi_fm25v02::device_show()
{
	eos_spi_chip::device_show();
	return 0;
}

int eos_spi_fm25v02::read_status()
{
	int file_descriptor = 0;
	unsigned char opertion_code = 0x05;
	char status[2];
	int busy = 0;
	int ret = 0;
	if(NULL != enable)
	{
		enable();
	}
	file_descriptor = get_fd();
	_recvBuf[0]=1;
	_recvBuf[1]=opertion_code;
	ret = spi_transfer(_recvBuf,1, 0);
	if(ret < 0)
	{
		printf("FM25V02_read_status transfer error!\n");
		if(NULL != disable)
		{
			disable();
		}
		return -1;
	}
	//printf("status = 0x%x\n",_recvBuf[0]);
	busy = _recvBuf[0] & 0x01;
	if(NULL != disable)
	{
		disable();
	}
	return  busy;
}
int eos_spi_fm25v02::read_id()
{
	int file_descriptor = 0;
	unsigned char opertion_code = 0x9f;
	int ret = 0;
	int id = 0;
	int i = 0;
	pthread_mutex_lock(&spi_lock);
	if(NULL != enable)
	{
		enable();
	}
	file_descriptor = get_fd();
	_recvBuf[0]=1;
	_recvBuf[1]=opertion_code;
	ret = spi_transfer(_recvBuf,9, 0);
	if(ret < 0)
	{
		printf("FM25V02_read_status transfer error!\n");
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
	for(i = 0;i < 6;i++)
	{
		if(_recvBuf[i] != 0x7f)
		{
			printf("FM25V02_Read_id error\n");
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
	}
	if(_recvBuf[6] != 0xc2)
	{
		printf("FM25V02_Read_id error\n");
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
	id = _recvBuf[7];
	pthread_mutex_unlock(&spi_lock);
	return id;
}



