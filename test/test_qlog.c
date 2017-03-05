#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

void usage() {
    printf("./hello DEVICE_PATH\n");
}

int main(int argc, const char** argv) {
    int fd, ret;
    const char* dname;
    char buf[255];

    if (argc < 2) {
        usage();
        return -1;
    }

    dname = argv[1];

    printf("Starting device test for '%s'\n", dname);
    
    fd = open(dname, O_RDONLY);
    if (fd < 0) {
        perror("Unable to open device");
        return -1;
    }

    ret = read(fd, buf, 255);
    if (ret < 0) {
        perror("Failed to read from device");
        close(fd);
        return -1;
    }

    close(fd);
    printf("Read Message: '%s'\n", buf);
    
    return 0;
}
