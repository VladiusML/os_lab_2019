#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Ошибка при создании процесса
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Child process started with PID: %d\n", getpid());
        sleep(2); // Имитируем работу дочернего процесса
        printf("Child process finished\n");
        exit(0); // Завершение дочернего процесса
    } else {
        // Родительский процесс не вызывает wait и делает паузу
        printf("Parent process with PID: %d\n", getpid());
        sleep(10); // Достаточно долго, чтобы наблюдать за зомби-процессом
        printf("Parent process finishing\n");
    }

    return 0;
}
