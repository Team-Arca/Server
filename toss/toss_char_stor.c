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

#define BUFFSIZE 64
int FileSend(int socket_desc);


int main(int argc, char* argv[]){

    char* server_IP = "220.149.85.54";
    int server_port = 4547;
    char* robot_IP = "192.168.2.100";
    int robot_port = 9002;
    unsigned char buffer[BUFFSIZE];

    while(1){
        struct sockaddr_in server_addr;
        int socket_desc = socket(PF_INET, SOCK_STREAM, 0);
        struct sockaddr_in robot_addr;
        int robot_desc = socket(PF_INET, SOCK_STREAM, 0);
        
        if(socket_desc < 0 || robot_desc <0 ){
            printf("Cannot create socket\n");
            return -1;
        }

        bzero(&server_addr,sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(server_IP);
        server_addr.sin_port = htons(server_port);

        bzero(&robot_addr,sizeof(robot_addr));
        robot_addr.sin_family = AF_INET;
        robot_addr.sin_addr.s_addr = inet_addr(robot_IP);
        robot_addr.sin_port = htons(robot_port);

        if(connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr))<0) {
            printf("Cannot sever connect\n");
            exit(0);
        }
        if(connect(robot_desc, (struct sockaddr *)&robot_addr, sizeof(robot_addr))<0) {
            printf("Cannot robot connect\n");
            exit(0);
        }
        printf("wait recv() from robot\n");
        bzero(buffer,BUFFSIZE);
        int numByteRcvd = read(socket_desc, buffer, BUFFSIZE);
        if(numByteRcvd < 0){
            printf("recv() failed");
            return 0;
        }
        
    // 수신한 문자영을 전송하고 여분의 데이터를 스트링이 끝날 떄까지 수신
        while( numByteRcvd > 0) {
            printf("recv command %s\n",buffer);
            printf("wait send() to server\n");

            int numByteSent = write(robot_desc, buffer, numByteRcvd);
            if( numByteSent < 0){
                printf("send() failed");
                return 0;
            }
            else if (numByteSent != numByteRcvd) {
                printf("send() sent unexpected number of bytes");
            }
            bzero(buffer,BUFFSIZE);
            printf("toss command success\n");
            
            // 받을 수 있는 데이터가 더 남아 있는지 확인
            numByteRcvd = read(socket_desc, buffer, BUFFSIZE);
            if(numByteRcvd < 0)
                printf("recv() failed");
        }

        close(socket_desc);
        close(robot_desc);

    }
    return 0;
}


