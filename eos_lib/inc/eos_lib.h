/******************************************************************************
 * �ļ���  �� eos_lib.h
 * ������  ��
 * �������ڣ� 20150403
 * �汾��  ��   v1.0
 * �ļ�������v3r1�ײ������ӿ�ͷ�ļ�
 * ��Ȩ˵���� Copyright (c) 2000-2020	���ͨ�ſƼ��ɷ����޹�˾
 * ��    ���� ��
 * �޸���־�� ��
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

#define CPU_CCSR_GPIOCR_OFFSET	0xe0030       /*����GPIO�ķ���*/
#define CPU_CCSR_GPOUTDR_OFFSET	0xe0040      /*GPIO������*/
#define CPU_CCSR_GPINDR_OFFSET	0xe0050     /*GPIO�����*/

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
*Cpld��fpga�ȱ��������豸��pcie�豸��ַ�ռ�ķ��ʽӿڣ�
*ȫ��ͨ��mmap��ʽ�ṩ�ӿڣ�����Ҫ����ko
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
* ������  �� eos_lib_init
* ������  ��
* �������ڣ�20150403
* �������ܣ�eos���ʼ��
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ����Ҫ�ڵ���eos�⺯��֮ǰ���ã��ڲ�������eos_cpu_mmap_init eos_core_cpld_mmap_init
* ��    ����
*************************************************************************/

extern int eos_lib_init();

/*************************************************************************
* ������  �� eos_mmap_init
* ������  ��
* �������ڣ�20150403
* �������ܣ�ӳ��ĳ�ʼ��
* ���������lenth:Ҫӳ���ַ�ռ��С
				  phy_offset:Ҫӳ���������ʼ��ַ
				  bits_len:����λ����
				  devname:ͨ��Ϊ/dev/mem
* �����������
* ����ֵ��	�ṹ��eos_mmap_unit: �ɹ�
       			NULL: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern struct eos_mmap_unit* eos_mmap_init(unsigned int lenth, unsigned long long phy_offset,  \
 unsigned int bits_len, const char *devname);

/*************************************************************************
* ������  �� eos_mmap_exit
* ������  ��
* �������ڣ�20150403
* �������ܣ�ӳ����ͷ�
* ���������mmap_info:����eos_mmap_initʱ���صĽṹ��
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_mmap_exit(struct eos_mmap_unit **mmap_info);

/*************************************************************************
* ������  �� eos_mmap_read
* ������  ��
* �������ڣ�20150403
* �������ܣ�������
* ���������mmap_info:����eos_mmap_initʱ���صĽṹ��
				  offset:ƫ��
				  data:����buf
				  len:���ݳ���
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_mmap_read(struct eos_mmap_unit *mmap_info, unsigned int offset, void *data, unsigned int len);

/*************************************************************************
* ������  �� eos_mmap_write
* ������  ��
* �������ڣ�20150403
* �������ܣ�д����
* ���������mmap_info:����eos_mmap_initʱ���صĽṹ��
				  offset:ƫ��
				  data:����buf
				  len:���ݳ���
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_mmap_write(struct eos_mmap_unit *mmap_info, unsigned int offset, void *data, unsigned int len);

/*************************************************************************
* ������  �� eos_mmap_write_data
* ������  ��
* �������ڣ�20150403
* �������ܣ�д����
* ���������mmap_info:����eos_mmap_initʱ���صĽṹ��
				  offset:ƫ��
				  data:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_mmap_write_data(struct eos_mmap_unit *mmap_info, unsigned int offset, unsigned int data);

/*************************************************************************
* ������  �� eos_cpu_mmap_init
* ������  ��
* �������ڣ�20150403
* �������ܣ�CPU�ڲ��Ĵ�����ӳ�亯��
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ����eos_lib_init����
* ��    ����
*************************************************************************/

extern int eos_cpu_mmap_init();

