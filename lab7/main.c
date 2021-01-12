#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 100

void set_check_table(int virt);


int table[256] = { 1234 };
int memory[256][256] = { 1234 };
FILE* backlog;
int tracker = 0;

int main(int argc, char* argv[])
{
	FILE* in;
	char* temp;
	char task[SIZE];
	int i, j;

	int virt;
	in = fopen(argv[1], "r");
	backlog = fopen("./BACKING_STORE.bin", "r");

	for (i = 0; i < 256; i++) {
		table[i] = 1234;
		for (j = 0; j < 256; j++){
			memory[i][j] = 1234;
		}
	}


	while (fgets(task, SIZE, in) != NULL) {
		virt = atoi(task);
		set_check_table(virt);
	}


	fclose(in);
	fclose(backlog);

	return 0;
}

void set_check_table(int virt) {
	int page;
	int offset;
	char hold[257];
	int i;

	page = virt;
	offset = virt;
	page &= 0x0000FF00;
	page = page >> 8;
	offset &= 0x000000FF;

	if (table[page] == 1234) {

		table[page] = tracker;

		fseek(backlog, (page*256), SEEK_SET);

		fread(&hold, sizeof(char), 256, backlog);

		for (i = 0; i < 256; i++) {
			memory[tracker][i] = (int)hold[i];
		}

		tracker++;
	}

	printf("Virtual address: %d ", virt);
	printf("Physical address: %d ", (table[page] * 256) + offset);
	printf("Value: %d\n", memory[table[page]][offset]);
}