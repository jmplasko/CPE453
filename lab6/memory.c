/**
 * James Plasko - Lab 6
 */

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct task {
	char* name;
	int sizer;
	int start;
	int end;
} Task;


struct node {
	Task* task;
	struct node* next;
};

void insert(struct node** head, Task* task);
void delete(struct node** head, char* name);
void traverse(struct node* head, struct node* headempty);
void* safe_malloc(size_t size);
void add(struct node** heading, char* name, int size_amount, int start, int end);
struct node* check_F(struct node** headempty, int amount);
struct node* check_W(struct node** headempty, int amount);
struct node* check_B(struct node** headempty, int amount);
void cleanser(struct node** head);
struct node* get(struct node** head, char* name);
void compactit(struct node** head, struct node** headempty);

struct node* head = NULL;
struct node* headempty = NULL;
static int count = 0;
static int countempty = 1;
static int max_mem = 0;
static int mem_tracker = 0;

int main(int argc, char* argv[])
{
	int should_run = 1;
	char input[1024] = { 0 };
	char mem_arr[256][256];
	char hold_arr[4][256];
	int newlinecheck;
	int newline_index;
	char* token;
	char name[256];
	char size_mem[256];
	char type[256];
	int i, j;
	struct node* random = NULL;
	
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			mem_arr[i][j] = '\0';
		}
	}

	if (argc != 2) {
		fprintf(stderr, "usage: %s [ memory_size ]\n", argv[0]);
		exit(1);
	}

	max_mem = atoi(argv[1]);
	mem_tracker = max_mem;
	char* ptr = "startempty";

	add(&headempty, ptr, max_mem, 0, max_mem - 1);

    while (should_run){   
       	printf("allocator>");
       	fflush(stdout);
        
		fgets(input, 1024, stdin);

		newlinecheck = strchr(input, '\n');
		newline_index = (int)strlen(input) - 1;
		input[newline_index] = '\0';
		
		token = strtok(input, " "); /* Split by spaces */

        if(strcmp(token,"X") == 0){
			exit(0);
		}
		else if(strcmp(token, "RQ") == 0) {
			token = strtok(NULL, " ");
			strcpy(name, token);
			token = strtok(NULL, " ");
			strcpy(size_mem, token);
			token = strtok(NULL, " ");
			strcpy(type, token);

			if (mem_tracker < atoi(size_mem)) {
				printf("Not Enough Memory\n");
			}else if(atoi(size_mem) == 0){
				printf("No Memory size chosen\n");
			}else if (strcmp(token, "F") == 0) {
				random = check_F(&headempty, atoi(size_mem));
				if (random == NULL) {
					printf("recompact");
				}
				else {
					mem_tracker -= atoi(size_mem);
					add(&head, name, atoi(size_mem), random->task->start, random->task->start + atoi(size_mem)-1);
					random->task->start = random->task->start + atoi(size_mem);
					count++;
				}
			}else if (strcmp(token, "B") == 0) {
				random = check_B(&headempty, atoi(size_mem));
				if (random == NULL) {
					printf("recompact");
				}
				else {
					mem_tracker -= atoi(size_mem);
					add(&head, name, atoi(size_mem), random->task->start, random->task->start + atoi(size_mem)-1);
					random->task->start = random->task->start + atoi(size_mem);
					count++;
				}
			}else if (strcmp(token, "W") == 0) {
				random = check_W(&headempty, atoi(size_mem));
				if (random == NULL) {
					printf("recompact");
				}
				else {
					mem_tracker -= atoi(size_mem);
					add(&head, name, atoi(size_mem), random->task->start, random->task->start + atoi(size_mem)-1);
					random->task->start = random->task->start + atoi(size_mem);
					count++;
				}
			}
		}
		else if(strcmp(token, "RL") == 0) {
			token = strtok(NULL, " ");
			strcpy(name, token);
			struct node* holdup = get(&head, name);
			mem_tracker += holdup->task->sizer;
			add(&headempty, name, holdup->task->sizer, holdup->task->start, holdup->task->end);
			delete(&head, name);
			count--;
			cleanser(&headempty);
		}
		else if(strcmp(token, "C") == 0) {
			compactit(&head, &headempty);
		}
		else if(strcmp(token, "STAT") == 0) {
			traverse(head, headempty);
		}
   	}
    
	return 0;
}

void add(struct node** heading, char* name, int size_amount, int start, int end)
{
	Task* taskhold = malloc(sizeof(Task));

	taskhold->name = (char*)safe_malloc((strlen(name) + 1) * sizeof(char));
	strcpy(taskhold->name, name);
	taskhold->sizer = size_amount;
	taskhold->start = start;
	taskhold->end = end;

	insert(heading, taskhold);
}

/* Safe Malloc */
void* safe_malloc(size_t size) {
	void* new;

	/* malloc returns a NULL pointer upon failure
	  This checks for failure. */
	if (NULL == (new = malloc(size))) {
		perror("safe_malloc: malloc failure");
		/* Exit program */
		exit(EXIT_FAILURE);
	}
	/* Returns a pointer with no associated data type */
	return new;
}

