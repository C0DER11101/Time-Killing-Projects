#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "typedefs.h"
#include "pq.h"
#include "functions.h"


int main(void) {
	int _id=0;
	priorq *queue=NULL;
	string input;
	input=(char*)calloc(1000, sizeof(char));

	// fetch tasks(if any) from the .tasks file
	fetchTasks(&queue, &_id);

	while(1) {
		printf("(task> ");
		fgets(input, 1000, stdin);
		if(!parse(&queue, input, &_id))
			puts("\nDidn\'t understand the command\n");

		if(strcmp(input, "task -q\n")==0)
			break;
	}


	free(input);
	free(queue);

	return 0;
}
