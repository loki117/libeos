/****************************************************************
* �ļ���  ��eos_lib.cpp
* ������  ��lmyan
* �������ڣ�20150403
* �汾��  �� v1.1
* �ļ���������ĳ�ʼ������
* ��Ȩ˵����Copyright (c) 2000-2020   ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��20160215 by  lmyan, ����ԭ�ļ�ע�ͣ���ʼ���������ӷ���ֵ
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