#include <stdio.h>
#include <unistd.h>
#include <string.h>

void Exec(char *cmd);

int main(void) {
    
	Exec("./toss_image_rtos");
	Exec("./toss_char_stor");

    return 0;
}

void Exec(char *cmd) {
	pid_t pid = fork();
    if(pid == 0) {
        execlp(cmd,cmd,NULL);
        exit(0);
    }
}