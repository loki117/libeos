/****************************************************************
* 文件名  ：spi_85xx.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：库的初始化函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
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
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "eos_spi.h"

pthread_mutex_t spi_lock = PTHREAD_MUTEX_INITIALIZER;

int eos_spi_chip::open_device(const char *dev_name)
{
	int fd = 0;
	fd = open(dev_name, O_RDWR);    //打开设备文件
	if ( -1 == fd)
  	{
  		printf("fd is %d\n", fd);
    		printf("%s\n", strerror(errno));
    		return -1;
  	}
  	_fd = fd;
	return 0;
}
void eos_spi_chip::close_device()
{
	close(_fd);
}

eos_spi_chip::eos_spi_chip()
{
	next=NULL;
	_recvLen=0;
	_sendLen=0;
	_fd = 0;
	_curStatus = 0;
	para.clock_polarity=0;
	para.clock_phase=0;
	para.bit_order=0;
	para.max_speed=0;
	para.charlen=0;
}

int eos_spi_chip::set_para(unsigned char clock_polarity,unsigned char clock_phase,unsigned char bit_order,unsigned int max_speed,unsigned char charlen )
{
	pthread_mutex_lock(&spi_lock);
	para.clock_polarity=clock_polarity;
	para.clock_phase=clock_phase;
	para.bit_order=bit_order;
	para.max_speed=max_speed;
	para.charlen=charlen;
	spi_init(clock_polarity, clock_phase,bit_order, max_speed, charlen);
	pthread_mutex_unlock(&spi_lock);
	return 0;
}

int eos_spi_chip::spi_transfer(char *buf,unsigned int rx_len,unsigned int tx_len)
{
	int file_operation = _fd;
	int size = 0;
	if( buf == NULL )
	{
		printf("transfer input error!\n");
		return -1;
	}
	if( (rx_len !=0) && (tx_len ==0) )
	{
		size = read(file_operation,buf,rx_len);
		return size;
	}
	else if( (rx_len ==0) && (tx_len !=0) )
	{
		size = write(file_operation,buf,tx_len);
		return size;
	}
	else
	{
		printf("transfer input error!\n");
		return -1;
	}
}

int eos_spi_chip::get_fd()
{
	return _fd;
}

int eos_spi_chip::spi_init(unsigned char clock_polarity, unsigned char clock_phase, unsigned char bit_order, int spiclock_div,unsigned char charlen)
{
	set_bit_per_word(charlen);
	set_lsb_first(bit_order);
	set_max_speed(spiclock_div);
	set_spi_cpha(clock_phase);
	set_spi_cpol(clock_polarity);
	return 0;
}

int eos_spi_chip::set_bit_per_word(unsigned char bit_per_word)
{
	int file_description = 0;
	unsigned char l_bit_per_word = bit_per_word;
	file_description = _fd;
	ioctl(file_description,SPI_IOC_WR_BITS_PER_WORD,&l_bit_per_word);
	return 0;
}

int eos_spi_chip::set_lsb_first(unsigned char lsb_first)
{
	int file_description = 0;
	file_description = _fd;
	unsigned char mode = 0;
	unsigned char l_lsb_first = lsb_first;
	ioctl(file_description,SPI_IOC_RD_MODE,&mode);
	//printf("mode = 0x%x\n",mode);
	if(1 == l_lsb_first)
	{
		mode = mode | 0x08;
	}
	if(0 == l_lsb_first)
	{
		mode = mode & (~0x08) ;
	}
	ioctl(file_description,SPI_IOC_WR_MODE,&mode);
	return 0;
}

int eos_spi_chip::set_max_speed(unsigned int max_speed)
{
	int file_description = 0;
	file_description = _fd;
	unsigned int l_max_speed = max_speed;
	ioctl(file_description,SPI_IOC_WR_MAX_SPEED_HZ,&l_max_speed);
	return 0;
}

int eos_spi_chip::set_spi_cpha(unsigned char spi_cpha)
{
	int file_description = 0;
	unsigned char l_spi_cpha = spi_cpha;
	unsigned char mode = 0;
	file_description = _fd;
	if((spi_cpha != 0) && (spi_cpha != 1))
	{
		printf("input error!\n");
		return -1;
	}
	ioctl(file_description,SPI_IOC_RD_MODE,&mode);
	//printf("mode = 0x%x\n",mode);
	if(1 == l_spi_cpha)
	{
		mode = mode | 0x01;
	}
	if(0 == l_spi_cpha)
	{
		mode = mode & (~0x01) ;
	}
	//printf("mode = 0x%x\n",mode);
	ioctl(file_description,SPI_IOC_WR_MODE,&mode);
	return 0;
}

int eos_spi_chip::set_spi_cpol(unsigned char spi_cpol)
{
	int file_description = 0;
	file_description = _fd;
	unsigned char l_spi_cpol = spi_cpol;
	unsigned char mode = 0;
	if((spi_cpol != 0) && (spi_cpol != 1))
	{
		printf("input error!\n");
		return -1;
	}
	ioctl(file_description,SPI_IOC_RD_MODE,&mode);
	//printf("mode = 0x%x\n",mode);
	if(1 == l_spi_cpol)
	{
		mode = mode | 0x02;
	}
	if(0 == l_spi_cpol)
	{
		mode = mode & (~0x02) ;
	}
	ioctl(file_description,SPI_IOC_WR_MODE,&mode);
	return 0;
}

int eos_spi_chip::device_show()
{
	printf("This Chip SPI para is:\n");
	printf("clock_polarity:%d\nclock_phase   :%d\nbit_order     :%d\nspiclock_div  :%d\ncharlen       :%d\n",
		para.clock_polarity,
		para.clock_phase,
		para.bit_order,
		para.max_speed,
		para.charlen);
	return 0;
}


