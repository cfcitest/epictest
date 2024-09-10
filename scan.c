#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/vm_sockets.h>
#include <fcntl.h>
#include <sys/select.h>

#define MAX_BUF_SIZE 1024

int main(void) {
    int sock, ret;
    struct sockaddr_vm sa = {
        .svm_family = AF_VSOCK,
        .svm_cid = 2, // Assuming the host is CID 2
        .svm_port = 0,
    };

    char buf[MAX_BUF_SIZE] = {0};
    for (unsigned int port = 1; port < 65536; ++port) {
        sa.svm_port = port;
        sock = socket(AF_VSOCK, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }

        // Set the socket as non-blocking
        fcntl(sock, F_SETFL, O_NONBLOCK);

        ret = connect(sock, (struct sockaddr*)&sa, sizeof(sa));
        if (ret == 0 || (errno == EINPROGRESS)) {
            fd_set set;
            struct timeval timeout;

            FD_ZERO(&set); /* clear the set */
            FD_SET(sock, &set); /* add our file descriptor to the set */

            timeout.tv_sec = 0;
            timeout.tv_usec = 500000; // 0.5 seconds

            ret = select(sock + 1, &set, NULL, NULL, &timeout);
            if (ret > 0) {
                int bytes_read = read(sock, buf, MAX_BUF_SIZE - 1);
                if (bytes_read > 0) {
                    printf("Received from port %u: %s\n", port, buf);
                    memset(buf, 0, MAX_BUF_SIZE); // Clear buffer for next read
                }
            }
        }
        close(sock);
    }
    return 0;
}
