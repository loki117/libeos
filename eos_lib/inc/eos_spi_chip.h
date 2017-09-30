/******************************************************************************
 * 文件名  ： eos_lib_chip.h
 * 负责人  ：
 * 创建日期： 20150403
 * 版本号  ：   v1.0
 * 文件描述：spi器件头文件
 * 版权说明： Copyright (c) 2000-2020	烽火通信科技股份有限公司
 * 其    他： 无
 * 修改日志： 无
******************************************************************************/
#ifndef __EOS_SPI_CHIP_H
#define __EOS_SPI_CHIP_H


#include "eos_spi.h"

/*铁电存储器*/
/*器件发送操作码的字节数*/
#define FRAM_CMD_LEN		1

/*器件发送地址的字节数*/

#define FRAM_ADDR_LEN		2
typedef int (*FM_ENABLE)();
typedef int (*FM_DISABLE)();

/*定义铁电存储器的相关操作*/
class eos_spi_fm25v02:public eos_spi_chip
{

public:
	eos_spi_fm25v02();

/*****************************************************
函数名：Init()

功 能：	将铁电存储器的使能和非使能函数挂载

输 入：	使能和非使能函数。必须同时存在或同时为NULL；

描 述：	这个函数初始化P2020处理器的SPI接口为master模式的SPI设备。
	例：
	enable()
	{

	}
	disable()
	{

	}
	FM25V02 fm
	fm.Init(enable,disable);

返回值　0	成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/

	int cs_init(FM_ENABLE enable_device , FM_DISABLE diable_device);

/*****************************************************
函数名：Read()

功 能：	读取铁电存储器的存储数据

输 入：	pucDataBuf	存储数据的BUFFER
	ucStartAddr	读取的数据地址
	ucLen	　　　	读取数据的长度
描 述：	这个函数用于获取铁电存储器的数据。
返回值　>=0　成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
函数名：Write()

功 能：	读取铁电存储器的存储数据

输 入：	pucDataBuf	存储数据的BUFFER
	ucStartAddr	发送的数据起始地址
	ucLen	　　　	读取数据的长度
描 述：	这个函数用于往铁电存储器写数据。
返回值　0　成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
函数名：Read_Id()

功 能：	读取铁电存储器的ID
描 述：	获取铁电存储器的ID。
	ID = 0x22,器件大小256K
	ID = 0x23,器件大小512K
	ID = 0x24,器件大小1M
返回值　ID值
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
	int read_id();
	int device_show();
private:
	int read_status();
	int (*enable)();
	int (*disable)();
};


/*EPCS64存储器*/
/*
	EPC64支持地址为三个字节的器件。
	其读操作码为0x3
	其写使能操作码为0x6
	其写操作码为0x2
	其擦除操作码为0xc7
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
函数名：Read_Id()

功 能：	读取铁电存储器的ID
描 述：	FPGA程序存储器件存在两种存储大小的器件64M和128M。目前使用有64M大小的EPCS64,128M大小的EPCS128和N25Q128A.
	当type=0时，指的是访问EPCS64和EPCS128两种器件。
	当type=1时，指的是访问N25Q128A器件
输入值　operation_code操作码，EPCS64　　	0xab
                              EPCS128和N25Q128A 0x9f
	type器件类型，EPCS64和EPCS128　0
		　　　		N25Q128A　1
返回值　ID值. EPCS64　0x16
	      EPCS128和N25Q128A 0x18
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
	int read_id(unsigned char operation_code,unsigned char type);
private:
	int (*enable)();
	int (*disable)();
};


/*ACS9520T时钟芯片*/

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

/*ZL30155时钟芯片*/

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



/*ZL30143时钟芯片，同30155*/

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

/*k10电源管理芯片*/
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

/*BCM5389 网络switch芯片*/
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


/*ZL30169时钟芯片*/
/*器件发送操作码的字节数*/
#define ZL30169_CMD_LEN		1

/*器件发送地址的字节数*/
#define ZL30169_ADDR_LEN		2
typedef int (*ZL30169_ENABLE)();
typedef int (*ZL30169_DISABLE)();

/*定义铁电存储器的相关操作*/
class eos_spi_zl30169:public eos_spi_chip
{

public:
	eos_spi_zl30169();

/*****************************************************
函数名：Init()

功 能：	将ZL30169的使能和非使能函数挂载

输 入：	使能和非使能函数。必须同时存在或同时为NULL；

描 述：	这个函数初始化P2020处理器的SPI接口为master模式的SPI设备。
	例：
	enable()
	{

	}
	disable()
	{

	}
	ZL30169 fm
	fm.Init(enable,disable);

返回值　0	成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2015.5.21
*****************************************************/
	int cs_init(ZL30169_ENABLE enable_device , ZL30169_DISABLE diable_device);

/*****************************************************
函数名：Read()

功 能：	读取时钟芯片ZL30169的存储数据

输 入：	pucDataBuf	存储数据的BUFFER
	ucStartAddr	读取的数据地址
	ucLen	　　　	读取数据的长度
描 述：	这个函数用于获取铁电存储器的数据。
返回值　>=0　成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2015.5.21
*****************************************************/
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
函数名：Write()

功 能：	写时钟芯片ZL30169的数据

输 入：	pucDataBuf	存储数据的BUFFER
	ucStartAddr	发送的数据起始地址
	ucLen	　　　	读取数据的长度
描 述：	这个函数用于往铁电存储器写数据。
返回值　0　成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2015.5.21
*****************************************************/
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

	int device_show();
private:
	int read_status();
	int (*enable)();
	int (*disable)();
};


#define EOS_SPI_MR25H10_CMD_LEN		1

/*器件发送地址的字节数*/
#define EOS_SPI_MR25H10_ADDR_LEN		3
typedef int (*eos_spi_mr25h10_enable)();
typedef int (*eos_spi_mr25h10_disable)();

/*定义存储器的相关操作*/
class eos_spi_mr25h10:public eos_spi_chip
{

public:
	eos_spi_mr25h10();

/*****************************************************
函数名：cs_init()

功 能：	将MR25H10存储器的使能和非使能函数挂载

输 入：	使能和非使能函数。必须同时存在或同时为NULL；

描 述：	这个函数初始化P2020处理器的SPI接口为master模式的SPI设备。
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
	int cs_init(eos_spi_mr25h10_enable enable_device , eos_spi_mr25h10_disable diable_device);

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
	int device_read(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

/*****************************************************
函数名：Write()

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
	int device_write(unsigned char *pucDataBuf, unsigned long ucStartAddr,unsigned long ucLen);

private:
	int (*enable)();
	int (*disable)();
};

#endif
