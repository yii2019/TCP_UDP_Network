#ifndef _HEAD_H_
#define _HEAD_H_ 


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

/***宏定义***/
#define TCP_PORT 4444
#define UDP_FILE  6666
#define	UDP_ONLINE 5555
#define MSG_BROAT_PORT 8888 
#define MY_SERVER "192.168.1.81 "
#define BROATCAST "192.168.1.255"



/***全局变量***/
char signal_file_rcv[4];	//文件接收是否继续信号
char path_snd[100]; //需要发送文件的路径缓存区
int ser_fd ;	//TCP socket_fd
pthread_t msg_snd,msg_rcv;	//TCP信息接收，发送PID

int bmsg_rcv_signal;  //信息广播开关信号

//char my_ip_get[100];
//	char IP_Get[100];

pthread_t msg_broad_rcv;

/***函数声明***/

void * Udp_File_Snd(void * arg);			//UDP文件发送

void * Udp_File_Rcv(void * arg);			//UDP文件接收

int Udp_Online();							//上下线UDP						

int client_init();							//TCP初始化

int Tcp_Client(int s_fd);

void * Msg_Snd(void * arg);					//聊天发送

void * Msg_Rcv(void * arg);					//聊天接收

void Free(void * arg);						//释放接收文件线程

/*
int broadcast_msg_snd();		//广播信息发送端

void * broadcast_msg_rcv(void * arg);

void Free_Fun(void * arg);

char IP_Myself_Get(int fd);

*/
#endif