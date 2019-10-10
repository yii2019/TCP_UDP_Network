#include "list.h"



void * My_Msg(void * arg)
{
	P_Snode node = (P_Snode) arg;
	int ret;
	char num[5] = {0};
	char con_ip[30] = {0};	//接收主动聊天客户端发来的查询IP号/回馈信息


	while(1)
	{
		printf("等待%s输入命令。。。\n", inet_ntoa(node->c_addr));
		ret = read(node->c_fd,num,10);
		if (ret <= 0)
		{
			printf("%s已下线\n", inet_ntoa(node->c_addr));
			node->sign = 1;				//删除节点
			return NULL;
		}
		else
		{
			switch(atoi(num))
			{
				case	1:
				{
					printf("进入聊天模式，等待操作！\n");
					ret = read(node->c_fd,con_ip,30);
					if (ret == -1)
					{
						perror("con_ip read");
						node->sign = 1;
						return NULL;
					}
					else
					{
						P_Snode found_node = Find_Node(head,con_ip);//先查找对应IP的节点
						if (found_node == NULL)
						{
							printf("没找到对应的地址！\n");
							write(node->c_fd,"没找到对应的地址！",sizeof("没找到对应的地址！"));
							break;
						}
						else
						{
							write(node->c_fd,"找到地址！",sizeof("找到地址！"));
							
						}
					Tcp_Connect * tcp_coat = (Tcp_Connect *)malloc(sizeof(Tcp_Connect));
						if (tcp_coat == NULL)
						{
							perror("tcp_coat");
							return NULL;
						}
					tcp_coat->node_one = node;
					tcp_coat->node_two = found_node;
					pthread_create(&node->msg_id,NULL,One_To_Two,(void * )tcp_coat);
					pthread_create(&node->msg_id,NULL,Two_To_One,(void * )tcp_coat);
					pthread_join(node->msg_id,NULL);	
					pthread_join(node->msg_id,NULL);	
					}
					break;
				}

				case	2:
					printf("%s,你就等着收发信息吧！\n",inet_ntoa(node->c_addr));
					break;

				case	3:
				{
					printf("%s用户使用了查询IP,效果拔群!\n",inet_ntoa(node->c_addr));
					Display_Node(head,node);
					break;
				}
				case	4:
				{
					printf("%s用户使用广播发送文件,效果拔群!\n",inet_ntoa(node->c_addr));
					break;
				}

				case	5:
				{
					printf("%s用户使用承受广播文件,效果拔群!\n", inet_ntoa(node->c_addr));
					break;
				}
				
				case	6:	//广播消息
				{
					printf("%s用户使用了信息广播,效果拔群！\n",  inet_ntoa(node->c_addr));
					pthread_create(&node->broad_id,NULL,Broad_Msg,(void * )node);
					pthread_join(node->broad_id,NULL);
				}
				default	 :printf("%s用户,在? 会不会用???\n",inet_ntoa(node->c_addr) );
			}  //switch end
			bzero(num,sizeof(num));	//清空一下接收的命令
		}
	}   //while end
}

void * One_To_Two(void * arg)
{
	int ret;
	char msg[100] = {0};
	char msg_mx[100] = {0};
	Tcp_Connect * tcp_coat = (Tcp_Connect *)arg;
	while(1)
	{
		bzero(msg,sizeof(msg));
		bzero(msg_mx,sizeof(msg_mx));
		ret = read(tcp_coat->node_one->c_fd,msg,sizeof(msg));
		if (ret <= 0)
		{
			printf("%s客户端退出聊天！\n",inet_ntoa(tcp_coat->node_one->c_addr) );
			tcp_coat->node_one->sign = 1;
			write(tcp_coat->node_two->c_fd,"quit",4);
			return NULL;
		}
		else
		{
			sprintf(msg_mx,"%s:%s\n", inet_ntoa(tcp_coat->node_one->c_addr),msg);
			write(tcp_coat->node_two->c_fd,msg_mx,strlen(msg_mx));
			if (strncmp(msg,"quit",4) == 0)
			{
				break;
			}
		}
	}
}

void * Two_To_One(void * arg)
{
	int ret;
	char msg[100] = {0};
	char msg_mx[100] = {0};
	Tcp_Connect * tcp_coat = (Tcp_Connect *)arg;
	while(1)
	{
		bzero(msg,sizeof(msg));
		bzero(msg_mx,sizeof(msg_mx));
		ret = read(tcp_coat->node_two->c_fd,msg,sizeof(msg));
		if (ret <= 0)
		{
			printf("%s客户端退出聊天！\n",inet_ntoa(tcp_coat->node_two->c_addr) );
			tcp_coat->node_two->sign = 1;
			write(tcp_coat->node_one->c_fd,"quit",4);
			return NULL;
		}
		else
		{
			sprintf(msg_mx,"%s:%s\n", inet_ntoa(tcp_coat->node_two->c_addr),msg);
			write(tcp_coat->node_one->c_fd,msg_mx,strlen(msg_mx));
			if (strncmp(msg,"quit",4) == 0)
			{
				break;
			}
		}
	}
}