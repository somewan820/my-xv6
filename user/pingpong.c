#include "kernel/types.h"
#include "user/user.h"

#define RE 0 // pipe read port
#define WR 1 // pipe write port

int main(int argc, char *argv[]) {
    char byte = 'A'; //the byte will be sent
    
    int p_fd[2];
    int c_fd[2];
    pipe(p_fd);
    pipe(c_fd);

    int pid = fork();
    int exit_status = 0;

    if (pid < 0) {
        fprintf(2, "fork() error!\n");
        close(c_fd[RE]);
        close(c_fd[WR]);
        close(p_fd[RE]);
        close(p_fd[WR]);       
        exit(1);
    } else if (pid == 0) { // child process
        close(c_fd[RE]);
        close(p_fd[WR]);

        if (read(p_fd[RE], &byte, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child read() error!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if (write(c_fd[WR], &byte, sizeof(char)) != sizeof(char)) {
            fprintf(2, "child write() error!\n");
            exit_status = 1;
        }

        close(c_fd[WR]);
        close(p_fd[RE]);
        exit(exit_status);
    } else { //parent process
        close(p_fd[RE]);
        close(c_fd[WR]);
        
        if (write(p_fd[WR], &byte, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent write() error!\n");
            exit_status = 1;
        }
        
        if (read(c_fd[RE], &byte, sizeof(char)) != sizeof(char)) {
            fprintf(2, "parent read() error!\n");
            exit_status = 1;
        } else {
            fprintf(1, "%d: received pong\n", getpid());
        }
        
        close(c_fd[RE]);
        close(p_fd[WR]);        
        exit(exit_status);
    }
}