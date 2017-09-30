#include "eos_lib.h"

#if defined(CPU_NAME_P204X)
int eos_get_gid(unsigned char *value)
{

}
#endif

#if defined(CPU_NAME_P2020)
int eos_get_gid(unsigned char *value)
{

}
#endif

#if defined(CPU_NAME_P1020)
int eos_get_gid(unsigned char *value)
{

}
#endif
 
#if defined(CPU_NAME_MPC8308)  || defined(CPU_NAME_MINIMPC8308)
int eos_get_gid(unsigned char *value)
{
	struct eos_mmap_unit *cs2_mmap;
	unsigned char id = 0;
    	cs2_mmap = eos_mmap_init(LOCALBUS_CS2_LEN, LOCALBUS_CS2, LOCALBUS_CS2_BITS_LEN, NULL); 
    	if (NULL == cs2_mmap)
    	{
        	printf("cs2_mmap_init  failed\n");
        	return -1;   
     	}
     	else
     	{
        	printf("cs2_mmap_init  ok\n");
     	}
	id = *(unsigned char *)(cs2_mmap->virt_mmapbase);
	*value = id;
	eos_mmap_exit(&cs2_mmap);
	return 0;
}
#endif

