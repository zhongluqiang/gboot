#include "arp.h"

#define HON(n) ((((u16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))

/*1.发送arp请求包*/
void arp_request()
{
	u32 packet_len;
	ARP_HDR arp_tx_buffer;

	memset(&arp_tx_buffer, 0, sizeof(arp_tx_buffer));
	
     /*1.构成arp请求包*/
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

     packet_len = 14+28;
     
     /*2.调用dm9000发送函数，发送应答包*/	
     eth_send(&arp_tx_buffer,packet_len);
}


