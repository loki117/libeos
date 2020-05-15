/****************************************************************
* �ļ���  ��eos_cpu_dma_proxy.cpp
* ������  ��liuk
* �������ڣ�20190724
* �汾��  ��v1.0
* �ļ�������DMA PROXY��������ӿں���
* ��Ȩ˵����Copyright (c) 2000-2020   ���ͨ�ſƼ��ɷ����޹�˾
* ��    ������
* �޸���־��
******************************************************************************/
#include "eos_lib.h"

#define EOS_DMA_PROXY_MAX_LEN (1 * 1024 * 1024)

typedef enum
{
    PROXY_NO_ERROR = 0, 
    PROXY_BUSY = 1, 
    PROXY_TIMEOUT = 2, 
    PROXY_ERROR = 3 
} proxy_status;

struct dma_proxy_channel_interface 
{
	unsigned char buffer[EOS_DMA_PROXY_MAX_LEN];
	proxy_status status;
	unsigned int length;//��Ч�ֽ���
};

#define EOS_DMA_PROXY_MAX_BLOCK 10

struct eos_dma_proxy_sg_arg
{
	unsigned long long addr_size[EOS_DMA_PROXY_MAX_BLOCK][2];//[0]���������ַ [1]����Ҫ��ȡ����,��λ�ֽ�
	unsigned int num;//�����������Ч����
	unsigned int dir;//0--read 1--write
};

struct eos_dma_proxy_memcpy_arg
{
	unsigned long long addr;//����dma���˵�������ʼ��ַ
	unsigned long long size;//����dma���˵������ֽ���
	unsigned int dir;//0--read 1--write
};

/*************************************************************************
* ������  ��eos_open_dma_proxy
* ��д��  ��liuk
* �������ڣ�20190724
* �������ܣ�dma proxy��
* ���������dmaproxyname: �豸����(/dev/dma_proxy_0 /dev/dma_proxy_1 /dev/dma_proxy_2 ��)
* ���������
* �� �� ֵ���ɹ�����0, ʧ�ܷ���-1   
* ���ù�ϵ��
* ��    ����
*************************************************************************/
int eos_open_dma_proxy(const char *dmaproxyname)
{
	int fd = 0;
	fd = open(dmaproxyname, O_RDWR);
	if(fd == -1)
	{
		printf("open dma proxy(%s) fail!%s\n", dmaproxyname, strerror(errno));
		return -1;
	}
	return fd;
}

/*************************************************************************
* ������  ��eos_close_dma_proxy
* ��д��  ��liuk
* �������ڣ�20190724
* �������ܣ�dma proxy�ر�
* ���������fd: eos_open_dma_proxy �������ص��ļ�������
* ���������
* �� �� ֵ���ɹ�����0, ʧ�ܷ���-1
* ���ù�ϵ������ eos_open_dma_proxy ������øú����ر��ļ�������
* ��    ����
*************************************************************************/
int eos_close_dma_proxy(int fd)
{
    int ret;
	ret = close(fd);
    if (ret < 0)
    {
        printf("close fd(%d) error!%s", strerror(errno));
    }
	return ret;
}

/*************************************************************************
* ������  ��eos_get_dma_proxy_addr
* ��д��  ��liuk
* �������ڣ�20190724
* �������ܣ���ȡĳdam proxy��buffer�����ַ, ͨ���õ�ַ�ɷ��ʵ�dma��ȡ����������
* ���������fd: eos_open_dma_proxy �������ص��ļ�������
* ���������addr: dam proxy��buffer�����ַ
* �� �� ֵ���ɹ�����0, ʧ�ܷ���-1
* ���ù�ϵ��
* ��    ����
*************************************************************************/
int eos_get_dma_proxy_addr(unsigned int fd, dma_addr *addr)
{
    struct dma_proxy_channel_interface *proxy_interface_p;
    proxy_interface_p = (struct dma_proxy_channel_interface *)mmap(NULL, 
                                                            sizeof(struct dma_proxy_channel_interface),
                                                            PROT_READ | PROT_WRITE, 
                                                            MAP_SHARED, 
                                                            fd, 
                                                            0);
    if (proxy_interface_p == MAP_FAILED) {
		printf("eos_get_dma_proxy_addr mmap failed!!!%s\n", strerror(errno));
		return -1;
	}
    
	//printf("mmap_addr = 0x%lx\n", proxy_interface_p);
	*addr = (dma_addr)(&proxy_interface_p->buffer[0]);

	return 0;
}

