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
void HandleTCPClient(int client_sock, int *pipe);

static const int MAXPENDING =5;

int main(int argc, char* argv[]) {
	
	int pipe[2];
	if(pipe(pipe) < 0){
		DieWithUserMessage("Error make pipe\n");
	}

	int pid = fork()
	if( pid < 0) {
		DieWithUserMessage("Error fork\n");
	}
	else if( pid == 0) { //child
		close(pipe[0]);
		char buffer[BUFFSIZE];
		while(1) {
			while( !(cmd_desc = ConnectSocket("127.0.0.1",611)) );
			
			while(1) {
				int numByteRcvd = read(pipe[1],buffer,20);
				printf("				%s\n",buffer);
				if(buffer == "image chane")
					write(cmd_desc, buffer, numByteRcvd);
			}
			close(cmd_desc);
		}
		close(pipe[1]);
	}
	

	// parent
	close(pipe[1]);
	int serv_sock = MakeSocket(4547);

	while(1) { // 무한 반복
		int client_sock = AcceptSocket(serv_sock);
		HandleTCPClient(client_sock, pipe);		
	}
	close(pipe[0]);
}

void DieWithUserMessage(char * message){
	printf("%s",message);
	exit(-1);
}

void HandleTCPClient(int client_sock, int *pipe) {
	unsigned char buffer[BUFFSIZE];
	int image_file;
	int total_recv=0;
	int image_size;
	int flag = 1;
	int numByteRcvd=0;

	while(1) {
		if(flag >= 1){
			if(flag ==1) {	// recv image data size
				flag = 0;
				bzero(buffer,BUFFSIZE);
				numByteRcvd = read(client_sock, buffer, 20);
				image_size = atoi(buffer);
				printf("image size : %d\n",image_size);
				if(image_size == 0 || image_size > 10000) { // if image_size ie wrong value cloase socket
					break;
				}
				image_file = open("ready_img.jpg", O_WRONLY | O_CREAT | O_TRUNC | S_IRWXU | S_IRWXG | S_IRWXO);
			}
			else {	// send ack message to robot
				flag = 1;
				write(client_sock, "OK", 20);
			}
			
		}	
		else{  // recv image JPEG data
         	if(total_recv == image_size) {
         		total_recv =0;
         		flag = 2;
				close(image_file);
	    		printf("success recv image\n");
	    		
		        pid_t pid = fork();
		        if(pid == 0) {
			        execlp("mv","mv","ready_img.jpg","robot_view.jpg",NULL);
			        exit(0);
				}
				write(pipe[0],"image change",20);
			}
			else { 
				bzero(buffer,image_size);
				numByteRcvd = read(client_sock, buffer, image_size - total_recv);
				total_recv += numByteRcvd;
				write(image_file, buffer, numByteRcvd); 
				printf("write data size : %d",numByteRcvd);
			}
    	}

		if(numByteRcvd < 0)
		DieWithUserMessage("recv() failed");
	}

	close(client_sock); // 클라이언트 소켓 종료
}