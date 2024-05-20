#include "kernel/types.h"
#include "user/user.h"

#define RE 0 // pipe read port
#define WR 1 // pipe write port

int lpipe_fir(int lpipe[2], int *dst) {
    if (read(lpipe[RE], dst, sizeof(int)) == sizeof(int)) {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

void trans(int lpipe[2], int rpipe[2], int first) {
    int data;
    while (read(lpipe[RE], &data, sizeof(int)) == sizeof(int)) {
        if (data % first) {
            write(rpipe[WR], &data, sizeof(int));
        }
    }
    close(lpipe[RE]);
    close(rpipe[WR]);
}

void primes(int lpipe[2]) {
    close(lpipe[WR]);
    int first;
    if (lpipe_fir(lpipe, &first) == 0) {
        int fd[2];
        pipe(fd);
        trans(lpipe, fd, first);
        int pid = fork();
        
        if (pid == 0) {
            primes(fd);
        } else {
            close(fd[RE]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int init_fd[2];
    pipe(init_fd);

    for (int i = 2; i <= 35; i++) {
        write(init_fd[WR], &i, sizeof(int));
    }
    int pid = fork();

    if (pid == 0) {
        primes(init_fd);
    } else {
        close(init_fd[WR]);
        close(init_fd[RE]);
        wait(0);
    }
    exit(0);
} 