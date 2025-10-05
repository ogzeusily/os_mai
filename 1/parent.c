#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linux/os_linux.h"

int main() {
    char input[1024];
    printf("Введите строку: ");
    if (!fgets(input, sizeof(input), stdin)) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    OsPipe pipe1, pipe_mid, pipe2;
    if (pipe_(pipe1) == -1 || pipe_(pipe_mid) == -1 || pipe_(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork_();
    if (pid1 == -1) {
        perror("fork child1");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {  // child 1
        close_(pipe1[1]); close_(pipe_mid[0]); close_(pipe2[0]); close_(pipe2[1]);
        dup2_(pipe1[0], STDIN_FILENO); 
        close_(pipe1[0]);
        dup2_(pipe_mid[1], STDOUT_FILENO); 
        close_(pipe_mid[1]);
        exec_("./build/child1", "child1");
        perror("child1 exec failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork_();
    if (pid2 == -1) {
        perror("fork child2");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {  // child 2
        close_(pipe1[0]); close_(pipe1[1]); close_(pipe_mid[1]); close_(pipe2[0]);
        dup2_(pipe_mid[0], STDIN_FILENO);  close_(pipe_mid[0]);
        dup2_(pipe2[1], STDOUT_FILENO);    close_(pipe2[1]);
        exec_("./build/child2", "child2");
        perror("child2 exec failed");
        exit(EXIT_FAILURE);
    }

    // parent
    close_(pipe1[0]); close_(pipe_mid[0]); close_(pipe_mid[1]); close_(pipe2[1]);
    write_(pipe1[1], input, strlen(input));
    close_(pipe1[1]);

    waitpid_(pid1, NULL, 0);
    waitpid_(pid2, NULL, 0);

    char result[1024];
    ssize_t n = read_(pipe2[0], result, sizeof(result) - 1);
    if (n > 0) {
        result[n] = '\0';
        printf("Результат: %s", result);
    }
    close_(pipe2[0]);

    return 0;
}