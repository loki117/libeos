/****************************************************************
 * 文件名  ：get_sys_resoure.c
 * 负责人  ：田运朴
 * 创建日期：20150727
 * 版本号  ： v1.1
 * 文件描述：主控盘获取系统资源(mem、)占用率接口
 * 版权说明：Copyright?(c)?2000-2020 烽火通信科技股份有限公司
 * 其    它：无
 * 修改日志：20150727 by 田运朴 yptian,在熊权洪源码的基础上，
 **按照主控软件人员的新需求，修改了接口及参数类型，还有参数返回值。
 **三个接口参数返回值为整形，并且在原有基础上*10。
 ******************************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <termios.h>
#include <dlfcn.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <mqueue.h>
#include <pty.h>
#include <execinfo.h>
#include <mtd/mtd-user.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <syslog.h>
#include "eos_get_sys_resoure.h"

/* Does str start with "cpu"? */
static int starts_with_cpu(const char *str)
{
    return ((str[0] - 'c') | (str[1] - 'p') | (str[2] - 'u')) == 0;
}

/*skip_whitespace*/
static char *skip_whitespace(const char *str)
{
    /* In POSIX/C locale (the only locale we care about: do we REALLY want
     * to allow Unicode whitespace in, say, .conf files? nuts!)
     * isspace is only these chars: "\t\n\v\f\r" and space.
     * "\t\n\v\f\r" happen to have ASCII codes 9,10,11,12,13.
     * Use that.
     */
    while (*str == ' ' || (unsigned char)(*str - 9) <= (13 - 9))
    {
        str++;
    }

    return (char *)str;
}

/*skip_non_whitespace*/
static char *skip_non_whitespace(const char *str)
{
    while (*str != '\0' && *str != ' ' && (unsigned char)(*str - 9) > (13 - 9))
    {
        str++;
    }

    return (char *)str;
}

/*get_interval*/
static cputime_t get_interval(cputime_t old, cputime_t news)
{
    cputime_t itv = news - old;

    return (itv == 0) ? 1 : itv;
}

#if CPUTIME_MAX > 0xffffffff

/*
 * Handle overflow conditions properly for counters which can have
 * less bits than cputime_t, depending on the kernel version.
 */

/* Surprisingly, on 32bit inlining is a size win */
static cputime_t overflow_safe_sub(cputime_t prev, cputime_t curr)
{
    cputime_t v = curr - prev;

    if ((icputime_t)v < 0     /* curr < prev - counter overflow? */
        && prev <= 0xffffffff /* kernel uses 32bit value for the counter? */
        )
    {
        /* Add 33th bit set to 1 to curr, compensating for the overflow */
        /* double shift defeats "warning: left shift count >= width of type" */
        v += ((cputime_t)1 << 16) << 16;
    }
    return v;
}

#else

/*percent_value*/
static cputime_t overflow_safe_sub(cputime_t prev, cputime_t curr)
{
    return curr - prev;
}

#endif

/*percent_value*/
static double percent_value(cputime_t prev, cputime_t curr, cputime_t itv)
{
    return ((double)overflow_safe_sub(prev, curr)) / itv * 100;
}

/* Fetch CPU statistics from /proc/stat */
static void get_cpu_time(stats_cpu_t *sc)
{
    FILE *fp;
    char buf[1024];
    unsigned char num;

    fp = fopen("/proc/stat", "r");
    if (NULL == fp)
    {
        printf("fopen /proc/stat ERROR.\n");
        return;
    }

    num = 0;
    memset(sc, 0, sizeof(*sc) * (BMU_MAX_CPU_NUM + 1));

    while (fgets(buf, sizeof(buf), fp))
    {
        int i;
        char *ibuf;

        /* Does the line start with "cpu "? */
        if (!starts_with_cpu(buf))
        {
            continue;
        }
        ibuf = buf + 4;
        for (i = STATS_CPU_USER; i <= STATS_CPU_GUEST; i++)
        {
            ibuf = skip_whitespace(ibuf);
            sscanf(ibuf, "%"FMT_DATA "u", &sc->vector[i]);
            if (i != STATS_CPU_GUEST)
            {
                sc->vector[GLOBAL_UPTIME] += sc->vector[i];
            }
            ibuf = skip_non_whitespace(ibuf);
        }

        if (++num >= (BMU_MAX_CPU_NUM + 1))
        {
            break;
        }
        sc += 1;
    }

    fclose(fp);
}

