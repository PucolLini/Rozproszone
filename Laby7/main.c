#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 10
#define MAX_PRIMES 1000

typedef struct {
    int id;
    int numberOfPrimes;
    int primes[MAX_PRIMES];
    int lowerBound;
    int upperBound;
} ThreadData;

int IsPrime(int number) {
    if (number <= 1) return 0;

    for (int i=2; i*i<number; i++){
        if (number % i == 0) {
            return 0;
        }
    }
    return 1;
}

void *find_primes(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int id = data->id;
    int start = data->lowerBound;
    int end = data->upperBound;
    data->numberOfPrimes = 0;

    for (int i = start; i < end; i++) {
        if (IsPrime(i)) {
            data->primes[data->numberOfPrimes++] = i;
            FILE *results = fopen("primes.log", "a");
            fprintf(results, "Thread %d found prime: %d\n", id, i);
            fclose(results);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <numberOfThreads> <lowerBound> <upperBound>\n", argv[0]);
        return 1;
    }

    int numberOfThreads = atoi(argv[1]);
    int lowerBound = atoi(argv[2]);
    int upperBound = atoi(argv[3]);

    if (numberOfThreads < 1 || numberOfThreads > MAX_THREADS) {
        printf("Wrong number of threads (must be between 1 and %d)\n", MAX_THREADS);
        return 1;
    }

    if (lowerBound >= upperBound) {
        printf("Wrong bounds (lower bound < upper bound)\n");
        return 1;
    }

    FILE *results = fopen("primes.log", "w");
    fclose(results);

    pthread_t threads[numberOfThreads];
    ThreadData thread_data[numberOfThreads];

    for (int i = 0; i < numberOfThreads; i++) {
        thread_data[i].id = i;
        thread_data[i].lowerBound = (int)((double)i * (upperBound-lowerBound)/numberOfThreads + lowerBound);
        thread_data[i].upperBound = (int)((double)(i+1) * (upperBound-lowerBound)/numberOfThreads + lowerBound);
        pthread_create(&threads[i], NULL, find_primes, &thread_data[i]);
    }

    for (int i = 0; i < numberOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    int numberOfAllFoundedNumbers = 0;

    printf("ALL FOUNDED PRIME NUMBERS:\n");
    for (int i = numberOfThreads - 1; i >= 0; i--) {
        for (int j = thread_data[i].numberOfPrimes - 1; j >= 0; j--) {
            printf("%d ", thread_data[i].primes[j]);
            numberOfAllFoundedNumbers++;
        }
    }
    printf("\nSTATISTICS:\n");
    for (int i = 0; i < numberOfThreads; i++) {
        float percentOfAllNumbers = (float)thread_data[i].numberOfPrimes/numberOfAllFoundedNumbers;
        printf("Thread %d found %d prime numbers, which is %f%% of all founded prime numbers.\n", i, thread_data[i].numberOfPrimes, percentOfAllNumbers);
    }


    return 0;
}
