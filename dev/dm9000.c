#include "dm9000x.h"
#include "net.h"

u8 buffer[1500];

/*
   Read a byte from I/O port
*/
static u8
DM9000_ior(int reg)
{
	DM9000_outb(reg, DM9000_IO);
	return DM9000_inb(DM9000_DATA);
}

/*
   Write a byte to I/O port
*/
static void
DM9000_iow(int reg, u8 value)
{
	DM9000_outb(reg, DM9000_IO);
	DM9000_outb(value, DM9000_DATA);
}

void dm9000_reset(void)
{
    printf("resetting DM9000\r\n");

    /* DEBUG: Make all GPIO0 outputs, all others inputs */
	DM9000_iow(DM9000_GPCR, GPCR_GPIO0_OUT);
	/* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
	DM9000_iow(DM9000_GPR, 0);
	/* Step 2: Software reset */
	DM9000_iow(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));

    do {
		printf("resetting the DM9000, 1st reset\r\n");
	} while (DM9000_ior(DM9000_NCR) & 1);

	DM9000_iow(DM9000_NCR, 0);
	DM9000_iow(DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST)); /* Issue a second reset */

	do {
		printf("resetting the DM9000, 2nd reset\r\n");
	} while (DM9000_ior(DM9000_NCR) & 1);

	/* Check whether the ethernet controller is present */
	if ((DM9000_ior(DM9000_PIDL) != 0x0) ||
	    (DM9000_ior(DM9000_PIDH) != 0x90))
		printf("ERROR: resetting DM9000 -> not responding\r\n");
}

u32 get_DM9000_ID(void)
{
    /*ID格式: 产品ID高8位 + 产品ID低8位 + 厂商ID高8位 + 厂商ID低8位*/
    
    u32 id_val;
	id_val = DM9000_ior(DM9000_VIDL);
	id_val |= DM9000_ior(DM9000_VIDH) << 8;
	id_val |= DM9000_ior(DM9000_PIDL) << 16;
	id_val |= DM9000_ior(DM9000_PIDH) << 24;

    return id_val;
}

#define Tacs  2
#define Tcos  2
#define Tacc  3
#define Tcoh  2
#define Tcah  2
#define Tacp  0

static void dm9000_outblk_16bit(volatile void *data_ptr, int count)
{
	int i;
	u32 tmplen = (count + 1) / 2;

	for (i = 0; i < tmplen; i++)
		DM9000_outw(((u16 *) data_ptr)[i], DM9000_DATA);
}

void cs_init()
{ 
	/*设置Bank1的数据宽度为16位*/
#define SROM_BW (*(volatile unsigned *)0x70000000) 
   SROM_BW &= (~(0xf<<4));
   SROM_BW |=  (0x1<<4);

#define SROM_BC1 (*(volatile unsigned *)0x70000008)
	SROM_BC1 = (Tacs << 28) | (Tcos << 24) | (Tacc << 16) | (Tcoh << 12) | 	(Tcah << 8)  | (Tacp << 4);

}

