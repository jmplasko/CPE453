#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "schedulers.h"
#include "cpu.h"


struct node* head = NULL;

void add(char* name, int priority, int burst)
{

	static int tidcounter = 1;
	Task* taskhold = malloc(sizeof(Task));

	taskhold->name = (char*)safe_malloc((strlen(name) + 1) * sizeof(char));
	strcpy(taskhold->name, name);
	taskhold->tid = tidcounter;
	taskhold->priority = priority;
	taskhold->burst = burst;

	__sync_fetch_and_add(&tidcounter, 1);

	insert(&head, taskhold);
}

struct node* pickNextTask()
{
	struct node* temp;
	temp = head;

	while (temp->next != NULL) {
		temp = temp->next;
	}

	return temp;
}

void schedule()
{
	int total = 0;
	int tatcollector = 0.0;
	int count = 0;

	struct node* up = NULL;

	while (head != NULL) {
		up = pickNextTask();
		run(up->task, up->task->burst);
		tatcollector = tatcollector + up->task->burst;
		total = total + tatcollector;
		count++;

		delete(&head, up->task);

		free(up->task->name);
		free(up->task);
		free(up);
	}
	
	printf("Average TAT: %f\n", (float)total / (float)count);
	printf("Average Wait Time: %f\n", (float)(total - tatcollector) / (float)count);
	printf("Average Response Time: %f\n", (float)(total - tatcollector) / (float)count);
}