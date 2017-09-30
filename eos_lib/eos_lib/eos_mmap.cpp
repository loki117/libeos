/****************************************************************
* 文件名  ：eos_mmap.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：库的初始化函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/
#include "eos_lib.h"

/******************************************************************************
*全局变量及接口：
******************************************************************************/


struct eos_mmap_unit *core_cpld_mmap = NULL;
struct eos_mmap_unit *cpu_mmap = NULL;

#define  MUNMAP_FAILED (-1)

#if defined(CPU_NAME_P204X) || defined(CPU_NAME_P2020) || defined(CPU_NAME_P1020) || defined(CPU_NAME_MPC8308) || \
defined(CPU_NAME_MINIMPC8308) || defined(CPU_NAME_T104X)
int eos_cpu_mmap_init()
{
    cpu_mmap = eos_mmap_init(CPU_CCSR_MEM_LEN, CPU_CCSR_BASE_ADDR, CPU_CCSR_BITS_LEN, NULL);
    if (NULL == cpu_mmap)
    {
        printf("eos_cpu_mmap_init  failed\n");
        return -1;
     }
     else
     {
        printf("eos_cpu_mmap_init  ok\n");
        return 0;
     }
}


int eos_cpu_mmap_exit()
{
    int ret = 0;
    ret = eos_mmap_exit(&cpu_mmap);
    if (0 != ret)
    {
        printf("eos_cpu_mmap_exit  failed\n");
        return -1;
     }
     else
     {
        printf("eos_cpu_mmap_exit  ok\n");
        return 0;
     }
}


int eos_cpu_reg_read(unsigned int addr, void *data, unsigned int len)
{
	int ret = 0;
	if(cpu_mmap == NULL)
	{
		eos_cpu_mmap_init();
	}
    ret = eos_mmap_read(cpu_mmap, addr, data, len);
	if (ret < 0)
	{
		printf("eos_cpu_reg_read failed\n");
	}
    return ret;
}

int eos_cpu_reg_write(unsigned int addr, void *data, unsigned int len)
{
	int ret = 0;
	if(cpu_mmap == NULL)
	{
		eos_cpu_mmap_init();
	}
    ret = eos_mmap_write(cpu_mmap, addr, data, len);
	if (ret < 0)
	{
		printf("eos_cpu_reg_write failed\n");
	}
    return ret;
}
#endif

#if defined(CPU_NAME_P204X) || defined(CPU_NAME_P2020) || defined(CPU_NAME_P1020) || defined(CPU_NAME_LS1021) \
	|| defined(CPU_NAME_T104X)
int eos_core_cpld_mmap_init()
{
    core_cpld_mmap = eos_mmap_init(CORE_CPLD_ADDR_LEN, CORE_CPLD_BASE_ADDR, CORE_CPLD_BITS_LEN, NULL);
    if (NULL == core_cpld_mmap)
    {
        printf("eos_cpld_mmap_init  failed\n");
        return -1;
     }
     else
     {
        printf("eos_cpld_mmap_init  ok\n");
        return 0;
     }
}


int eos_core_cpld_mmap_exit()
{
    int ret = 0;
    ret = eos_mmap_exit(&core_cpld_mmap);
    if (0 != ret)
    {
        printf("eos_cpld_mmap_exit  failed\n");
        return -1;
     }
     else
     {
        printf("eos_cpld_mmap_exit  ok\n");
        return 0;
     }
}


int eos_core_cpld_read(unsigned int regaddr, void *data, unsigned int len)
{
	int ret = 0;
	if(core_cpld_mmap == NULL)
	{
		eos_core_cpld_mmap_init();
	}
    ret = eos_mmap_read(core_cpld_mmap, regaddr, data, len);
	if (ret < 0)
	{
		printf("eos_core_cpld_read failed\n");
	}
    return ret;
}

int eos_core_cpld_write(unsigned int regaddr, void *data, unsigned int len)
{
	int ret = 0;
	if(core_cpld_mmap == NULL)
	{
		eos_core_cpld_mmap_init();
	}
    ret = eos_mmap_write(core_cpld_mmap, regaddr, data, len);
	if (ret < 0)
	{
		printf("eos_core_cpld_write failed\n");
	}
    return ret;
}

#endif

