#include "common.h"
#include "net.h"

#define ARP_TIMEOUT  5			/*arp二次请求发送超时*/
#define ARP_RESTART_COUNT 3   /*arp解析失败超时*/

const u8 mac_addr[6] = {9,8,7,6,5,4};       /*本机MAC地址*/
const u8 ip_addr[4] = {192,168,0,233};      /*本机IP*/

const u8 host_ip_addr[4] = {192,168,0,111};   /*目的主机IP，目前固定于宿主机，不可更改*/
u8 host_mac_addr[6];					/*目的主机IP*/

const u8 null_mac_addr[6] = {0};		/*无效mac地址*/

int g_netstate;  /*网络状态*/
int g_arpwait_try;  /*arp等待响应次数*/
int g_arp_restart_try; /*arp重启次数*/

void arp_timeout_check()
{
	if(!memcmp(host_mac_addr, null_mac_addr, 6)) /*已解析成功*/
		return;

	++g_arpwait_try;
	if(g_arpwait_try > ARP_TIMEOUT)
	{
		printf("arp timeout %d\r\n", g_arpwait_try);
		g_arpwait_try = 0;
		++g_arp_restart_try;
		g_netstate = NETLOOP_RESTART;
	}

	if(g_arp_restart_try > ARP_RESTART_COUNT)
	{
		printf("arp fail, please check your network\r\n");
		g_arp_restart_try = 0;
		g_netstate = NETLOOP_FAIL;
	}
}

int net_loop(proto_t protocol)
{
	g_netstate = NETLOOP_CONTINUE;

restart: /*针对arp之类的网络请求可能需要重启多次，直到超出预定次数*/	

	switch(protocol)
	{
		case ARP:
			arp_start();
			break;

		case TFTP:
			tftp_start();
			break;
			
		default:
			printf("unsupported protocol!!!\r\n");
			break;
	}

	for(;;)
	{

		eth_rx();

		arp_timeout_check();
		
		switch(g_netstate)
		{
			case NETLOOP_CONTINUE:
				break;
			
			case NETLOOP_RESTART:
				goto restart;
				
			case NETLOOP_SUCCESS:
				//eth_halt();
				return 1;
				
			case NETLOOP_FAIL:
				return -1;
		}
	}
}

