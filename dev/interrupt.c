#define EINT0CON0 *((volatile unsigned long*)0x7f008900)
#define EINT0MASK *((volatile unsigned long*)0x7f008920)
#define VIC0INTENABLE *((volatile unsigned long*)0x71200010)
#define EINT0_VICADDR *((volatile unsigned long*)0x71200100)
#define EINT1_VICADDR *((volatile unsigned long*)0x71200104)
#define EINT0PEND *((volatile unsigned long*)0x7f008924)
#define VIC0ADDRESS *((volatile unsigned long*)0x71200f00)
#define VIC1ADDRESS *((volatile unsigned int *)0x71300f00)

void key1_isr()
{
	//1.���滷��
	__asm__("stmdb sp!, {r0-r12, lr}");

	//2.�жϴ���
	led_on();
	printf("key0 interrupt finished\r\n");
	
	//3.����ж�
	EINT0PEND |= 0x1;
	VIC0ADDRESS = 0x0;
	VIC1ADDRESS = 0;
	
	//4.�ָ�����
	__asm__("ldmia sp!, {r0-r12, pc}^");
}

void key6_isr()
{
	//1.���滷��
	__asm__("stmdb sp!, {r0-r12, lr}");
	
	//2.�жϴ���
	led_off();
	printf("key6 interrupt finished\r\n");
	
	//3.����ж�
	EINT0PEND |= (0x1 << 5);
	VIC0ADDRESS = 0x0;
	VIC1ADDRESS = 0;
	
	//4.�ָ�����
	__asm__("ldmia sp!, {r0-r12, pc}^");
}

void irq_init()
{
	//1.���ð����ж����½��ز�����ʹ�õ�1�͵�6������
	EINT0CON0 = 0x2 | (0x2 << 8);
		
	//2.ȡ��EINT0������
	EINT0MASK = 0x0;

	 /*��������ж�*/
	EINT0PEND |= 0x1;
	EINT0PEND |= (0x1 << 5);
	VIC0ADDRESS = 0x0;
	
	//3.ʹ���ж�
	VIC0INTENABLE |= 0x3;
	
	//5.����EINT0��EINT1���ж�������ַ
	EINT0_VICADDR = (int)key1_isr;
	EINT1_VICADDR = (int)key6_isr;
	
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