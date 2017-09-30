/******************************************************************************
 * �ļ���  �� eos_lib_chip.h
 * ������  ��
 * �������ڣ� 20150403
 * �汾��  ��   v1.0
 * �ļ�������spi����ͷ�ļ�
 * ��Ȩ˵���� Copyright (c) 2000-2020	���ͨ�ſƼ��ɷ����޹�˾
 * ��    ���� ��
 * �޸���־�� ��
******************************************************************************/
#ifndef __EOS_SPI_CHIP_H
#define __EOS_SPI_CHIP_H


#include "eos_spi.h"

/*����洢��*/
/*�������Ͳ�������ֽ���*/
#define FRAM_CMD_LEN		1

/*�������͵�ַ���ֽ���*/

#define FRAM_ADDR_LEN		2
typedef int (*FM_ENABLE)();
typedef int (*FM_DISABLE)();

/*��������洢������ز���*/
class eos_spi_fm25v02:public eos_spi_chip
{

public:
	eos_spi_fm25v02();

/*****************************************************
��������Init()

�� �ܣ�	������洢����ʹ�ܺͷ�ʹ�ܺ�������

�� �룺	ʹ�ܺͷ�ʹ�ܺ���������ͬʱ���ڻ�ͬʱΪNULL��

�� ����	���������ʼ��P2020��������SPI�ӿ�Ϊmasterģʽ��SPI�豸��
	����
	enable()
	{

	}
	disable()
	{

	}
	FM25V02 fm
	fm.Init(enable,disable);

����ֵ��0	�ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/

	int cs_init(FM_ENABLE enable_device , FM_DISABLE diable_device);

/*****************************************************
��������Read()

�� �ܣ�	��ȡ����洢���Ĵ洢����

�� �룺	pucDataBuf	�洢���ݵ�BUFFER
	ucStartAddr	��ȡ�����ݵ�ַ
	ucLen	������	��ȡ���ݵĳ���
�� ����	����������ڻ�ȡ����洢�������ݡ�
����ֵ��>=0���ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
��������Write()

�� �ܣ�	��ȡ����洢���Ĵ洢����

�� �룺	pucDataBuf	�洢���ݵ�BUFFER
	ucStartAddr	���͵�������ʼ��ַ
	ucLen	������	��ȡ���ݵĳ���
�� ����	�����������������洢��д���ݡ�
����ֵ��0���ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
��������Read_Id()

�� �ܣ�	��ȡ����洢����ID
�� ����	��ȡ����洢����ID��
	ID = 0x22,������С256K
	ID = 0x23,������С512K
	ID = 0x24,������С1M
����ֵ��IDֵ
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	int read_id();
	int device_show();
private:
	int read_status();
	int (*enable)();
	int (*disable)();
};


/*EPCS64�洢��*/
/*
	EPC64֧�ֵ�ַΪ�����ֽڵ�������
	���������Ϊ0x3
	��дʹ�ܲ�����Ϊ0x6
	��д������Ϊ0x2
	�����������Ϊ0xc7
*/
#define EPCS_CMD_LEN		1
#define EPCS_ADDR_LEN		3
typedef int (*EPCS_ENABLE)();
typedef int (*EPCS_DISABLE)();
class eos_spi_epcs64:public eos_spi_chip
{

public:
	eos_spi_epcs64();
	int cs_init(EPCS_ENABLE enable_device , EPCS_DISABLE diable_device);
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);
	int  device_erase();
	int write_status(unsigned char status);
	int device_show();
	int read_status();

/*****************************************************
��������Read_Id()

�� �ܣ�	��ȡ����洢����ID
�� ����	FPGA����洢�����������ִ洢��С������64M��128M��Ŀǰʹ����64M��С��EPCS64,128M��С��EPCS128��N25Q128A.
	��type=0ʱ��ָ���Ƿ���EPCS64��EPCS128����������
	��type=1ʱ��ָ���Ƿ���N25Q128A����
����ֵ��operation_code�����룬EPCS64����	0xab
                              EPCS128��N25Q128A 0x9f
	type�������ͣ�EPCS64��EPCS128��0
		������		N25Q128A��1
����ֵ��IDֵ. EPCS64��0x16
	      EPCS128��N25Q128A 0x18
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2013.5.29
*****************************************************/
	int read_id(unsigned char operation_code,unsigned char type);
private:
	int (*enable)();
	int (*disable)();
};


