int gboot_main()
{
#ifdef MMU_ON
	mmu_init();
#endif
	led_init();
	
	button_init();
	
	irq_init();
	
	led_off();
	
	while(1);
	
	return 0;
}