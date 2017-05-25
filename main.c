int gboot_main()
{
	unsigned char buff[4096];
	
#ifdef MMU_ON
	mmu_init();
#endif

	led_init();
	button_init();
	irq_init();
	led_off();
	
	NF_erase(128*1 + 1); /*擦除第2个块，传入第2个块中第1个页的行地址*/
	buff[0] = 100;
	NF_page_write(128*1+1, buff);
	buff[0] = 10;
	NF_page_read(128*1+1, buff);
	
	if(buff[0] == 100)
		led_on();
	
	while(1);
	
	return 0;
}