/*ACS9520Tʱ��оƬ*/

#define ACS_CMD_LEN		0
#define ACS_ADDR_LEN		1

typedef int (*ACS_ENABLE)();
typedef int (*ACS_DISABLE)();

class eos_spi_acs9520t:public eos_spi_chip
{

public:
	eos_spi_acs9520t();
	int cs_init(ACS_ENABLE enable_device , ACS_DISABLE diable_device);
	int device_read(unsigned char *pucDataBuf , unsigned char ucStartAddr, int ucLen);
	int device_write(unsigned char *pucDataBuf , unsigned char ucStartAddr, int ucLen);
private:
	int (*enable)();
	int (*disable)();
};

/*ZL30155ʱ��оƬ*/

#define ZL30155_CMD_LEN		0
#define ZL30155_ADDR_LEN	1

typedef int (*ZL30155_ENABLE)();
typedef int (*ZL30155_DISABLE)();

class eos_spi_zl30155:public eos_spi_chip
{

public:
	eos_spi_zl30155();
	int cs_init(ZL30155_ENABLE enable_device , ZL30155_DISABLE diable_device);
	int device_read(unsigned char *pucDataBuf ,unsigned char page_addr, unsigned char ucStartAddr, int ucLen);
	int device_write(unsigned char *pucDataBuf ,unsigned char page_addr,unsigned char ucStartAddr, int ucLen);
private:
	int (*enable)();
	int (*disable)();
};



/*ZL30143ʱ��оƬ��ͬ30155*/

#define ZL30143_CMD_LEN		0
#define ZL30143_ADDR_LEN	1

typedef int (*ZL30143_ENABLE)();
typedef int (*ZL30143_DISABLE)();

class eos_spi_zl30143:public eos_spi_chip
{

public:
	eos_spi_zl30143();
	int cs_init(ZL30143_ENABLE enable_device , ZL30143_DISABLE diable_device);
	int device_read(unsigned char *pucDataBuf ,unsigned char page_addr, unsigned char ucStartAddr, int ucLen);
	int device_write(unsigned char *pucDataBuf ,unsigned char page_addr,unsigned char ucStartAddr, int ucLen);
private:
	int (*enable)();
	int (*disable)();
};

/*k10��Դ����оƬ*/
#define K10_CMD_LEN		0
#define K10_ADDR_LEN	0

typedef int (*K10_ENABLE)();
typedef int (*K10_DISABLE)();

class eos_spi_k10:public eos_spi_chip
{

public:
	eos_spi_k10();
	int cs_init(K10_ENABLE enable_device , K10_DISABLE diable_device);
	int device_read(unsigned char *pucDataBuf ,int ucLen);
	int device_write(unsigned char *pucDataBuf ,int ucLen);
private:
	int (*enable)();
	int (*disable)();
};

/*BCM5389 ����switchоƬ*/
#define BCM5389_CMD_LEN	1
#define BCM5389_ADDR_LEN	1

typedef int (*BCM5389_ENABLE)();
typedef int (*BCM5389_DISABLE)();

class eos_spi_bcm5389:public eos_spi_chip
{

public:
	eos_spi_bcm5389();
	int cs_init(BCM5389_ENABLE enable_device , BCM5389_DISABLE diable_device);
	int device_read(unsigned char *pucDataBuf,unsigned char pageaddr,unsigned char ucStartAddr, int ucLen);
	int device_write(unsigned char *pucDataBuf,unsigned char pageaddr,unsigned char ucStartAddr, int ucLen);
private:
	unsigned char read_status();
	int (*enable)();
	int (*disable)();
};

/*flash MX25U1635*/

