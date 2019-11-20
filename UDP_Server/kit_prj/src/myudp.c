#include "myudp.h"
#define   UDP_SERVER_PORT      5001      //板子的port
u16_t     UDP_CLIENT_PORT=0;             //目标主机port

static struct netif server_netif;
struct netif *echo_netif;
struct udp_pcb  *udppcb;
ip_addr_t ipaddr, netmask, gw, dst_addr;
unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

unsigned int recv_cnt=0;
unsigned int recv_len=0;
char *recv_ptr;

void UDP_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p,  ip_addr_t *addr, u16_t port)
{
	struct pbuf *tp=p;

	while(tp!=NULL)
	{
		memcpy(recv_ptr+recv_cnt,tp->payload,min(tp->len,recv_len-recv_cnt));
		recv_cnt+=tp->len;
		tp=tp->next;
	}

	UDP_CLIENT_PORT=port;
	//dst_addr.addr=addr->addr;

	//xil_printf("cilent addr=%x\n",addr->addr);
	//udp_sendto(udppcb,p,addr, port);

	pbuf_free(p);
}

static void print_ip(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

//打印board的IP网络信息
static void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	print_ip("Board IP:       ", ip);
	print_ip("Netmask :       ", mask);
	print_ip("Gateway :       ", gw);
}

void My_UDP_Init(void)
{
	IP4_ADDR(&ipaddr,   192,  168,   1,   10);//FPGA's ip, PC send to this IP
	IP4_ADDR(&netmask,  255,  255,   255, 0);
	IP4_ADDR(&gw, 192, 168,   1,   1);
	IP4_ADDR(&dst_addr, 192, 168,   1,   8);  //目标主机IP
	echo_netif = &server_netif;
	lwip_init();
	platform_enable_interrupts();

	if (!xemac_add(echo_netif, &ipaddr,&netmask , &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR))
	{
		xil_printf("Error adding N/W interface\n\r");
		return;
	}

	netif_set_default(echo_netif);
	netif_set_up(echo_netif);

	udppcb = udp_new();
    if (!udppcb)
    {
	    xil_printf("Error creating UDP PCB. Out of Memory\n\r");
	    return;
    }

    err_t err = udp_bind(udppcb, IP_ADDR_ANY, UDP_SERVER_PORT);
    if (err != ERR_OK)
    {
	    xil_printf("Unable to bind to port %d: err = %d\r\n", UDP_SERVER_PORT, err);
	    return;
    }

    udp_recv(udppcb,UDP_callback,NULL);

    xil_printf("UDP Initialize Done\n");
    //printf("---------------borad information----------------------\r\n");
    print_ip_settings(&(echo_netif->ip_addr),&(echo_netif->netmask),&(echo_netif->gw));
}

void platform_enable_interrupts(void)
{
	platform_setup_interrupts();
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	return;
}

void platform_setup_interrupts(void)
{
	Xil_ExceptionInit();

	XScuGic_DeviceInitialize(XPAR_SCUGIC_SINGLE_DEVICE_ID);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
			(Xil_ExceptionHandler)XScuGic_DeviceInterruptHandler,
			(void *)XPAR_SCUGIC_SINGLE_DEVICE_ID);

	return;
}

char* UDP_Recv(unsigned int length)
{
	recv_cnt=0;
	recv_len=length;
	recv_ptr=malloc(length);
	while (recv_cnt<length)
	{
		xemacif_input(echo_netif);
	}
	return recv_ptr;
}

void UDP_Send(char *data,unsigned int length)
{
	struct pbuf *pbuf_ptr = pbuf_alloc(PBUF_RAW ,length, PBUF_REF);
	pbuf_ptr->payload=data;

	udp_sendto(udppcb,pbuf_ptr,&dst_addr, UDP_CLIENT_PORT);
	pbuf_free(pbuf_ptr);
}

void Print_Pbuf(char *name,struct pbuf *p)
{
	xil_printf("%s.tot_len=%d\n",name,p->tot_len);
	xil_printf("%s.len=%d\n",name,p->len);
	xil_printf("%s.type=%d\n",name,p->type_internal);
	xil_printf("%s.flags=%d\n",name,p->flags);
	xil_printf("%s.ref=%d\n\n",name,p->ref);
}
