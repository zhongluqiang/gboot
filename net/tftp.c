#include "string.h"
#include "net.h"

u8 sendbuf[1024];
u8* tftp_down_addr;
u16 serverport = 0;
u16 curblock = 1;

#define HON(n) ((((u16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))

u16 checksum(u8 *ptr, int len)
{
    u32 sum = 0;
    u16 *p = (u16 *)ptr;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }
    
    if(len == 1)
        sum += *(u8 *)p;
    
    while(sum>>16)
        sum = (sum&0xffff) + (sum>>16);
    
    return (u16)((~sum)&0xffff);
}

void tftp_send_request(const char *filename)
{
    u8 *ptftp = &sendbuf[200];
    u32 tftp_len = 0;
    UDP_HDR *udphdr;
    u8 *iphdr;
    
    ptftp[0] = 0x00; /*操作码，两字节，读请求opcode=1，按照大端字节序进行赋值*/
    ptftp[1] = 0x01;
    tftp_len += 2 ;
    
    sprintf(&ptftp[tftp_len],"%s",filename);
    tftp_len += strlen(filename);
    ptftp[tftp_len] = '\0';
    tftp_len += 1;
    
    sprintf(&ptftp[tftp_len],"%s","octet");
    tftp_len += strlen("octect");
    ptftp[tftp_len] = '\0';
    tftp_len += 1;
    
    
    
    udphdr = (UDP_HDR*)(ptftp-sizeof(UDP_HDR));  /*UDP_HDR包含一个IP头结构体*/
    iphdr =  ptftp-sizeof(UDP_HDR) + sizeof(ETH_HDR); /*IP_HDR包含一个ETH_HDR结构体*/
    
    /*UDP帧头信息*/
    udphdr->sport = HON(48915);
    udphdr->dport = HON(69);
    udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR)); /*UDP中的长度字段包含本身8字节的长度*/
    udphdr->udpchksum = 0x00;
    
    /*IP帧头信息*/
    udphdr->iphdr.vhl = 0x45;
    udphdr->iphdr.tos = 0x00;
    udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
    udphdr->iphdr.ipid = HON(0x00);
    udphdr->iphdr.ipoffset = HON(0x4000);
    udphdr->iphdr.ttl = 0xff;
    udphdr->iphdr.proto = PROTO_UDP;
    memcpy(udphdr->iphdr.srcipaddr,ip_addr,4);
    memcpy(udphdr->iphdr.destipaddr,host_ip_addr,4);
    udphdr->iphdr.ipchksum = 0;
    udphdr->iphdr.ipchksum = checksum(iphdr,20);
    
    memcpy(udphdr->iphdr.ethhdr.s_mac,mac_addr,6);
    memcpy(udphdr->iphdr.ethhdr.d_mac,host_mac_addr,6);
    udphdr->iphdr.ethhdr.type = HON(PROTO_IP);
    
    eth_send((u32 *)udphdr,sizeof(UDP_HDR)+tftp_len);
}

void tftp_send_ack(u16 blocknum)
{
    u8 *ptftp = &sendbuf[200];
    u32 tftp_len = 0;
    UDP_HDR *udphdr;
    u8 *iphdr;
    
    ptftp[0] = 0x00;
    ptftp[1] = 0x04;
    tftp_len += 2 ;
    
    ptftp[2] = (blocknum&0xff00)>>8;
    ptftp[3] = (blocknum&0xff);
    tftp_len += 2 ;
    
    udphdr = (UDP_HDR*)(ptftp-sizeof(UDP_HDR));
    iphdr =  ptftp-sizeof(UDP_HDR)+ sizeof(ETH_HDR);
    
    /*UDP帧头信息*/
    udphdr->sport = HON(48915);
    udphdr->dport = HON(serverport);
    udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
    udphdr->udpchksum = 0x00;
    
    /*IP帧头信息*/
    udphdr->iphdr.vhl = 0x45;
    udphdr->iphdr.tos = 0x00;
    udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
    udphdr->iphdr.ipid = HON(0x00);
    udphdr->iphdr.ipoffset = HON(0x4000);
    udphdr->iphdr.ttl = 0xff;
    udphdr->iphdr.proto = 17;
    memcpy(udphdr->iphdr.srcipaddr,ip_addr,4);
    memcpy(udphdr->iphdr.destipaddr,host_ip_addr,4);
    udphdr->iphdr.ipchksum = 0;
    udphdr->iphdr.ipchksum = checksum(iphdr,20);
    
    memcpy(udphdr->iphdr.ethhdr.s_mac,mac_addr,6);
    memcpy(udphdr->iphdr.ethhdr.d_mac,host_mac_addr,6);
    udphdr->iphdr.ethhdr.type = HON(PROTO_IP);

    eth_send((u32 *)udphdr,sizeof(UDP_HDR)+tftp_len);
}


void tftp_process(u8 *buf, u32 len, u16 port)
{
     u32 i;
     u32 tftp_len;
     
     serverport = port;
     
     TFTP_PAK *ptftp = (TFTP_PAK*)(buf + sizeof(UDP_HDR));
     
     tftp_len = len - sizeof(UDP_HDR);

     if(HON(ptftp->opcode) == 3)
     {
     	 if (HON(ptftp->blocknum) == curblock)
     	 {
     	 	  tftp_down_addr = (u8*)(TFTP_DOWNADDR + (curblock - 1) * 512);
              for (i = 0;i<(tftp_len-4);i++)
              {
                  *(tftp_down_addr) = *(ptftp->data+i);
				  
				  if(curblock == 1)
				  {
						//printf("curaddr:%x %d,  data:%x#%x\r\n", tftp_down_addr, tftp_down_addr, tftp_down_addr[0], *(ptftp->data+i));
				  }
				  
				  ++tftp_down_addr;
              }
         
              tftp_send_ack(HON(ptftp->blocknum));
              
              curblock++;
              
              if ((tftp_len-4)<512)
              {
                  curblock = 1;
				  printf("tftp download over\r\n");
				  g_netstate = NETLOOP_SUCCESS;
              	}
         }
    }
}

void tftp_start(void)
{
	/*发送tftp请求包*/
	tftp_send_request("zImage");
}
