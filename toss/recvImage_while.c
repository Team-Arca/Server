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
void HandleTCPClient(int clientSock, int adrSock);

static const int MAXPENDING =5;

int main(int argc, char* argv[]) {


	int servSock[2]; 
	servSock[0] = MakeSocket(4547); // recv image from AP
	servSock[1] = MakeSocket(9865);	// send ack to Android
	
	while(1) { // 무한 반복
		printf("wait accept\n");
		int clientSock 	= AcceptSocket(servSock[0]);
		printf("client accept!!\n");
		int adrSock		= AcceptSocket(servSock[1]);
		printf("Adroid accept!!\n");
		if( clientSock < 0 || adrSock < 0 )
			DieWithUserMessage("accept() failed");
		
		HandleTCPClient(clientSock, adrSock);		
	}
}

void DieWithUserMessage(char * message){
	printf("%s",message);
	exit(-1);
}

void HandleTCPClient(int clientSock, int adrSock) {
	unsigned char buffer[BUFFSIZE];
	int image_file;
	int total_recv=0;
	int image_size;
	int flag = 0;
	int numByteRcvd=0;
	int numByteSent=0;
	int connect_state =1;
	int store_state =0;
	while(connect_state) {
		bzero(buffer,BUFFSIZE);
		switch(flag) {
		case 0:
			numByteRcvd = read(clientSock, buffer, 20);
			if(numByteRcvd <=0){
				printf("don't read data\n");
				connect_state =0;
				break;
			}

			image_size = atoi(buffer);
			printf("image size : %d\n",image_size);

			if( !(image_size == 0 || image_size > 10000)) { // if image_size ie wrong value cloase socket
				if(store_state)
					image_file = open("ready_img0.jpg", O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU | S_IRWXG | S_IRWXO);
				else
					image_file = open("ready_img1.jpg", O_WRONLY | O_CREAT | O_TRUNC , S_IRWXU | S_IRWXG | S_IRWXO);
				flag = 1;
			}
			else
				printf("image size wrong... numByteRcvd : %d",numByteRcvd);
			
			break;

		case 1:
			if(total_recv == image_size) {
				store_state = (store_state+1) %2;
         		total_recv =0;
         		flag = 0;
				close(image_file);
	    		printf("success recv image\n");

	    		// send ack to Android
	    		sprintf(buffer,"%s","N");
	    		numByteSent = write(adrSock, buffer, 1); 
		        if(numByteSent <0){
					printf("don't write data\n");
					connect_state =0;
					break;
				}
				printf("send ack message\n");

		        pid_t pid = fork();
		        if(pid == 0) {
		        	printf("name change\n");
			        if(store_state)
			        	execlp("mv","mv","ready_img1.jpg","robot_view.jpg",NULL);
			        else
			        	execlp("mv","mv","ready_img0.jpg","robot_view.jpg",NULL);
			        exit(0);
				}
			}
			else { 
				numByteRcvd = read(clientSock, buffer, image_size - total_recv);
				if(numByteRcvd <=0){
					printf("recv data error\n");
					connect_state =0;
					break;
				}
				total_recv += numByteRcvd;
				numByteSent = write(image_file, buffer, numByteRcvd); 
				printf("write data size : %d\n",numByteRcvd);
			}
			break;
		}	
		if(numByteRcvd < 0)
			DieWithUserMessage("recv() failed");
	}
	printf("Close socket\n");
	close(clientSock); // 클라이언트 소켓 종료
}