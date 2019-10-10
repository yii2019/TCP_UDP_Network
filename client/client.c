#include "head.h"


int client_init()				//初始化TCP后 返回一个socket了的 server_fd
{
	int server_fd = socket(AF_INET,SOCK_STREAM,0);
	if (server_fd == -1)
		{
			perror("socket client");
			return -1;
		}	
	struct sockaddr_in ser_addr;
	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(TCP_PORT);
	ser_addr.sin_addr.s_addr = inet_addr(MY_SERVER);

	int ret = connect(server_fd,(struct sockaddr * )&ser_addr,sizeof(ser_addr));
	if (ret == -1)
	{
		perror("connect");
		return -1;
	}
	return server_fd;
}

int Tcp_Client(int s_fd)
{
	int ret ;
	pthread_t file_snd,file_rcv;
	char ip[30] = {0};
	char num[5] = {0};
	while(1)
	{	
		bzero(num,sizeof(num));
		printf("1.进入聊天 2.Q我吧 3.查询已连接IP 4.发送文件 5.接收文件 6.进行广播\n");
		char num[5];
		scanf("%s",num);
		ret = write(s_fd,num,4);
		if (ret <= 0)
		{
			perror("服务器收不到命令");
			return -1;
		}
		switch(atoi(num))
		{
 			case	1:
 			{
 				printf("请输入聊天对象IP:\n");
 				scanf("%s",ip);
 				ret = write(s_fd,ip,30);
 				if (ret == -1)
 				{
 					perror("ip snd");
 					return -1;
 				}
 				bzero(ip,sizeof(ip));
 				ret = read(s_fd,ip,sizeof(ip));
 				if (ret <= 0)
 				{
 					perror("ip_msg rcv");
 					return -1;
 				}
 				else if(ret == 28)
 					{
 						printf("%s\n", ip);		//没找到 
 						break;
 					}
 				else
 					printf("%s\n", ip);		//找到
 				bzero(ip,sizeof(ip));
 				pthread_create(&msg_snd,NULL,Msg_Snd,NULL);
 				pthread_create(&msg_rcv,NULL,Msg_Rcv,NULL);
 				pthread_join(msg_snd,NULL);
				pthread_join(msg_rcv,NULL);
				break;
 			}	//case 1 end

 			case	2:															//最好能接受到对方的IP 然后就可以传参
 			{
 				printf("等待对方找上门来干架！\n");
 				pthread_create(&msg_snd,NULL,Msg_Snd,NULL);
 				pthread_create(&msg_rcv,NULL,Msg_Rcv,NULL);
 				pthread_join(msg_snd,NULL);
				pthread_join(msg_rcv,NULL);
				break;
 			}	//case 2 end

 			case	3:
 			{
 				printf("查询已连接IP中。。。\n");
 				char Seek_Buf[100] = {0};
 				ret = read(s_fd,Seek_Buf,sizeof(Seek_Buf));
 				if (ret <= 0)
 				{
 					perror("IP Seek");
 					return -1;
 				}
 				else
 					printf("%s\n", Seek_Buf);
 				break;
 			}		//case 3 end

 			case	4:
 			{				
 				printf("请输入需要广播的文件名：\n");
 				scanf("%s",path_snd);
 				pthread_create(&file_snd,NULL,Udp_File_Snd,NULL);
 				pthread_join(file_snd,NULL);
 				bzero(path_snd,sizeof(path_snd));
 				break;
 			}	//case 4 end

 			case	5:
 			{
 				printf("等待接受广播文件中。。。\n");
 				pthread_create(&file_rcv,NULL,Udp_File_Rcv,NULL);
 				while(1)
 				{
 					bzero(signal_file_rcv,sizeof(signal_file_rcv));
 					scanf("%s",signal_file_rcv);
 					if (strncmp(signal_file_rcv,"quit",4) == 0)
 					{
 						break;
 					}
 				}
 				pthread_cancel(file_rcv);
 				pthread_join(file_rcv,NULL);
 				break;
 			}

 			case	6: //广播消息
 			{
 				pthread_create(&msg_snd,NULL,Msg_Snd,NULL);
 				pthread_join(msg_snd,NULL);
 				break;
 			}

 			default	 :	
 				printf("你的输入有误，请重新输入！\n");break;

		} //switch end
	}	//while 1 end
	return 0;
}

void * Msg_Snd(void * arg )
{
	int ret;
	char msnd_buf[30] = {0};
	while(1)
	{		
		if(strncmp(msnd_buf,"quit",4) == 0)						//x = ?  buf +x  后4个  buf-4 =x
			break;
		bzero(msnd_buf,sizeof(msnd_buf));
		fgets(msnd_buf,sizeof(msnd_buf),stdin);
		ret = write(ser_fd,msnd_buf,strlen(msnd_buf));
		if (ret == -1)
		{
			perror("msg write");
			return NULL;
		}
	}
	pthread_cancel(msg_rcv);
	pthread_exit(0);
}

void * Msg_Rcv(void * arg)
{
	int ret;
	char mrcv_buf[100] = {0};
	while(1)
	{
		bzero(mrcv_buf,sizeof(mrcv_buf));
		ret = read(ser_fd,mrcv_buf,sizeof(mrcv_buf));
		if (ret <= 0)
		{
			perror("msg read");
			return NULL;
		}
		printf("From: %s\n", mrcv_buf);
	}
	pthread_cancel(msg_snd);
	pthread_exit(0);
}