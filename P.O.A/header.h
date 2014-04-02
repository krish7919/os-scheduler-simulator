
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"header.c"

void check_starvation(struct process_node *p,struct queue *ready_queue)

unsigned isEmpty(void *x)

int random_number(unsigned parameter)

int getArrivalTime(struct process_node *p,unsigned pid)

unsigned getPriority(struct process_node *p,unsigned pid)

unsigned getRemainingTime(struct process_node *p,struct queue *queue_node)

void display_bursts(struct burst_node *b)

void display_process(struct process_node *p)

void display_queue(struct process_node *p, struct queue *q)

void display_task_manager(unsigned current_pid, struct process_node *p, struct queue \
	*input_blocked_queue, struct queue *output_blocked_queue, struct queue *disk_blocked_queue,\
		struct queue *ready_queue,int dur)

void display_statistics(struct process_node *p, char *str,unsigned total_time)

void clear_statistics(struct process_node *p)

struct queue * insert_process_at_end_of_queue(struct queue *q,struct queue *node)

struct queue * insert_sort_by_priority(struct process_node *p, struct queue *r, struct queue *node)

struct queue * insert_sort_by_timestamp(struct process_node *p, struct queue *r, struct queue *node)

struct queue * insert_sort_by_remaining_time(struct process_node *p,struct queue *r,struct queue *node)

struct queue * insert_sort_by_burst_duration(struct queue *r, struct queue *node)

struct queue * insert_in_ready_queue(struct process_node *p,struct queue *r,struct queue *node,unsigned parameter)

struct queue * initialize_ready_queue(struct process_node *p, struct queue *r, unsigned parameter)

struct queue * switch_process_to_ready_queue(struct process_node *p, struct queue *ready_queue, struct queue **blocked_queue,unsigned parameter)


