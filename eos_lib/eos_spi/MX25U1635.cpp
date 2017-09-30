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


eos_spi_mx25u1635::eos_spi_mx25u1635()
{
	enable = NULL;
	disable = NULL;
}

int eos_spi_mx25u1635::cs_init(MX25U1635_ENABLE enable_device , MX25U1635_DISABLE diable_device)

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


unsigned char  eos_spi_mx25u1635::read_status()
{
	int ret = 0;
	unsigned int cout = 0;
	unsigned char status = 0;
	_recvBuf[0] = 1;
	_recvBuf[1] = 0x05;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_recvBuf,1, 0);
	if(ret < 0)
	{
		printf("MX25U1635 read_status transfer error!\n");
		if(NULL != disable)
		{
			disable();
		}
		return 0xff;
	}
	if(NULL != disable)
	{
		disable();
	}
	status = _recvBuf[0] ;
	return status;
}

int eos_spi_mx25u1635::read_id(char *id)
{
	int file_descriptor = 0;
	unsigned char opertion_code = 0x9f;
	int ret = 0;
	int i = 0;
	pthread_mutex_lock(&spi_lock);
	if(NULL != enable)
	{
		enable();
	}
	file_descriptor = get_fd();
	_recvBuf[0]=1;
	_recvBuf[1]=opertion_code;
	ret = spi_transfer(_recvBuf,3, 0);
	if(ret < 0)
	{
		printf("MX25U1635_read_status transfer error!\n");
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
	#if 1
	for(i = 0;i < 3;i++)
	{
		id[i] = _recvBuf[i];
	}
	#endif

	pthread_mutex_unlock(&spi_lock);
	return 0;


}

int  eos_spi_mx25u1635::device_erase()
{
	int ret = 0;
	unsigned char status = 0;
	unsigned char busy = 0;
	pthread_mutex_lock(&spi_lock);
	if(NULL != enable)
	{
		enable();
	}
	       
	_sendBuf[0] = 0x06;//使能写
	ret = spi_transfer(_sendBuf,0, 1);
	if(ret < 0)
	{
		printf("MX25U1635_Write transfer error!\n");
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
	_sendBuf[0] = 0x60;
	
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_sendBuf,0, 1);
	if(ret < 0)
	{
		printf("MX25U1635 Write_enable transfer error!\n");
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
	pthread_mutex_unlock(&spi_lock);
	#if 0
	status = read_status();
	if(status == 0xff)
	{
		if(NULL != enable)
		{
			disable();
		}
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
	busy = status & 0x1;
	while(busy == 0x1)
	{
		sleep(1);
		status = read_status();
		if(status == 0xff)
		{
			if(NULL != enable)
			{
				disable();
			}
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		busy = status & 0x1;
	}
	#endif
	return 0;
}


/*器件最多只允许写256个数据*/
int eos_spi_mx25u1635::device_write(unsigned char *pucDataBuf, unsigned int ucStartAddr,unsigned int ucLen)
{
	int file_description = 0;
	char operation_code = 0;
	int busy = 0;
	unsigned char status =0;
	int i = 0;
	int write_size = 0;
	unsigned long addr = 0;
	unsigned int delay_count =0;
	int ret;
	pthread_mutex_lock(&spi_lock);
	file_description = get_fd();
	_sendLen = 0;
	addr = ucStartAddr;
	write_size = SPI_MAX_WRITE_BUFF_LEN -MX25U1635_ADDR_LEN - MX25U1635_CMD_LEN;
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
			printf("MX25U1635_Write transfer error!\n");
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
		_sendBuf[1] = (addr >> 0x10) & 0xff;
		_sendBuf[2] = (addr >> 0x08) & 0xff;
		_sendBuf[3] = addr & 0xff;
		memcpy(_sendBuf + 4,pucDataBuf+_sendLen,write_size);
		
		if(NULL != enable)
		{
			enable();
		}	
		ret = spi_transfer(_sendBuf, 0,SPI_MAX_WRITE_BUFF_LEN);
		if(ret < 0)
		{
			printf("MX25U1635_Write transfer error!\n");
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
		status = read_status();	
		busy = (status & 0x01);
		while(busy == 1)
		{	
			usleep(100);
			delay_count++;
			if(delay_count > 0xff)
			{
				printf("MX25U1635 read status error!\n");
				pthread_mutex_unlock(&spi_lock);
				return -1;
			}
			status = read_status();
			busy = (status & 0x1);	
		}
		_sendLen= _sendLen+ write_size;
		addr = addr + write_size;
	}
	if((ucLen % write_size) !=0)
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
			printf("MX25U1635_Write transfer error!\n");
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
		_sendBuf[1] = (addr >> 0x10) & 0xff;
		_sendBuf[2] = (addr >> 0x08) & 0xff;
		_sendBuf[3] = addr & 0xff;
		memcpy(_sendBuf + 4,pucDataBuf+_sendLen,ucLen % write_size);

		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + MX25U1635_ADDR_LEN + MX25U1635_CMD_LEN);
		if(ret < 0)
		{
			printf("MX25U1635_Write transfer error!\n");
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
		status = read_status();
		busy = (status & 0x01);
		while(busy == 1)
		{
			usleep(100);
			delay_count++;
			if(delay_count > 0xff)
			{
				printf("MX25U1635 read status error!\n");
				pthread_mutex_unlock(&spi_lock);
				return -1;
			}
			status = read_status();
			busy = status & 0x1;	
		}
		_sendLen= _sendLen+ ucLen %write_size;
	}
	pthread_mutex_unlock(&spi_lock);
	return 0;
}

int eos_spi_mx25u1635::device_read(unsigned char *pucDataBuf, unsigned int ucStartAddr,unsigned int ucLen)
{
	unsigned int i = 0;
	unsigned char operation_code = 0;
	int file_description = 0;
	int ret = 0;
	int read_size = 0;
	unsigned long addr = ucStartAddr;
	unsigned int len;
	pthread_mutex_lock(&spi_lock);

	_recvLen = 0;
	file_description = get_fd();
	read_size = SPI_MAX_READ_BUFF_LEN -MX25U1635_ADDR_LEN - MX25U1635_CMD_LEN;
	//while((ucLen /read_size) !=0)
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = MX25U1635_ADDR_LEN + MX25U1635_CMD_LEN;
		_recvBuf[1] = 0x03;
		_recvBuf[2] = (addr >> 0x10) & 0xff;
		_recvBuf[3] = (addr >> 0x08) & 0xff;
		_recvBuf[4] = addr & 0xff;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)	
		{
			printf("MX25U1635_Read transfer error!\n");
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
		_recvBuf[0] = MX25U1635_ADDR_LEN + MX25U1635_CMD_LEN;
		_recvBuf[1] = 0x03;
		_recvBuf[2] = (addr >> 0x10) & 0xff;
		_recvBuf[3] = (addr >> 0x08) & 0xff;
		_recvBuf[4] = addr & 0xff;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)	
		{
			printf("MX25U1635_Read transfer error!\n");
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
