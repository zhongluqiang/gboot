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
	/*OK6410的Nandflash单页大小为4KB，但是前4个页都只用了上半页的
	存储空间，导致启动时拷贝的前8K代码要从4个页拷贝，而不是2个页。
	对于第4个页以后的页，才是全部用于存储代码。
	*/

	int i;
	
	for (i = 0; i < 4; i++, sdram_addr+=2048) 
	{
		NF_page_read(i,sdram_addr);
	}
        
	size -= 1024*8;
        
	for( i=4; size>0;)
	{
	    NF_page_read(i,sdram_addr);	
	    size -= 4096;
	    sdram_addr += 4096;
	    i++;
	}
}

int NF_erase(unsigned addr)
{
	int ret;
	
	/*选中芯片*/
	select_chip();
	
	/*清除RnB*/
	clear_RnB();
	
	/*发送命令0x60*/
	nand_cmd(0x60);
	
	/*发送行地址*/
	nand_addr(addr & 0xff);
	nand_addr((addr >> 8) & 0xff);
	nand_addr((addr >> 16) & 0xff);
	
	/*发送命令0xd0*/
	nand_cmd(0xd0);
	
	/*等待RnB*/
	wait_RnB();
	
	/*发送命令0x70*/
	nand_cmd(0x70);
	
	/*读取擦除结果*/
	ret = NFDATA;
	
	/*取消芯片选中信号*/
	deselect_chip();
	
	return ret;
}

/*向addr所指的页写入一个page大小的数据*/
int NF_page_write(unsigned long addr, unsigned char *buff)
{
	int i, ret;
	
	/*选中芯片*/
	select_chip();
	
	/*清除RnB*/
	clear_RnB();
	
	/*发送命令0x80*/
	nand_cmd(0x80);
	
	/*发送列地址*/
	nand_addr(0x00);
	nand_addr(0x00);
	
	/*发送行地址*/
	nand_addr(addr & 0xff);
	nand_addr((addr >> 8) & 0xff);
	nand_addr((addr >> 16) & 0xff);
	
	/*写入数据*/
	for(i = 0; i < 1024 * 4; i++)
	{
		NFDATA = buff[i];
	}
	
	/*发送命令0x10*/
	nand_cmd(0x10);
	
	/*等待RnB*/
	wait_RnB();
	
	/*发送命令0x70*/
	nand_cmd(0x70);
	
	/*读取写入结果*/
	ret = NFDATA;
	
	/*取消芯片选中信号*/
	deselect_chip();
	
	return ret;
}