/*************************************************************************
* ������  �� eos_cpu_mmap_exit
* ������  ��
* �������ڣ�20150403
* �������ܣ�CPU�ڲ��Ĵ���ӳ����ͷź���
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_cpu_mmap_exit();

/*************************************************************************
* ������  �� eos_cpu_reg_read
* ������  ��
* �������ڣ�20150403
* �������ܣ�CPU�ڲ��Ĵ���������
* ���������addr:��ַƫ��
				  data:����buf
				  len:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_cpu_reg_read(unsigned int addr, void *data, unsigned int len);

/*************************************************************************
* ������  �� eos_cpu_reg_write
* ������  ��
* �������ڣ�20150403
* �������ܣ�CPU�ڲ��Ĵ���д����
* ���������addr:��ַƫ��
				  data:����buf
				  len:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_cpu_reg_write(unsigned int addr, void *data, unsigned int len);

/*************************************************************************
* ������  �� eos_set_gpio_direction
* ������  ��
* �������ڣ�20150403
* �������ܣ�����CPU��GPIO����
* ���������gpio_num:�ܽ�IO 0~31
				  direction:���� 0/1   ����/���
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_set_gpio_direction(unsigned int gpio_num, unsigned int direction);

/*************************************************************************
* ������  �� eos_get_gpio_direction
* ������  ��
* �������ڣ�20150403
* �������ܣ���ȡCPU��GPIO����
* ���������gpio_num:�ܽ�IO 0~31
				  direction:���� 0/1   ����/���
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_get_gpio_direction(unsigned int gpio_num, unsigned int *direction);

/*************************************************************************
* ������  �� eos_set_gpio_status
* ������  ��
* �������ڣ�20150403
* �������ܣ�����CPU��GPIO������
* ���������gpio_num:�ܽ�IO 0~31
				  direction:���� 0/1   �͵�ƽ/�ߵ�ƽ
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_set_gpio_status(unsigned int gpio_num, unsigned int status);

/*************************************************************************
* ������  �� eos_get_gpio_status
* ������  ��
* �������ڣ�20150403
* �������ܣ���ȡCPU��GPIO������
* ���������gpio_num:�ܽ�IO 0~31
				  direction:���� 0/1   �͵�ƽ/�ߵ�ƽ
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_get_gpio_status(unsigned int gpio_num, unsigned int *status);


/*************************************************************************
* ������  �� eos_core_cpld_mmap_init
* ������  ��
* �������ڣ�20150403
* �������ܣ��۰�CPLD��ӳ�亯��
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ����eos_lib_init����
* ��    ����
*************************************************************************/

extern int eos_core_cpld_mmap_init();

/*************************************************************************
* ������  �� eos_core_cpld_mmap_exit
* ������  ��
* �������ڣ�20150403
* �������ܣ��۰�CPLD��ӳ����ͷź���
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_core_cpld_mmap_exit();

/*************************************************************************
* ������  �� eos_core_cpld_read
* ������  ��
* �������ڣ�20150403
* �������ܣ��۰�CPLD�Ķ�����
* ���������regaddr:ƫ��
				  data:����buf
				  len: ����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_core_cpld_read(unsigned int regaddr, void *data, unsigned int len);

/*************************************************************************
* ������  �� eos_core_cpld_write
* ������  ��
* �������ڣ�20150403
* �������ܣ��۰�CPLD��д����
* ���������regaddr:ƫ��
				  data:����buf
				  len: ����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int eos_core_cpld_write(unsigned int regaddr, void *data, unsigned int len);


/*************************************************************************
* ������  �� eos_set_dog_mode
* ������  ��
* �������ڣ�20160802
* �������ܣ�����ι����ʽ
* ���������int flag=0/1=���ι��/Ӳ��ι��
* �����������
* ����ֵ��	0: �ɹ�
       		-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int eos_set_dog_mode(int flag);

/*************************************************************************
* ������  �� eos_soft_feed_dog
* ������  ��
* �������ڣ�20160802
* �������ܣ����ι���ӿ�
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       		-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int eos_soft_feed_dog(void);

/*************************************************************************
* ������  �� eos_get_dog_mode
* ������  ��
* �������ڣ�20160802
* �������ܣ���ȡι����ʽ
* �����������
* ���������unsigned char *val=0/1=���ι��/Ӳ��ι��
* ����ֵ��	0: �ɹ�
       		-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int eos_get_dog_mode(unsigned char *val);


extern int eos_set_feed_dog_time(unsigned int count);

/*************************************************************************
* ������  �� eos_hard_feed_dog
* ������  ��
* �������ڣ�20160802
* �������ܣ�ʹ��/ȥʹ��Ӳ��ι��
* ���������unsigned char ucFlag=0/1=ȥʹ��/ʹ��
* �����������
* ����ֵ��	0: �ɹ�
       		-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int eos_hard_feed_dog(unsigned char ucFlag);


/*************************************************************************
* ������  �� eos_get_gid
* ������  ��
* �������ڣ�20150403
* �������ܣ���ȡGID
* ���������value:ָ�룬���GID
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_get_gid(unsigned char *value);


/*************************************************************************
* ������  �� eos_i2c_write_byte
* ������  ��
* �������ڣ�20150403
* �������ܣ�i2cд����
* ���������bus:Ƭѡ0/1...����0��ʼ
				  dev_addr:7λ��������ַ
				  reg_addr_len:�Ĵ�����λ����1/2/3/4  8bit/16bit/24bit/32bit
				  reg_addr:��ַ
				  len:����
				  data:����buf
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_i2c_write_byte(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned int len,unsigned char *data);

/*************************************************************************
* ������  �� eos_i2c_write_byte_data
* ������  ��
* �������ڣ�20150403
* �������ܣ�i2cд����
* ���������bus:Ƭѡ0/1...����0��ʼ
				  dev_addr:7λ��������ַ
				  reg_addr_len:�Ĵ�����λ����1/2/3/4  8bit/16bit/24bit/32bit
				  reg_addr:��ַ
				  len:����
				  data:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_i2c_write_byte_data(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned char data);

/*************************************************************************
* ������  �� eos_i2c_read_byte
* ������  ��
* �������ڣ�20150403
* �������ܣ�i2c������
* ���������bus:Ƭѡ0/1...����0��ʼ
				  dev_addr:7λ��������ַ
				  reg_addr_len:�Ĵ�����λ����1/2/3/4  8bit/16bit/24bit/32bit
				  reg_addr:��ַ
				  len:����
				  data:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_i2c_read_byte(unsigned int bus,unsigned char dev_addr,unsigned int reg_addr_len, \
	unsigned int reg_addr,unsigned int len,unsigned char *data);

/*************************************************************************
* ������  �� eos_i2c_send_byte
* ������  ��
* �������ڣ�20150403
* �������ܣ�i2c������
* ���������bus:Ƭѡ0/1...����0��ʼ
				  dev_addr:7λ��������ַ
				  command:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_i2c_send_byte(unsigned int bus,unsigned char dev_addr,unsigned int command);

/*************************************************************************
* ������  �� eos_i2c_receive_byte
* ������  ��
* �������ڣ�20150403
* �������ܣ�i2c������
* ���������bus:Ƭѡ0/1...����0��ʼ
				  dev_addr:7λ��������ַ
				  data:����buf
				  len:����
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_i2c_receive_byte(unsigned int bus,unsigned char dev_addr,unsigned char *data,unsigned int len);

extern int eos_get_curtemp(int *temp_cur);
extern int eos_set_warntemphigh(int temp_high);
extern int eos_set_warntemplow(int temp_low);
extern int eos_get_warntemphigh(int *temp_high);
extern int eos_get_warntemplow(int *temp_low);


/*************************************************************************
* ������  �� eos_scan_pcidev
* ������  ��
* �������ڣ�20150403
* �������ܣ�PCIEɨ�躯��
* ������������ߺ�
* �����������
* ����ֵ��	0: �޴�����
       			-1: ʧ��
       			1:ɨ�� ������
* ���ù�ϵ��
* ��    ����Ŀǰ��P2020,P204Xʹ�ô˺�����mpc8308,P1020ʹ�õ�������ʽ
*************************************************************************/

