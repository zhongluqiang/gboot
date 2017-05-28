int gboot_main()
{
	int num;
	unsigned char buff[4096];
	
#ifdef MMU_ON
	mmu_init();
#endif

	led_init();
	//button_init();
	//irq_init();
	led_off();
	
	NF_erase(128*1 + 1); /*擦除第2个块，传入第2个块中第1个页的行地址*/
	buff[0] = 100;
	NF_page_write(128*1+1, buff);
	buff[0] = 10;
	NF_page_read(128*1+1, buff);
	
	if(buff[0] == 100)
		led_on();
	
	uart_init();
	
	dma_init();
	dma_start();

    eth_init();
	dm9000_arp();
	
	while(1)
	{
		printf("\n\r********************************\n\r");
		printf("\n\r*************GBOOT**************\n\r");
		printf("1:Download kernel from tftp server\n\r");
		printf("2:Boot linux from RAM\n\r");
		printf("3:Boot linux from NAND\n\r");
		printf("pleare enter your selection:");
		
		scanf("%d", &num);
		
		switch(num)
		{
			case 1:
			//tftp_download();
				break;
			case 2:
			//boot_linux_ram();
				break;
			case 3:
			//boot_linux_nand();
				break;
			default:
			printf("Error: wrong selection!\n\r");
				break;
		}
	}
	
	return 0;
}