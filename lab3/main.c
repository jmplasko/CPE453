#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define SIZE 100

void* runner(void* param);
void* runner2(void* param);
void* runnertest(void* param);
int cmpfunc(const void* a, const void* b);

int* found;
int count = 0;

int* arr;
int* test;
int* finalarr;

int main(int argc, char *argv[])
{
	FILE *in, *out;
    char task[SIZE];
	pthread_t tid[3];
	pthread_attr_t attr[3];
	int i = 0;
	struct timespec start, finish;
	double elapsed;

	if (argc != 3) {
		fprintf(stderr, "usage: ./threads [input file] [outputfile]\n");
		return -1;
	}

    in = fopen(argv[1],"r");

	if (in == NULL) {
		printf("input file invalid\n");
		exit(1);
	}

	out = fopen(argv[2], "w");

	if (out == NULL) {
		printf("output file error\n");
		exit(2);
	}

    while (fgets(task,SIZE,in) != NULL) {
		count++;
    }
    fclose(in);

	arr = malloc(sizeof(int) * count);
	test = malloc(sizeof(int) * count);
	finalarr = malloc(sizeof(int) * count);

	found = arr;

	in = fopen(argv[1],"r");

	while (fgets(task,SIZE,in) != NULL) {
        arr[i] = atoi(strdup(task));
		test[i] = atoi(strdup(task));
		i++;		
    }

    fclose(in);

	pthread_attr_init(&attr[0]);
	pthread_attr_init(&attr[1]);
	pthread_attr_init(&attr[2]);
	
	clock_gettime(CLOCK_MONOTONIC, &start);

	for (i = 0; i < 2; i++) {
		pthread_create(&tid[i], &attr[i], runner, i);
	}

	for (i = 0; i < 2; i++) {
		pthread_join(tid[i], NULL);
	}

	pthread_create(&tid[2], &attr[2], runner2, 2);
	pthread_join(tid[2], NULL);

	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	printf("elapsed thread: %f\n", elapsed);

	clock_gettime(CLOCK_MONOTONIC, &start);

	pthread_create(&tid[0], &attr[0], runnertest, 0);
	pthread_join(tid[0], NULL);

	clock_gettime(CLOCK_MONOTONIC, &finish);

	elapsed = (finish.tv_sec - start.tv_sec);
	elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

	printf("elapsed no threads: %f\n", elapsed);

	for(i = 0; i < count; i++){
		fprintf(out, "%d\n", finalarr[i]);
	}

	free(finalarr);
	free(test);
	free(arr);

    return 0;
}

void* runner(void* param)
{
	int tid = (int)param;
	int i = 0;
	int start, end;

	if (tid == 1) {
		
		start = (count / 2) + 1;
		if (count % 2 == 1) {
			end = start + (count / 2) + 1;
		}
		else {
			end = start + (count / 2);
		}
	}
	else {
		
			start = 0;
			end = (count / 2);
	}
	
	int* p;
	p = found;

	for (i = 0; i < start-1; i++) {
		p++;
	}
	
	qsort(p, end - start, sizeof(int), cmpfunc);

	pthread_exit(0);
}

void* runner2(void* param)
{
	int i = 0;
	int j = (count / 2);
	int k = 0;

	while (i < (count/2) && j < count)
	{
		if (arr[i] < arr[j])
			finalarr[k++] = arr[i++];
		else
			finalarr[k++] = arr[j++];
	}

	while (i < (count / 2)) {
		finalarr[k++] = arr[i++];
	}

	while (j < count) {
		finalarr[k++] = arr[j++];
	}

	pthread_exit(0);
}

void* runnertest(void* param)
{
	qsort(test, count, sizeof(int), cmpfunc);

	pthread_exit(0);
}

int cmpfunc(const void* a, const void* b) {
	return (*(int*)a - *(int*)b);
}