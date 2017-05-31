#ifndef NET_H
#define NET_H

#include "common.h"

typedef enum { BOOTP, RARP, ARP, TFTP, DHCP, PING, DNS, NFS, CDP, NETCONS, SNTP } proto_t;

#define NETLOOP_CONTINUE	1
#define NETLOOP_RESTART		2
#define NETLOOP_SUCCESS		3
#define NETLOOP_FAIL		4


#define PROTO_ARP 0x0806
#define PROTO_IP 0x0800
#define PROTO_UDP 0x11

typedef struct eth_hdr
{
    u8 d_mac[6];
    u8 s_mac[6];
    u16 type;
}ETH_HDR;


typedef struct arp_hdr
{
    ETH_HDR ethhdr;
    u16 hwtype;
    u16 protocol;
    u8 hwlen;
    u8 protolen;
    u16 opcode;
    u8 smac[6];
    u8 sipaddr[4];
    u8 dmac[6];
    u8 dipaddr[4];
}ARP_HDR;

typedef struct ip_hdr
{
    ETH_HDR ethhdr;
    u8 vhl;
    u8 tos;
    u16 len;
    u16 ipid;
    u16 ipoffset;
    u8 ttl;
    u8 proto;
    u16 ipchksum;
    u8 srcipaddr[4];
    u8 destipaddr[4];
}IP_HDR;

typedef struct udp_hdr
{
    IP_HDR iphdr;
    u16 sport;
    u16 dport;
    u16 len;
    u16 udpchksum;
}UDP_HDR;

typedef struct tftp_package
{
    u16 opcode;
    u16 blocknum;
    u8 data[0];	
}TFTP_PAK;


extern const u8 mac_addr[6];       	/*本机MAC地址*/
extern const u8 ip_addr[4];      	/*本机IP*/

extern const u8 host_ip_addr[4];  	/*目的主机IP，目前固定于宿主机，不可更改*/
extern u8 host_mac_addr[6];			/*目的主机IP*/

extern int g_netstate;	/*网络状态*/
extern int g_arpwait_try;  /*arp等待响应次数*/
extern int g_arp_restart_try; /*arp重启次数*/

#endif
