#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            break;
          case 1:
            array_size = atoi(optarg);
            break;
          case 2:
            pnum = atoi(optarg);
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n", argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipe_fd[2];
  if (!with_files) {
    if (pipe(pipe_fd) == -1) {
      perror("pipe failed");
      return 1;
    }
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      active_child_processes += 1;
      if (child_pid == 0) {
        int begin = i * (array_size / pnum);
        int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);

        struct MinMax min_max = GetMinMax(array, begin, end);

        if (with_files) {
          char filename[255];
          snprintf(filename, 255, "min_max_%d.txt", i);
          FILE *file = fopen(filename, "w");
          if (file == NULL) {
            printf("Can't open file for writing\n");
            return 1;
          }
          fprintf(file, "%d %d\n", min_max.min, min_max.max);
          fclose(file);
        } else {
          close(pipe_fd[0]); // Закрываем дескриптор для чтения
          if (write(pipe_fd[1], &min_max, sizeof(struct MinMax)) == -1) {
            perror("write failed");
            return 1;
          }
          close(pipe_fd[1]); // Закрываем дескриптор для записи после записи
        }
        free(array);
        return 0;
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (!with_files) {
    close(pipe_fd[1]); // Закрываем дескриптор для записи в родительском процессе
  }

  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min, max;

    if (with_files) {
      char filename[255];
      snprintf(filename, 255, "min_max_%d.txt", i);
      FILE *file = fopen(filename, "r");
      if (file == NULL) {
        printf("Can't open file for reading\n");
        return 1;
      }
      if (fscanf(file, "%d %d", &min, &max) != 2) {
        perror("fscanf failed");
        return 1;
      }
      fclose(file);
    } else {
      struct MinMax min_max_part;
      if (read(pipe_fd[0], &min_max_part, sizeof(struct MinMax)) == -1) {
        perror("read failed");
        return 1;
      }
      min = min_max_part.min;
      max = min_max_part.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  if (!with_files) {
    close(pipe_fd[0]); // Закрываем дескриптор для чтения
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
