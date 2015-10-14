#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include "socket.h"

#define BUFFSIZE 10000
#define NUM_SOCKET 1

void DieWithUserMessage(char * message);
int HandleTCPClient(int client_sock[]);

int main(int argc, char *argv[]) {
	int sock[NUM_SOCKET];
	int i=0;
	for(i=0; i< NUM_SOCKET; i++)
		sock[i] = MakeSocket(9866+i);
		// 0(9866) : recv cmd from android
		// 1(9867) : recv&send cmd to IP
		// 2(9868) : send cmd to Robot

	
	while(1) { // 무한 반복
		int client_sock[NUM_SOCKET] ;
		int i;
		for(i=0; i< NUM_SOCKET; i++)
			client_sock[i] = AcceptSocket(sock[i]);
		HandleTCPClient(client_sock);		
	}


	return 0;
}

void DieWithUserMessage(char * message){
	printf("%s",message);
	exit(-1);
}

int HandleTCPClient(int client_sock[]){
	int num_recv;
	int num_send;
	int cmd=0;
	unsigned char buffer[BUFFSIZE];
	while(1) {
		printf("wait cmd from Robot\n");
		
		num_recv = read(client_sock[0],buffer,BUFFSIZE); // 동기
		if(num_recv <0){
				printf("read data error\n");
				exit(0);
		}
		printf("recv cmd from Robot : %s\n",buffer);

		cmd = atoi(buffer);
		// switch(cmd){
		// case 0:
		// case 1:
		// case 2:
		// 	num_send = write(client_sock[2],buffer,num_recv);
		// 	if(num_send <0){
		// 		printf("send data error\n");
		// 		exit(0);
		// 	}
		// 	break;
		// case 22:
		// case 23:
		// 	num_send = write(client_sock[1],buffer, num_recv);
		// 	if(num_send <0){
		// 		printf("send data error\n");
		// 		exit(0);
		// 	}
		// 	int cnt =5;
		// 	while(cnt --) {
		// 		num_recv = read(client_sock[1], buffer,BUFFSIZE)
		// 		if(num_recv <0){
		// 			printf("read data error\n");
		// 			exit(0);
		// 		}
		// 		num_send = write(client_sock[2],buffer,num_recv);
		// 		if(num_send <0){
		// 			printf("send data error\n");
		// 			exit(0);
		// 		}

		// 	}
		// 	break;
		// default:
		// 	printf("wrong cmd\n");
		// 	break;
		// }
		
		
	}
	int i =0;
	for(i =0; i<NUM_SOCKET ; i++)
		close(client_sock[NUM_SOCKET]);

	return 0;
}
