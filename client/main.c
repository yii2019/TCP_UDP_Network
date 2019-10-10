#include "head.h"


int main()
{
	pid_t x,y;
	x = fork();	
//	pthread_create(&msg_broad_rcv,NULL,broadcast_msg_rcv,NULL);
	if (x > 0)
	{
		ser_fd = client_init();
//		IP_Myself_Get(ser_fd);
		Tcp_Client(ser_fd);
	}
	if (x == 0)
	{
		Udp_Online();
	}
	
	return 0;
}