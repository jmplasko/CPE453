#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "schedulers.h"
#include "cpu.h"


struct node* head = NULL;
int flag = 0;
static int perm = 0;

void add(char* name, int priority, int burst)
{

	static int tidcounter = 1;
	Task* taskhold = malloc(sizeof(Task));

	perm++;
	taskhold->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
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
	struct node* holder;
	temp = head;
	holder = head;

	while (temp->next != NULL) {
		temp = temp->next;
		if (temp->task->priority > holder->task->priority) {
			holder = temp;
			flag = 0;
		}
		if ((temp->task->priority) == (holder->task->priority)) {
			flag = 1;
			holder = temp;
		}
	}

	return holder;
}

void schedule()
{
	int total = 0;
	int wait = 0;
	int tatcollector = 0.0;
	int count = 0;
	int subtractor = 0;
	int response = 0;
	int holdperm = 0;
	int tracker = 0;

	struct node* up = NULL;

	while (head != NULL) {
		up = pickNextTask();

		if (flag == 1) {
			if (up->task->burst <= 10) {

				tatcollector += up->task->burst;

				run(up->task, up->task->burst);

				delete(&head, up->task);

				if ((perm - 1) > holdperm) {
					response += tatcollector;
					holdperm++;
				}

				total += tatcollector;
				count++;

				int sub = 0;
				int i = 0;
				for (i = 0; i < (tracker / count); i++) {
					sub += 10;
				}

				tatcollector -= sub;

				wait += tatcollector;

				tatcollector += sub;
				sub = 0;
			}
			else {

				tatcollector += 10;

				run(up->task, 10);

				delete(&head, up->task);

				add(up->task->name, up->task->priority, (up->task->burst) - 10);

				perm--;

				if ((perm - 1) > holdperm) {
					response += tatcollector;
					holdperm++;
				}

			}
			flag = 0;
		}
		else {

			tatcollector += up->task->burst;

			run(up->task, up->task->burst);

			delete(&head, up->task);


			subtractor = up->task->burst;
			count++;

			if ((perm - 1) > holdperm) {
				response += tatcollector;
				holdperm++;
			}

			int sub = 0;
			int i = 0;
			for (i = 0; i < (tracker / count); i++) {
				sub += 10;
			}

			tatcollector -= sub;

			wait += tatcollector;

			tatcollector += sub;
			sub = 0;

		}


		tracker++;

		free(up->task->name);
		free(up->task);
		free(up);
	}

	wait -= subtractor;

	printf("Average TAT: %f\n", (float)total / (float)count);
	printf("Average Wait Time: %f\n", (float)wait / (float)count);
	printf("Average Response Time: %f\n", (float)response / (float)count);

}