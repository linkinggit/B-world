#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/in.h>
#include <signal.h>

#define PORT 80   /* 侦听端口地址 */

void sig_proccess(int signo);
void sig_pipe(int signo);
void process_conn_client(int s);


int main(int argc, char *argv[])
{
    int s;
    struct sockaddr_in server_addr; /* 服务器地址 */
    int err; /* 返回值 */

    signal(SIGINT, sig_proccess);
    signal(SIGPIPE, sig_pipe);

    /* 建立一个流式套接字 */
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){/* 出错 */
        printf("socket openning error\n");
        return -1;  
    }   

    /* 设置服务器地址 */
    bzero(&server_addr, sizeof(server_addr));       /* 清0 */
    server_addr.sin_family = AF_INET;               /* 协议族 */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);/* 本地地址 */
    server_addr.sin_port = htons(PORT);             /* 服务器端口, 转换成网络字节序 */

    /* 将用户输入的字符串类型的IP地址转为整型 */
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr); 
    
    /* 设置客户端地址 */
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(4455);
    if (bind(s, (struct sockaddr *) &client, sizeof(client)) < 0) 
    {
        printf("bind() failed.\n");
        close(s);
        return -1;
    }

    /* 连接服务器 */
    if (connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1)
    {
	printf("error connecting to socket \n");  
        return -1;  
    };
    process_conn_client(s); /* 客户端处理过程 */
    close(s);   /* 关闭连接 */

    return 1;
}

void sig_proccess(int signo)
{
    printf("Catch a exit signal\n");
    exit(0);    
}

void sig_pipe(int sign)
{
    printf("Catch a SIGPIPE signal\n");

    /* 释放资源 */  
}

/* 客户端的处理过程 */
void process_conn_client(int s)
{
    ssize_t size = 0;
    char buffer[10240];  /* 数据的缓冲区 */

    for(;;){/* 循环处理过程 */
        /* 从标准输入中读取数据放到缓冲区buffer中 */
        size = read(0, buffer, 10240);
        if(size > 0){/* 读到数据 */
            write(s, buffer, size);     /* 发送给服务器 */
            size = read(s, buffer, 10240);/* 从服务器读取数据 */
            write(1, buffer, size);     /* 写到标准输出 */
        }
    }   
}

