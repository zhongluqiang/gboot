#include "dm9000x.h"

#if 0
#define DM9000_IO  0x18000000
#define DM9000_DATA 0x18000004
#else
#define DM9000_IO  0x18000300
#define DM9000_DATA 0x18000304
#endif

#define DM9000_BASE		0x18000000
#define DM9000_ID		0x90000A46

u8 mac_addr[6] = {4, 5, 6, 7, 8, 9};
u8 buffer[1000];

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


/*��ʼ��DM9000ʹ�õ��ڴ�Bank*/
void cs_init()
{
	
}

void dm9000_interrupt_init(void)
{
    /*��GPN7���ó��жϹ���*/
#define GPNCON *(volatile unsigned*)0x7f008830
    GPNCON &= (~(0x3 << 14));  /*bit[7]*/
    GPNCON |= (0X2 << 14);

    /*�ߵ�ƽ����*/
#define EINT0CON0 *(volatile unsigned*)0x7f008900
    EINT0CON0 &= (~0x7);
    EINT0CON0 |= 0x1;

    /*ȡ��EINT1������*/
#define EINT0MASK *(volatile unsigned*)0x7f008920
    EINT0MASK &= (~0x2);
    EINT0MASK |= 0x2;

    /*ʹ��EINT1*/
#define VIC0INTENABLE *(volatile unsigned*)0x71200010
    VIC0INTENABLE |= 0x2;

    /*��������ж�*/
#define EINT0PEND *(volatile unsigned*)0x7f008924
    EINT0PEND |= 0x2;
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
    /*ID��ʽ: ��ƷID��8λ + ��ƷID��8λ + ����ID��8λ + ����ID��8λ*/
    
    u32 id_val;
	id_val = DM9000_ior(DM9000_VIDL);
	id_val |= DM9000_ior(DM9000_VIDH) << 8;
	id_val |= DM9000_ior(DM9000_PIDL) << 16;
	id_val |= DM9000_ior(DM9000_PIDH) << 24;

    return id_val;
}

void eth_init()
{
    int i, oft;
    u32 ID;

	/*ʹ��DM9000Ƭѡ�ź�*/
    /*ignored*/
	
	/*�жϳ�ʼ��*/
    //dm9000_interrupt_init();

    /*��λ�豸*/
    dm9000_reset();
	
	/*����DM9000�豸ID*/
	ID = get_DM9000_ID();
    if(ID != DM9000_ID)
    {
        printf("not found the dm9000 ID:%x\r\n",ID);
        return;
    }
    printf("Found DM9000 ID:%X at address %x !\r\n", ID,  DM9000_BASE);
	
	/*MAC��ʼ��*/
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

    /*���MAC��ַ*/
    /* fill device MAC address registers */
	for (i = 0, oft = DM9000_PAR; i < 6; i++, oft++)
		DM9000_iow(oft, mac_addr[i]);
	for (i = 0, oft = 0x16; i < 8; i++, oft++)
		DM9000_iow(oft, 0xff);
	
	/*����DM9000*/
	/* RX enable */
	DM9000_iow(DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
	/* Enable TX/RX interrupt mask */
	DM9000_iow(DM9000_IMR, IMR_PAR);

    return;
}

static void dm9000_outblk_16bit(volatile void *data_ptr, int count)
{
	int i;
	u32 tmplen = (count + 1) / 2;

	for (i = 0; i < tmplen; i++)
		DM9000_outw(((u16 *) data_ptr)[i], DM9000_DATA);
}

int eth_send(void *packet, u32 length)
{
    /*��ֹ�ж�*/
    DM9000_iow(DM9000_IMR, 0x80);
    DM9000_iow(DM9000_ISR, IMR_PTM); /* Clear Tx bit in ISR */

    /*д�뷢�����ݵĳ���*/
	/* Set TX length to DM9000 */
	DM9000_iow(DM9000_TXPLL, length & 0xff);
	DM9000_iow(DM9000_TXPLH, (length >> 8) & 0xff);
    
    /*д��Ҫ���͵�����*/
	/* Move data to DM9000 TX RAM */
	DM9000_outb(DM9000_MWCMD, DM9000_IO); /* Prepare for TX-data */
    /* push the data to the TX-fifo */
	dm9000_outblk_16bit(packet, length);
    
    /*��������*/
	/* Issue TX polling command */
	DM9000_iow(DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

    /*�ȴ����ͽ���*/
    while(1)
    {
        u8 status = DM9000_ior(DM9000_TCR);
        if(!(status & 0x1))
            break;
    }

    /*�������״̬*/
    DM9000_iow(DM9000_NSR, 0x2c);
    DM9000_iow(DM9000_ISR, IMR_PTM); /* Clear Tx bit in ISR */

    /*�ָ��ж�*/
    DM9000_iow(DM9000_IMR, 0x81);
    
    return 1;
}

int eth_rx(u8 *data)
{
    u32 i;
    u8 status;
    u16 len;
    u16 tmp;
    

    /*�ж��Ƿ��жϣ��������������ж�*/
    if (!(DM9000_ior(DM9000_ISR) & 0x01)) /* Rx-ISR bit must be set. */
		return 0;
    
    DM9000_iow(DM9000_ISR, 0x01); /* clear PR status latched in bit 0 */

    /*�ն�*/
    DM9000_ior(DM9000_MRCMDX);	/* Dummy read */

    /*��ȡ״̬*/
    status = DM9000_ior(DM9000_MRCMD);

    /*��ȡ���ĳ���*/
    len = DM9000_ior(DM9000_MRCMD);

    /*��ȡ��������*/
    if(len < 0x40 || len > DM9000_PKT_MAX)
    {
        printf("rx length error\r\n");
        dm9000_reset();
    }
    else
    {
        for(i = 0; i < len; i +=2)
        {
            tmp = DM9000_inw(DM9000_DATA);
            data[i] = tmp & 0xff;
            data[i+1] = (tmp>>8) & 0xff;
        }
    }

    return 1;
}

void int_issue()
{
    eth_rx(buffer);
}

