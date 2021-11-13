#include "funcsinneed.h"

void doit(int fd) {
	int is_static;
	struct stat sbuf;
	char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
	char filename[MAXLINE], cgiargs[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, fd);
	Rio_readlineb(&rio, buf, MAXLINE);
	printf("Request headers:\n");
	printf("%s", buf);
	sscanf(buf, "%s %s %s", method, uri, version);
	if (strcasecmp(method, "GET")) {
		clienterror(fd, method, "501", "Not implemented", "We fail to implement this method");
		return;
	}
	read_requesthdrs(&rio);

	is_static = parse_uri(uri, filename, cgiargs);
	if (stat(filename, &sbuf) < 0) {
		clienterror(fd, filename, "404", "Not found", "We fail to locate the file");
		return;
	}
	if (is_static) {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
			clienterror(fd, filename, "403", "Forbidden", "We fail to read the file");
			return;
		}
		serve_static(fd, filename, sbuf.st_size);
	}
	else {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
			clienterror(fd, filename, "403", "Forbidden", "We fail to run the cgi program");
				return;
		}
		serve_dynamic(fd, filename, cgiargs);
	}
}

void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg) {
	char buf[MAXLINE], body[MAXBUF];

	sprintf(body,"<html><title>Tiny Error</title>");
	sprintf(body,"%s<body bgcolor =""ffffff"">\r\n",body);
	sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
	sprintf(body, "%s%s: %s\r\n", body, longmsg, cause);
	sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);
	
	sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-type: text/html\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
	Rio_writen(fd, buf, strlen(buf));
	Rio_writen(fd, body, strlen(body));
}

void read_requesthdrs(rio_t* rp) {
	char buf[MAXLINE];
	Rio_readlineb(rp, buf, MAXLINE);
	while (strcmp(buf, "\r\n")) {
		Rio_readlineb(rp, buf, MAXLINE);
		printf("%s", buf);
	}
	return;
}

int parse_uri(char* uri, char* filename, char* cgiargs) { 
	char* ptr;
	if (!strstr(uri, "cgi-bin")) { //如果没在给定url找到该文件则进行文件名替换，该行为静态内容
		strcpy(cgiargs, "");
		strcpy(filename, ".");
		strcat(filename, uri);
		if (uri[strlen(uri) - 1] == "/") strcat(filename, "home.html");
		return 1;
	}
	else { //该行为动态内容
		ptr = index(uri, '?'); //查找字符串首次出现的位置
		if (ptr) {
			strcpy(cgiargs, ptr + 1);
			*ptr = '\0';
		}
		else{
			strcpy(cgiargs, "");
		}
		strcpy(filename, ".");
		strcat(filename, uri);
		return 0;
	}
}

void serve_static(int fd, char* filename, int filesize) {
	int srcfd;
	char* srcp, filetype[MAXLINE], buf[MAXBUF];

	get_filetype(filename, filetype);//首先确定文件的类型
	sprintf(buf, "HTTP/1.0 200 OK\r\n"); //将输出累积到buf缓冲区中
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
	Rio_writen(fd, buf, strlen(buf)); //将缓冲区内内容输出到fd文件描述符中
	printf("Response headers:\n");
	printf("%s", buf);

	/*以下代码将所需文件发送到客户端*/
	/*这样操作主要是为了避免因关闭文件失败所导致的内存泄漏*/
	srcfd = Open(filename, O_RDONLY, 0); //打开所需文件
	srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); //将文件内容映射到一个私有只读虚拟内存区域
	Close(srcfd); //关闭该文件
	Rio_writen(fd, srcp, filesize); //将内存区域内内容输出
	Munmap(srcp, filesize); //释放该内存区域
}

void get_filetype(char* filename, char* filetype) {
	if (strstr(filename, ".html")) strcpy(filetype, "text/html");
	else if (strstr(filename, ".gif")) strcpy(filetype, "image/gif");
	else if (strstr(filename, ".jpg")) strcpy(filetype, "image/jpeg");
	else strcpy(filetype, "text/plain");
}
void serve_dynamic(int fd, char* filename, char* cgiargs) {
	char buf[MAXLINE], * emptylist[] = { NULL };

	/*返回第一部分的HTTP响应内容*/
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	Rio_writen(fd, buf, strlen(buf));
	sprintf(buf, "Server: Tiny Web Server\r\n");
	Rio_writen(fd, buf, strlen(buf));

	if (Fork() == 0) { /*开启子进程*/
	/* Real server would set all CGI vars here */
		setenv("QUERY_STRING", cgiargs, 1); //设置环境变量
		Dup2(fd, STDOUT_FILENO); /*将标准输出重定向至客户端*/
		Execve(filename, emptylist, environ); /* 运行CGI程序 */ 
	}
	wait(NULL); //父进程等待并回收子进程
}
