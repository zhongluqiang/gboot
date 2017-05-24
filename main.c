//#define GPMCON (*(volatile unsigned long *)0x7F008820)
//#define GPMDAT (*(volatile unsigned long *)0x7F008824)	
#define GPMCON (*(volatile unsigned long *)0xA0008820)
#define GPMDAT (*(volatile unsigned long *)0xA0008824)	

#define MMU_SECTION (2 << 0)
#define MMU_CACHEABLE (1 << 3)
#define MMU_BUFFERABLE (1 << 2)
#define MMU_SPECIAL (1 << 4)
#define MMU_DOMAIN (0 << 5)
#define MMU_FULL_ACCESS (3 << 10)

#define SECDESC (MMU_SECTION | MMU_SPECIAL | MMU_DOMAIN | MMU_FULL_ACCESS)
#define SECDESC_WB SECDESC | MMU_CACHEABLE | MMU_BUFFERABLE

void create_page_table()
{
	unsigned long vaddr, paddr;
	unsigned long *ttb = (unsigned long *)0x50000000; //ttb放到内存的开始位置
	
	vaddr = 0xA0000000;
	paddr = 0x7f000000;
	
	*(ttb + ((vaddr) >> 20)) = (paddr & 0xfff00000) | SECDESC;
	
	vaddr = 0x50000000;
	paddr = 0x50000000;
	while(vaddr < 0x54000000)
	{
		*(ttb + ((vaddr) >> 20)) = (paddr & 0xfff00000) | SECDESC_WB;
		vaddr += 0x100000;
		paddr += 0x100000;
	}
}

void mmu_init()
{
	__asm__(
		/*设置TBB，参考手册2.3.6小节*/
		"ldr r0, =0x50000000\n"
		"mcr p15, 0, r0, c2, c0, 0\n"
		
		/*不进行域权限检查，参考手册2.3.7小节*/
		"mvn r0, #0x0\n"
		"mcr p15, 0, r0, c3, c0, 0\n"
		
		/*使能MMU，参考手册2.3.5小节*/
		"mrc p15, 0, r0, c1, c0, 0\n"
		"orr r0, r0, #0x1\n"
		"mcr p15, 0, r0, c1, c0, 0\n"
		:
		:
	);
}

int gboot_main()
{
	//1.建立页表
	create_page_table();
	
	//2.写入TTB
	//3.使用MMU
	mmu_init();
	
	GPMCON = 0x1111;
	GPMDAT = 0xe;
	
	return 0;
}