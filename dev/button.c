#define GPNCON *((volatile unsigned long*)0x7f008830)

void button_init()
{
	GPNCON = (0x2) | (0x2 << 10);  //使用第1个和第6个按键，对应EINT0和EINT1
	
	/*不能使用第1个和第2个按键来测试中断，
	因为它们对应的中断号是相同的，都是EINT0*/
}