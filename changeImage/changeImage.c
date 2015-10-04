#include <stdio.h>
#include <unistd.h>
#include <string.h>>

int main(void) {
    string recv_image_path = "1,jpg";
    string robot_view_path = "/home/huro/asdfdsf";;
    

    execlp("cp","cp",recv_image_path,robot_view_path);

    return 0;
}