/*************************************************************************
* ������  ��eos_start_dma_proxy_sg_transfer
* ��д��  ��liuk
* �������ڣ�20190724
* �������ܣ�ʹ��sglist��ʽ����dma����
* ���������fd: eos_open_dma_proxy �������ص��ļ�������
*           unsigned long long (*p)[2]: Ҫ��dma�����ĵ�ַ�ͳ��ȼ���, [0]���������ַ [1]����Ҫ��ȡ����,��λ�ֽ�
*           unsigned int num: ��dma�����ĵ�ַ�ͳ��ȼ��ϵ���Ч����
*           unsigned int direction: dma��������(0-read 1-write)
* ���������
* �� �� ֵ���ɹ�����0, ʧ�ܷ���-1
* ���ù�ϵ��
* ��    ����
*************************************************************************/
int eos_start_dma_proxy_sg_transfer(unsigned int fd, unsigned long long (*p)[2], unsigned int num, unsigned int direction)
{
	int i;
	int ret = -1;
	unsigned int all_length = 0;
	struct eos_dma_proxy_sg_arg dma_sg_arg;
    
	if(num > EOS_DMA_PROXY_MAX_BLOCK)
	{
		printf("arg num too large: %d\n", num);
		return -1;
	}
    for(i = 0; i < num; i++)
	{
		all_length += p[i][1];
	}
	if(all_length > EOS_DMA_PROXY_MAX_LEN)
	{
		printf("size too large: %dBytes\n", all_length);
		return -1;
	}
    if ((direction != 0) && (direction != 1))
    {
		printf("direction wrong: %d\n", direction);
		return -1;
	}
    
	for(i = 0; i < num; i++)
	{
		dma_sg_arg.addr_size[i][0] = p[i][0];
		dma_sg_arg.addr_size[i][1] = p[i][1];
	}
	dma_sg_arg.num = num;
    dma_sg_arg.dir = direction;

    struct dma_proxy_channel_interface *proxy_interface_p;
    proxy_interface_p = (struct dma_proxy_channel_interface *)mmap(NULL, 
																sizeof(struct dma_proxy_channel_interface),
																PROT_READ | PROT_WRITE, 
																MAP_SHARED, 
																fd, 
																0);
	if (proxy_interface_p == MAP_FAILED) {
		printf("mmap failed!!!%s\n", strerror(errno));
		return -1;
	}
    
    ioctl(fd, 0, &dma_sg_arg);
    if (proxy_interface_p->status != PROXY_NO_ERROR)
    {
        printf("eos_start_dma_proxy_sg_transfer proxy transfer error\n");
        ret = -1;
        goto end;
    }

end:
    munmap(proxy_interface_p, sizeof(struct dma_proxy_channel_interface));
	return ret;
}

/*************************************************************************
* ������  ��eos_start_dma_proxy_memcpy_transfer
* ��д��  ��liuk
* �������ڣ�20190724
* �������ܣ�ʹ��memcpy��ʽ����dma����, ls208x/ls1046ʹ�ø÷�ʽ
* ���������fd: eos_open_dma_proxy �������ص��ļ�������
*           unsigned long long addr: Ҫ��dma�����������ַ
*           unsigned long long size: Ҫ��dma�����ĳ���,��λ�ֽ�
*           unsigned int direction: dma��������(0-read 1-write)
* ���������
* �� �� ֵ���ɹ�����0, ʧ�ܷ���-1
* ���ù�ϵ��
* ��    ����
*************************************************************************/
int eos_start_dma_proxy_memcpy_transfer(unsigned int fd, unsigned long long addr, unsigned long long size, unsigned int direction)
{
    int ret = -1;
    struct eos_dma_proxy_memcpy_arg dma_memcpy_arg;
    
    if(size > EOS_DMA_PROXY_MAX_LEN)
	{
		printf("size too large: %dBytes\n", size);
		return -1;
	}
    if ((direction != 0) && (direction != 1))
    {
		printf("direction wrong: %d\n", direction);
		return -1;
	}
    
    dma_memcpy_arg.addr = addr;
    dma_memcpy_arg.size = size;
    dma_memcpy_arg.dir = direction;
    
    struct dma_proxy_channel_interface *proxy_interface_p;
    proxy_interface_p = (struct dma_proxy_channel_interface *)mmap(NULL, 
																sizeof(struct dma_proxy_channel_interface),
																PROT_READ | PROT_WRITE, 
																MAP_SHARED, 
																fd, 
																0);
	if (proxy_interface_p == MAP_FAILED) {
		printf("mmap failed!!!%s\n", strerror(errno));
		return -1;
	}
    
    ioctl(fd, 0, &dma_memcpy_arg);
    if (proxy_interface_p->status != PROXY_NO_ERROR)
    {
        printf("eos_start_dma_proxy_memcpy_transfer proxy transfer error\n");
        ret = -1;
        goto end;
    }

end:
    munmap(proxy_interface_p, sizeof(struct dma_proxy_channel_interface));
	return ret;
}

