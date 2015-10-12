#include "socket.h"
int MakeSocket(int port) {
	printf("Make Socket called\n");
	
	int servSock; 
	if((servSock = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
		printf("socket() faild (port number : %d)\n",port);
		return -1;
	}

	// 지역 주소 구조체 생성
	struct sockaddr_in servAddr;					// 지역 주소
	memset(&servAddr, 0, sizeof(struct sockaddr_in));			// 0으로 구조체 초기화
	servAddr.sin_family = AF_INET;					// IPv4 주소 패밀리
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// 호스트의 어떠한 IP로도 연결 요청 수락
	servAddr.sin_port = htons(port);	// 지역포트

	// 지역 주소에 바인드
	if(bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		printf("bind() faild (port number : %d)\n",port);
		return -1;
	}

	// 소켓이 들어오는 요청을 처리할 수 있도록 설정
	if(listen(servSock, MAXPENDING) < 0) {
		printf("listen() faild (port number : %d)\n",port);
		return -1;
	}

	return servSock;
}

int AcceptSocket(int socket) {
	printf("Accept Socket called\n");
	struct sockaddr_in clientAddr;
	socklen_t clientAddrlen = sizeof(clientAddr);

	//클라이언트의 연결을 기다림
	int clientSock = accept(servSock, (struct sockaddr *) &clientAddr, &clientAddrlen);
	if( clientSock <0 ) {
		printf("accept() faild\n");
	}
		
	return clientSock;
}

int ConnectSocket(char *IP, int port) {
	struct sockaddr_in client_addr;
    int client_desc = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&client_addr,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(IP);
    client_addr.sin_port = htons(port);

    if(connect(client_desc, (struct sockaddr *)&client_addr, sizeof(client_addr))<0) {
        printf("Error ConnectSocket : %s(%d)\n", IP, port);
        return -1;
    }

    return client_desc;
}
   
    
   
        
        
       

        

      
        
	