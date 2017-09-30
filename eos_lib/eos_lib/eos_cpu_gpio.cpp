/****************************************************************
* 文件名  ：eos_cpu_gpio.cpp
* 负责人  ：lmyan
* 创建日期：20150403
* 版本号  ： v1.1
* 文件描述：GPIO初始化函数
* 版权说明：Copyright (c) 2000-2020   烽火通信科技股份有限公司
* 其    它：无
* 修改日志：20160215 by  lmyan, 增加原文件注释，初始化函数增加返回值
******************************************************************************/

#include "eos_lib.h"

#if defined(CPU_NAME_P204X) || defined(CPU_NAME_P2020) || defined(CPU_NAME_P1020) || defined(CPU_NAME_MPC8308) || \
defined(CPU_NAME_MINIMPC8308)
int eos_set_gpio_direction(unsigned int gpio_num, unsigned int direction)
{
	int ret = 0;
   	unsigned int cpu_reg_value = 0;
	    /*0 means input; 1 means output;*/
     	if ((1 != direction) && (0 != direction))
    	{
        	printf("direction is NULL!\n");
        	return -1;
    	}
	ret = eos_cpu_reg_read(CPU_CCSR_GPIODIR_OFFSET, &cpu_reg_value, 1);
	if (-1 == ret)
	{
	        printf("eos_cpu_reg_read is error!\n");
	        return -1;
 	}
	if (direction == 1)
	{
		cpu_reg_value |= (1 << (31 - gpio_num));
	}
	else
	{
		cpu_reg_value &= ~(1 << (31 - gpio_num));
	}
	ret = eos_cpu_reg_write(CPU_CCSR_GPIODIR_OFFSET, &cpu_reg_value, 1);
	if (-1 == ret)
	{
	       printf("eos_cpu_reg_write is error!\n");
	       return -1;
	}
	return 0;
}


int eos_get_gpio_direction(unsigned int gpio_num, unsigned int *direction)
{
	int ret = 0;
   	unsigned int cpu_reg_value = 0;
	ret = eos_cpu_reg_read(CPU_CCSR_GPIODIR_OFFSET, &cpu_reg_value, 1);
	if (-1 == ret)
    	{
        	printf("eos_cpu_reg_read is error!\n");
        	return -1;
    	}
	if ((cpu_reg_value >> (31 - gpio_num)) & 0x1)
	{
		*direction = 1;
	}
	else
	{
		*direction = 0;
	}

    /*0 means input; 1 means output;*/
    	if ((1 != *direction) && (0 != *direction))
    	{
        	printf("direction is NULL!\n");
        	return -1;
    	}
	return 0;
}

int eos_set_gpio_status(unsigned int gpio_num, unsigned int status)
{
	int ret = 0;
    	unsigned int cpu_reg_value = 0;

    	/*0 means input; 1 means output;*/
    	if ((1 != status) && (0 != status))
    	{
        	printf("status is NULL!\n");
        	return -1;
    	}
	ret = eos_cpu_reg_read(CPU_CCSR_GPIODAT_OFFSET, &cpu_reg_value, 1);
	if (-1 == ret)
    	{
        	printf("eos_cpu_reg_read is error!\n");
        	return -1;
    	}
	if (status == 1)
	{
		cpu_reg_value |= (1 << (31 - gpio_num));
	}
	else
	{
		cpu_reg_value &= ~(1 << (31 - gpio_num));
	}
	ret = eos_cpu_reg_write(CPU_CCSR_GPIODAT_OFFSET, &cpu_reg_value, 1);
	if (-1 == ret)
    	{
        	printf("eos_cpu_reg_write is error!\n");
        	return -1;
    	}
	return 0;
}

int eos_get_gpio_status(unsigned int gpio_num, unsigned int *status)
{
    	int ret = 0;
	unsigned int cpu_reg_value = 0;
	ret = eos_cpu_reg_read(CPU_CCSR_GPIODAT_OFFSET, &cpu_reg_value, 1);
	if (-1 == ret)
    	{
        	printf("eos_cpu_reg_read is error!\n");
        	return -1;
    	}
	if ((cpu_reg_value >> (31 - gpio_num)) & 0x1)
	{
		*status = 1;
	}
	else
	{
		*status = 0;
	}
	return 0;
}


#endif

#if defined(CPU_NAME_MPC8548)

