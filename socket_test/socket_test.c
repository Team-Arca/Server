#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 128
void DieWithUserMessage(char * message);
void HandleTCPClient(int recv_client_sock);

static const int MAXPENDING =5;

int main(int argc, char* argv[]) {
	//if (argc !=2 )
	//	DieWithUserMessage("Parameter(s)", "<Server Port>");

	//in_port_t servPort = atoi(argv[1]);
	in_port_t recv_port = 9866; // 안드로이드\
	
	//연결 요청을 처리하는 소켓 생성
	int recv_sock; 
	if((recv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		DieWithUserMessage("recv socket() failed");

	// 지역 주소 구조체 생성

	struct sockaddr_in recv_addr;					// 지역 주소
	memset(&recv_addr, 0, sizeof(recv_addr));			// 0으로 구조체 초기화
	recv_addr.sin_family = AF_INET;					// IPv4 주소 패밀리
	recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 호스트의 어떠한 IP로도 연결 요청 수락
	recv_addr.sin_port = htons(recv_port);			// 지역포트

	
	// 지역 주소에 바인드
	if(bind(recv_sock, (struct sockaddr*) &recv_addr, sizeof(recv_addr)) < 0)
		DieWithUserMessage("recv bind() failed");
	printf("bind\n");
	// 소켓이 들어오는 요청을 처리할 수 있도록 설정
	if(listen(recv_sock, MAXPENDING) < 0)
		DieWithUserMessage("recv listen() failed");
	printf("listen\n");

	while(1) { // 무한 반복
		struct sockaddr_in recv_client_addr;
		socklen_t recv_client_addr_len = sizeof(recv_client_addr);
	
		//클라이언트의 연결을 기다림
		printf("before accept\n");
		int recv_client_sock = accept(recv_sock, (struct sockaddr *) &recv_client_addr, &recv_client_addr_len);
		printf("after accept\n");
		if( recv_client_sock <0 )
			DieWithUserMessage("recv accept() failed");
		HandleTCPClient(recv_client_sock);		
	}
}

void DieWithUserMessage(char * message){
	printf("%s",message);
	exit(-1);
}

void HandleTCPClient(int recv_client_sock ) {
	char buffer[BUFSIZE];

	// 클라이언트로 부터 메시지 수신
	ssize_t numByteRcvd = recv(recv_client_sock, buffer, BUFSIZE, 0);
	printf("recv\n");
	if(numByteRcvd < 0)
		DieWithUserMessage("recv() failed");

	// 수신한 문자영을 전송하고 여분의 데이터를 스트링이 끝날 떄까지 수신
	while( numByteRcvd > 0) {
		printf(">>>>>> data : %s",buffer);
		ssize_t numByteSent = send(recv_client_sock, buffer, numByteRcvd, 0);
		if( numByteSent < 0)
			DieWithUserMessage("send() failed");
		else if (numByteSent != numByteRcvd)
			DieWithUserMessage("send() sent unexpected number of bytes");

		// 받을 수 있는 데이터가 더 남아 있는지 확인
		numByteRcvd = recv(recv_client_sock, buffer, BUFSIZE, 0);
		if(numByteRcvd < 0)
		DieWithUserMessage("recv() failed");
	}

	close(recv_client_sock); // 클라이언트 소켓 종료


}
