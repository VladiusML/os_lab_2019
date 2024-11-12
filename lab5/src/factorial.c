#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int k, pnum, mod;  
long long result = 1; 
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;  

void *factorial_part(void *arg) {
    int thread_id = *((int *)arg);
    int start = (k / pnum) * thread_id + 1;
    int end = (thread_id == pnum - 1) ? k : (k / pnum) * (thread_id + 1);

    long long local_result = 1;
    for (int i = start; i <= end; i++) {
        local_result = (local_result * i) % mod;
    }

    pthread_mutex_lock(&mut);
    result = (result * local_result) % mod;  
    pthread_mutex_unlock(&mut);

    return NULL;
}


int parse_arguments(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            k = atoi(argv[++i]);
        } else if (strncmp(argv[i], "--pnum=", 7) == 0) {
            pnum = atoi(argv[i] + 7);
        } else if (strncmp(argv[i], "--mod=", 6) == 0) {
            mod = atoi(argv[i] + 6);
        } else {
            return -1; 
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        return -1; 
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (parse_arguments(argc, argv) != 0) {
        printf("Usage: %s -k <k_value> --pnum=<pnum> --mod=<mod>\n", argv[0]);
        return 1;
    }

    if (pnum > k) {
        printf("Warning: pnum cannot be greater than k, adjusting pnum to k.\n");
        pnum = k;
    }

    pthread_t threads[pnum];
    int thread_ids[pnum];

    for (int i = 0; i < pnum; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, factorial_part, (void *)&thread_ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    printf("Result: %lld\n", result);
    return 0;
}
