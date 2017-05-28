#define EINT0CON0 *((volatile unsigned long*)0x7f008900)
#define EINT0MASK *((volatile unsigned long*)0x7f008920)
#define VIC0INTENABLE *((volatile unsigned long*)0x71200010)
#define EINT0_VICADDR *((volatile unsigned long*)0x71200100)
#define EINT1_VICADDR *((volatile unsigned long*)0x71200104)
#define EINT0PEND *((volatile unsigned long*)0x7f008924)
#define VIC0ADDRESS *((volatile unsigned long*)0x71200f00)

#define GPNCON *(volatile unsigned long*)0x7f008830

void key1_isr()
{
	//1.保存环境
	__asm__(
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//2.中断处理
	led_on();
	
	//3.清除中断
	EINT0PEND |= 0x1;
	VIC0ADDRESS = 0x0;
	
	//4.恢复环境
	__asm__(
		"ldmfd sp!, {r0-r12, pc}^\n"
	);
}

void key2_isr()
{
	//1.保存环境
	__asm__(
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//2.中断处理
	led_off();
	
	//3.清除中断
	EINT0PEND |= 0x2;
	VIC0ADDRESS = 0x0;
	
	//4.恢复环境
	__asm__(
		"ldmfd sp!, {r0-r12, pc}^\n"
		:
		:
	);
}

void net_isr()
{
	//1.保存环境
	__asm__(
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//2.中断处理
	printf("dm9000 receive data\r\n");
	int_issue();
	
	//3.清除中断
	EINT0PEND |= (0x1 << 7);
	VIC0ADDRESS = 0x0;
	
	//4.恢复环境
	__asm__(
		"ldmfd sp!, {r0-r12, pc}^\n"
		:
		:
	);
}

net_irq_init()
{
    /*将GPN7配置成中断功能*/
    GPNCON &= (~(0x3 << 14));  /*bit[7]*/
    GPNCON |= (0X2 << 14);

    /*高电平触发*/
    EINT0CON0 &= (~(0x7 << 12));
    EINT0CON0 |= 0x1 << 12;

    /*取消EINT7的屏蔽*/
    EINT0MASK &= (~(0x1 << 7));

    /*使能EINT7*/
    VIC0INTENABLE |= 0x2;

    /*首先清空中断*/
	EINT0PEND |= (0x1 << 7);
	VIC0ADDRESS = 0x0;

	/*设置网络中断的跳转地址*/
	EINT1_VICADDR = (int)net_isr;

	/*设置CPSR，使能向量中断，打开总的中断*/
	__asm__(
		"mrc p15, 0, r0, c1, c0, 0\n"
		"orr r0, r0, #(1<<24)\n"
		"mcr p15, 0, r0, c1, c0, 0\n"
		
		"mrs r0, cpsr\n"
		"bic r0, r0, #0x80\n"
		"msr cpsr_c, r0\n"
		:
		:
	);
}

void irq_init()
{
	//1.配置按键中断在下降沿产生，使用第1和第6个按键
	//EINT0CON0 = (0x2) | (0x2 << 8);
	EINT0CON0 = 0x2;
		
	//2.取消EINT0和EIN1的屏蔽
	EINT0MASK = 0x0;
	
	//3.使能中断
	VIC0INTENABLE |= 0x3;
	
	//5.设置EINT0和EINT1的中断向量地址
	EINT0_VICADDR = (int)key1_isr;
	EINT1_VICADDR = (int)key2_isr;
	
	//4.设置CPSR，使能向量中断，打开总的中断
	__asm__(
		"mrc p15, 0, r0, c1, c0, 0\n"
		"orr r0, r0, #(1<<24)\n"
		"mcr p15, 0, r0, c1, c0, 0\n"
		
		"mrs r0, cpsr\n"
		"bic r0, r0, #0x80\n"
		"msr cpsr_c, r0\n"
		:
		:
	);
}