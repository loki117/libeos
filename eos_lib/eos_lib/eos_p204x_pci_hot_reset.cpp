/****************************************************************
* �ļ���  ��eos_p204x_pci_hot_reset.cpp
* ������  ��lmyan
* �������ڣ�20150403
* �汾��  �� v1.1
* �ļ�������p2040��PCIE�ȸ�λ����
* ��Ȩ˵����Copyright (c) 2000-2020   ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��20160617 by  lmyan, ����ԭ�ļ�ע�ͣ���ʼ���������ӷ���ֵ
******************************************************************************/
#if defined(CPU_NAME_P204X)
#include "eos_lib.h"
#include <sys/mman.h>

#define PCIE_CONTROLLER0_BASE 0x200000
#define PCIE_CONTROLLER1_BASE 0x201000
#define PCIE_CONTROLLER2_BASE 0x202000
#define HOT_RESET_OFFSET 0xf00

int eos_p204x_pcie_hot_reset(unsigned int num)
{
	int pcie_reset_addr = 0;
	int pcie_reset_data = 0;
	if(num < 0 || num > 2)
	{
		printf("cpu p204x no this pcie controller\n");
		//fprintf(stderr, "pcie_controller number %d is invalid,valid value is 0~2\n", num);
		return -1;
	}
	if(num == 0)
	{
		pcie_reset_addr = PCIE_CONTROLLER0_BASE + HOT_RESET_OFFSET;
	}
	if(num == 1)
	{
		pcie_reset_addr = PCIE_CONTROLLER1_BASE + HOT_RESET_OFFSET;
	}
	if(num == 2)
	{
		pcie_reset_addr = PCIE_CONTROLLER2_BASE + HOT_RESET_OFFSET;
	}
	eos_cpu_reg_read(pcie_reset_addr, &pcie_reset_data, 1);
	pcie_reset_data = pcie_reset_data | (0x1 << 27);
	eos_cpu_reg_write(pcie_reset_addr, &pcie_reset_data, 1);
	usleep(50);
	pcie_reset_data = pcie_reset_data & (~(0x1 << 27));
	eos_cpu_reg_write(pcie_reset_addr, &pcie_reset_data, 1);
	return 0;
}

int reset_pcie_controller(int contrnum)
{
	return eos_p204x_pcie_hot_reset(contrnum - 1);
}
#endif