int eos_set_gpio_direction(unsigned int gpio_num, unsigned int direction)
{
    int ret = 0;
    unsigned int cpu_reg_value = 0;
    unsigned int read_temp_value = 0;
    unsigned int write_temp_value = 0;

    /*0 means input; 1 means output;*/
    if ((1 != direction) && (0 != direction))
    {
        printf("direction is NULL!\n");
        return -1;
    }
    /*8548的gpio为0-15, 24 - 31*/
    if (((gpio_num > 15) && (gpio_num < 24)) || (gpio_num > 31))
    {
        printf("gpio_num is invalid, valid is 0-15, 24 - 31!\n");
        return -1;
    }

    if (0 == direction)
    {
        if (gpio_num < 8)
        {
            read_temp_value = 0x01000000;
        }
        else if (gpio_num < 16)
        {
            read_temp_value = 0x00010000;
        }
        else
        {
            read_temp_value = 0x00000400;
        }
    }
    else
    {
        if (gpio_num < 8)
        {
            read_temp_value = 0x04000000;
        }
        else if (gpio_num < 16)
        {
            read_temp_value = 0x00040000;
        }
        else
        {
            read_temp_value = 0x00000400;
        }
    }

    ret = eos_cpu_reg_read(CPU_CCSR_GPIOCR_OFFSET, &cpu_reg_value, 1);
    if (-1 == ret)
    {
        printf("eos_mmap_read is error!\n");
        return -1;
    }

    /*设置gpio功能管脚复用为gpio功能*/
    cpu_reg_value = cpu_reg_value | read_temp_value;

    ret = eos_cpu_reg_write(CPU_CCSR_GPIOCR_OFFSET, &cpu_reg_value, 1);
    if (-1 == ret)
    {
        printf("eos_mmap_write is error!\n");
        return -1;
    }
	return 0;
}

int eos_get_gpio_direction(unsigned int gpio_num, unsigned int *direction)
{
    int ret = 0;
    unsigned int cpu_reg_value = 0;
    unsigned int read_temp_value = 0;
    unsigned int write_temp_value = 0;
    /*8548的gpio为0-15, 24 - 31*/
    if (((gpio_num > 15) && (gpio_num < 24)) || (gpio_num > 31))
    {
        printf("gpio_num is invalid, valid is 0-15, 24 - 31!\n");
        return -1;
    }

    if (0 == direction)
    {
        if (gpio_num < 8)
        {
            read_temp_value = 0x01000000;
        }
        else if (gpio_num < 16)
        {
            read_temp_value = 0x00010000;
        }
        else
        {
            read_temp_value = 0x00000400;
        }
    }
    else
    {
        if (gpio_num < 8)
        {
            read_temp_value = 0x04000000;
        }
        else if (gpio_num < 16)
        {
            read_temp_value = 0x00040000;
        }
        else
        {
            read_temp_value = 0x00000400;
        }
    }

    ret = eos_cpu_reg_read(CPU_CCSR_GPIOCR_OFFSET, &cpu_reg_value, 1);
    if (-1 == ret)
    {
        printf("eos_mmap_read is error!\n");
        return -1;
    }

    /*设置gpio功能管脚复用为gpio功能*/
    cpu_reg_value = cpu_reg_value & read_temp_value;

    if (0 == cpu_reg_value)
    {
        printf("eos_mmap_read is error!\n");
        return -1;
    }
    *direction = 0x1;
    return 0;
}


int eos_set_gpio_status(unsigned int gpio_num, unsigned int status)
{
	int ret = 0;
    	unsigned int cpu_reg_value = 0;
	unsigned int write_temp_value = 0;
    	unsigned int cpu_reg_offset= 0;
    /*0 means input; 1 means output;*/
    	if ((1 != status) && (0 != status))
    	{
        	printf("status is NULL!\n");
        	return -1;
    	}

    /*8548的gpio为0-15, 24 - 31*/
    	if (((gpio_num > 15) && (gpio_num < 24)) || (gpio_num > 31))
    	{
        	printf("gpio_num is invalid, valid is 0-15, 24 - 31!\n");
        	return -1;
    	}

    	write_temp_value = status << (32 - gpio_num - 1);
    	cpu_reg_offset = CPU_CCSR_GPOUTDR_OFFSET;
	ret = eos_cpu_reg_read(cpu_reg_offset, &cpu_reg_value, 1);
    	if (-1 == ret)
    	{
        	printf("eos_mmap_read is error!\n");
        	return -1;
    	}

    /*设置gpio状态*/
    	cpu_reg_value = cpu_reg_value | write_temp_value;

    	ret = eos_cpu_reg_write(cpu_reg_offset, &cpu_reg_value, 1);
    	if (-1 == ret)
    	{
        	printf("eos_mmap_write is error!\n");
        	return -1;
    	}
	return 0;
}


