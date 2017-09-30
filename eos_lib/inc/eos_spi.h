/******************************************************************************
 * 文件名  ： eos_spi.h
 * 负责人  ：
 * 创建日期： 20150403
 * 版本号  ：   v1.0
 * 文件描述：spi头文件
 * 版权说明： Copyright (c) 2000-2020	烽火通信科技股份有限公司
 * 其    他： 无
 * 修改日志： 无
******************************************************************************/
#ifndef __EOS_SPI_H
#define __EOS_SPI_H

#include <pthread.h>

/*SPI接收数据的BUFFER长度*/
#define SPI_MAX_READ_BUFF_LEN	32

/*SPI发送数据的BUFFER长度*/
#define SPI_MAX_WRITE_BUFF_LEN 260

/*SPI相关操作的线程间互斥锁*/
extern pthread_mutex_t spi_lock;

typedef struct tageos_spi_para
{
	unsigned char clock_polarity;
	unsigned char clock_phase;
	unsigned char bit_order;
	unsigned int max_speed;
	unsigned char  charlen;
}EOS_SPI_PARA;

/*定义SPI的总线类，定义的总线相关的操作*/
class eos_spi_chip
{
public:
	eos_spi_chip();

/*****************************************************
函数名：SetPara()

功 能：	P2020处理器SPI接口设备的初始化

输 入：	clock_polarity 	时钟极性选择	系统默认的设置0
				0	SPICLK低电平时处于不激活,此时的时钟输出信号波形__|--|__|--|__|--|__
				1	SPICLK高电平时处于不激活,此时的时钟输出信号波形--|__|--|__|--|__|--
	clock_phase	时钟相位选择	系统默认的设置0
				0	SPICLK starts toggling at the middle of the data transfer.
				1 	SPICLK starts toggling at the beginning of the data transfer.
	bit_order	bit传送顺序	系统默认的设置0
				0	先发送和接收字符的msb位
				1	先发送和接收字符的lsb位
	max_speed	SPICLK的频率设置	系统的默认频率10000000

	charlen		发送和接收一个字符的数据bit数。系统的默认值为8

描 述：	这个函数初始化P2020处理器的SPI接口为master模式的SPI设备。

	例：
	EPCS64　EPCS;
	EPCS.SpiInit(0,0,0,10000000,8);

作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
	int set_para( unsigned char clock_polarity,unsigned char clock_phase,unsigned char bit_order, \
	unsigned int max_speed,unsigned char charlen );



/*****************************************************
函数名：Transfer()

功 能：	传输SPI的数据

输 入：	设备路径

描 述：	buf为传输数据的BUF；
			当读操作时，tx_len = 0;rx_len为要读取的数据个数。
			当写操作时，rx_len = 0;tx_len 为写的数据个数和操作码、地址字节数的总和。


返回值　大于0	成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/


	int spi_transfer(char *buf,unsigned int rx_len,unsigned int tx_len);

/*****************************************************
函数名：Open_Device()

功 能：	打开SPI的设备节点

输 入：	设备路径

描 述：	这个函数初始化P2020处理器的SPI接口为master模式的SPI设备。
	例：
	EPCS64　EPCS;
	EPCS.Open_Device("/dev/spi");

返回值　大于0	成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
*****************************************************/
	int open_device(const char * dev_name);

/*****************************************************
函数名：Close_Device()

功 能：	关闭SPI的设备节点

输 入：	设备路径

描 述：	这个函数初始化P2020处理器的SPI接口为master模式的SPI设备。
	例：
	EPCS64 EPCS
	EPCS.Close_Device();

返回值　大于0	成功
	-1	失败
作 者：	lmyan
修 改：	创建
时 间：	2013.5.29
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