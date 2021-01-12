#include <stdio.h>
#include <stdlib.h>
#include "lwp.h"

static rfile returnCont; /*original pointer*/
static void* returnSP; /*return address*/
static context* runningThread = NULL;
static context* head = NULL;
static thread shead = NULL; 
static tid_t tidCount = 1; /* told it has to start at 1 */

/*intitialize function*/
void rr_remove(thread chosen);
void rr_admit(thread new);
context* rr_next();

static struct scheduler globsched = { NULL, NULL, rr_admit, rr_remove, rr_next };
static scheduler sched = &globsched;

tid_t lwp_create(lwpfun function, void* argument, size_t stacksize) {
	unsigned long* tempSP;
	unsigned long* tempBP;
	thread holder = head;

	/*start list*/
	if (tidCount == 1) {
		holder = malloc(sizeof(context));
		holder->lib_one = NULL;
		holder->lib_two = NULL;
		head = holder;
	}
	else {

		while (holder->lib_two != NULL) {
			holder = holder->lib_two;
		}

		holder->lib_two = malloc(sizeof(context));

		/*return error*/
		if (holder->lib_two == NULL)
			return (tid_t) -1;

		holder->lib_two->lib_one = holder;
		holder = holder->lib_two;
		holder->lib_two = NULL;
	}

	/*saves floating point state*/
	holder->state.fxsave = FPU_INIT;

	/* assign tid, stack, and stack size to thread */
	holder->tid = tidCount++;
	holder->stack = malloc(stacksize * sizeof(unsigned long));
	holder->stacksize = stacksize;

	/* return error value if stack malloc fails */
	if (holder->stack == NULL)
		return (tid_t)-1;

	/* assign function parameters to stack, including return address */
	tempSP = holder->stack + stacksize;
	*(--tempSP) = (unsigned long)lwp_exit;
	*(--tempSP) = (unsigned long)function;
	tempBP = --tempSP;

	holder->state.rdi = (unsigned long)argument;
	holder->state.rsp = (unsigned long)tempSP;
	holder->state.rbp = (unsigned long)tempBP;

	sched->admit(holder);

	return holder->tid;
}

/*removes from thread*/
void removeFromLL(thread chosen) {
	/* special case if we're at the head */
	if (chosen == head) {
		if (chosen->lib_two) {
			chosen->lib_two->lib_one = NULL;
			head = chosen->lib_two;
		}
		else {
			head = NULL;
		}
	}

	if (chosen->lib_one) {
		chosen->lib_one->lib_two = chosen->lib_two;
	}

	if (chosen->lib_two) {
		chosen->lib_two->lib_one = chosen->lib_one;
	}

	chosen->lib_two = NULL;
	chosen->lib_one = NULL;

	return;
}

void lwp_exit(void) {
	unsigned long* safeStack = returnSP - (sizeof(unsigned long) * 4);
	unsigned long* tempBP;
	rfile safeRegisters;

	/*saves current context*/
	save_context(&safeRegisters);
	*(--safeStack) = (unsigned long)lwp_exit;
	*(--safeStack) = (unsigned long)safeRegisters.rsp;
	tempBP = --safeStack;
	safeRegisters.rsp = (unsigned long)safeStack;
	safeRegisters.rbp = (unsigned long)tempBP;

	/*remove*/
	removeFromLL(runningThread);
	sched->remove(runningThread);
	thread next = sched->next();

	/*frees malloc*/
	free(runningThread->stack);

	if (!next) {
		runningThread = NULL;
		lwp_stop();
	}
	else {
		runningThread = next;
		load_context(&(runningThread->state));
	}

	return;
}

tid_t lwp_gettid(void) {
	if (runningThread != NULL) {
		return runningThread->tid;
	}

	return NO_THREAD;
}

void lwp_yield(void) {
	/*save content*/
	save_context(&(runningThread->state));

	runningThread = sched->next();

	if (runningThread == NULL) {
		load_context(&returnCont);
	}
	else {
		load_context(&(runningThread->state));
	}

	return;
}

void lwp_start(void) {
	/*exit if no threads*/
	if (tidCount == 1) {
		return;
	}

	save_context(&returnCont);
	returnSP = (unsigned long*)returnCont.rsp;

	runningThread = sched->next();

	if (runningThread == NULL) {
		load_context(&returnCont);
	}
	else {
		load_context(&(runningThread->state));
	}

	return;
}

void lwp_stop(void) {
	if (runningThread) {
		save_context(&(runningThread->state));
	}

	load_context(&returnCont);

	return;
}

/* install a new scheduling function */
void lwp_set_scheduler(scheduler fun) {
	thread sets = shead;

	/*edit*/
	for (; sets; sets = shead) {
		sched->remove(sets);
		fun->admit(sets);
	}

	if (sched->shutdown != NULL)
		sched->shutdown();

	sched = fun;

	return;
}

scheduler lwp_get_scheduler(void) {
	return sched;
}

/* map a thread id to a context */
thread tid2thread(tid_t tid) {
	thread holder;

	for (holder = head; holder && holder->tid != tid; holder = holder->lib_two)
		;

	if (holder->tid == tid)
		return holder;

	return NULL;
}

void rr_init() {
	return;
}

void rr_shutdown() {
	return;
}

void rr_admit(thread new) {
	thread holder;

	if (shead == NULL) {
		shead = new;
		new->sched_two = NULL;
		new->sched_one = NULL;
		return;
	}

	holder = head;

	while (holder->sched_two != NULL) {
		holder = holder->sched_two;
	}

	holder->sched_two = new;
	new->sched_two = NULL;
	new->sched_one = holder;

	return;
}

thread rr_next() {
	thread before = NULL;
	thread holder = NULL;

	/*check special case*/
	if (runningThread == NULL) {
		if (shead != NULL) {
			return shead;
		}
		else {
			return NULL;
		}
	}

	if (runningThread->sched_two) {
		holder = runningThread->sched_two;
	}
	else {
		holder = shead;
	}

	if (runningThread == shead && runningThread->sched_two == NULL) {
		return runningThread;
	}

	for (before = holder; holder == runningThread;
		holder = holder->sched_two ? holder->sched_two : shead) {
		before = holder;
	}

	return before;
}

void rr_remove(thread chosen) {
	/*special case*/
	if (chosen == shead) {
		if (chosen->sched_two) {
			chosen->sched_two->sched_one = NULL;
			shead = chosen->sched_two;
		}
		else {
			shead = NULL;
		}
	}

	if (chosen->sched_one) {
		chosen->sched_one->sched_two = chosen->sched_two;
	}

	if (chosen->sched_two) {
		chosen->sched_two->sched_one = chosen->sched_one;
	}

	chosen->sched_two = NULL;
	chosen->sched_one = NULL;

	return;
}