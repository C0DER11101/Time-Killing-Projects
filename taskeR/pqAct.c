#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#include "typedefs.h"
#include "pq.h"
#include<stdbool.h>

int numTasks;


// ======= Perform file operations =======

bool clr(const char*filename) {

	if(!remove(filename))
		return true;
	return false;
}

int fetchID(priorq*queue) {
	int ID=0;
	while(queue!=NULL) {
		if(ID < queue->id)
			ID=queue->id;
		queue=queue->next;
	}

	return ID;
}

bool writeTasks(priorq*queue) {

	bool written=false;

	if(queue==NULL)  // if we didn't add any tasks
		return false;

	FILE *file=fopen(".tasks", "wb");

	if(!file)
		return false;

	while(queue!=NULL) {

		if(queue->isDone!=1) {
			fprintf(file, "%d %d %c %s\n", queue->priority, queue->id, queue->isDone, queue->taskName);
			written=true;  // if any tasks is incomplete then it will be written the file, in which case, it will be set to true
		}

		queue=queue->next;
	}

	if(!written) {  // a list may contain only completed tasks, in that case we don't need to save these completed tasks to the .tasks file
		remove(".tasks");  // so, we are removing the file
		return false;
	}

	return true;
}

void fetchTasks(priorq**queue, int*ID) {
	FILE *file=fopen(".tasks", "rb");

	if(!file) {
		puts("\nNo previously saved tasks!\n");
		return;
	}
	
	puts("\nPrevious tasks exist! Fetching...\n");

	priorq*prev=NULL, *temp=NULL;

	*queue=malloc(sizeof(priorq));
	temp=*queue;

	while(fscanf(file, "%d %d %c %[^\n]%*c", &temp->priority, &temp->id, &temp->isDone, temp->taskName)!=EOF) {
		prev=temp;
		temp=malloc(sizeof(priorq));
		temp->next=NULL;
		numTasks++;
		prev->next=temp;
	}

	prev->next=NULL;
	free(temp);
	(*ID)=fetchID((*queue));

	++(*ID);

	remove(".tasks");
}

// aligning the command given by the user
void align(string input) {
	int idx;

	// removing space before command
	for(int i=0; input[i]!='\0'; i++) {
		if(input[i]!=' ') {   // record the index of the first non-space character
			idx=i;
			break;
		}
	}

	// check if there was any space before the command
	if(idx) {   // if idx is non-zero
		for(int i=0; input[i]!='\0'; i++) {
			if(idx==strlen(input)) {
				input[i]='\0';
				break;
			}
			input[i]=input[idx];
			idx++;
		}
	}

	idx=0;

	// removing space after command
	for(int i=0; input[i]!='\0'; i++) {  // continue until the last non-space character
		if(input[i]!=' ' && input[i]!='\n') // because fgets adds a newline character at the end of every string
			idx=i;
	}

	if(input[idx+1]==' ') {  // check if there is any space after the last character
		input[idx+1]='\n';
		input[idx+2]='\0';
	}

	// removing space between the command
	int space=0;

	for(int i=0; input[i]!='\0'; i++) {
		if(input[i]==' ') {
			idx=i;  // get the last index of whitespace
			space++;
		}
	}

	if(space > 1) {
		char*end=&input[idx+1];   // end is a pointer to a character that now basically points to the string after the whitespaces

		while(input[idx-1]==' ')  // go back until the first whitespace(before which there are non-whitespace characters)
			idx--;

		idx++;

		for(int i=0; end[i]!='\0'; i++)
			input[idx++]=end[i];  // storing the end string after one whitespace

		input[idx]='\0';
	}

}

// ==== Perform queue operations ====

void help() {
	puts("\t\t\ttasker-\e[1mVersion-0.0.1\e[22m");

	puts("\n\e[3mA very mini-project.\e[23m\n");
	puts("This is a task application where you basically add tasks with priorities.");
	puts("You can add atmost 10 tasks.");
	puts("\n\e[1mFlags:\e[22m\n");
	puts("1. \e[1m-a\e[22m \u2192 \e[1madd a task\e[22m");
	puts("2. \e[1m-d\e[22m \u2192 \e[1mdelete a task\e[22m");
	puts("3. \e[1m-chp\e[22m \u2192 \e[1mchange the priority of a task\e[22m");
	puts("4. \e[1m-done\e[22m \u2192 \e[1mmark a task as done\e[22m");
	puts("5. \e[1m-l\e[22m \u2192 \e[1mlist the tasks\e[22m");
	puts("6. \e[1m-clr\e[22m \u2192 \e[1mclear the .tasks file\e[22m");
	puts("7. \e[1m-h\e[22m \u2192 \e[1mhelp session\e[22m");
	puts("8. \e[1m-q\e[22m \u2192 \e[1mquit tasker\e[22m");
	puts("9. \e[1mcls\e[22m \u2192 \e[1mclears the screen; this command is used without the task command\e[22m\n");

	puts("\e[1;3mPriorities from \e[4m0 to 4\e[24m can be assigned\e[0m\n");
	puts("\e[1m0\e[0m \u2192 \e[3mVery important\e[0m");
	puts("\e[1m1\e[0m \u2192 \e[3mImportant\e[0m");
	puts("\e[1m2\e[0m \u2192 \e[3mModerately important\e[0m");
	puts("\e[1m3\e[0m \u2192 \e[3mLess important\e[0m");
	puts("\e[1m4\e[0m \u2192 \e[3mOptional.\e[0m\n");
}