int eos_get_gpio_status(unsigned int gpio_num, unsigned int *status)
{
	int ret = 0;
	unsigned int cpu_reg_value = 0;
    	unsigned int write_temp_value = 0;
    	unsigned int cpu_reg_offset= 0;
    	/*8548的gpio为0-15, 24 - 31*/
    	if (((gpio_num > 15) && (gpio_num < 24)) || (gpio_num > 31))
    	{
        	printf("gpio_num is invalid, valid is 0-15, 24 - 31!\n");
        	return -1;
    	}

    	write_temp_value = 1 << (32 - gpio_num -1);
    	cpu_reg_offset = CPU_CCSR_GPINDR_OFFSET;
    	ret = eos_cpu_reg_read(cpu_reg_offset, &cpu_reg_value, 1);
    	if (-1 == ret)
    	{
        	printf("eos_mmap_read is error!\n");
        	return -1;
    	}

    	cpu_reg_value = cpu_reg_value & write_temp_value;
	/*0 means input; 1 means output;*/
    	if(0 == cpu_reg_value)
    	{
        	*status = 0;
    	}
    	else
    	{
        	*status = 1;
    	}
    	return 0;
}
#endif

#if defined(CPU_NAME_LS1021)
struct eos_mmap_unit *gpio_mmap ;


int eos_gpio_mmap_init()
{
	gpio_mmap = eos_mmap_init(GPIO_MEMLEN, GPIO_BASEADDR, GPIO_BITLEN, NULL);
	if (NULL == gpio_mmap)
	{
		printf("eos_gpio_mmap_init faild !\n");
		return -1;
	}else{
		return 0;
	}
}
int eos_gpio_mmap_exit()
{
    int ret = 0;
    ret = eos_mmap_exit(&gpio_mmap);
    if (0 != ret)
    {
        printf("eos_gpio_mmap_exit  failed\n");
        return -1;
     }
     else
     {
        printf("eos_gpio_mmap_exit  ok\n");
        return 0;
     }
}


int eos_set_gpio_direction(unsigned int gpio_num, unsigned int direction)
{
	unsigned int data ,cpu_reg_value, cpu_reg_value1;
	unsigned addr = 0;
	int ret = -1;

	if ((1 != direction) && (0 != direction))
    {
        printf("direction is NULL!\n");
        return -1;
    }
	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}

	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x10000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x20000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x30000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,addr/4 ,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	ret = eos_mmap_read(gpio_mmap,(addr + 0x18)/4,(void*)&cpu_reg_value1 ,1 );
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	cpu_reg_value = htonl(cpu_reg_value);
	cpu_reg_value1 = htonl(cpu_reg_value1);


	if(0 == direction)
	{
		cpu_reg_value &= ~data;
		cpu_reg_value1 |= data;
 	}else
 	{
		cpu_reg_value |= data;
		cpu_reg_value1 &= ~data;
	}

	cpu_reg_value = ntohl(cpu_reg_value);
	cpu_reg_value1 = ntohl(cpu_reg_value1);


	ret = eos_mmap_write(gpio_mmap,addr/4  	 ,(void*)&cpu_reg_value , 1);
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	ret = eos_mmap_write(gpio_mmap,(addr + 0x18)/4 ,(void*)&cpu_reg_value1 , 1);
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}



}
int eos_get_gpio_direction(unsigned int gpio_num, unsigned int *direction)
{
	unsigned int data ,cpu_reg_value, cpu_reg_value1;
	unsigned addr = 0;
	int ret = -1;

	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}
	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x10000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x20000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x30000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,addr/4 ,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	cpu_reg_value = htonl(cpu_reg_value);


	if(data & cpu_reg_value)
	{
		*direction = 1;
	}else
	{
		*direction = 0;
	}

	return 0;
}
int eos_set_gpio_status(unsigned int gpio_num, unsigned int status)
{
	unsigned int data ,cpu_reg_value;
	unsigned addr = 0;
	int ret = -1;

	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}
	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x10000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x20000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x30000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,(addr    + 0x8)/4,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_set_gpio_status failed\n");
		return -1;
	}

	cpu_reg_value = htonl(cpu_reg_value);


	if(status)
	{
		cpu_reg_value |= data;
	}else
	{
		cpu_reg_value &= ~data;
	}

	cpu_reg_value = ntohl(cpu_reg_value);

	ret = eos_mmap_write(gpio_mmap,(addr + 0x8)/4,(void*)&cpu_reg_value , 1);
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	return 0;
}
int eos_get_gpio_status(unsigned int gpio_num, unsigned int *status)
{
	unsigned int data ,cpu_reg_value;
	unsigned addr = 0;
	int ret = -1;

	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}
	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x10000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x20000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x30000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,(addr    + 0x8)/4 ,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_set_gpio_status failed\n");
		return -1;
	}

	cpu_reg_value = htonl(cpu_reg_value);

	if(cpu_reg_value & data)
	{
		*status = 1;
	}else
	{
		*status = 0;
	}

	return 0;
}


#endif

#if defined(CPU_NAME_T104X)
struct eos_mmap_unit *gpio_mmap ;


