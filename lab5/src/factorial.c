#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

long long result = 1;
int k, pnum, mod;

void *calculate_factorial(void *arg) {
    long long local_result = 1;
    int thread_id = *(int *)arg;

    for (int i = thread_id + 1; i <= k; i += pnum) {
        local_result = (local_result * i) % mod;
    }

    pthread_mutex_lock(&mutex);
    result = (result * local_result) % mod;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
        {"k", required_argument, 0, 'k'},
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {0, 0, 0, 0}};

        int c = getopt_long(argc, argv, "k:p:m:", long_options, &option_index);

        if (c == -1) break;

        switch (c) {
        case 'k':
            sscanf(optarg, "%d", &k);
            break;
        case 'p':
            sscanf(optarg, "%d", &pnum);
            break;
        case 'm':
            sscanf(optarg, "%d", &mod);
            break;
        default:
            fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    // Проверка наличия всех необходимых параметров
    if (k == -1 || pnum == -1 || mod == -1) {
        fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pthread_t threads[pnum];
    int thread_ids[pnum];

    for (int i = 0; i < pnum; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, calculate_factorial, &thread_ids[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    printf("Factorial %d! mod %d is: %lld\n", k, mod, result);

    return 0;
}

