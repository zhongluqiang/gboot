#include "arp.h"

#define HON(n) ((((u16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))

/*1.����arp�����*/
void arp_request()
{
	u32 pkt_len;
	ARP_HDR arp_tx_buffer;

	memset(&arp_tx_buffer, 0, sizeof(arp_tx_buffer));
	
     /*1.����arp�����*/
     memcpy(arp_tx_buffer.ethhdr.d_mac,host_mac_addr,6);
     memcpy(arp_tx_buffer.ethhdr.s_mac,mac_addr,6);
     arp_tx_buffer.ethhdr.type = HON(0x0806);
       
     arp_tx_buffer.hwtype = HON(1);
     arp_tx_buffer.protocol = HON(0x0800);
     
     arp_tx_buffer.hwlen = 6;
     arp_tx_buffer.protolen = 4;
        
     arp_tx_buffer.opcode = HON(1);
 
     memcpy(arp_tx_buffer.smac,mac_addr,6);
     memcpy(arp_tx_buffer.sipaddr,ip_addr,4);
     memcpy(arp_tx_buffer.dipaddr,host_ip_addr,4);

     pkt_len = 14+28;
     
     /*2.����dm9000���ͺ���������Ӧ���*/	
     eth_send(&arp_tx_buffer,pkt_len);
}

/*2.����arpӦ�������ȡmac*/
u8 arp_process()
{
	u8 host_ip[4];  /*Ŀ������IP*/
	u8 host_mac[6];

    u32 i;
    
    if (packet_len<28)
        return 0;
    
    memcpy(host_ip,arpbuf.sipaddr,4);
    printf("host ip is : ");
    for(i=0;i<4;i++)
        printf("%03d ",host_ip[i]);
    printf("\n\r");
    
    memcpy(host_mac,arpbuf.smac,6);
    printf("host mac is : ");
    for(i=0;i<6;i++)
        printf("%02x ",host_mac[i]);
    printf("\n\r");
	return 0;
}

