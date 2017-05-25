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
    /* 选中 */
    select_chip();
    
    /* 清除RnB */
    clear_RnB();
    
    /* 发出复位信号 */
    nand_cmd(0xff);
    
    /* 等待就绪 */
    wait_RnB();
    
    /* 取消选中 */
    deselect_chip();
}

void nand_init()
{
#define TACLS  1
#define TWRPH0 2
#define TWRPH1 1

	NFCONF &= ~((7<<12)|(7<<8)|(7<<4));
    NFCONF |= (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
    
    /* 使能 nandflash controller*/
    NFCONT = 1 | (1<<1);
    
    /* 复位 */
    nand_reset();
}

void NF_page_read(unsigned long addr, unsigned char *buff)
{
	int i;
	
	/*选中芯片*/
	select_chip();
	
	/*清除RnB*/
	clear_RnB();
	
	/*发送命令0x00*/
	nand_cmd(0x00);
	
	/*发送列地址*/
	nand_addr(0x00);   /*读取整页时列地址为0*/
	nand_addr(0x00);
	
	/*发送行地址*/
	nand_addr(addr & 0xff);   /*行地址用于表示页的编号，分三次发送，每次发送8位*/
	nand_addr((addr >> 8) & 0xff);
	nand_addr((addr >> 16) & 0xff);
	
	/*发送命令0x30*/
	nand_cmd(0x30);
	
	/*等待就绪*/
	wait_RnB();
	
	/*读数据*/
	for(i = 0; i < 1024 * 4; i++)  /*OK6410使用的Nandflash芯片单页大小为4KB*/
	{
		buff[i] = NFDATA;
	}
	
	/*取消片选*/
	deselect_chip();
}

void nand_to_ram(unsigned long start_addr, unsigned char *sdram_addr, int size)
{
	int i;
	
	/*拷贝前4页共16KB的代码到内存*/
	for(i = 0; i < 4; i++, sdram_addr += 4096)
	{
		NF_page_read(i, sdram_addr);
	}
	
	/*TODO:代码量大于16KB时还要继续拷贝*/
}