#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>

#include "find_min_max.h"
#include "utils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

int Sum(const struct SumArgs* args) {
    int sum = 0;

    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];
    }

    return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  int threads_num = 0;
  int array_size = 0;
  int seed = 0;


  struct option options[] = {{"threads_num", required_argument, 0, 0},{"seed", required_argument, 0, 0}, {"array_size", required_argument, 0, 0}, {0, 0, 0, 0}};
  int option_index = 0;
  while(1) {
      int c = getopt_long(argc, argv, "", options, &option_index);
      if (c == -1) break;

      switch (c) {
          case 0:
              switch(option_index) {
                  case 0:
                      threads_num = atoi(optarg);
                      break;
                  case 1:
                      seed = atoi(optarg);
                      break;
                  case 2:
                      array_size = atoi(optarg);
                      break;
              }
              break;
          default:
              printf("Index %d is not valid! \n", option_index);
              break;
      }
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  pthread_t threads[threads_num];
  struct SumArgs args[threads_num];
  int batch_size = array_size / threads_num;

  clock_t begin = clock();
  for (uint32_t i = 0; i < threads_num; i++) {
    // Calculate the segment of the array that each thread will sum
    args[i].array = array;
    args[i].begin = i * batch_size;
    args[i].end = (i == (threads_num - 1) ? array_size : (i+1) * batch_size);

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  free(array);
  printf("Total: %d\n", total_sum);
  printf("Time: %f\n", time_spent);

  return 0;
}
