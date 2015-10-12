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
void DieWithUserMessage(char * message);
int HandleTCPClient(int client_sock[]);

int main(int argc, char *argv[]) {

	int sock[4];
	for(int i=0; i<4; i++)
		sock[i] = MakeSocket(9865+i);
		// 0(9865) : recvimage
		// 1(9866) : android
		// 2(9867) : IP
		// 3(9868) : robot
	
	while(1) { // 무한 반복
		int client_sock[4];
		for(int i=0; i<4; i++)
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
	int pipe[2];

	if(pipe(pipe) < 0) {
		printf("pipe() failed\n");
		return -1;
	}

	int pid = fork();
	if(pid < 0 ){
		printf("fork() faild\n");
		return -1; // re accept
	}
	else if(pid == 0) { // chiled
		close(pipe[0]) // write close
		unsigned char buffer[BUFFSIZE];
		while(1) {
			bzero(buffer,BUFFSIZE);
			int num_recv = read(pipe[1],buffer,BUFFSIZE); // 동기
			stateIP = atoi(buffer);
			while(stateIP){
				int num_send = wirte(client_sock[2],buffer,num_recv);
				num_recv = read(client_sock[2],buffer,BUFFSIZE);
				if(buffer == "finish")
					stateIP = 0;
				else
					write(client_sock[3],buffer.num_recv);
			}
		}
	}
	else {
		close(pipe[1]) // read close
		unsigned char buffer[BUFFSIZE];
		while(1) {
			int num_recv = read(client_sock[0],buffer,BUFFSIZE); // 동기
			if(stateIP)
				wirte(client_sock[2],buffer,num_recv); // 비동기
			int num_send = wirte(client_sock[1],buffer,num_recv); //동기
			num_recv = read(client_sock[1],buffer,BUFFSIZE);

			switch(buffer){
				case "NULL" : 
				case "IP":
					write(pipe[0],1,1); // IP on 비동기
				case "Move" :
					write(client_sock[3],buffer.num_recv); // 비동기
				default :
					break;
			}
		}

	}
	for(int i =0; i<4 ; i++)
		close(client_sock[i]);

	return 0;
}