extern int eos_scan_pcidev(int busnum);

/*************************************************************************
* ������  �� eos_remove_pcidev
* ������  ��
* �������ڣ�20150403
* �������ܣ�PCIEɾ������
* ������������ߺ�
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����Ŀǰ��P2020,P204Xʹ�ô˺�����mpc8308,P1020ʹ�õ�������ʽ
*************************************************************************/

extern int eos_remove_pcidev(int busnum);


/*************************************************************************
* ������  �� eos_minimpc8308_reset_pcie
* ������  ��
* �������ڣ�20150403
* �������ܣ���λ8308��PCIE������
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_minimpc8308_reset_pcie();


/*************************************************************************
* ������  �� eos_mpc8308_reset_pcie
* ������  ��
* �������ڣ�20150403
* �������ܣ���λ8308��PCIE������
* �����������
* �����������
* ����ֵ��	0: �ɹ�
       			-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/

extern int eos_mpc8308_reset_pcie();

extern int eos_p204x_pcie_hot_reset(unsigned int num);

/****************************************************************
* �ļ���  ��p204x arad_pcie_api from arad_pcie_func.c
* �ļ�������arad�Ȱβ幦������ӿ�
* ��    ������
******************************************************************************/

/*************************************************************************
* ������  �� wri_xxx_pcidev
* ������  ��
* �������ڣ�
* �������ܣ��Ƴ�/����ɨ�� pcie�������豸
* ���������busnum    pcie bus���
* �����������
* ����ֵ��	0: �ɹ�
       		-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int wri_remove_pcidev(int busnum);
extern int wri_scan_pcidev(int busnum);

/*************************************************************************
* ������  �� reset_pcie_controller
* ������  ��
* �������ڣ�
* �������ܣ����� pcie������
* ���������contrnum   pcie��������� 1~3
* �����������
* ����ֵ��	0: �ɹ�
       		-1: ʧ��
* ���ù�ϵ��
* ��    ����
*************************************************************************/
extern int reset_pcie_controller(int contrnum);

/******************************************************************************
*GPIOȫ�ֱ������ӿڣ�
******************************************************************************/
extern struct eos_mmap_unit *gpio_mmap;
extern int eos_gpio_mmap_init();
extern int eos_gpio_mmap_exit();

/******************************************************************************
*eos_sys_resoure�ӿڣ�
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

