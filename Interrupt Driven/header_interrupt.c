
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define _HEADER_INTERRUPT_H

struct process_node
{
        int pid;
        char name[10];
	time_t time_of_arrival;
        unsigned number_of_bursts;
        unsigned priority;
	unsigned waiting_time;
	unsigned io_time;
	unsigned execution_time;
        struct burst_node *burst;
        struct process_node *next;
};

struct burst_node
{
        unsigned burst_duration;
        unsigned exit_code;     //try enum here 0-Input 1-Output 2-Disk Access 3-I/O burst
        struct burst_node *next;
};

struct queue
{
	unsigned pid;
	time_t timestamp;
	struct burst_node *burst;
	struct queue *next;
};

//========================================================================================================
//========================================================================================================

unsigned isEmpty(void *x);
//struct queue * insert_process_in_blocked_queue(struct queue *q,struct queue *node,struct interrupt **interrupt_queue);
int random_number(unsigned parameter);
int getArrivalTime(struct process_node *p,unsigned pid);
unsigned getPriority(struct process_node *p,unsigned pid);
unsigned getRemainingTime(struct process_node *p,struct queue *queue_node);
void display_bursts(struct burst_node *b);
void display_process(struct process_node *p);
void display_queue(struct process_node *p, struct queue *q);
void display_task_manager(unsigned current_pid, struct process_node *p, struct queue \
	*input_blocked_queue, struct queue *output_blocked_queue, struct queue *disk_blocked_queue,\
		struct queue *ready_queue,int dur);
void display_statistics(struct process_node *p, char *str,unsigned total_time);
void clear_statistics(struct process_node *p);
struct queue * insert_process_at_end_of_queue(struct queue *q,struct queue *node);
struct queue * insert_sort_by_priority(struct process_node *p, struct queue *r, struct queue *node);
struct queue * insert_sort_by_timestamp(struct process_node *p, struct queue *r, struct queue *node);
struct queue * insert_sort_by_remaining_time(struct process_node *p,struct queue *r,struct queue *node);
struct queue * insert_sort_by_burst_duration(struct queue *r, struct queue *node);
struct queue * insert_in_ready_queue(struct process_node *p,struct queue *r,struct queue *node,unsigned parameter);
//struct interrupt * insert_sort_by_interrupt(struct interrupt *interrupt_queue,struct interrupt *node);
struct queue * initialize_ready_queue(struct process_node *p, struct queue *r, unsigned parameter);
struct queue * switch_process_to_ready_queue(struct process_node *p, struct queue *ready_queue, struct queue **blocked_queue,unsigned parameter);
struct burst_node * create_burst_node(struct burst_node *node,unsigned duration);
int display_options();
struct process_node * create_process(unsigned pid);
void add_process(struct process_node *p,struct process_node *q);
//void subtract_burst_duration_from_interrupt_queue(struct interrupt *iq,unsigned dur);
//void display_interrupt_queue(struct interrupt *interrupt_ptr);



//========================================================================================================
//========================================================================================================

unsigned isEmpty(void *x)
{
	if(x==NULL)
		return 1;
        else
                return 0;
}

//========================================================================================================
//========================================================================================================

int random_number(unsigned parameter)
{
        int burst;

//	srand(time(NULL));
	switch(parameter)
	{
		case 1:
			burst=rand()%20;
		        if(burst%2==0)
		                return burst+1;
		        else
		                return burst;
		case 2:
			return(rand()%10+1);
		case 3:
			return((rand()%10)+1);
		case 4:
		        return(rand()%3);
	}
}

//========================================================================================================
//========================================================================================================

int getArrivalTime(struct process_node *p,unsigned pid)
{
	struct process_node *temp_process=NULL;

	for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
	{
		if(pid==temp_process->pid)
			return (int)temp_process->time_of_arrival;
	}
}

unsigned getPriority(struct process_node *p,unsigned pid)
{
	struct process_node *temp_process=NULL;

	for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
	{
		if(pid==temp_process->pid)
			return temp_process->priority;
	}
}

