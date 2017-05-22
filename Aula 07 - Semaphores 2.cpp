#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

#define RAND_DIVISOR 10000

/* The mutex lock */
pthread_mutex_t mutex;

/* the semaphores */
sem_t full, empty;

/* the buffer */
int buffer[BUFFER_SIZE];

/* buffer counter */
int counter;

pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *producer(void *param); /* the producer thread */
void *consumer(void *param); /* the consumer thread */

/* Add an item to the buffer */
int insert_item(int item) {
   /* When the buffer is not full add the item
      and increment the counter*/
   if(counter < BUFFER_SIZE) {
      buffer[counter] = item;
      counter++;
      return 0;
   }
   else { /* Error the buffer is full */
      return -1;
   }
}

/* Remove an item from the buffer */
int remove_item(int *item) {
	/* When the buffer is not empty remove the item
	  and decrement the counter */
	if(counter > 0) {
		*item = buffer[(counter-1)];
		counter--;
		return 0;
	}
	else { /* Error buffer empty */
		return -1;
	}
}

void initializeData() {

	/* Create the mutex lock */
	pthread_mutex_init(&mutex, NULL);
	
	/* Create the full semaphore and initialize to 0 */
	sem_init(&full, 0, 0);
	
	/* Create the empty semaphore and initialize to BUFFER_SIZE */
	sem_init(&empty, 0, BUFFER_SIZE);
	
	/* Get the default attributes */
	pthread_attr_init(&attr);
	
	/* init buffer */
	counter = 0;
}

/* Producer Thread */
void *producer(void *param) {
	int item;

    while(true) {
		/* sleep for a random period of time */
		int rNum = rand() / RAND_DIVISOR * RAND_DIVISOR;
		sleep(6);
		
		/* generate a random number */
		item = rand();
		
		/* acquire the empty lock */
		sem_wait(&empty);
		/* acquire the mutex lock */
		pthread_mutex_lock(&mutex);
		
		if(insert_item(item)) {
			fprintf(stderr, " Producer report error condition\n");
		}
		else {
			printf("producer produced %d\n", item);
		}
		
		/* release the mutex lock */
		pthread_mutex_unlock(&mutex);
		/* signal full */
		sem_post(&full);
	}
}

/* Consumer Thread */
void *consumer(void *param) {
   int item;

   	while(true) {
	    /* sleep for a random period of time */
	    int rNum = rand() / RAND_DIVISOR;
	    sleep(1);
	
	    /* aquire the full lock */
	    sem_wait(&full);
	    /* aquire the mutex lock */
	    pthread_mutex_lock(&mutex);
	    if(remove_item(&item)) {
	        fprintf(stderr, "Consumer report error condition\n");
	    }
	    else {
	        printf("consumer consumed %d\n", item);
	    }
	    /* release the mutex lock */
	    pthread_mutex_unlock(&mutex);
	    /* signal empty */
	    sem_post(&empty);
   }
}



int main() {

    int i;

    int mainSleepTime = 30; /* Time in seconds for main to sleep */
    int numProd = 1; /* Number of producer threads */
    int numCons = 1; /* Number of consumer threads */

    /* Initialize the app */
    initializeData();

    /* Create the producer threads */
    for(i = 0; i < numProd; i++) {
      /* Create the thread */
      pthread_create(&tid,&attr,producer,NULL);
    }

    /* Create the consumer threads */
  	for(i = 0; i < numCons; i++) {
      /* Create the thread */
      pthread_create(&tid,&attr,consumer,NULL);
    }

    /* Sleep for the specified amount of time in milliseconds */
    sleep(mainSleepTime);

    /* Exit the program */
    printf("Exit the program\n");
    exit(0);
}
