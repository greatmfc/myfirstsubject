#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "funcsinneed.h"

int main(int argc, char** argv) {
	int listenfd, connectfd; //监听和连接套接字
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr; //sockaddr_storage是通用结构体
	
	if (argc != 2) { //检测输入参数是否合法
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	listenfd = Open_listenfd(argv[1]); //创建监听套接字
	while (1) {
		clientlen = sizeof(clientaddr);
		connectfd = Accept(listenfd, (SA*)& clientaddr, &clientlen);
		getnameinfo((SA*)& clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accepted connection from (%s,%s)\n", hostname, port);
		doit(connectfd);
		close(connectfd);
	}
}
