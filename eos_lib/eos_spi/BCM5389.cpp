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

eos_spi_bcm5389::eos_spi_bcm5389()
{
	enable = NULL;
	disable = NULL;
}

int eos_spi_bcm5389::cs_init(BCM5389_ENABLE enable_device ,BCM5389_DISABLE diable_device)

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

int eos_spi_bcm5389::device_read(unsigned char *pucDataBuf,unsigned char pageaddr,unsigned char ucStartAddr, int ucLen)
{
	unsigned int i = 0;
	unsigned char operation_code = 0;
	int file_description = 0;
	int ret = 0;
	int read_size = 0;
	unsigned char addr = ucStartAddr;
	unsigned int len;
	unsigned char status = 0;
	unsigned int delay_count =0;
	pthread_mutex_lock(&spi_lock);
	if(NULL != enable)
	{
		enable();
	}
	_recvLen = 0;
	file_description = get_fd();
	/*�����ж�оƬ�Ƿ���Զ�д����0xfe״̬�Ĵ������ж����λSPIF�Ƿ�Ϊ0
	*Ϊ0����ɼ�������ִ�У����еĻ������޷�����������
	*/
	status = read_status();
	if(NULL != disable)
	{
		disable();
	}
	while(0x0 != (status & 0x80))  /*���ϵĶ�ȡ״̬���������û��׼����*/
	{
		usleep(100);
		delay_count++;
		if(delay_count > 0xff)
		{
			printf("BCM5389 read status error!\n");
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != enable)
		{
			enable();
		}
		status = read_status();
		if(NULL != disable)
		{
			disable();
		}
	}
	/*дҳ��ַ*/
	_sendBuf[0] = 0x61;
	_sendBuf[1] = 0xff;
	_sendBuf[2] = pageaddr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_sendBuf, 0,3); /*��0xff�Ĵ���дҳ��ַ*/
	if(ret < 0)
	{
		printf("BCM5389 transfer error!\n");
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

	/*д��ַ,ֻ���Ͷ�����͵�ַ������ȡ��������*/
	read_size = 10 -BCM5389_ADDR_LEN - BCM5389_CMD_LEN;
	_recvBuf[0] = BCM5389_ADDR_LEN + BCM5389_CMD_LEN;
	_recvBuf[1] = 0x60;
	_recvBuf[2] = addr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_recvBuf,ucLen, 0);
	if(ret < 0)	
	{
		printf("BCM5389_Read transfer error!\n");
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
/*д���ַ����Ҫ��ȡ0xfe״̬�Ĵ������ж�RACK�Ƿ����1������1����ɴ�0xf0��0xf8��ȡ����*/
	delay_count = 0;
	if(NULL != enable)
	{
		enable();
	}
	status = read_status();
	if(NULL != disable)
	{
		disable();
	}
	while(0x20 !=(status & 0x20))
	{
		usleep(100);
		delay_count++;
		if(delay_count > 0xff)
		{
			printf("BCM5389 read status error!\n");
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != enable)
		{
			enable();
		}
		status = read_status();
		if(NULL != disable)
		{
			disable();
		}
	}
/*����0xf0��ʼ�����ݣ����һ��ֻ����8��*/
		for(i=0;i<ucLen;i++)
		{
		_recvBuf[0] = BCM5389_ADDR_LEN + BCM5389_CMD_LEN;
		_recvBuf[1] = 0x60;
		_recvBuf[2] = 0xf0+i;
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,1, 0);
		if(ret < 0)	
		{
			printf("BCM5389_Read transfer error!\n");
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
	len = _recvLen;
	pthread_mutex_unlock(&spi_lock);
	return len;
}

int eos_spi_bcm5389::device_write(unsigned char *pucDataBuf,unsigned char pageaddr,unsigned char ucStartAddr, int ucLen)
{
	int file_description = 0;
	unsigned char command = 0;
	int busy = 0;
	int i = 0;
	int write_size = 0;
	unsigned char addr = 0;
	int ret;
	unsigned char status =0;
	unsigned int delay_count = 0;
	pthread_mutex_lock(&spi_lock);
	file_description = get_fd();
	if(NULL != enable)
	{
		enable();
	}
	_sendLen = 0;
	write_size = 10 -BCM5389_ADDR_LEN - BCM5389_CMD_LEN;
	/*�����ж�оƬ�Ƿ���Զ�д����0xfe״̬�Ĵ������ж����λSPIF�Ƿ�Ϊ0
	*Ϊ0����ɼ�������ִ�У����еĻ������޷�����������
	*/
	#if 1
	status =  read_status();
	if(NULL != disable)
	{
		disable();
	}
	while(0 != (status & 0x80))  /*���ϵĶ�ȡ״̬���������û��׼����*/
	{
		usleep(100);
		delay_count++;
		if(delay_count > 0xff)
		{
			printf("BCM5389 read status error!\n");
			pthread_mutex_unlock(&spi_lock);
			return -1;
		}
		if(NULL != enable)
		{
			enable();
		}
		status =  read_status();
		if(NULL != disable)
		{
			disable();
		}
		
	}
	command = 0x61; /*normal write*/
	addr = 0xff;
	write_size = 10 -BCM5389_ADDR_LEN - BCM5389_CMD_LEN;
	_sendBuf[0] = command;
	_sendBuf[1] = 0xff;
	_sendBuf[2] = pageaddr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_sendBuf, 0,3); /*��0xff�Ĵ���дҳ��ַ*/
	if(ret < 0)
	{
		printf("BCM5389_Write transfer error!\n");
		if(NULL != disable)
		{
			disable();
		}
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}	
	#endif
	if(NULL != disable)
	{
		disable();
	}
	_sendBuf[0] = 0x61;
	_sendBuf[1] = ucStartAddr;
	memcpy(_sendBuf + 2,pucDataBuf+_sendLen,ucLen);
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_sendBuf, 0,ucLen + BCM5389_ADDR_LEN + BCM5389_CMD_LEN);
	if(ret < 0)
	{
		printf("BCM5389_Write transfer error!\n");
		if(NULL != disable)
		{
			disable();
		}
		pthread_mutex_unlock(&spi_lock);
		return -1;
	}
	_sendLen= _sendLen+ ucLen;
	if(NULL != disable)
	{
		disable();
	}
	pthread_mutex_unlock(&spi_lock);
	return 0;
}

unsigned char eos_spi_bcm5389::read_status()
{
	unsigned char status = 0;
	int ret = 0;
	_recvBuf[0] = BCM5389_ADDR_LEN + BCM5389_CMD_LEN;
	_recvBuf[1] = 0x60;
	_recvBuf[2] = 0xfe;
	ret = spi_transfer(_recvBuf,1, 0);
	if(ret < 0)	
	{
		printf("BCM5389_Read transfer error!\n");
		return 0xff;
	}
	status = _recvBuf[0];
	return status;
}

