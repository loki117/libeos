/******************************************************************************
 * 文件名  ： eos_lib.h
 * 负责人  ：
 * 创建日期： 20150403
 * 版本号  ：   v1.0
 * 文件描述：v3r1底层物理库接口头文件
 * 版权说明： Copyright (c) 2000-2020	烽火通信科技股份有限公司
 * 其    他： 无
 * 修改日志： 无
******************************************************************************/

#ifndef _EOS_LIB_H_
#define _EOS_LIB_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <net/if.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <asm/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <arpa/inet.h>
#include "unistd.h"

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */


#if defined(CPU_NAME_P204X)
#define CPU_CCSR_BASE_ADDR	0xffe000000ull
#define CPU_CCSR_MEM_LEN	0x1000000
#define CPU_CCSR_BITS_LEN	32


#define CPU_CCSR_GPIODIR_OFFSET	0x130000
#define CPU_CCSR_GPIODAT_OFFSET	0x130008

#define LOCALBUS_CS0	0xfd0000000ull   // nor flash
#define LOCALBUS_CS1	0xfe0000000ull
#define LOCALBUS_CS2	0xfe8000000ull
#define LOCALBUS_CS3	0xff1000000ull	//corecpld


#define CORE_CPLD_BASE_ADDR LOCALBUS_CS3
#define CORE_CPLD_BITS_LEN  8
#define CORE_CPLD_ADDR_LEN 	0x10000

#elif defined(CPU_NAME_MPC8548)
#define CPU_CCSR_BASE_ADDR	0xe0000000ull
#define CPU_CCSR_MEM_LEN	0x100000
#define CPU_CCSR_BITS_LEN	32

#define CPU_CCSR_GPIOCR_OFFSET	0xe0030       /*设置GPIO的方向*/
#define CPU_CCSR_GPOUTDR_OFFSET	0xe0040      /*GPIO的输入*/
#define CPU_CCSR_GPINDR_OFFSET	0xe0050     /*GPIO的输出*/

#elif defined(CPU_NAME_MPC8308)
#define CPU_CCSR_BASE_ADDR	0xff000000ull
#define CPU_CCSR_MEM_LEN	0x100000
#define CPU_CCSR_BITS_LEN	32

#define CPU_CCSR_GPIODIR_OFFSET	0x130000
#define CPU_CCSR_GPIODAT_OFFSET	0x130008

#define LOCALBUS_CS2	0xfe0000000ull
#define LOCALBUS_CS2_LEN	0x100000ull
#define LOCALBUS_CS2_BITS_LEN 8



#elif defined(CPU_NAME_MINIMPC8308)
#define CPU_CCSR_BASE_ADDR	0xf0000000ull
#define CPU_CCSR_MEM_LEN	0x100000
#define CPU_CCSR_BITS_LEN	32

#define CPU_CCSR_GPIODIR_OFFSET	0xc00
#define CPU_CCSR_GPIODAT_OFFSET	0xc08

#define LOCALBUS_CS2	0x20000000ull
#define LOCALBUS_CS2_LEN	0x100000ull
#define LOCALBUS_CS2_BITS_LEN 8



#elif defined(CPU_NAME_P1020)
#define CPU_CCSR_BASE_ADDR	0xffe00000ull
#define CPU_CCSR_MEM_LEN	0x1000000
#define CPU_CCSR_BITS_LEN	32

#define CPU_CCSR_GPIODIR_OFFSET	0xfc00
#define CPU_CCSR_GPIODAT_OFFSET	0xfc08

#define CORE_CPLD_BASE_ADDR  0xda000000
#define CORE_CPLD_BITS_LEN   8
#define CORE_CPLD_ADDR_LEN  0x100

#elif defined(CPU_NAME_P1020_4G)
#define CPU_CCSR_BASE_ADDR	0xfffe00000ull
#define CPU_CCSR_MEM_LEN	0x1000000
#define CPU_CCSR_BITS_LEN	32

#define CPU_CCSR_GPIODIR_OFFSET	0xfc00
#define CPU_CCSR_GPIODAT_OFFSET	0xfc08

#define CORE_CPLD_BASE_ADDR  0xfda000000
#define CORE_CPLD_BITS_LEN   8
#define CORE_CPLD_ADDR_LEN  0x100


#elif defined(CPU_NAME_P2020)
#define CPU_CCSR_BASE_ADDR	0xfffe00000ull
#define CPU_CCSR_MEM_LEN	0x100000
#define CPU_CCSR_BITS_LEN	32

