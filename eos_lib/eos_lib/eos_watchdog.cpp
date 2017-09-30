/****************************************************************
* 文件名  ：eos_watchdog.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：看门狗的初始化函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/
#include "eos_lib.h"

#if defined(CPU_NAME_P204X)
int eos_set_dog_mode(int flag)
{
	int ret = -1;
	unsigned char value;
	value = flag;
	ret = eos_core_cpld_write(0x15, &value, 1);
	if (ret < 0)
	{
		printf("eos_hard_feed_dog eos_core_cpld_write error!\n");
		return -1;
	}
	return 0;
}

static int eos_feed_dog_count = 0;
int eos_soft_feed_dog( void)
{
	int ret = -1;
	static unsigned char setSignal = 0;
	if (setSignal == 0)
	{
		//set gpio28 out put
		if (0 != eos_set_gpio_direction(28, 1))
		{
			printf("eos_set_gpio_direction 28 error!\n");
			return -1;
		}
		setSignal = 1;
	}
	eos_feed_dog_count++;
	if (eos_feed_dog_count % 2)
	{
		ret = eos_set_gpio_status(28, 0);
	}
	else
	{
		ret = eos_set_gpio_status(28, 1);
	}
	return ret;
}

int eos_get_dog_mode(unsigned char *val)
{
	int ret = -1;
	unsigned char value;
	ret = eos_core_cpld_read(0x15, &value, 1);
	if (ret < 0)
	{
		printf("eos_get_dog_mode eos_core_cpld_read error!\n");
		return -1;
	}
	*val = value;
	return 0;
}
#endif

#if defined(CPU_NAME_P2020)
int eos_set_dog_mode(int flag)
{
	int ret = -1;
	unsigned char value;
	value = flag;
	ret = eos_core_cpld_write(CPLD_FEED_DOG, &value, 1);
	if (ret < 0)
	{
		printf("eos_hard_feed_dog eos_core_cpld_write error!\n");
		return -1;
	}
	return 0;
    
}

static int eos_feed_dog_count = 0;
int eos_soft_feed_dog( void)
{
	int ret = -1;
	static unsigned char setSignal = 0;
	if (setSignal == 0)
	{
		//set gpio15 out put
		if (0 != eos_set_gpio_direction(CPLD_FEED_DOG_GPIO, 1))
		{
			printf("eos_set_gpio_direction  error!\n");
			return -1;
		}
		setSignal = 1;
	}
	eos_feed_dog_count++;
	if (eos_feed_dog_count % 2)
	{
		ret = eos_set_gpio_status(CPLD_FEED_DOG_GPIO, 0);
	}
	else
	{
		ret = eos_set_gpio_status(CPLD_FEED_DOG_GPIO, 1);
	}
	return ret;
}

int eos_get_dog_mode(unsigned char *val)
{
	int ret = -1;
	unsigned char value;
	ret = eos_core_cpld_read(CPLD_FEED_DOG, &value, 1);
	if (ret < 0)
	{
		printf("eos_get_dog_mode eos_core_cpld_read error!\n");
		return -1;
	}
	*val = value;
	return 0;

}

int eos_hard_feed_dog(unsigned char ucFlag)
{
   	unsigned int cpu_reg_value = 0;
	int ret = -1;
	
    if ((0 != ucFlag) && (1 != ucFlag))
    {
        return -1;
    }
    if (ucFlag == 0)
    {
        cpu_reg_value = HARD_STOP_WATCHDOG;
    }
    else
    {
        cpu_reg_value = HARD_START_WATCHDOG;
    }
	
	ret = eos_cpu_reg_write(CPU_CCSR_HARD_WATCHDOG_ADDR, &cpu_reg_value, sizeof(unsigned int));
	if (ret < 0)
	{
		printf("eos_hard_feed_dog eos_cpu_write error!\n");
		return -1;
	}
	return 0;
    
}


#endif

#if defined(CPU_NAME_P1020)
int eos_set_dog_mode(int flag)
{

}

int eos_soft_feed_dog( void)
{

}

int eos_get_dog_mode(unsigned char *val)
{

}
#endif

#if defined(CPU_NAME_LS1021) || defined(CPU_NAME_T104X)
int eos_set_dog_mode(int flag)
{
	int ret = -1;
	unsigned char value;

	if(flag)
	{
		value = 0xff;
	}else
	{
		value = 0;
	}
	ret = eos_core_cpld_write(CPLD_FEED_DOG, &value, 1);
	if (ret < 0)
	{
		printf("eos_hard_feed_dog eos_core_cpld_write error!\n");
		return -1;
	}
	return 0;
}

static int eos_feed_dog_count = 0;
int eos_soft_feed_dog( void)
{
	int ret = -1;
	static unsigned char setSignal = 0;
	if (setSignal == 0)
	{
		if (0 != eos_set_gpio_direction(CPLD_FEED_DOG_GPIO, 1))
		{
			printf("eos_set_gpio_direction  error!\n");
			return -1;
		}
		setSignal = 1;
	}
	eos_feed_dog_count++;
	if (eos_feed_dog_count % 2)
	{
		ret = eos_set_gpio_status(CPLD_FEED_DOG_GPIO, 0);
	}
	else
	{
		ret = eos_set_gpio_status(CPLD_FEED_DOG_GPIO, 1);
	}
	return ret;
}
int eos_get_dog_mode(unsigned char *val)
{
	int ret = -1;
	unsigned char value;
	ret = eos_core_cpld_read(CPLD_FEED_DOG, &value, 1);
	if (ret < 0)
	{
		printf("eos_get_dog_mode eos_core_cpld_read error!\n");
		return -1;
	}
	*val = value;
	return 0;
}
int eos_set_feed_dog_time(unsigned int count)
{
	int ret = -1;
	if (count > 255)
	{
		printf("error! count should be less then 256\n");
		return -1;
	}
	unsigned char value;
	value = count;

	ret = eos_core_cpld_write(CPLD_FEED_DOG_TIME, &value, 1);
	if (ret < 0)
	{
		printf("eos_hard_feed_dog eos_core_cpld_write error!\n");
		return -1;
	}
	return 0;
}
#endif