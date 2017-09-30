/****************************************************************
* �ļ���  ��get_sys_resoure.h
* ������  ��������
* �������ڣ�20150727
* �汾��  �� v1.1
* �ļ������������̻�ȡϵͳ��Դ(mem��)ռ���ʽӿ�
* ��Ȩ˵����Copyright?(c)?2000-2020 ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��20150727 by ������ yptian,����Ȩ��Դ��Ļ����ϣ�
*�������������Ա���������޸��˽ӿڼ��������ͣ����в�������ֵ��
*�����ӿڲ�������ֵΪ���Σ�������ԭ�л�����*10��
******************************************************************************/
#ifndef _ARAD_PCIE_FUNC_H_
#define _ARAD_PCIE_FUNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#ifdef  __cplusplus
extern  "C"
{
#endif
#define BMU_MAX_CPU_NUM sysconf(_SC_NPROCESSORS_CONF)

/*#define DUSING_CPU_P2040*/

typedef unsigned long long cputime_t;
typedef long long icputime_t;
#define FMT_DATA "ll"
#define CPUTIME_MAX (~0ULL)

enum
{
    STATS_CPU_USER,
    STATS_CPU_NICE,
    STATS_CPU_SYSTEM,
    STATS_CPU_IDLE,
    STATS_CPU_IOWAIT,
    STATS_CPU_IRQ,
    STATS_CPU_SOFTIRQ,
    STATS_CPU_STEAL,
    STATS_CPU_GUEST,

    GLOBAL_UPTIME,
    SMP_UPTIME,

    N_STATS_CPU,
};

typedef struct
{
    cputime_t vector[N_STATS_CPU];
} stats_cpu_t;

extern int  eos_get_mem_info(int  *mem_val);
extern int  eos_get_disk_info(int *disk_val);
extern int eos_get_cpu_info(int *cpu_val);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif







