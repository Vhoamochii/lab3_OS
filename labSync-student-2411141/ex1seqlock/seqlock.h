#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock { /* TODO: implement the structure */
   unsigned int seq;
   int data;
   pthread_mutex_t lock;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   rw->seq = 0; 
   rw->data = 0; 
   pthread_mutex_init(&rw->lock, NULL); 
}

static inline void pthread_seqlock_destroy(pthread_seqlock_t *rw) {
   rw->seq = 0; 
   rw-> data = 0; 
   pthread_mutex_destroy(&rw->lock);
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   pthread_mutex_lock(&rw->lock);
   rw->seq++; 
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   rw->seq++;
   pthread_mutex_unlock(&rw->lock);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   unsigned int s; 
   do { 
      s = rw->seq;
   } while (s & 1);
   return s;
}


static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */

   return rw->seq;
}

