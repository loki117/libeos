/****************************************************************
* �ļ���  ��zl30144.cpp
* ������  ��lmyan
* �������ڣ�20150403
* �汾��  �� v1.1
* �ļ�������ʱ��оƬ�ӿ�
* ��Ȩ˵����Copyright (c) 2000-2020   ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��20160215 by  lmyan, ����ԭ�ļ�ע�ͣ���ʼ���������ӷ���ֵ
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

/*************************************************************************
* ������  ��eos_spi_zl30143
* ������  ��
* �������ڣ�
* �������ܣ�
* ���������
* ���������
* ����ֵ��
* ���ù�ϵ��
* ��?   ����
*************************************************************************/

eos_spi_zl30143::eos_spi_zl30143()
{
	enable = NULL;
	disable = NULL;
}

/*************************************************************************
* ������  ��cs_init
* ������  ��
* �������ڣ�
* �������ܣ�
* ���������
* ���������
* ����ֵ��
* ���ù�ϵ��
* ��?   ����
*************************************************************************/

int eos_spi_zl30143::cs_init(ZL30143_ENABLE enable_device , ZL30143_DISABLE diable_device)

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

/*************************************************************************
* ������  ��device_read
* ������  ��
* �������ڣ�
* �������ܣ�
* ���������
* ���������
* ����ֵ��
* ���ù�ϵ��
* ��?   ����
*************************************************************************/

int eos_spi_zl30143::device_read(unsigned char *pucDataBuf,unsigned char page_addr,unsigned char ucStartAddr, int ucLen)
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

	/*����0x7f�Ĵ���дֵ��ѡ��ҳ��ַ*/
	_recvBuf[0] = 0x64;
	_recvBuf[1] = page_addr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_recvBuf, 0 , 2);
	if(ret < 0)
	{
		printf("ZL30143_Read write error!\n");
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

	/*�ٴӵ�ַ��������*/
	read_size = SPI_MAX_READ_BUFF_LEN - ZL30143_ADDR_LEN - ZL30143_CMD_LEN;
	for(i = 0;i < ucLen/read_size;i++)
	{
		_recvBuf[0] = ZL30143_ADDR_LEN + ZL30143_CMD_LEN;
		_recvBuf[1] = ucStartAddr | (1 << 7);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,read_size, 0);
		if(ret < 0)
		{
			printf("ZL30143_Read transfer error!\n");
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
		_recvBuf[0] = ZL30143_ADDR_LEN + ZL30143_CMD_LEN;
		_recvBuf[1] = ucStartAddr | (1 << 7);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_recvBuf,ucLen % read_size, 0);
		if(ret < 0)
		{
			printf("ZL30143_Read transfer error!\n");
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
	len = _recvLen;
	pthread_mutex_unlock(&spi_lock);
	return len;
}

/*************************************************************************
* ������  ��device_write
* ������  ��
* �������ڣ�
* �������ܣ�
* ���������
* ���������
* ����ֵ��
* ���ù�ϵ��
* ��?   ����
*************************************************************************/

int eos_spi_zl30143::device_write(unsigned char *pucDataBuf,
		unsigned char page_addr,unsigned char ucStartAddr, int ucLen)
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
	write_size = SPI_MAX_WRITE_BUFF_LEN - ZL30143_ADDR_LEN - ZL30143_CMD_LEN;

	/*����0x7f�Ĵ���дֵ��ѡ��ҳ��ַ*/
	_recvBuf[0] = 0x64;
	_recvBuf[1] = page_addr;
	if(NULL != enable)
	{
		enable();
	}
	ret = spi_transfer(_recvBuf, 0 , 2);
	if(ret < 0)
	{
		printf("ZL30143_write write error!\n");
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

	/*������ַд������*/
	for(i = 0;i < ucLen/write_size;i++)
	{
		_sendBuf[0] = ucStartAddr & (~( 1 << 7));
		memcpy(_sendBuf + 1,pucDataBuf + _sendLen,write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,SPI_MAX_WRITE_BUFF_LEN);
		if(ret < 0)
		{
			printf("ZL30143_Write transfer error!\n");
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
		_sendBuf[0] = ucStartAddr &(~( 1 << 7));
		memcpy(_sendBuf + 1,pucDataBuf + _sendLen,ucLen % write_size);
		if(NULL != enable)
		{
			enable();
		}
		ret = spi_transfer(_sendBuf, 0,ucLen % write_size + ZL30143_ADDR_LEN + ZL30143_CMD_LEN);
		if(ret < 0)
		{
			printf("ZL30143_Write transfer error!\n");
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

/****************************************************************
* �ļ���  ��zl30144.cpp
* ������  ��lmyan
* �������ڣ�20150403
* �汾��  �� v1.1
* �ļ�������ʱ��оƬ�ӿ�
* ��Ȩ˵����Copyright (c) 2000-2020   ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��20160215 by  lmyan, ����ԭ�ļ�ע�ͣ���ʼ���������ӷ���ֵ
******************************************************************************/

/****************************************************************
* �ļ���  ��zl30144.cpp
* ������  ��lmyan
* �������ڣ�20150403
* �汾��  �� v1.1
* �ļ�������ʱ��оƬ�ӿ�
* ��Ȩ˵����Copyright (c) 2000-2020   ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��20160215 by  lmyan, ����ԭ�ļ�ע�ͣ���ʼ���������ӷ���ֵ
******************************************************************************/


