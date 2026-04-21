#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define CHECK_INTERVAL 5
#define MAX_ERRORS 3
#define RECOVERY_THRESHOLD 10

pthread_mutex_t lock;
int finished = 0;
int error_count = 0;
int resource_value = 0;
int total_operations = 0;

/* Simulated safety check - checks if resource is in valid state */
int is_safe() {
    /* Check if resource value is within acceptable range */
    if (resource_value < 0 || resource_value > 100) {
        printf("[DETECTOR] Unsafe state detected! Resource value: %d\n", resource_value);
        return -1; /* Unsafe detected */
    }
    
    /* Check if error count is too high */
    if (error_count >= MAX_ERRORS) {
        printf("[DETECTOR] Too many errors detected! Error count: %d\n", error_count);
        return -1; /* Unsafe detected */
    }
    
    return 0; /* Safe */
}

/* Recovery action when unsafe state is detected */
void perform_recovery() {
    printf("[RECOVERY] Taking corrective action...\n");
    
    /* Reset resource to safe state */
    if (resource_value < 0) {
        printf("[RECOVERY] Resource value too low (%d), resetting to 0\n", resource_value);
        resource_value = 0;
    } else if (resource_value > 100) {
        printf("[RECOVERY] Resource value too high (%d), resetting to 100\n", resource_value);
        resource_value = 100;
    }
    
    /* Reset error count if threshold reached */
    if (error_count >= MAX_ERRORS) {
        printf("[RECOVERY] Resetting error count from %d to 0\n", error_count);
        error_count = 0;
    }
    
    printf("[RECOVERY] System recovered successfully\n");
}

void* periodical_detector(void *arg) {
    int check_count = 0;
    
    printf("[DETECTOR] Periodic detector started (checking every %d seconds)\n", CHECK_INTERVAL);
    
    while (1) {
        sleep(CHECK_INTERVAL); /* Periodic check every 5 seconds */
        
        pthread_mutex_lock(&lock);
        
        check_count++;
        printf("\n[DETECTOR] Performing safety check #%d...\n", check_count);
        printf("[DETECTOR] Current state - Resource: %d, Errors: %d, Operations: %d\n", 
               resource_value, error_count, total_operations);
        
        if (is_safe() != 0) {
            /* Abnormal detected! Taking corrective action */
            perform_recovery();
        } else {
            printf("[DETECTOR] System is in safe state\n");
        }
        
        /* Check if main work is finished */
        if (finished) {
            printf("[DETECTOR] Work completed, terminating detector\n");
            pthread_mutex_unlock(&lock);
            break; /* Break after checking finished flag */
        }
        
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}

/* Worker thread that performs operations and may cause errors */
void* worker_thread(void* arg) {
    int id = *(int*)arg;
    int i;
    
    printf("[WORKER %d] Started\n", id);
    
    for (i = 0; i < 20; i++) {
        pthread_mutex_lock(&lock);
        
        /* Simulate random operations that may cause unsafe states */
        int operation = rand() % 100;
        
        if (operation < 10) {
            /* Simulate error condition */
            error_count++;
            printf("[WORKER %d] Error occurred! (error_count: %d)\n", id, error_count);
        } else if (operation < 30) {
            /* Increase resource value */
            resource_value += rand() % 20;
            printf("[WORKER %d] Increased resource to %d\n", id, resource_value);
        } else if (operation < 50) {
            /* Decrease resource value */
            resource_value -= rand() % 20;
            printf("[WORKER %d] Decreased resource to %d\n", id, resource_value);
        } else {
            /* Normal operation */
            resource_value = (resource_value + rand() % 10) % 80 + 10;
        }
        
        total_operations++;
        
        pthread_mutex_unlock(&lock);
        
        usleep(500000); /* Sleep 500ms between operations */
    }
    
    printf("[WORKER %d] Completed\n", id);
    return NULL;
}

int main() {
    pthread_t detector_tid;
    pthread_t worker_tids[3];
    int worker_ids[3];
    int i;
    
    /* Initialize random seed */
    srand(time(NULL));
    
    /* Initialize mutex */
    pthread_mutex_init(&lock, NULL);
    
    printf("=== Periodic Detection with Recovery System ===\n");
    printf("Starting system with 3 worker threads and 1 detector thread\n\n");
    
    /* Create periodic detector thread */
    pthread_create(&detector_tid, NULL, periodical_detector, NULL);
    
    /* Create worker threads */
    for (i = 0; i < 3; i++) {
        worker_ids[i] = i + 1;
        pthread_create(&worker_tids[i], NULL, worker_thread, &worker_ids[i]);
    }
    
    /* Wait for all workers to complete */
    for (i = 0; i < 3; i++) {
        pthread_join(worker_tids[i], NULL);
    }
    
    /* Signal detector that work is finished */
    pthread_mutex_lock(&lock);
    finished = 1;
    pthread_mutex_unlock(&lock);
    
    printf("\n[MAIN] All workers completed, waiting for detector to finish...\n");
    
    /* Wait for detector to finish */
    pthread_join(detector_tid, NULL);
    
    /* Cleanup */
    pthread_mutex_destroy(&lock);
    
    printf("\n=== System shutdown complete ===\n");
    printf("Final statistics:\n");
    printf("  Total operations: %d\n", total_operations);
    printf("  Final resource value: %d\n", resource_value);
    printf("  Final error count: %d\n", error_count);
    
    return 0;
}