unsigned getRemainingTime(struct process_node *p,struct queue *queue_node)
{
	struct process_node *temp_process=NULL;
	struct burst_node *b=NULL,*temp_burst=NULL;
	unsigned remaining_time=0;
	
	for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
	{
		if(queue_node->pid==temp_process->pid)
		{
			b=queue_node->burst;
			for(temp_burst=b;temp_burst!=NULL;temp_burst=temp_burst->next)
			{
				if(temp_burst->exit_code!=3) 	//not an i/o burst
					remaining_time+=temp_burst->burst_duration;
				//else				//DO NOTHING - An I/O Burst
								//Just add CPU burst times
			}
			return remaining_time;
		}
	}
}

//========================================================================================================
//========================================================================================================

void display_bursts(struct burst_node *b)
{
	int counter=0,i=0;
	const int DISPLAY_BURST=8;
        if(!isEmpty(b))
        {
                printf("\n\t|\tBursts: ");
                while(b!=NULL)
                {
			counter++;
			if(!(counter%DISPLAY_BURST==0))
			{
				printf("|%3u(%u)",b->burst_duration,b->exit_code);
				b=b->next;
			}
			else
			{
				printf("|\t\t    |\n\t|\t\t");
				printf("|------|------|------|------|------|------|------|\t\t    |");
				printf("\n\t|\t\t");
				counter=0;
			}
                }
		counter=DISPLAY_BURST-1-counter;
		if(counter)
		{
			for(i=0;i<counter;i++)
				printf("|      ");
		}
		printf("|\t\t    |\n\t|\t\t|______|______|______|______|______|______|______|\t\t    |");
        }
        else
                printf("\nNO BURSTS PRESENT\n");
}

void display_process(struct process_node *p)
{
        struct process_node *q=p;
        struct burst_node *b;

        if(!isEmpty(q))
        {
		system("clear");
		printf("\n\n\n\t ___________________________________________________________________________________ ");
                printf("\n\t|   PID   |      NAME      |   TIME OF ARRIVAL   |      BURSTS     |    PRIORITY    |");
		printf("\n\t|_________|________________|_____________________|_________________|________________| ");

                while(q!=NULL)
                {
			//Convert time to strung to display in readable format???
                        printf("\n\t| %5u   |  %-14s|\t%15u  |\t%4u\t   |\t  %3u       |",q->pid,q->name,\
				q->time_of_arrival,q->number_of_bursts,q->priority);
		printf("\n\t|_________|________________|_____________________|_________________|________________| ");
                        b=q->burst;
                        display_bursts(b);
                        q=q->next;
		printf("\n\t|___________________________________________________________________________________|");
                }
        }
        else
                printf("\nNO PROCESS PRESENT (PROCESS QUEUE IS EMPTY)\n");

        printf("\n\n");
}

void display_queue(struct process_node *p, struct queue *q)
{
	struct process_node *temp_process=NULL;
	struct queue *temp_queue=q;

	if(!isEmpty(q))
	{
		for(temp_queue=q;temp_queue!=NULL;temp_queue=temp_queue->next)
			for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
				if(temp_process->pid==temp_queue->pid)
				{
					printf("\n\t\tPID: %3u\tProcess: %-10s",temp_process->pid,temp_process->name);
printf("\tTS: %u",temp_process->time_of_arrival);
printf("\tP: %u",temp_process->priority);
if(temp_queue->burst!=NULL)
printf("\tBD: %u",temp_queue->burst->burst_duration);
printf("\tRemT: %u",getRemainingTime(p,temp_queue));
//printf("\n");
//display_bursts(temp_queue->burst);
//printf("\n");
					break;
				}
	}
	else
		printf("\n\t\tNo process in queue");
}

void display_task_manager(unsigned current_pid, struct process_node *p, struct queue \
	*input_blocked_queue, struct queue *output_blocked_queue, struct queue *disk_blocked_queue,\
		struct queue *ready_queue,int dur)
{
	struct process_node *temp_process=p;

static int counter=0;

//		system("clear");
printf("\n\n==================================================================================\n");

printf("\n%d\n%u",++counter,time(NULL));

	for(temp_process;temp_process!=NULL;temp_process=temp_process->next)
	{
		if(current_pid==temp_process->pid)
			break;
	}

	printf("\n\n\n\tProcess in running state:");
	if(current_pid==-1)
		printf("\n\t\tNo process is currently running");
	else
		printf("\n\t\tPID: %u\tProcess: %s\tBD: %u",current_pid,temp_process->name,dur);

	printf("\n\n\tBlocked Processes:");
	printf("\n\n\t\tInput Blocked:");
	//Traverse input blocked queue and print the PID and process name
	display_queue(p,input_blocked_queue);
	
