#include "head.h"

/****发送文件函数***/
void * Udp_File_Snd(void * arg)  	
{
	int file_snd_fd = socket(AF_INET,SOCK_DGRAM,0);
	if (file_snd_fd == -1)
	{
		perror("socket udp_snd");
		return NULL;
	}
	int mask = 1;					//发送端 需要设置广播属性
	int ret = setsockopt(file_snd_fd,SOL_SOCKET,SO_BROADCAST,&mask,sizeof(mask));
	if (ret == -1)
	{
		perror("setsockopt");
		return NULL;
	}
	struct sockaddr_in s_addr;
	memset(&s_addr,0,sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(UDP_FILE);
	s_addr.sin_addr.s_addr = inet_addr(BROATCAST); //负责广播出去

/************************************************************************/

	//1.打开所要传输的文件 发送文件大小 名字
	FILE *  fd = fopen(path_snd,"r");
	if (fd == NULL)
	{
		perror("fopen");
		return NULL;
	}

	/*********自定义包头（大小+名字） 发送包头*****/
	fseek(fd,0,2);  //将指针移动到文本末尾 define 2 SEEK_END
	int f_size = ftell(fd); //文件内容的大小
	fseek(fd,0,0);	//define 0 SEEK_SET 将光标移回开头

	char name_snd[sizeof(int )+strlen(path_snd)]; //定义包头数组
	memset(name_snd,0,sizeof(name_snd));
	//把大小和名字 复制到数组
	memcpy(name_snd,&f_size,sizeof(int));			//大小
	memcpy(name_snd+4,path_snd,strlen(path_snd));	//名字

	//把包头发送给服务器
	int len = sizeof(struct sockaddr_in);
	ret = sendto(file_snd_fd,name_snd,sizeof(name_snd),0,(struct sockaddr * )&s_addr,sizeof(struct sockaddr_in)); //后面2参数可以填空NULL
	if (ret == -1)
	{
		perror("包头发送");
		return NULL;
	}
	else
	{
		printf("包头发送成功！\n");
	}		
	sleep(1);
	char file_snd[1024] = {0};
	while(1)
	{
		int size = fread(file_snd,1,1024,fd);
		if (size == -1)
		{
			perror("文件读取失败");
			return NULL;
		}
		if(size == 0)
		{
			break;
		}
	ret = sendto(file_snd_fd,file_snd,size,0,(struct sockaddr * )&s_addr,len); //后面2参数可以填空NULL
	if (ret <= 0)
	{
		perror("数据发送失败");
		return NULL;	
	}	
		memset(file_snd,0,sizeof(file_snd));
	}
	printf("文件发送成功！\n");

	fclose(fd);
	close(file_snd_fd);
	pthread_exit(0);
}


/***接收文件函数***/
void * Udp_File_Rcv(void * arg)
{
	int file_rcv_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(file_rcv_fd == -1)
	{
		perror("socket udp_rcv");
		return NULL;
	}
	struct sockaddr_in ser_addr;
	memset(&ser_addr,0,sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(UDP_FILE);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int  ret = bind(file_rcv_fd,(struct sockaddr * )&ser_addr,sizeof(struct sockaddr_in));
	if (ret == -1)
	{
		perror("bind file_rcv");
		return NULL;
	}
	int len = sizeof(struct sockaddr_in);

	char file_rcv[1024] = {0};
	int size;
	pthread_cleanup_push(Free,(void * )&file_rcv_fd);
	while(1)
	{
//		if(strncmp(signal_file_rcv,"quit",4) == 0 )		//达到条件使之退出	输入quit	不然可以继续接收
//			break;
		printf("-------------------\n");
		ret = recvfrom(file_rcv_fd,file_rcv,sizeof(file_rcv),0,(struct sockaddr *)&ser_addr,&len);   
		if (ret == -1)
		{
			perror("recvfrom file");
			return NULL;
		}
		memcpy(&size,file_rcv,4);
		printf("传过来文件大小：%d 字节\n", size);
		printf("传过来文件名字：%s\n", file_rcv+4);

		/***拼接存放文件的路径***/
		char rcv_path[strlen("./")+strlen(file_rcv+4)];	//拼接当前路径下文件名
		sprintf(rcv_path,"./%s",file_rcv+4);

		/***创建存放新文件数据的文本***/
		FILE * fd = fopen(rcv_path,"w");
		if (fd == NULL)
		{
			perror("fopen rcv");
			return NULL;
		}	
		int num = 0;
		bzero(file_rcv,sizeof(file_rcv));
		while(num < size)
		{
			ret = recvfrom(file_rcv_fd,file_rcv,sizeof(file_rcv),0,(struct sockaddr *)&ser_addr,&len);
			if (ret == -1)
			{
				perror("read");
				return NULL;
			}
			int p = (int)(((float)num/(float)size)*100);	
	//		system("clear");								//清屏	
			printf("进度：%d%%\n", p);						//输出进度条
			fwrite(file_rcv,1,ret,fd);
			num+=ret;
			bzero(file_rcv,sizeof(file_rcv));
		}
		printf("接收完成！\n");
		fclose(fd);

	}
	pthread_cleanup_pop(10);
	pthread_exit(0);
}

void Free(void * arg)
{
	int * file_rcv_fd = (int * )arg;
	close(*file_rcv_fd);
	printf("接收文件通道关闭！!\n");
}