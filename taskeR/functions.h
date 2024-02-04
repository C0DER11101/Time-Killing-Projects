#ifndef funH
#define funH

#include<stdbool.h>

// functions to perform operations on tasks
bool parse(priorq**, string, int*);
void taskOps(priorq**, int, int*); // number indicates the operation to perform on the task! Returns status of operation!
void fetchTasks(priorq**, int*); // fetch tasks(if any) from the .tasks file
bool writeTasks(priorq*); // write tasks(if any) into the .tasks file
int fetchID(priorq*); // fetch the task with the largest ID

// functions for priority queue
priorq* nq(priorq*, int*); // add a task; returns 0 or 1; 0 -> insertion unsuccessful, 1 -> insertion successful
void arrange(priorq**, string); // arrange the tasks of the queue in the correct order
int dq(priorq**, int); // delete a task; returns 0 if deletion is unsuccessful, 1 if deletion is successful
int chp(priorq**, int, int); // change the priority of a task; returns the old priority
void done(priorq*, int); // mark a task as done; returns the id of completed tasks
void list(priorq*); // list the tasks
bool clr(const char*); // clear the .tasks file
void help(); // open help session
void align(string);  // align the commands

#endif
