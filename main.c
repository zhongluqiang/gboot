int gboot_main()
{
#ifdef MMU_ON
	mmu_init();
#endif
	led_init();
	
	led_on();
	
	while(1);
	
	return 0;
}