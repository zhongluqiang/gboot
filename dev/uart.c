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
    //1.配置引脚功能
    GPACON &= ~0xff;
    GPACON |= 0x22;
    
    //2.1 设置数据格式
    ULCON0 = 0b11;
    
    //2.2 设置工作模式
    UCON0 = 0b0101; 
	
	//2.3 使能FIFO
	UFCON0 = 1;
    
    //3. 设置波特率
	/*
	DIV_VAL = 66500000 / (115200 * 16) - 1
	        = 35.08
	x / 16 = 0.08
	x = 1
	查表得UDIVSLOT0 = 0x0080
	*/
    UBRDIV0 =(int)(PCLK/(BAUD*16)-1);  //UBRDIV0保存该公式计算后的整数部分
    UDIVSLOT0 = 0x0080;   //UDISLOT0=保存该公式计算后的小数部分*16
}


void putc(unsigned char ch)
{
    while ( UFSTAT0 & (1<<14) ); /*如果TX FIFO满，等待*/
	UTXH0 = ch;  
}


unsigned char getc(void)
{
    unsigned char ret;

	while (!(UFSTAT0 & 0x7f)); /*如果RX FIFO空，等待*/
	// 取数据
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