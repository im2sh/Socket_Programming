#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0

void error_handling(const char *message);

int main(int argc, char *argv[])
{
	int serv_sock;
	int clnt_sock;
	char message[30];
	int str_len = 0;
	int option;
	socklen_t optlen, clnt_adr_size;

	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error"); 
	
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	
	clnt_adr_size=sizeof(clnt_adr);  
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_size);
	if(clnt_sock==-1)
		error_handling("accept() error");  

	while((str_len = read(clnt_sock,message,sizeof(message)))!=0)
	{
		write(clnt_sock, message, str_len);
		write(1, message, str_len);
	}
	close(serv_sock);
	close(clnt_sock);
	return 0;
}

void error_handling(const char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
