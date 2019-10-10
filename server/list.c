#include "list.h"

//创建，添加，删除

P_Snode Create_Node()	//创建节点的函数
{
	P_Snode node = (P_Snode)malloc(sizeof(Snode));
	if(node == NULL)
	{
		perror("malloc ...");
		return NULL;
	}
	else
	{
		INIT_LIST_HEAD(&node->list);
	}

	return node;
}


P_Snode Add_Node(P_Snode head,client_inf * cl_inf)	//添加节点函数
{
	if(head == NULL)
	{
		printf("宿主结构为空！,添加节点失败！\n");
		return NULL;
	}

	//创建要添加的节点
	P_Snode add_node = Create_Node();
	if(add_node == NULL)
	{
		printf("创建添加的宿主结构失败！\n");
		return NULL;
	}
	else
	{
		add_node->c_fd = cl_inf->c_fd;
		add_node->c_addr.s_addr = cl_inf->c_addr.s_addr;
		
		add_node->cond = cl_inf->cond;

		add_node->my_id = cl_inf->my_id;
		add_node->msg_id = cl_inf->msg_id;
		add_node->on_id = cl_inf->on_id;
		add_node->broad_id = cl_inf->broad_id;

		add_node->sign = 0;
		list_add(&add_node->list,&head->list);  //这个应该是遍历添加节点
	}
		
	

	return add_node;
}



int Destory_Node(P_Snode head)	//删除节点函数
{
	if(head == NULL)
	{
		printf("头结点为空！\n");

		return -1;
	}
	else
	{
		P_Snode pos 	= NULL;
		P_Snode new_pos = head;
		list_for_each_entry(pos,&head->list,list)
		{
			free(pos);
		}

		free(head);
		new_pos->list.next = NULL;
		new_pos->list.prev = NULL;
	}

	return 0;
}


P_Snode Find_Node(P_Snode head ,char buf[100])
{
	if(head == NULL || head->list.next == NULL)
	{
		printf("头节点为空！\n");
		return NULL;
	}
	else
	{
		
		P_Snode pos 	= NULL;
		list_for_each_entry(pos,&head->list,list)
		{
			if(strncmp(inet_ntoa(pos->c_addr),buf,strlen(inet_ntoa(pos->c_addr))) == 0)
				return pos;
		}
	
	}


	return NULL;
}

int Display_Node(P_Snode head,P_Snode node)
{
	if(head == NULL || head->list.next == NULL)
	{

		printf("添加的节点为空！\n");
		return -1;	
	}
	else
	{
		P_Snode pos = NULL;
		P_Snode new_head =  head;
		char i[] = " ";
		list_for_each_entry(pos,&new_head->list,list)
		{
			strcat(ip_snd,inet_ntoa(pos->c_addr));
			strcat(ip_snd,i);
		}
		write(node->c_fd,ip_snd,strlen(ip_snd));
		bzero(ip_snd,sizeof(ip_snd));
	}

	return 0;
}

/*
//	printf("%s  ",inet_ntoa(pos->c_addr));
		//	fgets(ip_snd,sizeof(ip_snd),inet_ntoa(pos->c_addr));
			strcpy(ip_snd,inet_ntoa(pos->c_addr));
			strcpy(ip_snd,' ');

			*/