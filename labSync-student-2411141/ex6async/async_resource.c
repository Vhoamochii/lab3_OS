#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_RESOURCES 10
#define NUM_PROCESSES 5

/* Process request structure */
typedef struct {
    int id;                      /* Process ID */
    int requested_resources;     /* Resources the process requests */
    void (*callback)(int);       /* Callback function for resource allocation */
} process_request_t;

int available_resources = NUM_RESOURCES;
pthread_mutex_t resource_lock;
pthread_cond_t resource_cond;

/* Callback mechanism: invoked when resource becomes available */
void resource_callback(int process_id) {
    printf("[CALLBACK] Process %d received resources and is now executing\n", process_id);
    
    /* Simulate work with the allocated resources */
    sleep(2);
    
    printf("[CALLBACK] Process %d completed work\n", process_id);
}

/* Non-blocking request management */
void* resource_manager(void *arg) {
    process_request_t *request = (process_request_t *)arg;
    
    pthread_mutex_lock(&resource_lock);
    
    printf("[PROCESS %d] Requesting %d resources (available: %d)\n", 
           request->id, request->requested_resources, available_resources);
    
    /* Non-blocking allocation - wait if not enough resources */
    while (request->requested_resources > available_resources) {
        printf("[PROCESS %d] Waiting for resources...\n", request->id);
        pthread_cond_wait(&resource_cond, &resource_lock);
    }
    
    /* Allocate resources */
    available_resources -= request->requested_resources;
    printf("[PROCESS %d] Allocated %d resources (remaining: %d)\n", 
           request->id, request->requested_resources, available_resources);
    
    pthread_mutex_unlock(&resource_lock);
    
    /* Invoke callback to notify process */
    request->callback(request->id);
    
    /* Release resources after work is done */
    pthread_mutex_lock(&resource_lock);
    available_resources += request->requested_resources;
    printf("[PROCESS %d] Released %d resources (available: %d)\n", 
           request->id, request->requested_resources, available_resources);
    
    /* Broadcast to all waiting processes */
    pthread_cond_broadcast(&resource_cond);
    
    pthread_mutex_unlock(&resource_lock);
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_PROCESSES];
    process_request_t requests[NUM_PROCESSES];
    int i;
    
    /* Initialize mutex and condition variable */
    pthread_mutex_init(&resource_lock, NULL);
    pthread_cond_init(&resource_cond, NULL);
    
    printf("=== Asynchronous Resource Request System ===\n");
    printf("Total resources: %d\n", NUM_RESOURCES);
    printf("Number of processes: %d\n\n", NUM_PROCESSES);
    
    /* Create process requests with varying resource needs */
    int resource_needs[] = {3, 2, 4, 2, 3};
    
    for (i = 0; i < NUM_PROCESSES; i++) {
        requests[i].id = i + 1;
        requests[i].requested_resources = resource_needs[i];
        requests[i].callback = resource_callback;
        
        /* Create thread for each process */
        pthread_create(&threads[i], NULL, resource_manager, &requests[i]);
        
        /* Small delay to stagger requests */
        usleep(100000);
    }
    
    /* Wait for all processes to complete */
    for (i = 0; i < NUM_PROCESSES; i++) {
        pthread_join(threads[i], NULL);
    }
    
    /* Cleanup */
    pthread_mutex_destroy(&resource_lock);
    pthread_cond_destroy(&resource_cond);
    
    printf("\n=== All processes completed ===\n");
    printf("Final available resources: %d\n", available_resources);
    
    return 0;
}
