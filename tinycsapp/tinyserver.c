#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "funcsinneed.h"

int main(int argc, char** argv) {
	int listenfd, connectfd; //�����������׽���
	char hostname[MAXLINE], port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr; //sockaddr_storage��ͨ�ýṹ��
	
	if (argc != 2) { //�����������Ƿ�Ϸ�
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	listenfd = Open_listenfd(argv[1]); //���������׽���
	while (1) {
		clientlen = sizeof(clientaddr);
		connectfd = Accept(listenfd, (SA*)& clientaddr, &clientlen);
		getnameinfo((SA*)& clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
		printf("Accepted connection from (%s,%s)\n", hostname, port);
		doit(connectfd);
		close(connectfd);
	}
}