	printf("\n\n\t\tOutput Blocked:");
	//Traverse output blocked queue and print the PID and process name
	display_queue(p,output_blocked_queue);

	printf("\n\n\t\tDisk Blocked:");
	//Traverse disk blocked queue and print the PID and process name
	display_queue(p,disk_blocked_queue);

	printf("\n\n\tReady Processes:");
	display_queue(p,ready_queue);
	printf("\n\n");
}

void display_statistics(struct process_node *p, char *str,unsigned total_time)
{
	struct process_node *start=p;
	unsigned time_required=0;
	float utilization=0;

	printf("\n**********   Statistics for %s  **********\n",str);
	for(start=p;start!=NULL;start=start->next)
	{
		printf("\n\tPID: %-3u    Process Name: %-10s",start->pid,start->name);
		printf("\n\t\tWait Time: %3u",start->waiting_time);
		printf("\n\t\tI/O Time : %3u",start->io_time);		//no change for any scheduler
		printf("\n\t\tExec Time: %3u",start->execution_time);	//no change for any scheduler
		printf("\n\t\tTurnaround Time: %3u\n",start->waiting_time+start->io_time+start->execution_time);
	}
	
	for(start=p;start!=NULL;start=start->next)
	{
		time_required+=start->execution_time;
	}

	printf("\n\tDuration of Scheduler: %u",total_time);
	printf("\n\tTotal Execution Time: %u",time_required);
	utilization=(time_required*100)/total_time;
	printf("\n\tCPU Utilization: %.2f %",utilization);
}


void clear_statistics(struct process_node *p)
{
	struct process_node *start=p;
	for(start=p;start!=NULL;start=start->next)
	{
		start->waiting_time=0;
		start->io_time=0;
		start->execution_time=0;
	}
}

//========================================================================================================
//========================================================================================================

struct queue * insert_process_at_end_of_queue(struct queue *q,struct queue *node)
{
	struct queue *temp_queue=NULL;
	time_t current_time;
	unsigned time1,time2;
	
	time(&current_time);
	node->timestamp=current_time;

	if(!isEmpty(q))
	{
		for(temp_queue=q;temp_queue->next!=NULL;temp_queue=temp_queue->next);
			//empty loop for traversal
		
		//append node to end of ready queue
		temp_queue->next=node;
	}
	else
		q=node;

	return q;
}

struct queue * insert_sort_by_priority(struct process_node *p, struct queue *r, struct queue *node)
{
	struct queue *temp_queue=NULL,*prev_node=NULL;
	time_t current_time;
	
	int flag=0,priority1,priority2;
		
	time(&current_time);
	node->timestamp=current_time;

