
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 1024

#define TTL 64

int main(int argc, char * argv[]) {
    int send_s, recv_s;
    int pid;
    unsigned int option = 1;
    struct sockaddr_in mcast_group;
    struct ip_mreq mreq;
    char line[MAXLINE];
    char name[10];
    int n, len;
    int time_live = TTL;
    int no = 1;

    if(argc != 4) {
        printf("Usage : %s <multicast_address> <port> <My_name> \n", argv[0]);
        exit(0);
    }
    sprintf(name, "[%s]", argv[3]);

    memset(&mcast_group, 0, sizeof(mcast_group));
    mcast_group.sin_family = AF_INET;
    mcast_group.sin_port = htons(atoi(argv[2]));
    mcast_group.sin_addr.s_addr = inet_addr(argv[1]);
    if ( (recv_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("create receive socket error!!\n");
        exit(0);
    }
    
   
    mreq.imr_multiaddr = mcast_group.sin_addr;
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(recv_s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))<0){
        printf("add membership error!!\n");
        exit(0);
    }
    
    if (setsockopt(recv_s, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) <0) {
        printf("reuse setsockopt() error!!\n");
        exit(0);
    }
    
    if (bind(recv_s, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) {
        printf("bind() error!!\n");
        exit(0);
    }

    
    if ((send_s=socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("create send socket error!!\n");
        exit(0);
    }

    setsockopt(send_s, IPPROTO_IP, IP_MULTICAST_LOOP, &no, sizeof(no));
    setsockopt(send_s, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));

    
    if ((pid=fork()) < 0) {
        printf("fork() error!!\n");
        exit(0);
    
    } else if (pid == 0) {
        struct sockaddr_in from;
        char message[MAXLINE+1];
        for (;;) {
        len = sizeof(from);
        if ((n=recvfrom(recv_s, message, MAXLINE, 0, (struct sockaddr*)&from, &len))<0) {
            printf("recvfrom() error!\n");
            exit(0);
        }
        message[n] = 0;
        printf("Received Message: %s", message);
        }
   
    } else {
        char message [MAXLINE+1], line[MAXLINE+1];
        while (fgets(message, MAXLINE, stdin) != NULL) {
            sprintf(line, "%s %s", name, message);
            len = strlen(line);
            if((!strcmp(message, "q\n") || !strcmp(message,"Q\n"))){
                if(setsockopt(recv_s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0){
                    printf("add membership error!!\n");
                    exit(0);
                }
                exit(0);
            }
            if(sendto(send_s, line, strlen(line), 0, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < len) {
                printf("sendto() error!!\n");
                exit(0);
            }
        }
    }
    return 0;
}
