#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int MakeSocket(int servPort);
void DieWithUserMessage(char * message);

static const int MAXPENDING =5;

int main(void) {

	recvImage_sock	= MakeSocket(4547); // recv Image from robot
	android_sock	= MakeSocket(9866); // recv command from android
	sendCmd_sock	= MakeSocket(9865);	// send command to robot
	IP_sock 		= MakeSocket(1127); // socket ip

	pid_t pid = fork();
    if(pid < 0) {
        DieWithUserMessage("fork error");
	}
	else if(pid == 0) { // child
		execlp("./recvImage", "./recvImage", recvImage_sock, android_sock, IP_sock);
	}
	
	pid_t pid2 = fork();
    if(pid2 < 0) {
        DieWithUserMessage("fork error");
	}
	else if(pid2 == 0) { // child
		execlp("./sendCmd", "./sendCmd", android_sock, sendCmd_sock, IP_sock);
	}
	
	execlp("./IP", "./IP", IP_sock, sendCmd_sock);

	return 0;
}

int MakeSocket(int servPort){
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

	return servSock;
}

void DieWithUserMessage(char * message) {
	printf("%s",message);
	exit(-1);
}