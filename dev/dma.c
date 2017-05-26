#define SDMA_SEL      			(*((volatile unsigned long *)0x7E00F110))
#define DMACIntTCClear      	(*((volatile unsigned long *)0x7DB00008))
#define DMACIntErrClr      		(*((volatile unsigned long *)0x7DB00010))
#define DMACConfiguration      	(*((volatile unsigned long *)0x7DB00030))
#define DMACSync     			(*((volatile unsigned long *)0x7DB00034))
#define DMACC0SrcAddr     		(*((volatile unsigned long *)0x7DB00100))
#define DMACC0DestAddr     		(*((volatile unsigned long *)0x7DB00104))
#define DMACC0Control0      	(*((volatile unsigned long *)0x7DB0010c))
#define DMACC0Control1      	(*((volatile unsigned long *)0x7DB00110))
#define DMACC0Configuration     (*((volatile unsigned long *)0x7DB00114))

#define UTXH0      		(volatile unsigned long *)0x7F005020

char src[100] = "\n\rHello World-> This is a test!\n\r";

void dma_init()
{
	//DMA控制器选择（SDMA0）
	SDMA_SEL = 0;
	
	//DMA控制器使能
	DMACConfiguration = 1;
	
	//初始化源地址
	DMACC0SrcAddr = (unsigned int)src;
	
	//初始化目的地址
	DMACC0DestAddr = (unsigned int)UTXH0;
	
	//对寄存器进行配置
	/*
	源地址自增
	目的地址固定
	目标主机选择AHB主机2
	源主机选择AHB主机1
	*/
	DMACC0Control0 = (1 << 25) | (1 << 26) | (1 << 31);
	DMACC0Control1 = 0x64;
	
	/*
	流控制和传输类型：MTP 为 001
	目标外设：DMA_UART0_1，源外设：DMA_MEM
	通道有效: 1
	*/
	DMACC0Configuration = (1<<6) | (1<<11) | (1<<14) | (1<<15);
}

void dma_start()
{
	//开启channel0 DMA
	DMACC0Configuration  = 1;	
}