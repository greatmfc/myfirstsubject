#include "headersfromcsapp.h"

void doit(int fd); //根据HTTP方法处理具体事务
void read_requesthdrs(rio_t* rp); //仅仅只是用于读取并忽略报头\r\n段
int parse_uri(char* uri, char* filename, char* cgiargs);//对给定的url实行默认行为或进行处理
void serve_static(int fd, char* filename, int filesize);//提供静态服务响应
void get_filetype(char* filename, char* filetype);
void serve_dynamic(int fd, char* filename, char* cgiargs);
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg); //向客户端发送具体错误信息以响应
