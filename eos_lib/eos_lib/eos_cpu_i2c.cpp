/****************************************************************
* 文件名  ：eos_cpu_i2c.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：i2c通用接口函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/

#include "eos_lib.h"
/******************************************************************************
*I2c设备通过设备地址进行区分。不一定要在dts文件中声明。
*但i2c控制器必须声明。
*对于特殊的i2c器件（如fpga方式的i2c等需特殊考虑，在此暂不涉及）。
******************************************************************************/
pthread_mutex_t eos_i2c_device_lock = PTHREAD_MUTEX_INITIALIZER;

#define CMD_I2C_RD  0x01
#define CMD_I2C_WR  0x00

int eos_i2c_write_byte(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len,unsigned int reg_addr,unsigned int len,unsigned char *data)
{
	int ret;
	int i;
	int fd = 0;
    	struct i2c_rdwr_ioctl_data rw_data;
    	struct i2c_msg mesg;
	char dev[100];
	unsigned int shift = 0xff;
	memset(dev,0,100);
	snprintf(dev,100,"/dev/i2c-%d",bus);
	pthread_mutex_lock(&eos_i2c_device_lock);
	fd = open(dev, O_RDWR);
	{
		if(fd == -1)
		{
			printf("open i2c dev fail! errno == %d\n" ,errno);
			pthread_mutex_unlock(&eos_i2c_device_lock);
			return -1;
		}
	}

    	if(reg_addr_len > 4)
    	{
		printf("input args error\n");
		close(fd);
		pthread_mutex_unlock(&eos_i2c_device_lock);
		return -1;
    	}
    	bzero(&mesg, sizeof(mesg));
    	mesg.buf = (__u8 *)malloc(sizeof(char)*(reg_addr_len + len));
    	bzero(mesg.buf, sizeof(char)*(reg_addr_len + len));
    	rw_data.msgs = &mesg;
    	rw_data.nmsgs = 1;
    	mesg.addr = (__u16)dev_addr;
    	mesg.flags = CMD_I2C_WR;
    	mesg.len = (__u16)(reg_addr_len + len);
	for(i = reg_addr_len;i > 0;i --)
	{
		mesg.buf[i-1] = reg_addr & shift;
		shift = shift << 8;
	}
	if((data != NULL) && (len != 0))
	{
		memcpy(&mesg.buf[reg_addr_len],data,len);
	}
	ret = ioctl(fd, I2C_RDWR, &rw_data);
    	if (ret < 0)
    	{
    		printf("eos_i2c_read_ops ioctl error! errno == %d\n" ,errno);
		free(mesg.buf);
		close(fd);
		pthread_mutex_unlock(&eos_i2c_device_lock);
    		return -1;
    	}
	free(mesg.buf);
	close(fd);
	pthread_mutex_unlock(&eos_i2c_device_lock);
    	return 0;
}

int eos_i2c_write_byte_data(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned char data)
{
	return eos_i2c_write_byte(bus,dev_addr,reg_addr_len,reg_addr,1,&data);
}

int eos_i2c_read_byte(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len,unsigned int reg_addr,unsigned int len,unsigned char *data)
{
    	struct i2c_rdwr_ioctl_data rw_data;
    	struct i2c_msg mesg[2];
	int i =0;
	int ret = 0;
	int fd =0;
	unsigned int shift = 0xff;
	char dev[100];
	memset(dev,0,100);
	snprintf(dev,100,"/dev/i2c-%d",bus);

	fd = open(dev, O_RDWR);
	{
		if(fd == -1)
		{
			printf("open i2c dev fail ,errno == %d\n" ,errno);
			return -1;
		}
	}
	pthread_mutex_lock(&eos_i2c_device_lock);
    	if(reg_addr_len > 4)
    	{
		printf("input args error\n");
		pthread_mutex_unlock(&eos_i2c_device_lock);
		close(fd);
		
		return -1;
    	}
    	bzero(mesg, sizeof(mesg));
       	rw_data.msgs = mesg;
    	rw_data.nmsgs = 2;

    	mesg[0].buf = (__u8 *)malloc(sizeof(char)*(reg_addr_len));
	if(mesg[0].buf == NULL)
	{
		printf("malloc fail\n");
		pthread_mutex_unlock(&eos_i2c_device_lock);
		close(fd);
		return -1;
	}
	bzero(mesg[0].buf, sizeof(char)*(reg_addr_len));
	mesg[0].addr = (__u16)dev_addr;
	mesg[0].flags = CMD_I2C_WR;
	mesg[0].len = reg_addr_len;
	for(i = reg_addr_len;i > 0;i --)
	{
		mesg[0].buf[i-1] = reg_addr & shift;
		shift = shift << 8;
	}
    	mesg[1].buf = (__u8 *)malloc(sizeof(char)*(len));
	if(mesg[1].buf == NULL)
	{
		printf("malloc fail\n");
		free(mesg[0].buf);
		pthread_mutex_unlock(&eos_i2c_device_lock);
		close(fd);
		return -1;
	}
	bzero(mesg[1].buf, sizeof(char)*(len));
        mesg[1].flags = CMD_I2C_RD;
    	mesg[1].addr = (__u16)dev_addr;
    	mesg[1].len = (__u16)(len);
    	ret = ioctl(fd, I2C_RDWR, &rw_data);
    	if (ret < 0)
    	{
    		printf("eos_i2c_read_ops ioctl error! errno == %d\n" ,errno);
		free(mesg[0].buf);
		free(mesg[1].buf);
		pthread_mutex_unlock(&eos_i2c_device_lock);
		close(fd);
    		return -1;
    	}
	memcpy(data,mesg[1].buf,len);
	free(mesg[0].buf);
	free(mesg[1].buf);
	pthread_mutex_unlock(&eos_i2c_device_lock);
	close(fd);
    	return 0;
}

int eos_i2c_send_byte(unsigned int bus,unsigned char dev_addr,unsigned int command)
{
	return eos_i2c_write_byte(bus,dev_addr,1,command,0,NULL);
}

int eos_i2c_receive_byte(unsigned int bus,unsigned char dev_addr,unsigned char *data,unsigned int len)
{
    	struct i2c_rdwr_ioctl_data rw_data;
    	struct i2c_msg mesg;
	int ret = 0;
	int fd = 0;
	char dev[100];
	memset(dev,0,100);
	snprintf(dev,100,"/dev/i2c-%d",bus);
	fd = open(dev, O_RDWR);
	{
		if(fd == -1)
		{
			printf("open i2c dev fail ,errno == %d\n" ,errno);
			return -1;
		}
	}
	pthread_mutex_lock(&eos_i2c_device_lock);
	rw_data.msgs = &mesg;
	rw_data.nmsgs = 1;
	mesg.addr = (__u16)dev_addr;
	mesg.buf = (__u8 *)malloc(sizeof(char)*len);
	if(mesg.buf == NULL)
	{
		printf("malloc fail\n");
		pthread_mutex_unlock(&eos_i2c_device_lock);
		close(fd);
		return -1;
	}
	mesg.len=len;
	mesg.flags=CMD_I2C_RD;
	ret = ioctl(fd, I2C_RDWR, &rw_data);
    	if (ret < 0)
    	{
    		printf("eos_i2c ioctl error! errno == %d\n" ,errno);
		free(mesg.buf);
		pthread_mutex_unlock(&eos_i2c_device_lock);
		close(fd);
    		return -1;
    	}
	memcpy(data,mesg.buf,len);
	free(mesg.buf);
	pthread_mutex_unlock(&eos_i2c_device_lock);
	close(fd);
	return 0;
}

