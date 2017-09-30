#include "eos_lib.h"
#include<sys/mman.h>
extern "C" {
union word_data 
{
	unsigned char data[4];
	unsigned int  u32_data;
};

union short_data
{
	unsigned char data[2];
	unsigned short u16_data;
};

static unsigned int eos_rd_le32(unsigned char *addr)
{
	unsigned int reg = 0;
	union word_data reg_data; 	
	reg_data.u32_data = *((unsigned int *)addr);
	reg = (reg_data.data[3] << 24) | (reg_data.data[2] << 16) | (reg_data.data[1] << 8) | (reg_data.data[0]);
	return reg;
}

static int eos_wr_le32(unsigned char *addr,unsigned int reg32)
{
	unsigned int reg = 0;
	union word_data reg_data; 
	reg_data.u32_data = reg32;
	reg = (reg_data.data[0]) | (reg_data.data[1] << 8) | (reg_data.data[2] << 16) | (reg_data.data[3] << 24);
	(*(unsigned int *)addr) = reg; 
	return 0;
}

static unsigned short eos_rd_le16(unsigned char *addr)
{
	unsigned short reg;
	union short_data reg_data; 	
	reg_data.u16_data = *((unsigned short *)addr);
	reg = (reg_data.data[1] << 8) | (reg_data.data[0]);
	return reg;
}

static int eos_wr_le16(unsigned char *addr,unsigned short reg16)
{
	unsigned short reg = 0;
	union short_data reg_data; 
	reg_data.u16_data = reg16;
	reg = (reg_data.data[0]) | (reg_data.data[1] << 8);
	*((unsigned int *)addr) = reg; 
	return 0;
}

}