	if(!isEmpty(r))
	{
		temp_queue=r;

		priority1=getPriority(p,temp_queue->pid);
		priority2=getPriority(p,node->pid);

		if(temp_queue->next==NULL)
		{
			if(priority1>priority2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
				temp_queue->next=node;
		}
		else
		{
			if(priority1>priority2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
			{
				while(temp_queue->next!=NULL)	
				{
					prev_node=temp_queue;
					temp_queue=temp_queue->next;

					priority1=getPriority(p,temp_queue->pid);

					if(priority1>priority2)
					{
						prev_node->next=node;
						node->next=temp_queue;
						flag=1;
						break;
					}
				}
				if(flag==0)
					temp_queue->next=node;
			}
		}
	}
	else
		r=node;

	return r;
}

struct queue * insert_sort_by_timestamp(struct process_node *p, struct queue *r, struct queue *node)
{
	struct queue *temp_queue=NULL,*prev_node=NULL;
	time_t current_time;
	unsigned time1,time2;
	int flag=0;
	
	time(&current_time);
	node->timestamp=current_time;

	if(!isEmpty(r))
	{
		temp_queue=r;

		time1=getArrivalTime(p,temp_queue->pid);
		time2=getArrivalTime(p,node->pid);

		if(temp_queue->next==NULL)
		{
			if(time1>time2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
				temp_queue->next=node;
		}
		else
		{
			if(time1>time2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
			{
				while(temp_queue->next!=NULL)	
				{
					prev_node=temp_queue;
					temp_queue=temp_queue->next;
					
					time1=getArrivalTime(p,temp_queue->pid);

					if(time1>time2)
					{
						prev_node->next=node;
						node->next=temp_queue;
						flag=1;
						break;
					}
				}
				if(flag==0)
					temp_queue->next=node;
			}
		}
	}
	else
		r=node;

	return r;
}

struct queue * insert_sort_by_remaining_time(struct process_node *p,struct queue *r,struct queue *node)
{
	struct queue *temp_queue=NULL,*prev_node=NULL;
	time_t current_time;
	unsigned remaining_time1,remaining_time2;
	int flag=0;
	
	time(&current_time);
	node->timestamp=current_time;

	if(!isEmpty(r))
	{
		temp_queue=r;

		remaining_time1=getRemainingTime(p,temp_queue);
		remaining_time2=getRemainingTime(p,node);

		if(temp_queue->next==NULL)
		{
			if(remaining_time1>remaining_time2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
				temp_queue->next=node;
		}
		else
		{
			if(remaining_time1>remaining_time2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
			{
				while(temp_queue->next!=NULL)	
				{
					prev_node=temp_queue;
					temp_queue=temp_queue->next;
					
					remaining_time1=getRemainingTime(p,temp_queue);

					if(remaining_time1>remaining_time2)
					{
						prev_node->next=node;
						node->next=temp_queue;
						flag=1;
						break;
					}
				}
				if(flag==0)
					temp_queue->next=node;
			}
		}
	}
	else
		r=node;

	return r;
}

struct queue * insert_sort_by_burst_duration(struct queue *r, struct queue *node)
{
	struct queue *temp_queue=NULL,*prev_node=NULL;
	time_t current_time;
	unsigned burst_duration1,burst_duration2;
	int flag=0;
	
	time(&current_time);
	node->timestamp=current_time;

	if(!isEmpty(r))
	{
		temp_queue=r;
		burst_duration1=temp_queue->burst->burst_duration;		
		burst_duration2=node->burst->burst_duration;

		if(temp_queue->next==NULL)
		{
			if(burst_duration1>burst_duration2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
				temp_queue->next=node;
		}
		else
		{
			if(burst_duration1>burst_duration2)
			{
				node->next=temp_queue;
				r=node;
			}
			else
			{
				while(temp_queue->next!=NULL)	
				{
					prev_node=temp_queue;
					temp_queue=temp_queue->next;
					
					burst_duration1=temp_queue->burst->burst_duration;

					if(burst_duration1>burst_duration2)
					{
						prev_node->next=node;
						node->next=temp_queue;
						flag=1;
						break;
					}
				}
				if(flag==0)
					temp_queue->next=node;
			}
		}
	}
	else
		r=node;

	return r;
}

struct queue * insert_in_ready_queue(struct process_node *p,struct queue *r,struct queue *node,unsigned parameter)
{
	switch(parameter)
	{
		case 1:	//timestamp
			r=insert_sort_by_timestamp(p,r,node);
			break;
		case 2:	//insert at end of q
		case 6:	//insert at end of q
		case 7:	//insert at end of q
			r=insert_process_at_end_of_queue(r,node);
			break;
		case 3:	//priority
			r=insert_sort_by_priority(p,r,node);
			break;
		case 4:	//sjf
			r=insert_sort_by_burst_duration(r,node);
			break;
		case 5:	//srtf
			r=insert_sort_by_remaining_time(p,r,node);
			break;
	}
	return r;
}

struct queue * initialize_ready_queue(struct process_node *p, struct queue *r, unsigned parameter)
{
	struct process_node *start=p;
	struct queue *queue_node=NULL,*temp_queue=NULL,*prev_node=NULL;
	int flag=0;

	for(start=p;start!=NULL;start=start->next)
	{
		queue_node=(struct queue *)calloc(1,sizeof(struct queue));
		queue_node->pid=start->pid;
		queue_node->timestamp=start->time_of_arrival;
		queue_node->burst=start->burst;
		queue_node->next=NULL;
		
		switch(parameter)
		{
			case 1:	//according to arrival of process in system
				r=insert_sort_by_timestamp(p,r,queue_node);
				break;
			case 2:	//according to arrival of process in ready queue
			case 6:	//for round robin, same as case 2 above

			case 7:	//for round robin 2, same as case 6, same as case 2
				r=insert_process_at_end_of_queue(r,queue_node);
				break;

			case 3:	//according to priority
				r=insert_sort_by_priority(p,r,queue_node);
				break;

			case 4:	//for shortest job first
				r=insert_sort_by_burst_duration(r,queue_node);
				break;

			case 5:	//shortest remaining time first
				r=insert_sort_by_remaining_time(p,r,queue_node);
				break;
		}
	}
	
	return r;
}

//========================================================================================================
//========================================================================================================

struct queue * switch_process_to_ready_queue(struct process_node *p, struct queue *ready_queue, struct queue **blocked_queue,unsigned parameter)
{
	time_t current_time;
	struct queue *temp_queue=NULL,*r=ready_queue,*prev_node=NULL;
	struct process_node *temp_process=NULL;

	if(!isEmpty(blocked_queue))
	{
		temp_queue=*blocked_queue;
		while(temp_queue!=NULL)
		{
			for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
			{
				if(temp_queue->pid==temp_process->pid)
					break;
			}
			time(&current_time);

			if((temp_queue->burst->burst_duration)<=(current_time-temp_queue->timestamp))
			{
				temp_process->io_time+=temp_queue->burst->burst_duration;
				if(temp_queue==*blocked_queue)
				{
					*blocked_queue=(*blocked_queue)->next;
					temp_queue->burst=temp_queue->burst->next;
					temp_queue->next=NULL;
					r=insert_in_ready_queue(p,r,temp_queue,parameter);
					temp_queue=*blocked_queue;
					continue;
				}
				else
				{
					prev_node->next=temp_queue->next;
					temp_queue->burst=temp_queue->burst->next;
					temp_queue->next=NULL;
					r=insert_in_ready_queue(p,r,temp_queue,parameter);					
					temp_queue=prev_node;
				}
			}
			prev_node=temp_queue;
			temp_queue=temp_queue->next;
		}
	}
	return r;
}

struct burst_node * create_burst_node(struct burst_node *node,unsigned duration)
{
	struct burst_node *new_burst_node=(struct burst_node *)calloc(1,sizeof(struct burst_node));
	new_burst_node->burst_duration=node->burst_duration-duration;
	new_burst_node->exit_code=node->exit_code;
	new_burst_node->next=node->next;
	return new_burst_node;
}

int display_options()
{
		int scheduler;
		printf("\n\nSelect an option:");
		printf("\n\t1. FCFS according to system");
		printf("\n\t2. FCFS according to ready queue");
		printf("\n\t3. Priority");
		printf("\n\t4. Shortest Job First");
		printf("\n\t5. Shortest Remaining Time First");
		printf("\n\t6. Round Robin Scheduling");
		printf("\n\t7. Round Robin Scheduling 2");
		printf("\n\t8. Display Process Details");
		printf("\n\t9. Add another process");
		printf("\n\t\b10. Quit");
		printf("\n\nEnter your option: ");
		fflush(stdin);
		scanf("%u",&scheduler);
		return scheduler;
}


struct process_node * create_process(unsigned pid)
{
	struct process_node *q=NULL;
	struct burst_node *b=NULL,*l=NULL,*m=NULL;
	time_t current_time;
	unsigned counter=0;

	q=(struct process_node *)calloc(1,sizeof(struct process_node));
	q->pid=pid;
	printf("\nEnter process name: ");
	fflush(stdin);
	scanf("%s",q->name);
	q->number_of_bursts=random_number(1);
	q->next=NULL;
	time(&current_time);
	q->time_of_arrival=current_time;
	q->execution_time=0;
	q->waiting_time=0;
	q->io_time=0;
	
	q->priority=random_number(2);

	for(counter=1;counter<=q->number_of_bursts;counter++)
	{
		l=(struct burst_node *)calloc(1,sizeof(struct burst_node));
		l->burst_duration=random_number(3);
		if(counter%2==0)	//even I/O burst
			l->exit_code=3;
		else			//odd CPU burst
			l->exit_code=random_number(4);
			l->next=NULL;

		if(!isEmpty(b))
		{
			m=b;
			while(m->next!=NULL)
				m=m->next;
				m->next=l;
		}
		else
			b=l;
	}
		q->burst=b;
	return q;
}

void add_process(struct process_node *p,struct process_node *q)
{
	struct process_node *r=NULL;
	if(!isEmpty(p))
	{
		r=p;
		while(r->next!=NULL)
			r=r->next;
		r->next=q;
	}
	else
		p=q;
}

//========================================================================================================
//========================================================================================================
//End of header.c