/******************************************************************************
*Cpld、fpga等本地总线设备及pcie设备地址空间的访问接口：
*全部通过mmap方式提供接口，不需要驱动ko
******************************************************************************/
struct eos_mmap_unit* 
eos_mmap_init(unsigned int lenth, unsigned long long phy_offset, unsigned int bits_len, const char *devname)
{
    struct eos_mmap_unit *eos_mmap_info = NULL;
    int ret = 0;
    char mmap_devname[50] = {0};

    if (lenth > 0x10000000)
    {
        printf("lenth is invalid\n");
        return NULL;
    }

    if (!devname)
    {
        //printf("devname is invalid,default dev is /dev/mem\n");
        strcpy(mmap_devname, "/dev/mem");
     }
     else
     {
        strcpy(mmap_devname, devname);
     }

    eos_mmap_info = (struct eos_mmap_unit *)malloc(sizeof(struct eos_mmap_unit));
    if (NULL == eos_mmap_info)
    {
        printf("eos_mmap_init malloc failed\n");
        return NULL;
    }

    memset(eos_mmap_info, 0, sizeof(struct eos_mmap_unit));
    eos_mmap_info->fd = open(mmap_devname, O_RDWR);
    if (-1 == eos_mmap_info->fd)
    {
        printf("mmapfd is %d\n", eos_mmap_info->fd);
        printf("%s\n", strerror(errno));
        if (NULL != eos_mmap_info)
        {
            free(eos_mmap_info);
            eos_mmap_info = NULL;
        }
        return NULL;
    }
    eos_mmap_info->phy_mmapbase = phy_offset;
    eos_mmap_info->virt_mmapbase = (unsigned char *)mmap64(0,
        lenth,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        eos_mmap_info->fd,
        phy_offset);
    if (MAP_FAILED == eos_mmap_info->virt_mmapbase)
    {
        printf("mmap failed....................................\n");
        close(eos_mmap_info->fd);
        if (NULL != eos_mmap_info)
        {
            free(eos_mmap_info);
            eos_mmap_info = NULL;
        }
        return NULL;
    }

    ret = pthread_mutex_init(&eos_mmap_info->lock_mmap,NULL);
    if(ret != 0)
    {
        printf("lock_mmap init Fail\n");
        return NULL;
    }

    eos_mmap_info->lenth = lenth;
    eos_mmap_info->bits_len = bits_len;
    return eos_mmap_info;
}


int eos_mmap_exit(struct eos_mmap_unit **eos_mmap_info)
{
    if (NULL == *eos_mmap_info)
    {
        printf("input args is NULL\n");
        return -1;
    }

    if (MUNMAP_FAILED == munmap((*eos_mmap_info)->virt_mmapbase, (*eos_mmap_info)->lenth))
    {
        printf(" munmap fail ................................\n");
        return -1;
    }

    pthread_mutex_destroy(&(*eos_mmap_info)->lock_mmap);
    close((*eos_mmap_info)->fd);
    (*eos_mmap_info)->virt_mmapbase =  NULL;

    if (NULL != *eos_mmap_info)
    {
        free(*eos_mmap_info);
        *eos_mmap_info =  NULL;
    }

    return 0;
}


int eos_mmap_read(struct eos_mmap_unit *eos_mmap_info, unsigned int offset, void *data, unsigned int len)
{
    int i = 0;

    if (!eos_mmap_info->virt_mmapbase)
    {
        printf("mmapbase is null,maybe you have not  mmaped,please check read again\n");
        return -1;
    }
    if( CPU_CCSR_BASE_ADDR == eos_mmap_info->phy_mmapbase )
    {
        if(eos_mmap_info->lenth < (offset))
        {
            printf("cpu offset too long");
	    return -1;
        }
    }
    else
    {
	 if(eos_mmap_info->lenth < (offset*(eos_mmap_info->bits_len/8)))
         {
             printf("cpu offset too long");
             return -1;
         }
    }

    if (NULL == data)
    {
        printf("data buf is NULL!\n");
        return -1;
    }

    pthread_mutex_lock(&eos_mmap_info->lock_mmap);

    if (32 == eos_mmap_info->bits_len)
    {
        if (CPU_CCSR_BASE_ADDR == eos_mmap_info->phy_mmapbase)
        {
            //memcpy(data, eos_mmap_info->virt_mmapbase + offset, len);
            for (i = 0; i < len; i++)
			{
				*(uint32_t *)((uint32_t *)data + i) = *((uint32_t *)((uint32_t *)((uint8_t *)eos_mmap_info->virt_mmapbase + offset) + i));
			}
        }
        else
        {
            //memcpy(data, eos_mmap_info->virt_mmapbase + offset * 4, len * 4);
            for (i = 0; i < len; i++)
			{
				*(uint32_t *)((uint32_t *)data + i) = *((uint32_t *)((uint32_t *)eos_mmap_info->virt_mmapbase + offset + i));
			}
        }
    }
    else if (16 == eos_mmap_info->bits_len)
    {
        //memcpy(data, eos_mmap_info->virt_mmapbase + offset * 2, len * 2);
        for (i = 0; i < len; i++)
		{
			*(uint16_t *)((uint16_t *)data + i) = *((uint16_t *)((uint16_t *)eos_mmap_info->virt_mmapbase + offset + i));
		}
    }
    else
    {
        //memcpy(data, eos_mmap_info->virt_mmapbase + offset, len);
        for (i = 0; i < len; i++)
		{
			*(uint8_t *)((uint8_t *)data + i) = *((uint8_t *)((uint8_t *)eos_mmap_info->virt_mmapbase + offset + i));
		}
    }

    pthread_mutex_unlock(&eos_mmap_info->lock_mmap);

    return 0;

}