#define CPU_CCSR_GPIODIR_OFFSET	0xfc00
#define CPU_CCSR_GPIODAT_OFFSET	0xfc08


#define CPU_CCSR_HARD_WATCHDOG_ADDR         0xe0e00
#define HARD_START_WATCHDOG                 0x80000003 /*25M clk feed dog*/
#define HARD_STOP_WATCHDOG                  0x00000003


#define CORE_CPLD_BASE_ADDR  0xfda000000
#define CORE_CPLD_BITS_LEN   8
#define CORE_CPLD_ADDR_LEN  0x1000000
#define	CPLD_FEED_DOG   0x95
#define	CPLD_FEED_DOG_TIME 
#define	CPLD_FEED_DOG_GPIO  15


#elif defined(CPU_NAME_LS1021)
#define CPU_CCSR_BASE_ADDR	0x10000000ull
#define CPU_CCSR_MEM_LEN	0x10000000
#define CPU_CCSR_BITS_LEN	32

#define LOCALBUS_CS0	0x60000000ull   // nor flash
#define LOCALBUS_CS1	0x71000000ull
#define LOCALBUS_CS2	0x72000000ull
#define LOCALBUS_CS3	0x73000000ull	//corecpld


#define CORE_CPLD_BASE_ADDR LOCALBUS_CS3
#define CORE_CPLD_BITS_LEN  8
#define CORE_CPLD_ADDR_LEN 	0x10000

#define GPIO_BASEADDR 0x2300000
#define GPIO_MEMLEN 0x40000
#define GPIO_BITLEN 32

#define	CPLD_FEED_DOG 0x55
#define	CPLD_FEED_DOG_TIME 0x56
#define	CPLD_FEED_DOG_GPIO 14


#elif defined(CPU_NAME_T104X)
#define CPU_CCSR_BASE_ADDR	0xffe000000ull
#define CPU_CCSR_MEM_LEN	0x800000
#define CPU_CCSR_BITS_LEN	32


#define CPU_CCSR_GPIODIR_OFFSET	0x130000
#define CPU_CCSR_GPIODAT_OFFSET	0x130008

#define LOCALBUS_CS0	0xfe0000000ull  // nor flash
#define LOCALBUS_CS1	0xfd0000000ull
#define LOCALBUS_CS2	0xfe8000000ull


#define CORE_CPLD_BASE_ADDR LOCALBUS_CS1
#define CORE_CPLD_BITS_LEN  8
#define CORE_CPLD_ADDR_LEN 	0x10000

#define GPIO_BASEADDR (CPU_CCSR_BASE_ADDR + 0x130000)
#define GPIO_MEMLEN 0x4000
#define GPIO_BITLEN 32

#define	CPLD_FEED_DOG 0xb
#define	CPLD_FEED_DOG_TIME 0xc
#define	CPLD_FEED_DOG_GPIO 46

#endif


/******************************************************************************
*Cpld、fpga等本地总线设备及pcie设备地址空间的访问接口：
*全部通过mmap方式提供接口，不需要驱动ko
******************************************************************************/


struct eos_mmap_unit
{
    int fd;
    unsigned int lenth;
    unsigned int bits_len;
    unsigned long long phy_mmapbase;
    unsigned char *virt_mmapbase;
    pthread_mutex_t lock_mmap;
};

/*************************************************************************
* 函数名  ： eos_lib_init
* 负责人  ：
* 创建日期：20150403
* 函数功能：eos库初始化
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：需要在调用eos库函数之前调用，内部调用了eos_cpu_mmap_init eos_core_cpld_mmap_init
* 其    它：
*************************************************************************/

extern int eos_lib_init();