int eos_gpio_mmap_init()
{
	gpio_mmap = eos_mmap_init(GPIO_MEMLEN, GPIO_BASEADDR, GPIO_BITLEN, NULL);
	if (NULL == gpio_mmap)
	{
		printf("eos_gpio_mmap_init faild !\n");
		return -1;
	}else{
		return 0;
	}
}
int eos_gpio_mmap_exit()
{
    int ret = 0;
    ret = eos_mmap_exit(&gpio_mmap);
    if (0 != ret)
    {
        printf("eos_gpio_mmap_exit  failed\n");
        return -1;
     }
     else
     {
        printf("eos_gpio_mmap_exit  ok\n");
        return 0;
     }
}


int eos_set_gpio_direction(unsigned int gpio_num, unsigned int direction)
{
	unsigned int data ,cpu_reg_value, cpu_reg_value1;
	unsigned addr = 0;
	int ret = -1;

	if ((1 != direction) && (0 != direction))
    {
        printf("direction is NULL!\n");
        return -1;
    }
	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}

	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x1000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x2000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x3000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,addr/4 ,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	ret = eos_mmap_read(gpio_mmap,(addr + 0x18)/4,(void*)&cpu_reg_value1 ,1 );
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	//cpu_reg_value = htonl(cpu_reg_value);
	//cpu_reg_value1 = htonl(cpu_reg_value1);


	if(0 == direction)
	{
		cpu_reg_value &= ~data;
		cpu_reg_value1 |= data;
 	}else
 	{
		cpu_reg_value |= data;
		cpu_reg_value1 &= ~data;
	}

	//cpu_reg_value = ntohl(cpu_reg_value);
	//cpu_reg_value1 = ntohl(cpu_reg_value1);


	ret = eos_mmap_write(gpio_mmap,addr/4  	 ,(void*)&cpu_reg_value , 1);
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	ret = eos_mmap_write(gpio_mmap,(addr + 0x18)/4 ,(void*)&cpu_reg_value1 , 1);
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}



}
int eos_get_gpio_direction(unsigned int gpio_num, unsigned int *direction)
{
	unsigned int data ,cpu_reg_value, cpu_reg_value1;
	unsigned addr = 0;
	int ret = -1;

	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}
	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x1000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x2000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x3000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,addr/4 ,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	//cpu_reg_value = htonl(cpu_reg_value);


	if(data & cpu_reg_value)
	{
		*direction = 1;
	}else
	{
		*direction = 0;
	}

	return 0;
}
int eos_set_gpio_status(unsigned int gpio_num, unsigned int status)
{
	unsigned int data ,cpu_reg_value;
	unsigned addr = 0;
	int ret = -1;

	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}
	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x1000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x2000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x3000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,(addr    + 0x8)/4,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_set_gpio_status failed\n");
		return -1;
	}

	//cpu_reg_value = htonl(cpu_reg_value);


	if(status)
	{
		cpu_reg_value |= data;
	}else
	{
		cpu_reg_value &= ~data;
	}

	//cpu_reg_value = ntohl(cpu_reg_value);

	ret = eos_mmap_write(gpio_mmap,(addr + 0x8)/4,(void*)&cpu_reg_value , 1);
	if(ret < 0)
	{
		printf("eos_gpio_set_direction failed\n");
		return -1;
	}

	return 0;
}
int eos_get_gpio_status(unsigned int gpio_num, unsigned int *status)
{
	unsigned int data ,cpu_reg_value;
	unsigned addr = 0;
	int ret = -1;

	if(gpio_num < 0 || gpio_num > 127)
	{
		printf("gpio_num is invalid !\n");
        return -1;
	}
	if(gpio_num < 32)
	{
		data = 1 << (31 - gpio_num);
		addr = 0;
	}else if(gpio_num > 31 && gpio_num < 64)
	{
		data = 1 << (63 - gpio_num);
		addr = 0x1000;
 	}else if(gpio_num > 63 && gpio_num < 96)
	{
		data = 1 << (95 - gpio_num);
		addr = 0x2000;
	}else if(gpio_num > 95 && gpio_num < 128)
	{
		data = 1 << (127 - gpio_num);
		addr = 0x3000;
	}

	//data = htonl(data);
	if(gpio_mmap == NULL)
	{
		eos_gpio_mmap_init();
	}

	ret = eos_mmap_read(gpio_mmap,(addr    + 0x8)/4 ,(void*)&cpu_reg_value ,1 );
	if(ret < 0)
	{
		printf("eos_set_gpio_status failed\n");
		return -1;
	}

	//cpu_reg_value = htonl(cpu_reg_value);

	if(cpu_reg_value & data)
	{
		*status = 1;
	}else
	{
		*status = 0;
	}

	return 0;
}




#endif

