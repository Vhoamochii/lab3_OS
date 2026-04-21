#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "seqlock.h"  /* TODO implement this header file */

pthread_seqlock_t lock;

int main()
{
   //int val = 0;

   pthread_seqlock_init(&lock);

   pthread_seqlock_wrlock(&lock);               //seq is now 1
   lock.data++;
   lock.data  = lock.data * 5;                  //val is now 5                          
   pthread_seqlock_wrunlock(&lock);             //seq is now 2 


   unsigned int s;
   unsigned int after_s; 
   int val;

   do {
      s = pthread_seqlock_rdlock(&lock);        //seq is now 2     
      val = lock.data;                          //data is now 1
      after_s = pthread_seqlock_rdunlock(&lock);   //return data = 2 because 2 's' are the same
   } while (s != after_s);

   // if(pthread_seqlock_rdlock(&lock) == 1){
   //    printf("val = %d\n", val); 
   //    pthread_seqlock_rdunlock(&lock);
   // }


   printf("val = %d\n", val);
}
