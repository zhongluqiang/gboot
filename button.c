#define GPNCON *((volatile unsigned long*)0x7f008830)

void button_init()
{
	GPNCON = (0x2) | (0x2 << 10);  //使用前两个按键，对应EINT0和EINT5
	
	/*不能使用EINT0和EINT1来测试中断，因为EIN0和EINT1对应的中断是相同的，它们共享一个中断向量地址，所以无法分别测试*/
}