#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SIZE 100

void* reader(void* param);
void* writer(void* param);

FILE* inout;
sem_t mutex, wrt;
int data = 0, readcount = 0;
char task[SIZE][SIZE];

int main(int argc, char *argv[])
{
	int N = atoi(argv[2]);
	int i = 0;
	int j = 0;
	pthread_t r[N], w;
	sem_init(&mutex, 0, 1);
	sem_init(&wrt, 0, 1);
	
	if (argc != 3) {
		fprintf(stderr, "usage: ./threads [input file r/w] [# of Reader threads]\n");
		return -1;
	}

    inout = fopen(argv[1],"r+");

	if (inout == NULL) {
		printf("input file invalid\n");
		exit(1);
	}

	for (i = 0; i < N/2; i++) {
		pthread_create(&r[i], NULL, (void*)reader, (void*)i);
	}

	pthread_create(&w, NULL, (void*)writer, (void*)0);

	for (j = i; j < N; j++) {
		pthread_create(&r[j], NULL, (void*)reader, (void*)j);
	}

	for (i = 0; i < N / 2; i++) {
		pthread_join(r[i], NULL);
	}

	pthread_join(w, NULL);

	for (j = i; j < N; j++) {
		pthread_join(r[j], NULL);
	}


	fclose(inout);

    return 0;
}

void* reader(void* param)
{
	int c = (int)param;
	int g = 0;
	printf("\nreader % d is made\n", c);
	sem_wait(&mutex);
	readcount++;
	if (readcount == 1)
		sem_wait(&wrt);
	sem_post(&mutex);
	/*Critcal Section */
	printf("\nreader % d is reading\n", c);
	fseek(inout, 0, SEEK_SET);
	for (g=0; g <=c; g++) {
		fgets(task[c], SIZE, inout);
	}
	sleep(1);
	printf("\nreader % d finished reading: %d\n", c, atoi(task[c]));
	/* critical section completd */
	sem_wait(&mutex);
	readcount--;

	if (readcount == 0)
		sem_post(&wrt);
	sem_post(&mutex);
}

void* writer(void* param)
{
	int i = (int)param;
	char name[10] = { 'L','e','s','s','o','n','s','\0' };

	printf("\nWriter %d made\n", i);

	sem_wait(&wrt);
	printf("\nwriter %d is writing\n", i);
	fseek(inout, 0, SEEK_END);
	fputs(name, inout);
	sleep(1);
	printf("\nwriter %d is finished writing\n", i);
	sem_post(&wrt);
}