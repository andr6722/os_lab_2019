#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *thread1_function(void *arg) {
    pthread_mutex_lock(&mutex1);
    printf("Thread 1 locked mutex1\n");

    // Задержка для усиления вероятности deadlock
    sleep(1);

    pthread_mutex_lock(&mutex2);
    printf("Thread 1 locked mutex2\n");

    // Critical section

    pthread_mutex_unlock(&mutex2);
    printf("Thread 1 unlocked mutex2\n");

    pthread_mutex_unlock(&mutex1);
    printf("Thread 1 unlocked mutex1\n");

    return NULL;
}

void *thread2_function(void *arg) {
    pthread_mutex_lock(&mutex2);
    printf("Thread 2 locked mutex2\n");

    // Задержка для усиления вероятности deadlock
    sleep(1);

    pthread_mutex_lock(&mutex1);
    printf("Thread 2 locked mutex1\n");

    // Critical section

    pthread_mutex_unlock(&mutex1);
    printf("Thread 2 unlocked mutex1\n");

    pthread_mutex_unlock(&mutex2);
    printf("Thread 2 unlocked mutex2\n");

    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, thread1_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_create(&thread2, NULL, thread2_function, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_join(thread1, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    if (pthread_join(thread2, NULL) != 0) {
        perror("pthread_join");
        return 1;
    }

    printf("Program completed.\n");

    return 0;
}