/*************************************************************************
* 函数名  ：bmu_get_cpu_info
* 负责人  ：田运朴
* 创建日期：20150627
* 函数功能：获取cpu占用率信息
* 输出参数：mem_val 为占用百分比，如20%，则mem_val = 20

* 返回值：	0:成功
                            -1:失败
* 调用关系：被eos_get_cpu_info调用
* 其   它：无
*************************************************************************/

/*************************************************************************
* 函数名  ：bmu_get_mem_info
* 负责人  ：田运朴
* 创建日期：20150627
* 函数功能：获取mem占用率信息
* 输出参数：mem_val 为占用百分比，如20%，则mem_val = 20

* 返回值：	0:成功
                            -1:失败
* 调用关系：被eos_get_mem_info调用
* 其   它：无
*************************************************************************/

/*************************************************************************
* 函数名  ：bmu_get_disk_info
* 负责人  ：田运朴
* 创建日期：20150627
* 函数功能：获取disk占用率信息
* 输出参数：disk_val 为占用百分比，如20%，则mem_val = 20
                                disk_name为磁盘挂载的目录
* 返回值：	0:成功
                            -1:失败
* 调用关系：被eos_get_disk_info调用
* 其   它：默认为/mnt/cfastdisk，也支持/mnt/sddisk
*************************************************************************/
static int eos_get_disk_base(const char *disk_name, float *disk_val)
{
    struct statfs s;
    FILE *mount_table;
    struct mntent *mount_entry;
    unsigned long blocks_used;
    int status = -1;

    if ((NULL == disk_name) || (NULL == disk_val))
    {
        return -1;
    }

    /*Get mount point info*/
    mount_table = setmntent("/etc/mtab", "r");
    if (NULL == mount_table)
    {
        fprintf(stderr, "%s %d:%s\r\n", __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }

    while (1)
    {
        mount_entry = getmntent(mount_table);
        if (NULL == mount_entry)
        {
            break;
        }

        if (strcmp(mount_entry->mnt_dir, disk_name) != 0)
        {
            continue;
        }

        if (statfs(mount_entry->mnt_dir, &s) != 0)
        {
            fprintf(stderr, "%s %d:%s\r\n", __FUNCTION__, __LINE__, strerror(errno));
            break;
        }

        if (s.f_blocks > 0)
        {
            blocks_used = s.f_blocks - s.f_bfree;
            if (blocks_used + s.f_bavail)
            {
                *disk_val = (float)(blocks_used * 100ULL + (blocks_used + s.f_bavail) / 2) / (blocks_used + s.f_bavail);
                status = 0;
            }
        }
    }
    endmntent(mount_table);
    return status;
}

#ifdef CPU_NAME_LS1021

/*************************************************************************
* 函数名  ：eos_get_mem_info
* 负责人  ：gaojian2014
* 创建日期：20160810
* 函数功能：获取mem占用率信息
* 输出参数：mem_val 为占用百分比，如20%，则mem_val = 200

* 返回值：	0:成功
                            -1:失败
* 调用关系：调用bmu_get_mem_info
* 其   它：区别于2.6内核，3.0版本以上的内核可通过MemAvailable判断可用内存大小
*************************************************************************/

#define EOS_GET_TOTAL_MEM       "cat /proc/meminfo |grep MemTotal|awk '{print $2}'"
#define EOS_GET_AVAIL_MEM       "cat /proc/meminfo |grep MemAvailable|awk '{print $2}'"

int eos_get_mem_info(int *mem_val)
{
    unsigned long total_mem = 0;
    unsigned long avail_mem = 0;
    char var[32] = {0};

    if (NULL == mem_val)
    {
        return -1;
    }

    /* 获取物理内存大小 */
    FILE *pp = popen(EOS_GET_TOTAL_MEM, "r");
    if (NULL == pp)
    {
        return -1;
    }
    while (fgets(var, sizeof(var), pp) != NULL)
    {
        ;
    }
    pclose(pp);

    total_mem = atol(var);

    /* 获取可用物理内存大小 */
    pp = popen(EOS_GET_AVAIL_MEM, "r");
    if (NULL == pp)
    {
        return -1;
    }
    while (fgets(var, sizeof(var), pp) != NULL)
    {
        ;
    }
    pclose(pp);

    avail_mem = atol(var);

    *mem_val = (total_mem - avail_mem) * 1000 / total_mem;

    //	printf("total: %ld\navail: %ld\nrate: %d\n", total_mem, avail_mem, *mem_val);

    return 0;
}

#else

/*************************************************************************
* 函数名  ：eos_get_mem_info
* 负责人  ：田运朴
* 创建日期：20150627
* 函数功能：获取mem占用率信息
* 输出参数：mem_val 为占用百分比，如20%，则mem_val = 200

* 返回值：	0:成功
                            -1:失败
* 调用关系：调用bmu_get_mem_info
* 其   它：无
*************************************************************************/
int eos_get_mem_info(int *mem_val)
{
    float bmu_mem_val = 0;

    struct sysinfo info;

    if (NULL == mem_val)
    {
        return -1;
    }

    /*Get mem info*/
    if (sysinfo(&info) != 0)
    {
        fprintf(stderr, "%s %d:%s\r\n", __FUNCTION__, __LINE__, strerror(errno));
        return -1;
    }

    bmu_mem_val = (float)(info.totalram - info.freeram) / info.totalram * 100;
    *mem_val = bmu_mem_val * 10;
    return 0;
}

#endif

/*************************************************************************
* 函数名  ：eos_get_disk_info
* 负责人  ：田运朴
* 创建日期：20150627
* 函数功能：获取disk占用率信息
* 输出参数：disk_val 为占用百分比，如20%，则mem_val = 200

* 返回值：	0:成功
                            -1:失败
* 调用关系：调用bmu_get_disk_info
* 其   它：默认为/mnt/cfastdisk，也支持/mnt/sddisk 、/media
*************************************************************************/
int eos_get_disk_info(int *disk_val)
{
    int ret = 0;
    float bmu_disk_val = 0;
    ret = eos_get_disk_base("/mnt/cfastdisk", &bmu_disk_val);
    if (ret < 0)
    {
        ret = eos_get_disk_base("/mnt/sddisk", &bmu_disk_val);
    }

#if defined(CPU_NAME_LS1021)
    if (ret < 0)
    {
        ret = eos_get_disk_base("/media/emmc", &bmu_disk_val);
    }
#else
    if (ret < 0)
    {
        ret = eos_get_disk_base("/media", &bmu_disk_val);
    }
#endif
    *disk_val = bmu_disk_val * 10;
    return ret;
}

/*************************************************************************
* 函数名  ：eos_get_cpu_info
* 负责人  ：田运朴
* 创建日期：20150627
* 函数功能：获取cpu占用率信息
* 输出参数：bmu_cpu_val [5]为占用百分比，如20%，则mem_val = 200
   其中bmu_cpu_val[0]返回4核平均值
                bmu_cpu_val[1]返回核1平均值
                bmu_cpu_val[2]返回核2平均值
                bmu_cpu_val[3]返回核3平均值
                bmu_cpu_val[4]返回核4平均值

* 返回值：	0:成功
            -1:失败
* 调用关系：调用bmu_get_cpu_info
* 其   它：
                        不论当前cpu的核数，使用前对bmu_cpu_val分配的空间必须 >= 5
*************************************************************************/
int eos_get_cpu_info(int *bmu_cpu_val)
{
    float bmu_cpu_val_tmp[5];

    //ret = bmu_get_cpu_info(bmu_cpu_val_tmp);

#if  defined(CPU_NAME_MPC8308) || defined(CPU_NAME_MINIMPC8308)
    stats_cpu_t old_stats_data[1 + 1];
    stats_cpu_t new_stats_data[1 + 1];
#elif defined(CPU_NAME_P204X)
    stats_cpu_t old_stats_data[4 + 1];
    stats_cpu_t new_stats_data[4 + 1];
#else
    stats_cpu_t old_stats_data[2 + 1];
    stats_cpu_t new_stats_data[2 + 1];
#endif

    cputime_t itv;
    unsigned int i;

    /* Fetch pre CPU statistics */
    get_cpu_time(&old_stats_data[0]);

    sleep(1);

    /* Fetch current CPU statistics */
    get_cpu_time(&new_stats_data[0]);

    /* Get interval */
    for (i = 0; i < (BMU_MAX_CPU_NUM + 1); i++)
    {
        itv = get_interval(old_stats_data[i].vector[GLOBAL_UPTIME], new_stats_data[i].vector[GLOBAL_UPTIME]);

        /*Cal cpu percent*/
        bmu_cpu_val_tmp[i] = 100 - (float)percent_value(old_stats_data[i].vector[STATS_CPU_IDLE],
            new_stats_data[i].vector[STATS_CPU_IDLE],
            itv);

    }

    bmu_cpu_val[0] = bmu_cpu_val_tmp[0] * 10;
    bmu_cpu_val[1] = bmu_cpu_val_tmp[1] * 10;
    bmu_cpu_val[2] = bmu_cpu_val_tmp[2] * 10;
    bmu_cpu_val[3] = bmu_cpu_val_tmp[3] * 10;
    bmu_cpu_val[4] = bmu_cpu_val_tmp[4] * 10;

    return 0;
}

