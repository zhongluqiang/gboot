#define GPMCON (*(volatile unsigned long *)0x7F008820)
#define GPMDAT (*(volatile unsigned long *)0x7F008824)	

int gboot_main()
{
	GPMCON = 0x1111;
	GPMDAT = 0xe;
	
	return 0;
}