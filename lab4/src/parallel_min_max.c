#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <getopt.h>
#include <signal.h>

#include "find_min_max.h"
#include "utils.h"

pid_t* ch_pid;
int pnum;

void alarm_handler() {
for (int i = 0; i < pnum; i++){
    kill(ch_pid[i], SIGKILL);
  }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  pnum = -1;
  int timeout = -1;
  int i = 0;
  bool with_files = false;
  while (true) {
    int current_optind = optind ? optind : 1;
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
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
          if (seed <= 0) {
            printf("seed is a positive number\n");
            return 1;
          }
          break;

        case 1:
          array_size = atoi(optarg);
          if (array_size <= 0) {
              printf("array_size is a positive number\n");
              return 1;
            }
            break;

        case 2:
          pnum = atoi(optarg);
             if (pnum <= 0) {
            printf("pnum is a positive number\n");
            return 1;
            }
            break;
        
        case 3:
          timeout = atoi(optarg);
             if (timeout <= 0) {
            printf("timeout is a positive number\n");
            return 1;
            }
            break;

        case 4:
          with_files = true;
          break;

        defalut: printf("Index %d is out of options\n", option_index);
      }
      break;

      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default: printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1 || timeout == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
    argv[0]);
    return 1;
  }

  signal(SIGALRM, alarm_handler);
  
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);


  int active_child_processes = 0;
  int sub_array_size = array_size / pnum;

  ch_pid = malloc(sizeof(pid_t) * pnum);
  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int *array_fd_read = malloc(sizeof(int) * pnum);
  

  for (i = 0; i < pnum; i++) {
    int pipefd[2];

    if (!with_files) {
      pipe(pipefd);
    }

    pid_t child_pid = fork();
    ch_pid[i] = child_pid;

    if (child_pid >= 0) {
      active_child_processes += 1;
      
      if (child_pid == 0) {    
        struct MinMax min_max;
        if (i != pnum - 1) {
          min_max = GetMinMax(array, i * sub_array_size, (i + 1) * sub_array_size);
        } else min_max = GetMinMax(array, i * sub_array_size, array_size);
        
        printf("Prc[%03d]:\t%d\t%d\n", active_child_processes, min_max.min, min_max.max);
      
        if (with_files) {
          char* str = (char*)malloc(15 * sizeof(char));
          sprintf(str, "data_%d.txt", i);

          FILE * fp = fopen (str, "a");

          if (fp == 0)  {
            printf( "Could not open file\n" );
            return -1;
          } else {
              fwrite(&min_max, sizeof(struct MinMax), 1, fp);
          }
        } else {
            write(pipefd[1],&min_max,sizeof(struct MinMax));
        }
        return 0;
      } else {
        if (!with_files) {
          array_fd_read[i] = pipefd[0];
        }
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  int counter = 0;
  int status;
  pid_t wpid;

  alarm(timeout); // выполняет в вызвавшийего процесс доставку сигнала SIGALRM через timeout секунд
  for(int i = 0; i < pnum; i++){
    wpid = waitpid(ch_pid[i], &status, 0);
    if (WIFEXITED(status)) {
      printf("\nChild process with PID %d successfully completed.", wpid);
      counter += 1;
    } else if (WIFSIGNALED(status)) {
      printf("\nChild process with PID %d terminated by signal %d.", wpid, WTERMSIG(status));
    } else {
      printf("\nChild process with PID %d exited abnormally.", wpid);
    }
  }
  printf("\n");
  
  if (counter == pnum){
     struct MinMax min_max;
      min_max.min = INT_MAX;
      min_max.max = INT_MIN;

      for (i = 0; i < pnum; i++) {
        struct MinMax cur;
        cur.min = INT_MAX;
        cur.max = INT_MIN;

        if (with_files) {
            char* str = (char*)malloc(15 * sizeof(char));
            sprintf(str, "data_%d.txt", i);
            FILE * fp = fopen (str, "r");
            if (fp == 0) {
              printf( "Could not open file\n" );
              return 1;
            } else {
                fread(&cur, sizeof(struct MinMax), 1, fp);
            }
            fclose(fp);
        } else {
            read(array_fd_read[i], &cur, sizeof(struct MinMax));
        }

        if (cur.min < min_max.min) min_max.min = cur.min;
        if (cur.max > min_max.max) min_max.max = cur.max;
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
  } else {
      printf("Не все процессы завершились корректно");
      return 0;
  }
}
