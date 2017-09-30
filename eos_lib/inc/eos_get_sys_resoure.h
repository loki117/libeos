/****************************************************************
* 文件名  ：get_sys_resoure.h
* 负责人  ：田运朴
* 创建日期：20150727
* 版本号  ： v1.1
* 文件描述：主控盘获取系统资源(mem、)占用率接口
* 版权说明：Copyright?(c)?2000-2020 烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20150727 by 田运朴 yptian,在熊权洪源码的基础上，
*按照主控软件人员的新需求，修改了接口及参数类型，还有参数返回值。
*三个接口参数返回值为整形，并且在原有基础上*10。
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







