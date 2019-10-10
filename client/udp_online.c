#include "head.h"


int Udp_Online()			//创建该函数 读取服务器广播来的IP上下线情况
{
	int online_fd = socket(AF_INET,SOCK_DGRAM,0);
	if (online_fd == -1)
	{
		perror("socket online_fd");
		return -1;
	}
	struct sockaddr_in online_addr;
	bzero(&online_addr,sizeof(online_addr));
	online_addr.sin_family = AF_INET;
	online_addr.sin_port = htons(UDP_ONLINE);
	online_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int len = sizeof(struct sockaddr_in);
	

	int ret = bind(online_fd,(struct sockaddr * )&online_addr,sizeof(online_addr));
	if (ret == -1)
	{
		perror("bind online");
		return -1;
	}
	
	char online_ip[50];

	while(1)
	{
		ret = recvfrom(online_fd,online_ip,sizeof(online_ip),0,(struct sockaddr* )&online_addr,&len);
		if (ret == -1)
		{
			perror("recvfrom online");
			return -1;
		}
		else
		{
			printf("%s\n", online_ip);			//读的上线 下线
		}
		bzero(online_ip,sizeof(online_ip));
	}

	return 0;
}