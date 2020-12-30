/**
 * Carter Mooring
 * Project 5
 * CPCS 346 OS
 * Nov. 13th, 2020
 */

#include "buffer.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define TRUE 1

buffer_item buffer[BUFFER_SIZE];
/*define semaphores and mutex*/

pthread_mutex_t mutex;
sem_t full, empty;

int counter;
int insertPointer = 0, removePointer = 0;

void *producer(void *param);
void *consumer(void *param);

int insert_item(buffer_item item)
{
    /* Acquire Empty Semaphore */
	sem_wait(&empty);
	
	/* Acquire mutex lock to protect buffer */
	pthread_mutex_lock(&mutex);

	if(counter != BUFFER_SIZE){
		buffer[insertPointer] = item;
		insertPointer = (insertPointer + 1) % BUFFER_SIZE;
		counter++;
	}

	/* Release mutex lock and full semaphore */
	pthread_mutex_unlock(&mutex);
	sem_post(&full);

	return 0;
}

int remove_item(buffer_item *item)
{
	/* Acquire Full Semaphore */
	sem_wait(&full);

	/* Acquire mutex lock to protect buffer */
    pthread_mutex_lock(&mutex);

	if(counter != 0){
		*item = buffer[removePointer];
		removePointer = (removePointer + 1) % BUFFER_SIZE;
		counter--;
	}
	/* Release mutex lock and empty semaphore */
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);

	return 0;
}


int main(int argc, char *argv[])
{
	int sleepTime, producerThreads, consumerThreads;
	int i, j;

	if(argc != 4)
	{
		fprintf(stderr, "Useage: <sleep time> <producer threads> <consumer threads>\n");
		return -1;
	}

	/*call atoi to get arguments */
	sleepTime = atoi(argv[1]);
	producerThreads = atoi(argv[2]);
	consumerThreads = atoi(argv[3]);
 
	srand(time(NULL));
	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	counter = insertPointer = removePointer = 0;

	/* Create the producer and consumer threads */
	pthread_t producers[producerThreads];
	pthread_t consumers[consumerThreads];

	for(i = 0; i < producerThreads; i++){
		pthread_create(&producers[i], NULL, producer, NULL);
	}
	for(j = 0; j< consumerThreads; j++)
	{
		pthread_create(&producers[j], NULL, consumer, NULL);
	}

	/* Sleep for user specified time */
	sleep(sleepTime);
	return 0;
}

void *producer(void *param)
{
	buffer_item item;
	while(TRUE)
	{
		sleep(rand() % 5 + 1); //Generates a random number to sleep between 1 and 5
		
		item = rand();
		if (insert_item(item))
			fprintf(stderr, "report error condition, Producer");
		else
			printf("producer produced %d\n",item);
	}
}

void *consumer(void *param)
{
	buffer_item item;
	while(TRUE)
	{
		sleep(rand() % 5 + 1); //Generates a random number to sleep between 1 and 5
		
		if (remove_item(&item))
			fprintf(stderr, "report error condition, Consumer");
		else
			printf("consumer consumed %d\n",item);

	}
}