/*************************************************************************
* 函数名  ： eos_mmap_init
* 负责人  ：
* 创建日期：20150403
* 函数功能：映射的初始化
* 输入参数：lenth:要映射地址空间大小
				  phy_offset:要映射的物理起始地址
				  bits_len:多少位访问
				  devname:通常为/dev/mem
* 输出参数：无
* 返回值：	结构体eos_mmap_unit: 成功
       			NULL: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern struct eos_mmap_unit* eos_mmap_init(unsigned int lenth, unsigned long long phy_offset,  \
 unsigned int bits_len, const char *devname);

/*************************************************************************
* 函数名  ： eos_mmap_exit
* 负责人  ：
* 创建日期：20150403
* 函数功能：映射的释放
* 输入参数：mmap_info:调用eos_mmap_init时返回的结构体
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_mmap_exit(struct eos_mmap_unit **mmap_info);

/*************************************************************************
* 函数名  ： eos_mmap_read
* 负责人  ：
* 创建日期：20150403
* 函数功能：读函数
* 输入参数：mmap_info:调用eos_mmap_init时返回的结构体
				  offset:偏移
				  data:数据buf
				  len:数据长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_mmap_read(struct eos_mmap_unit *mmap_info, unsigned int offset, void *data, unsigned int len);

/*************************************************************************
* 函数名  ： eos_mmap_write
* 负责人  ：
* 创建日期：20150403
* 函数功能：写函数
* 输入参数：mmap_info:调用eos_mmap_init时返回的结构体
				  offset:偏移
				  data:数据buf
				  len:数据长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_mmap_write(struct eos_mmap_unit *mmap_info, unsigned int offset, void *data, unsigned int len);

/*************************************************************************
* 函数名  ： eos_mmap_write_data
* 负责人  ：
* 创建日期：20150403
* 函数功能：写函数
* 输入参数：mmap_info:调用eos_mmap_init时返回的结构体
				  offset:偏移
				  data:数据
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_mmap_write_data(struct eos_mmap_unit *mmap_info, unsigned int offset, unsigned int data);

/*************************************************************************
* 函数名  ： eos_cpu_mmap_init
* 负责人  ：
* 创建日期：20150403
* 函数功能：CPU内部寄存器的映射函数
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：被eos_lib_init调用
* 其    它：
*************************************************************************/

extern int eos_cpu_mmap_init();

/*************************************************************************
* 函数名  ： eos_cpu_mmap_exit
* 负责人  ：
* 创建日期：20150403
* 函数功能：CPU内部寄存器映射的释放函数
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_cpu_mmap_exit();

/*************************************************************************
* 函数名  ： eos_cpu_reg_read
* 负责人  ：
* 创建日期：20150403
* 函数功能：CPU内部寄存器读函数
* 输入参数：addr:地址偏移
				  data:数据buf
				  len:长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_cpu_reg_read(unsigned int addr, void *data, unsigned int len);

/*************************************************************************
* 函数名  ： eos_cpu_reg_write
* 负责人  ：
* 创建日期：20150403
* 函数功能：CPU内部寄存器写函数
* 输入参数：addr:地址偏移
				  data:数据buf
				  len:长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_cpu_reg_write(unsigned int addr, void *data, unsigned int len);

/*************************************************************************
* 函数名  ： eos_set_gpio_direction
* 负责人  ：
* 创建日期：20150403
* 函数功能：设置CPU的GPIO方向
* 输入参数：gpio_num:管脚IO 0~31
				  direction:方向 0/1   输入/输出
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_set_gpio_direction(unsigned int gpio_num, unsigned int direction);

/*************************************************************************
* 函数名  ： eos_get_gpio_direction
* 负责人  ：
* 创建日期：20150403
* 函数功能：获取CPU的GPIO方向
* 输入参数：gpio_num:管脚IO 0~31
				  direction:方向 0/1   输入/输出
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_get_gpio_direction(unsigned int gpio_num, unsigned int *direction);

/*************************************************************************
* 函数名  ： eos_set_gpio_status
* 负责人  ：
* 创建日期：20150403
* 函数功能：设置CPU的GPIO的数据
* 输入参数：gpio_num:管脚IO 0~31
				  direction:方向 0/1   低电平/高电平
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_set_gpio_status(unsigned int gpio_num, unsigned int status);

/*************************************************************************
* 函数名  ： eos_get_gpio_status
* 负责人  ：
* 创建日期：20150403
* 函数功能：获取CPU的GPIO的数据
* 输入参数：gpio_num:管脚IO 0~31
				  direction:方向 0/1   低电平/高电平
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_get_gpio_status(unsigned int gpio_num, unsigned int *status);


/*************************************************************************
* 函数名  ： eos_core_cpld_mmap_init
* 负责人  ：
* 创建日期：20150403
* 函数功能：扣板CPLD的映射函数
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：被eos_lib_init调用
* 其    它：
*************************************************************************/

extern int eos_core_cpld_mmap_init();