#define MX25U1635_CMD_LEN	1
#define MX25U1635_ADDR_LEN	3

typedef int (*MX25U1635_ENABLE)();
typedef int (*MX25U1635_DISABLE)();

class eos_spi_mx25u1635:public eos_spi_chip
{

public:
	eos_spi_mx25u1635();
	int cs_init(MX25U1635_ENABLE enable_device , MX25U1635_DISABLE diable_device);
	int device_erase();
	int read_id(char *id);
	int device_read(unsigned char *pucDataBuf,unsigned int ucStartAddr, unsigned int ucLen);
	int device_write(unsigned char *pucDataBuf,unsigned int ucStartAddr, unsigned int ucLen);
	unsigned char read_status();
private:

	int (*enable)();
	int (*disable)();
};


/*ZL30169ʱ��оƬ*/
/*�������Ͳ�������ֽ���*/
#define ZL30169_CMD_LEN		1

/*�������͵�ַ���ֽ���*/
#define ZL30169_ADDR_LEN		2
typedef int (*ZL30169_ENABLE)();
typedef int (*ZL30169_DISABLE)();

/*��������洢������ز���*/
class eos_spi_zl30169:public eos_spi_chip
{

public:
	eos_spi_zl30169();

/*****************************************************
��������Init()

�� �ܣ�	��ZL30169��ʹ�ܺͷ�ʹ�ܺ�������

�� �룺	ʹ�ܺͷ�ʹ�ܺ���������ͬʱ���ڻ�ͬʱΪNULL��

�� ����	���������ʼ��P2020��������SPI�ӿ�Ϊmasterģʽ��SPI�豸��
	����
	enable()
	{

	}
	disable()
	{

	}
	ZL30169 fm
	fm.Init(enable,disable);

����ֵ��0	�ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2015.5.21
*****************************************************/
	int cs_init(ZL30169_ENABLE enable_device , ZL30169_DISABLE diable_device);

/*****************************************************
��������Read()

�� �ܣ�	��ȡʱ��оƬZL30169�Ĵ洢����

�� �룺	pucDataBuf	�洢���ݵ�BUFFER
	ucStartAddr	��ȡ�����ݵ�ַ
	ucLen	������	��ȡ���ݵĳ���
�� ����	����������ڻ�ȡ����洢�������ݡ�
����ֵ��>=0���ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2015.5.21
*****************************************************/
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
��������Write()

�� �ܣ�	дʱ��оƬZL30169������

�� �룺	pucDataBuf	�洢���ݵ�BUFFER
	ucStartAddr	���͵�������ʼ��ַ
	ucLen	������	��ȡ���ݵĳ���
�� ����	�����������������洢��д���ݡ�
����ֵ��0���ɹ�
	-1	ʧ��
�� �ߣ�	lmyan
�� �ģ�	����
ʱ �䣺	2015.5.21
*****************************************************/
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

	int device_show();
private:
	int read_status();
	int (*enable)();
	int (*disable)();
};


#define EOS_SPI_MR25H10_CMD_LEN		1

/*�������͵�ַ���ֽ���*/
#define EOS_SPI_MR25H10_ADDR_LEN		3
typedef int (*eos_spi_mr25h10_enable)();
typedef int (*eos_spi_mr25h10_disable)();

/*����洢������ز���*/
class eos_spi_mr25h10:public eos_spi_chip
{

public:
	eos_spi_mr25h10();

/*****************************************************
��������cs_init()

�� �ܣ�	��MR25H10�洢����ʹ�ܺͷ�ʹ�ܺ�������

�� �룺	ʹ�ܺͷ�ʹ�ܺ���������ͬʱ���ڻ�ͬʱΪNULL��

�� ����	���������ʼ��P2020��������SPI�ӿ�Ϊmasterģʽ��SPI�豸��
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
	int cs_init(eos_spi_mr25h10_enable enable_device , eos_spi_mr25h10_disable diable_device);

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
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
��������Write()

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
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

private:
	int (*enable)();
	int (*disable)();
};

#endif
