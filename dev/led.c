#ifdef MMU_ON
#define GPMCON (*(volatile unsigned long *)0xA0008820)
#define GPMDAT (*(volatile unsigned long *)0xA0008824)
#else
#define GPMCON (*(volatile unsigned long *)0x7F008820)
#define GPMDAT (*(volatile unsigned long *)0x7F008824)	
#endif


void led_init()
{
	GPMCON = 0x1111;
}	

void led_on()
{
	GPMDAT = 0x0;
}

void led_off()
{
	GPMDAT = 0xf;
}