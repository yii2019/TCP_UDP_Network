#include "list.h"


P_Snode Server_Init()  //初始化头节点 跟TCP的socket
{
	P_Snode head = Create_Node();
	if (head == NULL)
	{
		perror("Init head");
		return NULL;
	}

	ser_fd = socket(AF_INET,SOCK_STREAM,0);
	if (ser_fd == -1)
	{
		perror("socket");
		return NULL;
	}
	
	struct sockaddr_in ser_addr;
	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MY_PORT);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(ser_fd,(struct sockaddr * )&ser_addr,sizeof(ser_addr));
	if (ret == -1)
	{
		perror("bind");
		return NULL;
	}

	ret = listen(ser_fd,10);
	if (ret == -1)
	{
		perror("listen");
		return NULL;
	}

	return head;
}

int Tcp_Connect_Fun(P_Snode head)	//一直卡住等待客户端连接
{
	struct sockaddr_in cli_addr;
	int cli_len = sizeof(struct sockaddr_in);
	int cli_fd;
	int ret;
	while(1)
	{
		cli_fd = accept(ser_fd,(struct sockaddr * )&cli_addr,&cli_len);
		if (cli_fd == -1)
		{
			perror("accept");
			return -1;
		}
		else
		{
			client_inf * cli_inf = (client_inf *)malloc(sizeof(client_inf));
			if (cli_inf == NULL)
			{
				perror("cli_inf");
				return -1;
			}
			cli_inf->c_fd = cli_fd;
			cli_inf->c_addr.s_addr = cli_addr.sin_addr.s_addr;

			P_Snode add_node = Add_Node(head,cli_inf); //返回一个节点
			if (add_node == NULL)
			{
				printf("创建节点失败！\n");
			}
			ret = pthread_create(&add_node->on_id,NULL,udp_ip,(void * )add_node);
			if (ret != 0)
			{
				perror("ip pthread");
				return -1;
			}
			ret = pthread_create(&add_node->my_id,NULL,My_Msg,(void * )add_node);
			if (ret != 0)
			{
				perror("msg pthread");
				return -1;
			}
			printf("创建%s上线成功！\n", inet_ntoa(cli_inf->c_addr));
		}
	}	//while end

	return 0;
}