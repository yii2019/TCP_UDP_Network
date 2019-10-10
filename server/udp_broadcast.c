#include "list.h"

int  udp_ip_init()  //		初始化消息广播
{
	online_fd = socket(AF_INET,SOCK_DGRAM,0);
	if (online_fd == -1)
	{
		perror("socket");
		return -1;
	}
	int mask = 1;					//发送端 需要设置广播属性
	int ret = setsockopt(online_fd,SOL_SOCKET,SO_BROADCAST,&mask,sizeof(mask));
	if (ret == -1)
	{
		perror("setsockopt");
		return -1;
	}

	memset(&on_addr,0,sizeof(on_addr));
	on_addr.sin_family = AF_INET;
	on_addr.sin_port = htons(UDP_ONLINE);
	on_addr.sin_addr.s_addr = inet_addr(MY_BROATCAST); //负责广播出去


	return 0;
}

void * udp_ip(void * arg)	//传的是客户端节点   统一在这删除节点
{
	P_Snode node = (P_Snode)arg;
	sprintf(node->online_buf,"%s已上线",inet_ntoa(node->c_addr));
	int ret = sendto(online_fd,node->online_buf,strlen(node->online_buf),0,(struct sockaddr * )&on_addr,sizeof(struct sockaddr_in));
	bzero(node->online_buf,sizeof(node->online_buf));
	if (ret <= 0)
	{
		perror("udp_ip come");
		return NULL;
	}
	
	while(1)
	{
		if (node->sign == 1)
		{
			sprintf(node->online_buf,"%s已下线",inet_ntoa(node->c_addr));
			ret = sendto(online_fd,node->online_buf,strlen(node->online_buf),0,(struct sockaddr * )&on_addr,sizeof(struct sockaddr_in));
			bzero(node->online_buf,sizeof(node->online_buf));
			if (ret <= 0)
			{
				perror("udp_ip leave");
				return NULL;
			}
			else
			{
				list_del(&node->list);
				break;
			}
		}
	}
	pthread_exit(0);
}


void * Broad_Msg(void * arg)		//1客户端想广播 先接受信息 然后再广播 出去
{
	P_Snode node = (P_Snode)arg;

	char msg_broad[100] = {0};
	char msg_broad_mx[100] = {0};

	int ret;

	while(1)
	{
		bzero(msg_broad,sizeof(msg_broad));
		bzero(msg_broad_mx,sizeof(msg_broad_mx));
		ret = read(node->c_fd,msg_broad,sizeof(msg_broad));
		if(ret <= 0)
		{
			perror("广播接收信息");
			node->sign = 1;
			return NULL;
		}
		else
		{
			if(strncmp(msg_broad,"quit",4) == 0)
				break;
			sprintf(msg_broad_mx,"%s:%s\n",inet_ntoa(node->c_addr),msg_broad);
			sendto(online_fd,msg_broad_mx,strlen(msg_broad_mx),0,(struct sockaddr * )&on_addr,sizeof(struct sockaddr_in));
		}
	}
	pthread_exit(0);
}