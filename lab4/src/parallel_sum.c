#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "utils.h"
#include "sum.h"  // Подключаем заголовочный файл библиотеки

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    int *sum = malloc(sizeof(int)); // Выделяем память для результата
    *sum = Sum(sum_args); // Вызываем функцию для суммирования
    return (void *)sum; // Возвращаем указатель на результат
}

int main(int argc, char **argv) {
    if (argc != 7) {
        printf("Usage: %s --threads_num num --seed num --array_size num\n", argv[0]);
        return 1;
    }

    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;

    // Обработка аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--threads_num") == 0) {
            threads_num = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--seed") == 0) {
            seed = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--array_size") == 0) {
            array_size = atoi(argv[++i]);
        }
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed); // Генерация массива

    struct SumArgs args[threads_num];
    pthread_t threads[threads_num];

    // Разделение работы между потоками
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * (array_size / threads_num);
        args[i].end = (i + 1) * (array_size / threads_num);
        if (i == threads_num - 1) { // Обработка последнего потока
            args[i].end = array_size;
        }
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            free(array);
            return 1;
        }
    }

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int *sum;
        pthread_join(threads[i], (void **)&sum);
        total_sum += *sum; // Суммируем результаты
        free(sum); // Освобождаем память для результата
    }

    free(array);
    printf("Total: %d\n", total_sum);
    return 0;
}
