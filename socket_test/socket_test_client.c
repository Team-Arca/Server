#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#define BUFFSIZE 128

void DieWithUserMessage(char * message);

int main(int argc, char* argv[]){

    char* server_IP = "220.149.85.54";
    int server_port = 9866;
    char buffer[BUFFSIZE];
 
    while(1){

        struct sockaddr_in server_addr;
        int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        
        if(socket_desc < 0){
            printf("Cannot create socket\n");
            return -1;
        }
        bzero(&server_addr,sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(server_IP);
        server_addr.sin_port = htons(server_port);

        if(connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) {
            printf("Cannot connect\n");
            exit(0);
        }
        
        printf("send :");
        scanf("%s", buffer);
        printf("data : %s\n", buffer);
        ssize_t numByteSent = send(socket_desc, buffer, BUFFSIZE, 0);
        
        ssize_t numByteRcvd = recv(socket_desc, buffer, BUFFSIZE, 0);
        printf("recv\n");
        if(numByteRcvd < 0)
            DieWithUserMessage("recv() failed");

        // 수신한 문자영을 전송하고 여분의 데이터를 스트링이 끝날 떄까지 수신
        while( numByteRcvd > 0) {
            printf(">>>>>> data : %s",buffer);
            
            printf("send :");
            scanf("%s", buffer);
            printf("data : %s\n", buffer);
            ssize_t numByteSent = send(socket_desc, buffer, BUFFSIZE, 0);
            // 받을 수 있는 데이터가 더 남아 있는지 확인
            numByteRcvd = recv(socket_desc, buffer, BUFFSIZE, 0);
            if(numByteRcvd < 0)
            DieWithUserMessage("recv() failed");
        }
        close(socket_desc);
    }
    return 0;
}

void DieWithUserMessage(char * message){
    printf("%s",message);
    exit(-1);
}
