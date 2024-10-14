#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    // Проверяем аргументы командной строки
    if (argc != 3) {
        printf("Usage: %s seed array_size\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        // Ошибка при создании нового процесса
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // Это дочерний процесс
        printf("Running sequential_min_max with seed=%s and array_size=%s\n", argv[1], argv[2]);

        // Выполняем программу sequential_min_max с аргументами
        execl("./sequential_min_max", "sequential_min_max", argv[1], argv[2], NULL);

        // Если exec не удастся, напечатаем ошибку
        perror("Exec failed");
        return 1;
    } else {
        // Это родительский процесс
        int status;
        waitpid(pid, &status, 0);  // Ждем завершения дочернего процесса

        if (WIFEXITED(status)) {
            printf("sequential_min_max finished with exit code %d\n", WEXITSTATUS(status));
        } else {
            printf("sequential_min_max did not terminate normally\n");
        }
    }

    return 0;
}