void list(priorq*queue) {
	int complete=0, incomplete=0;
	printf("\n\e[4m\e[1mNumber of tasks\e[24m: %d\e[22m\n\n", numTasks);
	if(queue==NULL) {
		puts("\nNo tasks in tasklist!\n");
		return;
	}

	while(queue) {
		if(queue->isDone==1) {
			printf("(id-[%d]).[%d]: \e[32m%s \u2611\e[0m\n",queue->id, queue->priority, queue->taskName);
			complete++;
		}
		else {
			printf("(id-[%d]).[%d]: \e[31m%s \u2610\e[0m\n",queue->id, queue->priority, queue->taskName);
			incomplete++;
		}

		queue=queue->next;
	}

	printf("\n\e[36mComplete: %d\e[0m          \e[33mIncomplete: %d\e[0m\n\n", complete, incomplete);
}

void done(priorq*queue, int id) {
	while(queue!=NULL) {
		if(id==queue->id) {
			if(queue->isDone==0) {
				queue->isDone=1;
				return;
			}

			else {
				puts("Task is already done!");
				return;
			}
		}
		queue=queue->next;
	}
}

int chp(priorq**queue, int newPr, int id) {

	priorq*temp=(*queue), *prev=NULL;
	int old;

	if((*queue)==NULL)  // list is empty
		return 2;

	if((*queue)->next==NULL) { // list contains only one task
		(*queue)->priority=newPr;
		return 1;
	}

	while(temp!=NULL) {
		if(id==temp->id)
			break;

		prev=temp;
		temp=temp->next;
	}

	if(temp==NULL)
		return -1;
	old=temp->priority;

	// detach the task from the list
	if(prev==NULL) {// the first task's priority was changed
		// detach the target task from the list
		(*queue)=(*queue)->next;
		temp->next=NULL;
	}

	else if(prev) {
		if(temp->next==NULL) { // if the task is the last task
			prev->next=NULL; // detach the last task from the list
		}

		else {
			prev->next=temp->next;  // detach the last task from the list
			temp->next=NULL;
		}
	}

	temp->priority=newPr;
	prev=NULL;

	priorq*temp2=(*queue);

	// re-attaching the detached task to the tasklist
	while(temp2!=NULL) {
		if(temp->priority < temp2->priority) {
			if(prev==NULL) {  // task with new priority has the highest priority
				temp->next=temp2;
				(*queue)=temp2;  // make this task the first task in the list
				return old;
			}
			else {
				prev->next=temp;
				temp->next=temp2;
				return old;
			}
		}

		prev=temp2;
		temp2=temp2->next;
	}

	if(temp2==NULL) {  // the new priority for the task was lower than any of the available tasks
		prev->next=temp;
	}

	return old;
}

void arrange(priorq**queue, priorq*temp) {

	if((*queue)==NULL) {// list is empty
		(*queue)=temp;
		numTasks++;
		return;
	}
	
	priorq*tmp=(*queue);
	// search for duplicate tasks
	char tempS1[10000], tempS2[10000];

	while(tmp!=NULL) {

		// converting each task name into lowercase
		for(int i=0; temp->taskName[i]!='\0'; i++)
			tempS1[i]=tolower(temp->taskName[i]);

		for(int i=0; tmp->taskName[i]!='\0'; i++)
			tempS2[i]=tolower(tmp->taskName[i]);

		if(strcmp(tempS1, tempS2)==0) {
			puts("Task already exists!");
			return;
		}

		tmp=tmp->next;
	}

	// if no duplicate tasks exist, then insert the task in its appropriate position
	tmp=(*queue);
	priorq*prev;

	while(tmp!=NULL) {
		if(temp->priority < tmp->priority) {
			if(tmp==(*queue)) { // first task
				temp->next=tmp;
				(*queue)=temp;
				numTasks++;
				return;
			}

			else {
				prev->next=temp;
				temp->next=tmp;
				numTasks++;
				return;
			}
		}

		prev=tmp;
		tmp=tmp->next;
	}

	prev->next=temp;  // this new task has a priority lower than all other tasks in the tasklist!
	numTasks++;
}

