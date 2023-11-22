#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execl("./program_1", "./program_1", "3", "3", NULL);

        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Дочерний процесс успешно завершил выполнение с кодом: %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс завершил выполнение с ошибкой\n");
        }
    }

    printf("Родительский процесс успешно завершил выполнение с кодом: 0\n");
    return 0;
}
