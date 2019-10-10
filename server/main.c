
#include "list.h"


int main()
{
	udp_ip_init();	//广播初始化

	head = Server_Init();	//Tcp初始化

	Tcp_Connect_Fun(head);
	
	return 0;
}