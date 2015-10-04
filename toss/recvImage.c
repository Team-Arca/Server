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
	int flag=1;
	// 클라이언트로 부터 메시지 수신
	bzero(buffer,BUFFSIZE);
	int numByteRcvd = read(clientSock, buffer, BUFFSIZE);
	//printf("image size is : %d\n",*size);
	if(numByteRcvd < 0)
		DieWithUserMessage("recv() failed");

	// 수신한 문자영을 전송하고 여분의 데이터를 스트링이 끝날 떄까지 수신
	while( numByteRcvd > 0) {
		printf("recive data size : %d\n",numByteRcvd);
		//클라이언트로 에코 메시지를 돌려보냄
		if(numByteRcvd == 20){
			printf("first accept\n");
		}
		else{
			if(flag == 1) {
				image_file = open("test.jpg", O_WRONLY | O_CREAT | O_TRUNC);
				flag = 0;
			}
			if(numByteRcvd % 1460 != 0)
				flag = 1;
				
			write(image_file, buffer, numByteRcvd);   
        //printf("[write image_fd!]\n");
        	if(flag ==1) {
        		close(image_file);
        		printf("success recv image\n");
		        pid_t pid = fork();
		        if(pid == 0) {
			        execlp("chmod","chmod","777","test.jpg",NULL);
			        exit(0);
			    }
			}
    	}
		// 받을 수 있는 데이터가 더 남아 있는지 확인
		bzero(buffer,BUFFSIZE);
		numByteRcvd = read(clientSock, buffer, BUFFSIZE);
		if(numByteRcvd < 0)
		DieWithUserMessage("recv() failed");
	}

	close(clientSock); // 클라이언트 소켓 종료
}
