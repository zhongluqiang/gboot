#include "net.h"

int gboot_main()
{
	int num;

	uart_init();
	
	led_init();
	led_off();

	button_init();
	irq_init();
	
    eth_init();

	while(1)
	{
		printf("\n\r*************GBOOT**************\n\r");
		printf("0:ARP request\r\n");
		printf("1:Download kernel from tftp server\n\r");
		printf("2:Boot linux from RAM addr 0x50008000\n\r");
		printf("\n\r*************select*************\n\r");
		printf("pleare enter your selection:");
		
		scanf("%d", &num);
		
		switch(num)
		{
			case 0:
				net_loop(ARP);
				break;
			case 1:
				net_loop(TFTP);
				break;
			case 2:
				boot_linux();
				break;
			default:
			printf("Error: wrong selection!\n\r");
				break;
		}
	}
	
	return 0;
}