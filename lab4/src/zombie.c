#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t child_pid;

    child_pid = fork();

    if (child_pid > 0) {
        // Это родительский процесс
        sleep(10);  // Ожидаем некоторое время
    } else if (child_pid == 0) {
        // Это дочерний процесс
        printf("Дочерний процесс завершился\n");
        exit(0);
    } else {
        // Ошибка при создании процесса
        perror("fork");
        exit(1);
    }

    printf("Родительский процесс завершился\n");

    return 0;
}

