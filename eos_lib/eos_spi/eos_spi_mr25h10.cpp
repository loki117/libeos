/****************************************************************
* �ļ���  ��eos_spi_mr25h10.cpp
* ������  ��lmyan
* �������ڣ�20150403
* �汾��  �� v1.1
* �ļ���������ĳ�ʼ������
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


eos_spi_mr25h10::eos_spi_mr25h10()
{
	enable = NULL;
	disable = NULL;
}

/*****************************************************
��������cs_init()

�� �ܣ�	��MR25H10�洢����ʹ�ܺͷ�ʹ�ܺ�������

�� �룺	ʹ�ܺͷ�ʹ�ܺ���������ͬʱ���ڻ�ͬʱΪNULL��

�� ����	���������ʼ����������SPI�ӿ�Ϊmasterģʽ��SPI�豸��
	����
	enable()
	{

	}
	disable()
	{

	}
	MR25H10 fm
	fm.Init(enable,disable);

����ֵ��0	�ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
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
��������device_read()

�� �ܣ�	��ȡMR25H10�洢���Ĵ洢����

�� �룺	pucDataBuf	�洢���ݵ�BUFFER
	ucStartAddr	��ȡ�����ݵ�ַ
	ucLen	������	��ȡ���ݵĳ���
�� ����	����������ڻ�ȡMR25H10�洢�������ݡ�
����ֵ��>=0���ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
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
		_recvBuf[1] = 0x03; //��������
		_recvBuf[2] = (addr >> 0x10) & 0xff;//��ַ
		_recvBuf[3] = (addr >> 0x08) & 0xff;//��ַ
		_recvBuf[4] = (addr) & 0xff;//��ַ

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
		_recvBuf[1] = 0x03;//��������
		_recvBuf[2] = (addr >> 0x10) & 0xff;//��ַ
		_recvBuf[3] = (addr >> 0x08) & 0xff;//��ַ
		_recvBuf[4] = addr & 0xff;//��ַ
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
��������device_write

�� �ܣ�	��ȡMR25H10�洢���Ĵ洢����

�� �룺	pucDataBuf	�洢���ݵ�BUFFER
	ucStartAddr	���͵�������ʼ��ַ
	ucLen	������	��ȡ���ݵĳ���
�� ����	�������������MR25H10�洢��д���ݡ�
����ֵ��0���ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
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
		operation_code = 0x06;       //ʹ��д
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
		_sendBuf[0] = 0x02;//д������
		_sendBuf[1] = (addr >> 0x10) & 0xff;//��ַ
		_sendBuf[2] = (addr >> 0x08) & 0xff;//��ַ
		_sendBuf[3] = addr & 0xff;//��ַ
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
		operation_code = 0x06;       //ʹ��д
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

		_sendBuf[0] = 0x02;//д������
		_sendBuf[1] = (addr >> 0x10) & 0xff;//��ַ
		_sendBuf[2] = (addr >> 0x08) & 0xff;//��ַ
		_sendBuf[3] = addr & 0xff;//��ַ
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







