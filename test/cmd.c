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
#define NUM_SOCKET 2

void DieWithUserMessage(char * message);
int HandleTCPClient(int client_sock[]);

int main(int argc, char *argv[]) {
	int sock[NUM_SOCKET];
	int i=0;
	for(i=0; i< NUM_SOCKET; i++)
		sock[i] = MakeSocket(9865+i);
		// 0(9865) : recvimage
		// 1(9866) : android
		// 2(9867) : IP
		// 3(9868) : robot
	
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
	int pip[2];

	if(pipe(pip) < 0) {
		printf("pip() failed\n");
		return -1;
	}

	int pid = fork();
	if(pid < 0 ){
		printf("fork() faild\n");
		return -1; // re accept
	}
	else if(pid == 0) { // chiled  : if recv IP cmd from pip  send to IP
		close(pip[0]); // write close
		unsigned char buffer[BUFFSIZE];
		while(1) {
			bzero(buffer,BUFFSIZE);
			printf("							wait pip\n");
			int num_recv = read(pip[1],buffer,BUFFSIZE); // 동기
			int stateIP = atoi(buffer);
			while(stateIP){
				int num_send = write(client_sock[2],buffer,num_recv);
				num_recv = read(client_sock[2],buffer,BUFFSIZE);
				if( !strcmp(buffer, "finish"))
					stateIP = 0;
				else
					write(client_sock[3],buffer,num_recv);
			}
		}
	}
	else {
		close(pip[1]); // read close
		unsigned char buffer[BUFFSIZE];
		while(1) {
			printf("wait read from recvImage\n");
			//이미지 변경되는거 받기 
			int num_recv = read(client_sock[0],buffer,BUFFSIZE); // 동기
			if(num_recv <0){
					printf("read data error\n");
					exit(0);
			}
			printf("recv data form recvImage : %s\n",buffer);
			


			if( stateIP){
				printf("send ack to IP\n");
				//wirte(client_sock[2],buffer,num_recv); // 비동기
				//write(pip[0],b)
			}
			// send ack to android & recv cmd from android
			printf("send ack to android\n");
			buffer[0] = 'N';

			// 안드로이드에 알리기
			int num_send = write(client_sock[1],buffer,1); //동기
			if(num_send <0){
					printf("write data error\n");
					exit(0);
			}
			num_recv = read(client_sock[1],buffer,BUFFSIZE);
			if(num_recv <0){
					printf("read data error\n");
					exit(0);
			}

			printf("read data from android : %s\n", buffer);
		}

	}
	int i =0;
	for(i =0; i<NUM_SOCKET ; i++)
		close(client_sock[NUM_SOCKET]);

	return 0;
}