int eos_minimpc8308_reset_pcie()
{
	unsigned char *cpu_base;
	unsigned int   reg32 = 0;
	unsigned short reg16 = 0;
	unsigned char  reg8 = 0;
	int fd = 0;
	unsigned int count = 0;
	int i = 0;

	fd = open("/dev/mem", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(-1 == fd)
	{
		printf("open /dev/mem fail\n");
		return -1;
	}
	cpu_base = (unsigned char *)mmap(0, 0x100000, PROT_READ | PROT_WRITE, MAP_SHARED, fd , CPU_CCSR_BASE_ADDR);
	if(MAP_FAILED == cpu_base)
	{
		printf("mmap fail\n");
		return -1;
	}

	reg32 = *((unsigned int *)(cpu_base + 0xe3000));
	reg32 &= ~0x00008800;
	*((unsigned int *)(cpu_base + 0xe3000)) = reg32;

	reg32 = *((unsigned int *)(cpu_base + 0xe3008));
	reg32 &= ~0x00800000;
	*((unsigned int *)(cpu_base + 0xe3008)) = reg32;

	reg32 = *((unsigned int *)(cpu_base + 0xe3004));
	reg32 |= 0x00000040;
	*((unsigned int *)(cpu_base + 0xe3004)) = reg32;

	reg32 = *((unsigned int *)(cpu_base + 0xe3008));
	reg32 &= ~0x00001c1c;
	reg32 |= 0x00001010;
	*((unsigned int *)(cpu_base + 0xe3008)) = reg32;

	reg32 = 0x01010000;
	*((unsigned int *)(cpu_base + 0xe300c)) = reg32;

	reg32 =  (0 << 28) | 0x00000101 ;
	*((unsigned int *)(cpu_base + 0xe3010)) = reg32;

	reg32 = *((unsigned int *)(cpu_base + 0xe3020));
	reg32 |= 0x80000000;
	*((unsigned int *)(cpu_base + 0xe3020)) = reg32;
 	
	reg32 = *((unsigned int *)(cpu_base + 0xe3020));
	

	while((reg32 & 0x40000000) != 0x40000000)
	{
		reg32 = *((unsigned int *)(cpu_base + 0xe3020));
		usleep(1);
		i++;
		if(i>100)
		{
			printf("reset pcie serdes error\n");
			return -1;		
		}
	}

	*((unsigned int *)(cpu_base + 0x140)) = 0;
	usleep(10);
	*((unsigned int *)(cpu_base + 0x140)) = 0xe0008000;
	usleep(2000);
	sleep(3);
	
	/*enable pex csb bridge inbound & outbound transaction*/	
	eos_wr_le32(cpu_base + 0x9808,eos_rd_le32(cpu_base + 0x9808) | 0x1 | 0x2);

	/* Enable bridge outbound */
	eos_wr_le32(cpu_base + 0x9840, 0x1 | 0x2 | 0x4 | 0x8);

	/*config outbound*/

	/*config outbound0*/
	eos_wr_le32(cpu_base + 0x9ca0,0x1000001);
	eos_wr_le32(cpu_base + 0x9ca4,0xb0000000);
	eos_wr_le32(cpu_base + 0x9ca8,0x0);
	eos_wr_le32(cpu_base + 0x9cac,0x0);
	
	/*config outbound1*/
	eos_wr_le32(cpu_base + 0x9cb0,0x20000005);
	eos_wr_le32(cpu_base + 0x9cb4,0x80000000);
	eos_wr_le32(cpu_base + 0x9cb8,0x80000000);
	eos_wr_le32(cpu_base + 0x9cbc,0x0);

	/*config outbound2*/
	eos_wr_le32(cpu_base + 0x9cc0,0x800003);
	eos_wr_le32(cpu_base + 0x9cc4,0xb1000000);
	eos_wr_le32(cpu_base + 0x9cc8,0x0);
	eos_wr_le32(cpu_base + 0x9ccc,0x0);

	/*config outbound3*/
	eos_wr_le32(cpu_base + 0x9cd0,0x0);
	eos_wr_le32(cpu_base + 0x9cd4,0x0);
	eos_wr_le32(cpu_base + 0x9cd8,0x0);
	eos_wr_le32(cpu_base + 0x9cdc,0x0);

	/*enable bridge inbound*/
	eos_wr_le32(cpu_base + 0x98e0, 0x1);
	/*config inbound*/

	/*config inbound0*/
	eos_wr_le32(cpu_base + 0x9e60,0xffff00d);
	eos_wr_le32(cpu_base + 0x9e64,0x0);
	eos_wr_le32(cpu_base + 0x9e68,0x0);
	eos_wr_le32(cpu_base + 0x9e6c,0x0);

	/*config inbound2*/
	eos_wr_le32(cpu_base + 0x9e70,0xffff00d);
	eos_wr_le32(cpu_base + 0x9e74,0x10000000);
	eos_wr_le32(cpu_base + 0x9e78,0x10000000);
	eos_wr_le32(cpu_base + 0x9e7c,0x0);

	/*config inbound3*/
	eos_wr_le32(cpu_base + 0x9e80,0xffff00d);
	eos_wr_le32(cpu_base + 0x9e84,0x20000000);
	eos_wr_le32(cpu_base + 0x9e88,0x20000000);
	eos_wr_le32(cpu_base + 0x9e8c,0x0);

	/*config inbound4*/
	eos_wr_le32(cpu_base + 0x9e90,0xffff00d);
	eos_wr_le32(cpu_base + 0x9e94,0x30000000);
	eos_wr_le32(cpu_base + 0x9e98,0x30000000);
	eos_wr_le32(cpu_base + 0x9e9c,0x0);

	/*config interrupt*/
	eos_wr_le32(cpu_base + 0x9bec,( eos_rd_le32(cpu_base + 0x9bec) | 0x1E0 ) &(~0x2));
	//eos_wr_le32(cpu_base + 0x9bec,( eos_rd_le32(cpu_base + 0x9bec) | 0x1E0 ));
	/*config clock*/
	eos_wr_le32(cpu_base + 0x9440, 0x6);

	/*do type 1 bridge configuration*/
	*((unsigned char *)(cpu_base + 0x9018)) = 0;
	*((unsigned char *)(cpu_base + 0x9019)) = 1;
	*((unsigned char *)(cpu_base + 0x901a)) = 255;

	/*write to command register*/
	eos_wr_le32(cpu_base + 0x9004,eos_rd_le32(cpu_base + 0x9004) | 0x4 | 0x2 | 0x1 | 0x100 | 0x40);
	/*clear non-reserved bits in status register*/	
	eos_wr_le16(cpu_base + 0x9006,0xffff);
	*((unsigned char *)(cpu_base + 0x900d)) = 0x80;
	*((unsigned char *)(cpu_base + 0x900c)) = 0x08;
	
	
	reg32 = eos_rd_le32(cpu_base + 0x9404);
	//count++;
	printf("reg = 0x%x\n",reg32);
	if(reg32 != 0x16)
	{
		//printf("reset time is 0x%x\n",count);
		printf("detect pcie link status error\n");
		printf("reg = 0x%x\n",reg32);
		return -1;
	}
	sleep(1);	

	munmap(cpu_base, 0x100000);
	close(fd);
	return 0;

}


int eos_mpc8308_reset_pcie()
{
	return eos_minimpc8308_reset_pcie();
}



