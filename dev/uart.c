#define GPACON     	(*((volatile unsigned short *)0x7F008000))
#define ULCON0     	(*((volatile unsigned long *)0x7F005000))
#define UCON0      	(*((volatile unsigned long *)0x7F005004))
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define UTRSTAT0   	(*((volatile unsigned long *)0x7F005010))
#define UFSTAT0    (*((volatile unsigned long *)0x7F005018))
#define UBRDIV0    	(*((volatile unsigned short *)0x7F005028))
#define UTXH0      	(*((volatile unsigned char *)0x7F005020))
#define URXH0      	(*((volatile unsigned char *)0x7F005024))
#define UDIVSLOT0  	(*((volatile unsigned short *)0x7F00502C))

#define PCLK 66500000
#define BAUD 115200

void uart_init()
{
    //1.�������Ź���
    GPACON &= ~0xff;
    GPACON |= 0x22;
    
    //2.1 �������ݸ�ʽ
    ULCON0 = 0b11;
    
    //2.2 ���ù���ģʽ
    UCON0 = 0b0101; 
	
	//2.3 ʹ��FIFO
	UFCON0 = 1;
    
    //3. ���ò�����
	/*
	DIV_VAL = 66500000 / (115200 * 16) - 1
	        = 35.08
	x / 16 = 0.08
	x = 1
	����UDIVSLOT0 = 0x0080
	*/
    UBRDIV0 =(int)(PCLK/(BAUD*16)-1);  //UBRDIV0����ù�ʽ��������������
    UDIVSLOT0 = 0x0080;   //UDISLOT0=����ù�ʽ������С������*16
}


void putc(unsigned char ch)
{
    while ( UFSTAT0 & (1<<14) ); /*���TX FIFO�����ȴ�*/
	UTXH0 = ch;  
}


unsigned char getc(void)
{
    unsigned char ret;

	while (!(UFSTAT0 & 0x7f)); /*���RX FIFO�գ��ȴ�*/
	// ȡ����
	ret = URXH0;  
	
	if ( (ret == 0x0d) || (ret == 0x0a) )
	{
	    putc(0x0d);
	    putc(0x0a);	
	}      	
	else
	    putc(ret);
	    
        return ret;
}