#include "headersfromcsapp.h"

void doit(int fd); //����HTTP���������������
void read_requesthdrs(rio_t* rp); //����ֻ�����ڶ�ȡ�����Ա�ͷ\r\n��
int parse_uri(char* uri, char* filename, char* cgiargs);//�Ը�����urlʵ��Ĭ����Ϊ����д���
void serve_static(int fd, char* filename, int filesize);//�ṩ��̬������Ӧ
void get_filetype(char* filename, char* filetype);
void serve_dynamic(int fd, char* filename, char* cgiargs);
void clienterror(int fd, char* cause, char* errnum, char* shortmsg, char* longmsg); //��ͻ��˷��;��������Ϣ����Ӧ
