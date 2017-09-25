#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "queue.h"
#define NUM_THREADS_P 3
#define NUM_THREADS_C 2
typedef struct filaint_t
{
   struct filaint_t *prev ;
   struct filaint_t *next ;
   int id ;
} filaint_t ;


//////////////////////////////////////////////////
sem_t s_vaga, s_item, s_buffer;
filaint_t *fila;
/////////////////////////////////////////////////

filaint_t *create_item(int id){
  filaint_t *item = (filaint_t*) malloc(sizeof(filaint_t));
  (*item).id = id;
  return item;
}
void init_sem(sem_t *sem, int n)
{
  sem_init(sem, 0, n);
}

int rand_num(int n)
{

  return rand() % n + 1;
}

void *produtor (void *id)
{
   int item = 0;
   long tid = (long) id ;
   //printf("Thread produtor p_%ld\n", tid);
   while(1)
   {
     int pause = rand_num(3);
     sleep(pause);
     sem_wait(&s_vaga);//down s_vaga
     sem_wait(&s_buffer);//down s_buffer
     item = rand_num(99);
     //insere no buffer
     filaint_t *p_item = create_item(item);
     queue_append((queue_t**)&fila, (queue_t*)p_item);
     sem_post(&s_buffer);//up s_buffer
     sem_post(&s_item);//up s_item
     printf("Item produzido em p_%ld: %d\n", tid,item);
   }

   pthread_exit (NULL) ;
}

void *consumidor (void *id)
{
   long tid = (long) id ;
   //printf("Thread consumidor c_%ld\n", tid);
   while(1)
   {
     sem_wait(&s_item);//down s_item
     sem_wait(&s_buffer);//down s_buffer
     //retira do buffer
     filaint_t *p_item =(filaint_t *)queue_remove((queue_t**)&fila, (queue_t*)fila);
     sem_post(&s_buffer);//up s_buffer
     sem_post(&s_vaga);//up s_vaga
     printf("Item consumido em c_%ld: %d\n", tid, (*p_item).id);
     int pause = rand_num(3);
     sleep(pause);
   }

   pthread_exit (NULL) ;
}

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  pthread_t threadp [NUM_THREADS_P];
  pthread_t threadc [NUM_THREADS_C];
  long i, status;
  i = 0;
  init_sem(&s_vaga, 5);
  init_sem(&s_item, 0);
  init_sem(&s_buffer, 1);
  printf("%d\n", queue_size((queue_t*)fila));

  //produtor
  for (i = 0; i < NUM_THREADS_P; i++) {
    status = pthread_create (&threadp[i], NULL, produtor, (void *) i) ;
    if (status)
    {
       perror ("pthread_create") ;
       exit (1) ;
    }
  }

  for (i = 0; i < NUM_THREADS_C; i++) {
    status = pthread_create (&threadc[i], NULL, consumidor, (void *) i) ;
    if (status)
    {
       perror ("pthread_create") ;
       exit (1) ;
    }
  }

  pthread_exit (NULL) ;
  return 0;
}