/*************************************************************************
* 函数名  ： eos_core_cpld_mmap_exit
* 负责人  ：
* 创建日期：20150403
* 函数功能：扣板CPLD的映射的释放函数
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_core_cpld_mmap_exit();

/*************************************************************************
* 函数名  ： eos_core_cpld_read
* 负责人  ：
* 创建日期：20150403
* 函数功能：扣板CPLD的读函数
* 输入参数：regaddr:偏移
				  data:数据buf
				  len: 长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_core_cpld_read(unsigned int regaddr, void *data, unsigned int len);

/*************************************************************************
* 函数名  ： eos_core_cpld_write
* 负责人  ：
* 创建日期：20150403
* 函数功能：扣板CPLD的写函数
* 输入参数：regaddr:偏移
				  data:数据buf
				  len: 长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int eos_core_cpld_write(unsigned int regaddr, void *data, unsigned int len);


/*************************************************************************
* 函数名  ： eos_set_dog_mode
* 负责人  ：
* 创建日期：20160802
* 函数功能：设置喂狗方式
* 输入参数：int flag=0/1=软件喂狗/硬件喂狗
* 输出参数：无
* 返回值：	0: 成功
       		-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int eos_set_dog_mode(int flag);

/*************************************************************************
* 函数名  ： eos_soft_feed_dog
* 负责人  ：
* 创建日期：20160802
* 函数功能：软件喂狗接口
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       		-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int eos_soft_feed_dog(void);

/*************************************************************************
* 函数名  ： eos_get_dog_mode
* 负责人  ：
* 创建日期：20160802
* 函数功能：获取喂狗方式
* 输入参数：无
* 输出参数：unsigned char *val=0/1=软件喂狗/硬件喂狗
* 返回值：	0: 成功
       		-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int eos_get_dog_mode(unsigned char *val);


extern int eos_set_feed_dog_time(unsigned int count);

/*************************************************************************
* 函数名  ： eos_hard_feed_dog
* 负责人  ：
* 创建日期：20160802
* 函数功能：使能/去使能硬件喂狗
* 输入参数：unsigned char ucFlag=0/1=去使能/使能
* 输出参数：无
* 返回值：	0: 成功
       		-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int eos_hard_feed_dog(unsigned char ucFlag);


/*************************************************************************
* 函数名  ： eos_get_gid
* 负责人  ：
* 创建日期：20150403
* 函数功能：获取GID
* 输入参数：value:指针，存放GID
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_get_gid(unsigned char *value);


/*************************************************************************
* 函数名  ： eos_i2c_write_byte
* 负责人  ：
* 创建日期：20150403
* 函数功能：i2c写函数
* 输入参数：bus:片选0/1...，从0开始
				  dev_addr:7位从器件地址
				  reg_addr_len:寄存器的位数，1/2/3/4  8bit/16bit/24bit/32bit
				  reg_addr:地址
				  len:长度
				  data:数据buf
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_i2c_write_byte(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned int len,unsigned char *data);

/*************************************************************************
* 函数名  ： eos_i2c_write_byte_data
* 负责人  ：
* 创建日期：20150403
* 函数功能：i2c写函数
* 输入参数：bus:片选0/1...，从0开始
				  dev_addr:7位从器件地址
				  reg_addr_len:寄存器的位数，1/2/3/4  8bit/16bit/24bit/32bit
				  reg_addr:地址
				  len:长度
				  data:数据
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_i2c_write_byte_data(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned char data);

/*************************************************************************
* 函数名  ： eos_i2c_read_byte
* 负责人  ：
* 创建日期：20150403
* 函数功能：i2c读函数
* 输入参数：bus:片选0/1...，从0开始
				  dev_addr:7位从器件地址
				  reg_addr_len:寄存器的位数，1/2/3/4  8bit/16bit/24bit/32bit
				  reg_addr:地址
				  len:长度
				  data:数据
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_i2c_read_byte(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned int len,unsigned char *data);

/*************************************************************************
* 函数名  ： eos_i2c_send_byte
* 负责人  ：
* 创建日期：20150403
* 函数功能：i2c发命令
* 输入参数：bus:片选0/1...，从0开始
				  dev_addr:7位从器件地址
				  command:命令
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_i2c_send_byte(unsigned int bus,unsigned char dev_addr,unsigned int command);

/*************************************************************************
* 函数名  ： eos_i2c_receive_byte
* 负责人  ：
* 创建日期：20150403
* 函数功能：i2c收数据
* 输入参数：bus:片选0/1...，从0开始
				  dev_addr:7位从器件地址
				  data:数据buf
				  len:长度
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_i2c_receive_byte(unsigned int bus,unsigned char dev_addr,unsigned char *data,unsigned int len);

extern int eos_get_curtemp(int *temp_cur);
extern int eos_set_warntemphigh(int temp_high);
extern int eos_set_warntemplow(int temp_low);
extern int eos_get_warntemphigh(int *temp_high);
extern int eos_get_warntemplow(int *temp_low);


/*************************************************************************
* 函数名  ： eos_scan_pcidev
* 负责人  ：
* 创建日期：20150403
* 函数功能：PCIE扫描函数
* 输入参数：总线号
* 输出参数：无
* 返回值：	0: 无此器件
       			-1: 失败
       			1:扫描 到器件
* 调用关系：
* 其    它：目前有P2020,P204X使用此函数，mpc8308,P1020使用的其他方式
*************************************************************************/

