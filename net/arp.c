#include "common.h"
#include "net.h"

#define HON(n) ((((u16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))

void arp_start(void)
{
	g_arpwait_try = 0;
	g_arp_restart_try = 0;

	arp_request();
}

/*1.发送arp请求包*/
void arp_request()
{
	u32 pkt_len;
	ARP_HDR arp_tx_buffer;
	const u8 broadcast_mac_addr[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

	memset(&arp_tx_buffer, 0, sizeof(arp_tx_buffer));

     /*1.构成arp请求包*/
     memcpy(arp_tx_buffer.ethhdr.d_mac,broadcast_mac_addr,6);
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
     
     /*2.调用dm9000发送函数，发送应答包*/	
     eth_send(&arp_tx_buffer,pkt_len);
}

void arp_response(u8 *dstmac, u8 *dstipaddr)
{
	u32 pkt_len;
	ARP_HDR arp_tx_buffer;

	memset(&arp_tx_buffer, 0, sizeof(arp_tx_buffer));
	
     /*1.构成arp响应包*/
     memcpy(arp_tx_buffer.ethhdr.d_mac,dstmac,6);
     memcpy(arp_tx_buffer.ethhdr.s_mac,mac_addr,6);
     arp_tx_buffer.ethhdr.type = HON(0x0806);
       
     arp_tx_buffer.hwtype = HON(1);
     arp_tx_buffer.protocol = HON(0x0800);
     
     arp_tx_buffer.hwlen = 6;
     arp_tx_buffer.protolen = 4;
        
     arp_tx_buffer.opcode = HON(2);
 
     memcpy(arp_tx_buffer.smac,mac_addr,6);
     memcpy(arp_tx_buffer.sipaddr,ip_addr,4);
	 memcpy(arp_tx_buffer.smac, dstmac, 6);
     memcpy(arp_tx_buffer.dipaddr,dstipaddr,4);

     pkt_len = 14+28;
     
     /*2.调用dm9000发送函数，发送应答包*/	
     eth_send(&arp_tx_buffer,pkt_len);

}

/*2.arp包处理，根据opcode确定是进行arp解析或是进行arp响应*/
u8 arp_process(u8 *buf, u32 length)
{
    u32 i;
	ARP_HDR *arpbuf = (ARP_HDR *)buf;
    
    if (length < 28)
        return 0;

	switch(HON(arpbuf->opcode))
	{
		case 2: /*arp响应包，进行MAC地址解析*/
			printf("arp response packet received\r\n");
			/*
			memcpy(host_ip_addr, arpbuf->sipaddr, 4);
			printf("host ip is : ");
		    for(i=0;i<4;i++)
		        printf("%03d ",host_ip_addr[i]);
		    printf("\n\r");
		    */

			memcpy(host_mac_addr, arpbuf->smac, 6);
			printf("host mac is : ");
		    for(i=0;i<6;i++)
		        printf("%02x ",host_mac_addr[i]);
			printf("\n\r");

			g_netstate = NETLOOP_SUCCESS;
			
			break;
			
		case 1: /*arp请求包，发送ARP响应包*/
			printf("arp request packet received\r\n");
			printf("host ip is : ");
			 for(i=0;i<4;i++)
		        printf("%03d ",arpbuf->sipaddr[i]);
		    printf("\n\r");
			
			printf("target ip is : ");
			 for(i=0;i<4;i++)
		        printf("%03d ",arpbuf->dipaddr[i]);
		    printf("\n\r");
			
			if(memcmp(arpbuf->dipaddr, ip_addr, 4) != 0) /*不是对本主机的arp请求不响应*/
				break;
			
			printf("send arp response\r\n");
			arp_response(arpbuf->smac, arpbuf->sipaddr);
			break;
			
		default:
			printf("unknown arp packet!!!\r\n");
			break;
	}
	
	return 0;
}

void udp_process(u8* buf, u32 len)
{
     UDP_HDR *udphdr = (UDP_HDR *)buf;
     
     tftp_process(buf,len,HON(udphdr->sport));     	
}


u8 ip_process(u8 *buf, u32 len)
{
	IP_HDR *p = (IP_HDR *)buf;	
		 
	switch(p->proto)
	{
		case PROTO_UDP:
			udp_process(buf,len);
			break;
		default:
			printf("unsupported ip packet\r\n");
			break; 
	}

	return 0;
}

void net_receive(u8 *buf, u32 len)
{
     ETH_HDR *p = (ETH_HDR *)buf;
     
     switch (HON(p->type))
     {
		case PROTO_ARP:
			arp_process(buf,len);
			break;

		case PROTO_IP:
			ip_process(buf,len);
			break;
			
		  default:
		        break;
     }
}


