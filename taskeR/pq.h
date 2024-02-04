#ifndef pqH
#define pqH

typedef struct priorq {
	int priority;
	int id;  // must be unique
	char taskName[10000];
	char isDone;  // 0 indicates incomplete task, 1 indicates complete task
	struct priorq *next;
}priorq;

#endif
