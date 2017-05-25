#define NFCONF *((volatile unsigned long *)0x70200000)
#define NFCONT *((unsigned volatile long *)0x70200004)
#define NFSTAT *((unsigned volatile char *)0x70200028)
#define NFCMMD *((unsigned volatile char *)0x70200008)
#define NFADDR *((unsigned volatile char *)0x7020000C)
#define NFDATA *((unsigned volatile char *)0x70200010)

void select_chip()
{
	NFCONT &= ~(1 << 1);
}

void deselect_chip()
{
	NFCONT |= (1 << 1);
}

void clear_RnB()
{
	NFSTAT |= (1 << 4);
}

void nand_cmd(unsigned char cmd)
{
	NFCMMD = cmd;
}

void wait_RnB()
{
	while(!(NFSTAT & 0x1));
}

void nand_addr(unsigned char addr)
{
	NFADDR = addr;
}

void nand_reset(void)
{
    /* ѡ�� */
    select_chip();
    
    /* ���RnB */
    clear_RnB();
    
    /* ������λ�ź� */
    nand_cmd(0xff);
    
    /* �ȴ����� */
    wait_RnB();
    
    /* ȡ��ѡ�� */
    deselect_chip();
}

void nand_init()
{
#define TACLS  1
#define TWRPH0 2
#define TWRPH1 1

	NFCONF &= ~((7<<12)|(7<<8)|(7<<4));
    NFCONF |= (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
    
    /* ʹ�� nandflash controller*/
    NFCONT = 1 | (1<<1);
    
    /* ��λ */
    nand_reset();
}

void NF_page_read(unsigned long addr, unsigned char *buff)
{
	int i;
	
	/*ѡ��оƬ*/
	select_chip();
	
	/*���RnB*/
	clear_RnB();
	
	/*��������0x00*/
	nand_cmd(0x00);
	
	/*�����е�ַ*/
	nand_addr(0x00);   /*��ȡ��ҳʱ�е�ַΪ0*/
	nand_addr(0x00);
	
	/*�����е�ַ*/
	nand_addr(addr & 0xff);   /*�е�ַ���ڱ�ʾҳ�ı�ţ������η��ͣ�ÿ�η���8λ*/
	nand_addr((addr >> 8) & 0xff);
	nand_addr((addr >> 16) & 0xff);
	
	/*��������0x30*/
	nand_cmd(0x30);
	
	/*�ȴ�����*/
	wait_RnB();
	
	/*������*/
	for(i = 0; i < 1024 * 4; i++)  /*OK6410ʹ�õ�NandflashоƬ��ҳ��СΪ4KB*/
	{
		buff[i] = NFDATA;
	}
	
	/*ȡ��Ƭѡ*/
	deselect_chip();
}

void nand_to_ram(unsigned long start_addr, unsigned char *sdram_addr, int size)
{
	int i;
	
	/*����ǰ4ҳ��16KB�Ĵ��뵽�ڴ�*/
	for(i = 0; i < 4; i++, sdram_addr += 4096)
	{
		NF_page_read(i, sdram_addr);
	}
	
	/*TODO:����������16KBʱ��Ҫ��������*/
}