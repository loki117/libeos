/******************************************************************************
 * �ļ���  �� eos_spi.h
 * ������  ��
 * �������ڣ� 20150403
 * �汾��  ��   v1.0
 * �ļ�������spiͷ�ļ�
 * ��Ȩ˵���� Copyright (c) 2000-2020	���ͨ�ſƼ��ɷ����޹�˾
 * ��    ���� ��
 * �޸���־�� ��
******************************************************************************/
#ifndef __EOS_SPI_H
#define __EOS_SPI_H

#include <pthread.h>

/*SPI�������ݵ�BUFFER����*/
#define SPI_MAX_READ_BUFF_LEN	32

/*SPI�������ݵ�BUFFER����*/
#define SPI_MAX_WRITE_BUFF_LEN 260

/*SPI��ز������̼߳以����*/
extern pthread_mutex_t spi_lock;

typedef struct tageos_spi_para
{
	unsigned char clock_polarity;
	unsigned char clock_phase;
	unsigned char bit_order;
	unsigned int max_speed;
	unsigned char  charlen;
}EOS_SPI_PARA;

/*����SPI�������࣬�����������صĲ���*/
class eos_spi_chip
{
public:
	eos_spi_chip();

/*****************************************************
��������SetPara()

�� �ܣ�	P2020������SPI�ӿ��豸�ĳ�ʼ��

�� �룺	clock_polarity 	ʱ�Ӽ���ѡ��	ϵͳĬ�ϵ�����0
				0	SPICLK�͵�ƽʱ���ڲ�����,��ʱ��ʱ������źŲ���__|--|__|--|__|--|__
				1	SPICLK�ߵ�ƽʱ���ڲ�����,��ʱ��ʱ������źŲ���--|__|--|__|--|__|--
	clock_phase	ʱ����λѡ��	ϵͳĬ�ϵ�����0
				0	SPICLK starts toggling at the middle of the data transfer.
				1 	SPICLK starts toggling at the beginning of the data transfer.
	bit_order	bit����˳��	ϵͳĬ�ϵ�����0
				0	�ȷ��ͺͽ����ַ���msbλ
				1	�ȷ��ͺͽ����ַ���lsbλ
	max_speed	SPICLK��Ƶ������	ϵͳ��Ĭ��Ƶ��10000000

	charlen		���ͺͽ���һ���ַ�������bit����ϵͳ��Ĭ��ֵΪ8

�� ����	���������ʼ��P2020��������SPI�ӿ�Ϊmasterģʽ��SPI�豸��

	����
	EPCS64��EPCS;
	EPCS.SpiInit(0,0,0,10000000,8);

�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	int set_para( unsigned char clock_polarity,unsigned char clock_phase,unsigned char bit_order, \
	unsigned int max_speed,unsigned char charlen );



/*****************************************************
��������Transfer()

�� �ܣ�	����SPI������

�� �룺	�豸·��

�� ����	bufΪ�������ݵ�BUF��
			��������ʱ��tx_len = 0;rx_lenΪҪ��ȡ�����ݸ�����
			��д����ʱ��rx_len = 0;tx_len Ϊд�����ݸ����Ͳ����롢��ַ�ֽ������ܺ͡�


����ֵ������0	�ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/


	int spi_transfer(char *buf,unsigned int rx_len,unsigned int tx_len);

/*****************************************************
��������Open_Device()

�� �ܣ�	��SPI���豸�ڵ�

�� �룺	�豸·��

�� ����	���������ʼ��P2020��������SPI�ӿ�Ϊmasterģʽ��SPI�豸��
	����
	EPCS64��EPCS;
	EPCS.Open_Device("/dev/spi");

����ֵ������0	�ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	int open_device(const char * dev_name);

/*****************************************************
��������Close_Device()

�� �ܣ�	�ر�SPI���豸�ڵ�

�� �룺	�豸·��

�� ����	���������ʼ��P2020��������SPI�ӿ�Ϊmasterģʽ��SPI�豸��
	����
	EPCS64 EPCS
	EPCS.Close_Device();

����ֵ������0	�ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	void close_device();
	int get_fd();
	virtual int device_show();
	char _recvBuf[SPI_MAX_READ_BUFF_LEN];
	int _recvLen;
	char _sendBuf[SPI_MAX_WRITE_BUFF_LEN];
	int _sendLen;
	EOS_SPI_PARA para;
	eos_spi_chip * next;
private:

	int spi_init(unsigned char clock_polarity, unsigned char clock_phase, unsigned char bit_order, \
		int spiclock_div,unsigned char charlen);
	int set_bit_per_word(unsigned char bit_per_word);
	int set_lsb_first(unsigned char lsb_first);
	int set_max_speed(unsigned int max_speed);
	int set_spi_cpha(unsigned char spi_cpha);
	int set_spi_cpol(unsigned char spi_cpol);
	int _fd;
	int _curStatus;
};

#endif