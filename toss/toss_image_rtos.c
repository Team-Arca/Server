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

#define BUFFSIZE 10000
int FileSend(int socket_desc);


int main(int argc, char* argv[]){

    char* server_IP = "220.149.85.54";
    //char* server_IP = "127.0.0.1";
    int server_port = 4547;
    char* robot_IP = "192.168.2.100";
    //char* robot_IP = "127.0.0.1";
    int robot_port = 9002;
    unsigned char buffer[BUFFSIZE];
    int connect_state = 1
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
        
        int flag=0;
        int numByteRcvd=0;
        int total_recv = 0;
        int image_size = 0;
        
        //char tmp[20];
        while(connect_state) {
            switch(flag) {
                case 0:
                    flag = 1;

                    // read image size from robot
                    bzero(buffer,BUFFSIZE);
                   
                    numByteRcvd = read(robot_desc, buffer, 20);
                    if(numByteRcvd < 0) {
                        printf("numByteRcvd wrong number\n");
                        connect_state =0;
                        break;
                    }
                    image_size = atoi(buffer);
                    printf("image size : %d\n",image_size);
                    if (image_size <= 0 | image_size >10000){
                        printf("image size wrong number\n");
                        connect_state =0;
                        break;
                    }

                    // send image size to server
                    int numByteSent = write(socket_desc, buffer, numByteRcvd);
                    if( numByteSent < 0){
                        printf("send() failed");
                        return 0;
                    }
                    else if (numByteSent != numByteRcvd) {
                        printf("send() sent unexpected number of bytes");
                    }
                    break;

                case 1:
                    if(total_recv == image_size) {
                        total_recv =0;
                        flag = 2;
                        printf("send image\n");
                    }
                    else { 
                        bzero(buffer, image_size);
                        numByteRcvd = read(robot_desc, buffer, image_size - total_recv);
                        total_recv += numByteRcvd;
                        write(socket_desc, buffer, numByteRcvd);
                    }
                    break;

                default:
                    flag = 0;
                    bzero(buffer,BUFFSIZE);
                    numByteRcvd = read(socket_desc, buffer, 20);
                    printf("server recv image? : %s\n",buffer);

                    // send image size to server
                    int numByteSent = write(robot_desc, buffer, numByteRcvd);
                    if( numByteSent < 0){
                        printf("send() failed");
                        return 0;
                    }
                    else if (numByteSent != numByteRcvd) {
                        printf("send() sent unexpected number of bytes");
                    }
                    break;
            }
            
            if(numByteRcvd < 0){
                printf("numByteRcvd error - socket close\n");
                break;
            }
        }


        close(socket_desc);
        close(robot_desc);
    }
    return 0;
}
