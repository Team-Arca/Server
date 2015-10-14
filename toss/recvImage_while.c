#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#define BUFFSIZE 10000
void DieWithUserMessage(char * message);
void HandleTCPClient(int clientSock);

static const int MAXPENDING =5;

int main(int argc, char* argv[]) {

	in_port_t servPort = 4547;
	//연결 요청을 처리하는 소켓 생성
	int servSock; 
	if((servSock = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
		DieWithUserMessage("socket() failed");

	// 지역 주소 구조체 생성
	struct sockaddr_in servAddr;					// 지역 주소
	memset(&servAddr, 0, sizeof(struct sockaddr_in));			// 0으로 구조체 초기화
	servAddr.sin_family = AF_INET;					// IPv4 주소 패밀리
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// 호스트의 어떠한 IP로도 연결 요청 수락
	servAddr.sin_port = htons(servPort);	// 지역포트

	// 지역 주소에 바인드
	if(bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
		DieWithUserMessage("bind() failed");

	// 소켓이 들어오는 요청을 처리할 수 있도록 설정
	if(listen(servSock, MAXPENDING) < 0)
		DieWithUserMessage("listen() failed");

	while(1) { // 무한 반복
		struct sockaddr_in clientAddr;
		printf("wait accept\n");
		socklen_t clientAddrlen = sizeof(clientAddr);

		//클라이언트의 연결을 기다림
		int clientSock = accept(servSock, (struct sockaddr *) &clientAddr, &clientAddrlen);
		if( clientSock <0 )
			DieWithUserMessage("accept() failed");
		printf("client accept!!\n");
		HandleTCPClient(clientSock);		
	}
}

void DieWithUserMessage(char * message){
	printf("%s",message);
	exit(-1);
}

void HandleTCPClient(int clientSock) {
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
		switch(flag) {
		case 0:
			bzero(buffer,BUFFSIZE);
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
		        pid_t pid = fork();
		        if(pid == 0) {
		        	printf("name change\n");
			        if(store_state)
			        	execlp("mv","mv","ready_img1.jpg","robot_view.jpg",NULL);
			        else
			        	execlp("mv","mv","ready_img0.jpg","robot_view.jpg",NULL);
			        exit(0);
				}
				write(pipe[0],"image change",20);
			}
			else { 
				bzero(buffer,image_size);
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