/****************************************************************
* 文件名  ：eos_lib.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：库的初始化函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/

#include "eos_lib.h"

#if defined(CPU_NAME_P204X) || defined(CPU_NAME_P2020) || defined(CPU_NAME_P1020)
int eos_lib_init()
{
	int ret = 0;
	ret = eos_cpu_mmap_init();
	if(ret == -1)
	{
		return ret;
	}
	ret = eos_core_cpld_mmap_init();
	return ret;
}
#endif

#if defined(CPU_NAME_MPC8308) ||defined(CPU_NAME_MINIMPC8308) ||defined(CPU_NAME_MPC8548)
int eos_lib_init()
{
	int ret = 0;
	ret = eos_cpu_mmap_init();
	return ret;
}
#endif

#if defined(CPU_NAME_LS1021)
int eos_lib_init()
{
	int ret = 0;
	ret = eos_gpio_mmap_init();
	if(ret == -1)
	{
		return ret;
	}
	ret = eos_core_cpld_mmap_init();
	return ret;
}
#endif

#if defined(CPU_NAME_T104X)
int eos_lib_init()
{
	int ret = 0;
	ret = eos_gpio_mmap_init();
	if(ret == -1)
	{
		return ret;
	}

	ret = eos_cpu_mmap_init();
	if(ret == -1)
	{
		return ret;
	}
	ret = eos_core_cpld_mmap_init();
	return ret;
}
#endif