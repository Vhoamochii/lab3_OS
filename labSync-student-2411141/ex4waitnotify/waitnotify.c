#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_WRITERS 3
#define NUM_READERS 2
#define ITEMS_PER_WRITER 5

int buffer[BUFFER_SIZE];
int count = 0;

pthread_mutex_t mutex;
pthread_cond_t cond_not_full;   /* Condition for writers */
pthread_cond_t cond_not_empty;  /* Condition for readers */

void write_item(int item) {
    pthread_mutex_lock(&mutex);

    while (count == BUFFER_SIZE) { /* Buffer is full */
        printf("Writer waiting (buffer full)...\n");
        pthread_cond_wait(&cond_not_full, &mutex);
    }

    /* Add item to buffer */
    buffer[count++] = item;
    printf("Writer added item %d (buffer count: %d)\n", item, count);

    /* Signal that buffer is not empty */
    pthread_cond_signal(&cond_not_empty);

    pthread_mutex_unlock(&mutex);
}

int read_item() {
    pthread_mutex_lock(&mutex);

    while (count == 0) { /* Buffer is empty */
        printf("Reader waiting (buffer empty)...\n");
        pthread_cond_wait(&cond_not_empty, &mutex);
    }

    /* Retrieve item from buffer */
    int item = buffer[--count];
    printf("Reader retrieved item %d (buffer count: %d)\n", item, count);

    /* Signal that buffer is not full */
    pthread_cond_signal(&cond_not_full);

    pthread_mutex_unlock(&mutex);

    return item;
}

void* writer_thread(void* arg) {
    int id = *(int*)arg;
    int i;
    
    for (i = 0; i < ITEMS_PER_WRITER; i++) {
        int item = id * 100 + i;  /* Generate unique item based on thread id */
        write_item(item);
        usleep(100000);  /* Sleep 100ms */
    }
    
    return NULL;
}

void* reader_thread(void* arg) {
    int id = *(int*)arg;
    int i;
    
    for (i = 0; i < (NUM_WRITERS * ITEMS_PER_WRITER) / NUM_READERS; i++) {
        int item = read_item();
        usleep(150000);  /* Sleep 150ms (slower than writers) */
    }
    
    return NULL;
}

int main() {
    pthread_t writers[NUM_WRITERS];
    pthread_t readers[NUM_READERS];
    int writer_ids[NUM_WRITERS];
    int reader_ids[NUM_READERS];
    int i;

    /* Initialize mutex and condition variables */
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_not_full, NULL);
    pthread_cond_init(&cond_not_empty, NULL);

    printf("Starting producer-consumer with condition variables\n");
    printf("Buffer size: %d, Writers: %d, Readers: %d\n\n", 
           BUFFER_SIZE, NUM_WRITERS, NUM_READERS);

    /* Create writer threads */
    for (i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]);
    }

    /* Create reader threads */
    for (i = 0; i < NUM_READERS; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader_thread, &reader_ids[i]);
    }

    /* Wait for all writers to finish */
    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }

    /* Wait for all readers to finish */
    for (i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }

    /* Cleanup */
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_not_full);
    pthread_cond_destroy(&cond_not_empty);

    printf("\nAll threads completed successfully!\n");

    return 0;
}