int eos_mmap_write(struct eos_mmap_unit *eos_mmap_info, unsigned int offset, void *data, unsigned int len)
{
    int i = 0;

    if (!eos_mmap_info->virt_mmapbase)
    {
        printf("mmapbase is null,maybe you have not  mmaped,please check read again\n");
        return -1;
    }
    if( CPU_CCSR_BASE_ADDR == eos_mmap_info->phy_mmapbase )
    {
        if(eos_mmap_info->lenth < (offset))
        {
            printf("cpu offset too long");
	    return -1;
        }
    }
    else
    {
	 if(eos_mmap_info->lenth < (offset*(eos_mmap_info->bits_len/8)))
         {
             printf("cpu offset too long");
             return -1;
         }
    }
    if (NULL == data)
    {
        printf("data buf is NULL!\n");
        return -1;
    }

    pthread_mutex_lock(&eos_mmap_info->lock_mmap);

    if (32 == eos_mmap_info->bits_len)
    {
        if (CPU_CCSR_BASE_ADDR == eos_mmap_info->phy_mmapbase)
        {
            //memcpy( eos_mmap_info->virt_mmapbase + offset, data, len);
			for (i = 0; i < len; i++)
			{
				*((uint32_t *)((uint32_t *)((uint8_t *)eos_mmap_info->virt_mmapbase + offset) + i)) = *(uint32_t *)((uint32_t *)data + i);
			}
        }
        else
        {
			//memcpy(eos_mmap_info->virt_mmapbase + offset * 4, data, len*4);
			for (i = 0; i < len; i++)
			{
				*((uint32_t *)((uint32_t *)eos_mmap_info->virt_mmapbase + offset + i)) = *(uint32_t *)((uint32_t *)data + i);
        	}
        }

    }
    else if (16 == eos_mmap_info->bits_len)
    {
		//memcpy(eos_mmap_info->virt_mmapbase + offset * 2, data, len * 2);
		for (i = 0; i < len; i++)
		{
			*((uint16_t *)((uint16_t *)eos_mmap_info->virt_mmapbase + offset + i)) = *(uint16_t *)((uint16_t *)data + i);
		}
    }
    else
    {
        //memcpy(eos_mmap_info->virt_mmapbase + offset, data, len);
		for (i = 0; i < len; i++)
		{
			*((uint8_t *)((uint8_t *)eos_mmap_info->virt_mmapbase + offset + i)) = *(uint8_t *)((uint8_t *)data + i);
		}
    }

    pthread_mutex_unlock(&eos_mmap_info->lock_mmap);

    return 0;
}


int eos_mmap_write_data(struct eos_mmap_unit *eos_mmap_info, unsigned int offset, unsigned int data)
{
    int i = 0;

    if (!eos_mmap_info->virt_mmapbase)
    {
        printf("mmapbase is null,maybe you have not  mmaped,please check read again\n");
        return -1;
    }
    if( CPU_CCSR_BASE_ADDR == eos_mmap_info->phy_mmapbase )
    {
        if(eos_mmap_info->lenth < (offset))
        {
            printf("cpu offset too long");
	    return -1;
        }
    }
    else
    {
	 if(eos_mmap_info->lenth < (offset*(eos_mmap_info->bits_len/8)))
         {
             printf("cpu offset too long");
             return -1;
         }
    }
    pthread_mutex_lock(&eos_mmap_info->lock_mmap);

    if (32 == eos_mmap_info->bits_len)
    {
        if (CPU_CCSR_BASE_ADDR == eos_mmap_info->phy_mmapbase)
        {
            //memcpy( eos_mmap_info->virt_mmapbase + offset, data, len);
		*((uint32_t *)((uint32_t *)((uint8_t *)eos_mmap_info->virt_mmapbase + offset))) = data;
        }
        else
        {
			//memcpy(eos_mmap_info->virt_mmapbase + offset * 4, data, len*4);
		*((uint32_t *)((uint32_t *)eos_mmap_info->virt_mmapbase + offset)) = data;

        }

    }
    else if (16 == eos_mmap_info->bits_len)
    {
		//memcpy(eos_mmap_info->virt_mmapbase + offset * 2, data, len * 2);
	*((uint16_t *)((uint16_t *)eos_mmap_info->virt_mmapbase + offset + i)) = data;

    }
    else
    {
        //memcpy(eos_mmap_info->virt_mmapbase + offset, data, len);

	*((uint8_t *)((uint8_t *)eos_mmap_info->virt_mmapbase + offset + i)) = data;

    }

    pthread_mutex_unlock(&eos_mmap_info->lock_mmap);

    return 0;
}