int dq(priorq**queue, int id) {
	if((*queue)==NULL) {
		puts("No tasks!");
		return 0;
	}

	priorq*tmp=(*queue), *prev=NULL;

	while(tmp!=NULL) {
		if(id==tmp->id) {
			if(tmp==(*queue)) { // first task has to be deleted!
				(*queue)=tmp->next;
				free(tmp);
				numTasks--;
				return 1;
			}

			else if(tmp->next==NULL){ // last task
				prev->next=NULL;
				free(tmp);
				numTasks--;
				return 1;
			}

			else {  // any task from the middle
				prev->next=tmp->next;
				free(tmp);
				numTasks--;
				return 1;
			}
		}

		prev=tmp;
		tmp=tmp->next;
	}

	return 0;
}

priorq* nq(priorq*queue, int*ID) {
	if(numTasks==10) {
		puts("Maximum tasks have been set!!");
		return NULL;
	}
	if(queue==NULL) {
		priorq*temp=malloc(sizeof(priorq));

		printf("Enter task: ");
		scanf("%[^\n]%*c", temp->taskName);

		printf("Enter priority: ");
		scanf("%d", &(temp->priority));

		if(temp->priority < 0) {
			puts("\nPriority value should be between 0 and 4, assigning new task a priority value of 0\n");
			temp->priority=0;
		}

		else if(temp->priority > 4) {
			puts("\nPriority value should be between 0 and 4, assigning new task a priority value of 4\n");
			temp->priority=4;
		}

		getchar();

		temp->id=(*ID)++;
		temp->isDone=0;   // task incomplete!
		temp->next=NULL;

		return temp;
	}

	return nq(queue->next, ID);
}

// ==== Perform task operations ====
void taskOps(priorq**queue, int opMode, int*ID) {
	int id, newPr, status;
	priorq*temp;
	switch(opMode) {
		case 0:
			if((temp=nq(*queue, ID))!=NULL)
				arrange(queue, temp);
			break;

		case 1:
			printf("Enter task id: ");
			scanf("%d", &id);

			getchar();

			if(!dq(queue, id))
				puts("\nTask doesn\'t exist!");

			break;

		case 2:
			printf("Enter task id: ");
			scanf("%d", &id);
			printf("Enter new priority of the task: ");
			scanf("%d", &newPr);
			status=chp(queue, newPr, id);

			getchar();

			if(status==-1)
				puts("Task doesn\'t exist");

			else if(status==2)
				puts("Task list is empty!");

			break;

		case 3:
			printf("Enter task id: ");
			scanf("%d", &id);
			getchar();

			done(*queue, id);

			break;

		case 4:
			list(*queue);

			break;

		case 5:
			if(clr(".tasks"))
				puts(".tasks file removed!");
			else
				puts("Failed to remove! No such file or directory");

			break;

		case 6:
			help();

			break;

		case 7:
			if(!writeTasks(*queue))
				puts("\nNo tasks written!\n");
			else
				puts("\nTasks saved for next session!\n");
			return;

			break;
	}
}

// ==== Parse instructions ====
bool parse(priorq**queue, string input, int*ID) {
	if(strcmp(input, "\n")==0) {
		return true;
	}

	else if(strcmp(input, "task\n")==0) {
		strcpy(input, "");
		puts("Task??");
		return true;
	}

	align(input);

	if(strcmp(input, "task -a\n")==0) {
		taskOps(queue, 0, ID); // 0 means "add a task"
	}

	else if(strcmp(input, "task -d\n")==0) {
		taskOps(queue, 1, ID); // 1 means "delete a task"
	}

	else if(strcmp(input, "task -chp\n")==0) {
		taskOps(queue, 2, ID); // 2 means "change the priority of a task"
	}

	else if(strcmp(input, "task -done\n")==0) {
		taskOps(queue, 3, ID); // 3 means "mark a task as done"
	}

	else if(strcmp(input, "task -l\n")==0) {
		taskOps(queue, 4, ID); // 4 means "list the tasks"
	}

	else if(strcmp(input, "task -clr\n")==0) {
		taskOps(queue, 5, ID); // 5 means "clear the .tasks file"
	}

	else if(strcmp(input, "task -h\n")==0) {
		taskOps(queue, 6, ID); // 6 means "open up help session"
	}

	else if(strcmp(input, "task -q\n")==0) {
		taskOps(queue, 7, ID); // 7 means "quit tasker"
	}
	
	else if(strcmp(input, "cls\n")==0) {
		system("clear");
	}
	
	else {
		return false;
	}

	return true;
}
