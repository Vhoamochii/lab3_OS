#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];

int count;
void flushlog();

/* Synchronization primitives */
pthread_mutex_t mtx;
sem_t empty_slots;  /* Counts available slots */
sem_t filled_slots; /* Counts filled slots */

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
   char str[MAX_LOG_LENGTH];
   int id = *(int*) data;

   usleep(20);
   
   /* Wait for an empty slot */
   sem_wait(&empty_slots);
   
   /* Lock mutex to safely access buffer */
   pthread_mutex_lock(&mtx);
   
   sprintf(str, "%d", id);
   strcpy(logbuf[count], str);
   count = (count >= MAX_BUFFER_SLOT)? count :(count + 1); /* Only increase count to size MAX_BUFFER_SLOT*/
   //printf("wrlog(): %d \n", id);
   
   /* Unlock mutex */
   pthread_mutex_unlock(&mtx);
   
   /* Signal that a slot is now filled */
   sem_post(&filled_slots);

   return 0;
}

void flushlog()
{
   int i;
   char nullval[MAX_LOG_LENGTH];
   int slots_to_flush;

   /* Wait for at least one filled slot before flushing */
   sem_wait(&filled_slots);
   
   /* Lock mutex to safely access buffer */
   pthread_mutex_lock(&mtx);
   
   slots_to_flush = count;
   
   sprintf(nullval, "%d", -1);
   for (i = 0; i < slots_to_flush; i++)
   {
      printf("Slot  %i: %s\n", i, logbuf[i]);
      strcpy(logbuf[i], nullval);
   }

   fflush(stdout);

   /*Reset buffer */
   count = 0;
   
   /* Unlock mutex */
   pthread_mutex_unlock(&mtx);
   
   /* Signal that all slots are now empty */
   for (i = 0; i < slots_to_flush; i++)
   {
      sem_post(&empty_slots);
   }
   
   /* Consume remaining filled_slots signals to reset state */
   for (i = 1; i < slots_to_flush; i++)
   {
      sem_trywait(&filled_slots);
   }

   return;

}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];

   struct _args args;
   args.interval = 500e3;
   /*500 msec ~ 500 * 1000 usec */

   /* Initialize synchronization primitives */
   pthread_mutex_init(&mtx, NULL);
   sem_init(&empty_slots, 0, MAX_BUFFER_SLOT);  /* Initially all slots are empty */
   sem_init(&filled_slots, 0, 0);                /* Initially no slots are filled */

   /*Setup periodically invoke flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /*Asynchronous invoke task writelog */
   for (i = 0; i < MAX_LOOPS; i++)
   {
      id[i] = i;
      pthread_create(&tid[i], NULL, wrlog, (void*) &id[i]);
   }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL);

   sleep(5);

   /* Cleanup */
   pthread_cancel(lgrid);
   pthread_join(lgrid, NULL);
   pthread_mutex_destroy(&mtx);
   sem_destroy(&empty_slots);
   sem_destroy(&filled_slots);

   return 0;
}
