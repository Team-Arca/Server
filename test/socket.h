#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

int MakeSocket(int port) ;
int AcceptSocket(int socket);
int ConnectSocket(char *IP, int port);
    
   
        
        
       

        

      
        
	