#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<arpa/inet.h>
#include	<sys/socket.h>
#define	BUF_SIZE	1024
#define	RLT_SIZE	4
#define	OPSZ	4
void error_handling(char *message);
int calculate(int opnum, int opnds[], char op); 

int main(int argc, char* argv[]){
	int serv_sock;
	int clnt_sock;
	int opmsg[BUF_SIZE];
	int op_len = 0;
	char op;
	int result, opnd_cnt, i;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	if(argc!=2){
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		error_handling("Socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("bind() error");

	if(listen(serv_sock,5)==-1)
		error_handling("listen() error");
	clnt_addr_size = sizeof(clnt_addr);
	
	for(int i = 0; i< 5; i++)
	{
		int op_len = 0;
		opnd_cnt = 0;
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
		if(clnt_sock == -1)
			error_handling("accept() error");
	
		read(clnt_sock, &opnd_cnt, 1);
		
		while(op_len < opnd_cnt)
			read(clnt_sock, &opmsg[op_len++], sizeof(int));

		read(clnt_sock, &op, sizeof(char));
		result = calculate(opnd_cnt, opmsg, op);

		write(clnt_sock, &result, sizeof(result));
		close(clnt_sock);
	}	
	close(serv_sock);
	return 0;
}

int calculate(int opnum, int opnds[], char op) {
	int result = opnds[0], i;
	switch(op){
		case '+':
			for(i = 1; i < opnum; i++)
				result += opnds[i];
			break;
		case '-':
			for(i = 1; i < opnum; i++)
				result -= opnds[i];
		case '*':
			for(i = 1; i < opnum; i++)
				result *= opnds[i];
	}
	return result;
}

void error_handling(char *message){
	fputs(message,stderr);
	fputc('\n', stderr);
	exit(1);
}