extern int eos_scan_pcidev(int busnum);

/*************************************************************************
* 函数名  ： eos_remove_pcidev
* 负责人  ：
* 创建日期：20150403
* 函数功能：PCIE删除函数
* 输入参数：总线号
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：目前有P2020,P204X使用此函数，mpc8308,P1020使用的其他方式
*************************************************************************/

extern int eos_remove_pcidev(int busnum);


/*************************************************************************
* 函数名  ： eos_minimpc8308_reset_pcie
* 负责人  ：
* 创建日期：20150403
* 函数功能：复位8308的PCIE控制器
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_minimpc8308_reset_pcie();


/*************************************************************************
* 函数名  ： eos_mpc8308_reset_pcie
* 负责人  ：
* 创建日期：20150403
* 函数功能：复位8308的PCIE控制器
* 输入参数：无
* 输出参数：无
* 返回值：	0: 成功
       			-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/

extern int eos_mpc8308_reset_pcie();

extern int eos_p204x_pcie_hot_reset(unsigned int num);

/****************************************************************
* 文件名  ：p204x arad_pcie_api from arad_pcie_func.c
* 文件描述：arad热拔插功能所需接口
* 其    它：无
******************************************************************************/

/*************************************************************************
* 函数名  ： wri_xxx_pcidev
* 负责人  ：
* 创建日期：
* 函数功能：移除/重新扫描 pcie总线上设备
* 输入参数：busnum    pcie bus编号
* 输出参数：无
* 返回值：	0: 成功
       		-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int wri_remove_pcidev(int busnum);
extern int wri_scan_pcidev(int busnum);

/*************************************************************************
* 函数名  ： reset_pcie_controller
* 负责人  ：
* 创建日期：
* 函数功能：重启 pcie控制器
* 输入参数：contrnum   pcie控制器编号 1~3
* 输出参数：无
* 返回值：	0: 成功
       		-1: 失败
* 调用关系：
* 其    它：
*************************************************************************/
extern int reset_pcie_controller(int contrnum);

/******************************************************************************
*GPIO全局变量及接口：
******************************************************************************/
extern struct eos_mmap_unit *gpio_mmap;
extern int eos_gpio_mmap_init();
extern int eos_gpio_mmap_exit();

/******************************************************************************
*eos_sys_resoure接口：
******************************************************************************/

extern int  eos_get_mem_info(int  *mem_val);
extern int  eos_get_disk_info(int *disk_val);
extern int eos_get_cpu_info(int *cpu_val);

extern int eos_open_dma(const char *devname);
extern int eos_close_dma(int fd);
extern int eos_init_dma_arg(unsigned int fd,unsigned long long (*p)[2],unsigned int num);
extern int eos_start_dma_transfer(unsigned int fd);
extern int  eos_get_dma_buffer_addr(unsigned int fd,unsigned int *addr);
extern int eos_get_dma_status(unsigned int fd);
extern int eos_print_dma_data(const char *devname,unsigned int addr);

#if 1

/*xbluo add 20161025*/

extern int eos_open_dma_channel(const char *devname);
extern int eos_close_dma_channel(int fd);
extern int eos_init_dma_arg_channel(unsigned int fd,unsigned long long (*p)[2],unsigned int num, int channel);
extern int eos_get_dma_buffer_addr_channel(unsigned int fd,unsigned int *addr, int channel);
extern int eos_get_dma_status_channel(unsigned int fd, int channel);
extern int eos_start_dma_transfer_channel(unsigned int fd, int channel);
#endif

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif

