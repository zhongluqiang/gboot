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
	//1.���滷��
	__asm__(
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//2.�жϴ���
	led_on();
	
	//3.����ж�
	EINT0PEND |= 0x1;
	VIC0ADDRESS = 0x0;
	
	//4.�ָ�����
	__asm__(
		"ldmfd sp!, {r0-r12, pc}^\n"
	);
}

void key2_isr()
{
	//1.���滷��
	__asm__(
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//2.�жϴ���
	led_off();
	
	//3.����ж�
	EINT0PEND |= 0x2;
	VIC0ADDRESS = 0x0;
	
	//4.�ָ�����
	__asm__(
		"ldmfd sp!, {r0-r12, pc}^\n"
		:
		:
	);
}

void net_isr()
{
	//1.���滷��
	__asm__(
		"sub lr, lr, #4\n"
		"stmfd sp!, {r0-r12, lr}\n"
		:
		:
	);
	
	//2.�жϴ���
	printf("dm9000 receive data\r\n");
	int_issue();
	
	//3.����ж�
	EINT0PEND |= (0x1 << 7);
	VIC0ADDRESS = 0x0;
	
	//4.�ָ�����
	__asm__(
		"ldmfd sp!, {r0-r12, pc}^\n"
		:
		:
	);
}

net_irq_init()
{
    /*��GPN7���ó��жϹ���*/
    GPNCON &= (~(0x3 << 14));  /*bit[7]*/
    GPNCON |= (0X2 << 14);

    /*�ߵ�ƽ����*/
    EINT0CON0 &= (~(0x7 << 12));
    EINT0CON0 |= 0x1 << 12;

    /*ȡ��EINT7������*/
    EINT0MASK &= (~(0x1 << 7));

    /*ʹ��EINT7*/
    VIC0INTENABLE |= 0x2;

    /*��������ж�*/
	EINT0PEND |= (0x1 << 7);
	VIC0ADDRESS = 0x0;

	/*���������жϵ���ת��ַ*/
	EINT1_VICADDR = (int)net_isr;

	/*����CPSR��ʹ�������жϣ����ܵ��ж�*/
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
	//1.���ð����ж����½��ز�����ʹ�õ�1�͵�6������
	//EINT0CON0 = (0x2) | (0x2 << 8);
	EINT0CON0 = 0x2;
		
	//2.ȡ��EINT0��EIN1������
	EINT0MASK = 0x0;
	
	//3.ʹ���ж�
	VIC0INTENABLE |= 0x3;
	
	//5.����EINT0��EINT1���ж�������ַ
	EINT0_VICADDR = (int)key1_isr;
	EINT1_VICADDR = (int)key2_isr;
	
	//4.����CPSR��ʹ�������жϣ����ܵ��ж�
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