// add a new task to the list of tasks
void insert(struct node** head, Task* newTask) {
	// add the new task to the list 
	struct node* newNode = safe_malloc(sizeof(struct node));

	newNode->task = newTask;
	newNode->next = *head;
	*head = newNode;
}

// delete the selected task from the list
void delete(struct node** head, char* name) {
	struct node* temp;
	struct node* prev;

	temp = *head;
	// special case - beginning of list
	if (strcmp(name, temp->task->name) == 0) {
		/*mem_tracker += temp->task->sizer;
		add(&headempty, temp->task->name, temp->task->sizer, temp->task->start, temp->task->end);
		countempty++;*/
		*head = (*head)->next;
	}
	else {
		// interior or last element in the list
		prev = *head;
		temp = temp->next;
		while (strcmp(name, temp->task->name) != 0) {
			prev = temp;
			temp = temp->next;
		}

		/*mem_tracker += temp->task->sizer;
		add(&headempty, temp->task->name, temp->task->sizer, temp->task->start, temp->task->end);
		countempty++;*/
		prev->next = temp->next;
	}
}

// traverse the list
void traverse(struct node* head, struct node* headempty) {
	struct node* temp;
	struct node* tempempty;
	struct node* hold;
	temp = head;
	tempempty = headempty;
	int front = 0;

	while (front < max_mem) {
		
		if (temp != NULL && temp->task->start == front) {
			printf("Addresses [%d:%d] Process %s\n", temp->task->start, temp->task->end, temp->task->name);
			front = temp->task->end + 1;
			temp = head;
			tempempty = headempty;
		}
		else if (tempempty != NULL && tempempty->task->start == front) {
			printf("Addresses [%d:%d] Unused\n", tempempty->task->start, tempempty->task->end, tempempty->task->name);
			fflush(stdout);
			front = tempempty->task->end + 1;
			tempempty = headempty;
			temp = head;
		}
		else {
			if (temp != NULL) {
				temp = temp->next;
			}
			if (tempempty != NULL) {
				tempempty = tempempty->next;
			}
		}
	}
}

struct node* check_F(struct node** headempty, int amount) {
	struct node* temp = NULL;
	struct node* send = NULL;
	temp = *headempty;

	if (temp == NULL) {
		return send;
	}

	int start = max_mem;

	while (temp != NULL) {
		if (temp->task->sizer >= amount && temp->task->start < start) {
			send = temp;
			start = temp->task->start;
		}
		temp = temp->next;
	}

	return send;
}

struct node* check_B(struct node** headempty, int amount) {
	struct node* temp = NULL;
	struct node* send = NULL;
	temp = *headempty;

	if (temp == NULL) {
		return send;
	}

	int largest = temp->task->sizer;
	send = temp;
	temp = temp->next;

	while (temp != NULL) {
		if (temp->task->sizer >= amount && temp->task->sizer < largest) {
			send = temp;
			largest = temp->task->start;
		}
		temp = temp->next;
	}

	return send;
}

struct node* check_W(struct node** headempty, int amount) {
	struct node* temp = NULL;
	struct node* send = NULL;
	temp = *headempty;

	if (temp == NULL) {
		return send;
	}

	int largest = temp->task->sizer;
	send = temp;
	temp = temp->next;

	while (temp != NULL) {
		if (temp->task->sizer >= amount && temp->task->sizer > largest) {
			send = temp;
			largest = temp->task->start;
		}
		temp = temp->next;
	}

	return send;
}

void cleanser(struct node** head) {
	struct node* temp;
	struct node* temp2;

	temp = *head;
	temp2 = *head;

	while (temp != NULL) {


		if (temp->task->start == temp->task->end) {
			delete(head, temp->task->name);
			countempty--;
			temp = *head;
			temp2 = *head;
		}

		while (temp2 != NULL) {
			if (temp->task->start == temp2->task->end + 1) {
				temp2->task->end = temp->task->end;
				delete(head, temp->task->name);
				countempty--;
				temp = *head;
				temp2 = *head;
			}

			temp2 = temp2->next;

		}

		temp2 = *head;
		temp = temp->next;

	}
}

struct node* get(struct node** head, char* name) {
	struct node* temp;
	struct node* prev;

	temp = *head;
	// special case - beginning of list
	if (strcmp(name, temp->task->name) == 0) {
		return temp;
	}
	else {
		// interior or last element in the list
		prev = *head;
		temp = temp->next;
		while (strcmp(name, temp->task->name) != 0) {
			prev = temp;
			temp = temp->next;
		}
		return temp;
	}
}

void compactit(struct node** head, struct node** headempty) {
	struct node* temp;
	struct node* tempempty;
	struct node* temp1;
	struct node* tempempty1;
	temp = *head;
	tempempty = *headempty;
	int front = 0;


	while (tempempty != NULL) {
		tempempty1 = tempempty->next;
		delete(headempty, tempempty->task->name);
		tempempty = tempempty1;
	}

	add(headempty, "reset", mem_tracker, max_mem-mem_tracker, max_mem-1);


	while (temp != NULL) {

		temp->task->start = front;
		temp->task->end = front + temp->task->sizer - 1;
		front += front + temp->task->sizer;
		temp = temp->next;

	}
}