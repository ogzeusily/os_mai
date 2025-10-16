#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "win_api.h"

int main() {
    os_init_child_process();
    if (os_is_child_process()) {
        int fork_num = os_get_fork_number();
        printf("Child process %d started\n", fork_num);
        return 0;
    }
    
    char input[1024];
    if (!fgets(input, sizeof(input), stdin)) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    OsPipe pipe1, pipe_mid, pipe2;
    if (os_pipe(pipe1) == -1 || os_pipe(pipe_mid) == -1 || os_pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = os_fork();
    if (pid1 == -1) {
        perror("fork child1");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {  // child 1
        os_close(pipe1[1]); os_close(pipe_mid[0]); os_close(pipe2[0]); os_close(pipe2[1]);
        os_dup2(pipe1[0], STDIN_FILENO); 
        os_close(pipe1[0]);
        os_dup2(pipe_mid[1], STDOUT_FILENO); 
        os_close(pipe_mid[1]);
        os_exec("./child1", "child1");
        perror("child1 exec failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = os_fork();
    if (pid2 == -1) {
        perror("fork child2");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {  // child 2
        os_close(pipe1[0]); os_close(pipe1[1]); os_close(pipe_mid[1]); os_close(pipe2[0]);
        os_dup2(pipe_mid[0], STDIN_FILENO);  os_close(pipe_mid[0]);
        os_dup2(pipe2[1], STDOUT_FILENO);    os_close(pipe2[1]);
        os_exec("./child2", "child2");
        perror("child2 exec failed");
        exit(EXIT_FAILURE);
    }

    // parent
    os_close(pipe1[0]); os_close(pipe_mid[0]); os_close(pipe_mid[1]); os_close(pipe2[1]);
    os_write(pipe1[1], input, strlen(input));
    os_close(pipe1[1]);

    os_waitpid(pid1, NULL, 0);
    os_waitpid(pid2, NULL, 0);

    char result[1024];
    ssize_t n = os_read(pipe2[0], result, sizeof(result) - 1);
    if (n > 0) {
        result[n] = '\0';
        printf("%s", result);
    }
    os_close(pipe2[0]);

    return 0;
}