#include <stdio.h>
#include <unistd.h>
#include <string.h>>

int main(void) {
    string recv_image_path[2] = {"1.jpg", "2,jpg"};
    string robot_view_path = "/home/huro/asdfdsf";
    int index=0;
    while(1) {
        sleep(1000);
        execlp("cp","cp",recv_image_path[++index%2],robot_view_path);
    }
    return 0;
}