void eth_init()
{
    int i, oft;
    u32 ID;

	/*内存bank初始化，DM9000使用内存bank1*/
	cs_init();

    /*复位设备*/
    dm9000_reset();
	
	/*查找DM9000设备ID*/
	ID = get_DM9000_ID();
    if(ID != DM9000_ID)
    {
        printf("not found the dm9000 ID:%x\r\n",ID);
        return;
    }
    printf("Found DM9000 ID:%X at address %x !\r\n", ID,  DM9000_BASE);
	
    /* Program operating register, only internal phy supported */
	DM9000_iow(DM9000_NCR, 0x0);
	/* TX Polling clear */
	DM9000_iow(DM9000_TCR, 0);
	/* Less 3Kb, 200us */
	DM9000_iow(DM9000_BPTR, BPTR_BPHW(3) | BPTR_JPT_600US);
	/* Flow Control : High/Low Water */
	DM9000_iow(DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8));
	/* SH FIXME: This looks strange! Flow Control */
	DM9000_iow(DM9000_FCR, 0x0);
	/* Special Mode */
	DM9000_iow(DM9000_SMCR, 0);
	/* clear TX status */
	DM9000_iow(DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
	/* Clear interrupt status */
	DM9000_iow(DM9000_ISR, ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS);

    /*填充MAC地址*/
    /* fill device MAC address registers */
	for (i = 0, oft = DM9000_PAR; i < 6; i++, oft++)
		DM9000_iow(oft, mac_addr[i]);
	for (i = 0, oft = 0x16; i < 8; i++, oft++)
		DM9000_iow(oft, 0);  /*将广播地址设为0，避免接收多播和广播包*/
	
	/*激活DM9000*/
	/* RX enable */
	DM9000_iow(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	DM9000_iow(DM9000_IMR, IMR_PAR);

    return;
}

void eth_halt(void)
{
	
}

int eth_send(void *packet, u32 length)
{
    /*禁止中断*/
    DM9000_iow(DM9000_IMR, 0x80);
    //DM9000_iow(DM9000_ISR, IMR_PTM); /* Clear Tx bit in ISR */

	/*写入发送数据的长度*/
	/* Set TX length to DM9000 */
	DM9000_iow(DM9000_TXPLL, length & 0xff);
	DM9000_iow(DM9000_TXPLH, (length >> 8) & 0xff);

	/*写入要发送的数据*/
	/* Move data to DM9000 TX RAM */
	DM9000_outb(DM9000_MWCMD, DM9000_IO); /* Prepare for TX-data */
    /* push the data to the TX-fifo */
	dm9000_outblk_16bit(packet, length);
   
    /*启动发送*/
	/* Issue TX polling command */
	DM9000_iow(DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

    /*等待发送结束*/
    while(1)
    {
        u8 status = DM9000_ior(DM9000_TCR);
		//printf("sending data...\r\n");
        if(!(status & 0x1))
            break;
    }

    /*清除发送状态*/
    DM9000_iow(DM9000_NSR, 0x2c);
    //DM9000_iow(DM9000_ISR, IMR_PTM); /* Clear Tx bit in ISR */

    /*恢复中断*/
    DM9000_iow(DM9000_IMR, 0x81);
    //printf("eth_send done\r\n");
    return 1;
}

u16 eth_rx(void)
{
	u16 status,len;
    u16 tmp;
	u32 i;
	u8 ready = 0;
    

    /*判断是否中断，如果产生则清除中断*/
    if (DM9000_ior(DM9000_ISR) & 0x01) /* Rx-ISR bit must be set. */
    {
        DM9000_iow(DM9000_ISR, 0x01);
    }
	else
	{
		return 0;
	}

    /*空读*/
	DM9000_outb(DM9000_MRCMDX, DM9000_IO);
	ready = DM9000_inw(DM9000_DATA);
    //ready = DM9000_ior(DM9000_MRCMDX);	/* Dummy read */
	if((ready & 0x01) != 0x01)
	{
		DM9000_outb(DM9000_MRCMDX, DM9000_IO);
		ready = DM9000_inw(DM9000_DATA);
		//ready = DM9000_ior(DM9000_MRCMDX);	/* Dummy read */
		if((ready & 0x01) != 0x01)
		{
			printf("dummy read error\r\n");
			return 0;
		}
	}

    /*读取状态*/
    //status = DM9000_ior(DM9000_MRCMD);
	DM9000_outb(DM9000_MRCMD, DM9000_IO);
	status = DM9000_inw(DM9000_DATA);

    /*读取包的长度*/
    //len = DM9000_ior(DM9000_MRCMD);
	DM9000_outb(DM9000_MRCMD, DM9000_IO);
	len = DM9000_inw(DM9000_DATA);
	//printf("packet length:%d\r\n", len);

	if((status & 0xbf00) || (len < 0x40) || (len > DM9000_PKT_MAX))
	{
		if (status & 0x100) 
		{
			printf("rx fifo error\n");
		}
		if (status & 0x200) {
			printf("rx crc error\n");
		}
		if (status & 0x8000) {
			printf("rx length error\n");
		}
		if (len > DM9000_PKT_MAX) {
			printf("rx length too big\n");
			//dm9000_reset();
		}
	}

    for(i = 0; i < len; i +=2)
    {
        tmp = DM9000_inw(DM9000_DATA);
        buffer[i] = tmp & 0xff;
        buffer[i+1] = (tmp>>8) & 0xff;
    }

	net_receive(&buffer[0], len);

    return len;
}


