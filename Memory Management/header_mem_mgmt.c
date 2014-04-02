
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define _HEADER_MEM_MGMT_C
#define MAX_MEMORY 64
#define NUMBER_OF_FRAMES 3

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

	unsigned number_of_pages;
	struct page_node *page;
	
	unsigned relative_time_of_arrival;//stores relative time diff (compared to prev proces), -1 if process already included in scheduler
	unsigned isIncluded;		//change this to 1 if process included in scheduler, set tis to 0 at start of schedulr

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

struct memory_node
{
	unsigned frame_number;
	unsigned occupied;
};

struct page_node
{
	unsigned frame_index;
	unsigned page_number;
	unsigned modified;
	struct page_node *next;
};

struct memory_node MEMORY[MAX_MEMORY];		//GLOBAL VARIABLE

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
        int burst=0;
	int number_of_pages=0;

//	srand(time(NULL));
	switch(parameter)
	{
		case 1:		//bursts
			burst=rand()%30;
		        if(burst%2==0)
		                return burst+1;
		        else
		                return burst;
		case 2:
		case 3:
			return(rand()%10+1);
		case 4:		//exit_code
		        return(rand()%3);
		case 5:		//number_of_pages
			number_of_pages=rand()%20;
			if(number_of_pages<3)
				number_of_pages=3;
			return(number_of_pages);
		case 6:		//page_fault_code
			return(rand()%NUMBER_OF_FRAMES);
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
                printf("\n\t|Bursts: ");
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
				printf("|  |");
				printf("\n\t|\t ");
				printf("|------|------|------|------|------|------|------|  |");
				printf("\n\t|\t ");
				counter=0;
			}
                }
		counter=DISPLAY_BURST-1-counter;
		if(counter)
		{
			for(i=0;i<counter;i++)
				printf("|      ");
		}
		printf("|  |");
		printf("\n\t|\t |______|______|______|______|______|______|______|  |");
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
		printf("\n\n");
		printf("\n\t ____________________________________________________________");
                printf("\n\t| PID |   NAME   | CREATION TIME | BURSTS | PRIORITY | PAGES |");
		printf("\n\t|_____|__________|_______________|________|__________|_______|");

                while(q!=NULL)
                {
			//Convert time to strung to display in readable format????? not implemented
                        printf("\n\t| %3u |%-10s|%13u  |\t\b\b%2u  |   %4u   |  %3u  |", \
				q->pid,q->name,(int)q->time_of_arrival,q->number_of_bursts,q->priority,q->number_of_pages);
			printf("\n\t|_____|__________|_______________|________|__________|_______|");
                        b=q->burst;
                        display_bursts(b);
                        q=q->next;
			printf("\n\t|____________________________________________________________|");
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
printf("\tTS: %u",(int)temp_process->time_of_arrival);
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

//static int counter=0;

		system("clear");
printf("\n\n==================================================================================\n");

//printf("\n%d\n%u",++counter,(int)time(NULL));

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
	printf("\n\tCPU Utilization: %.2f %%",utilization);
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

/*
struct queue * initialize_ready_queue_modded_as_per_maam(struct process_node *p, struct queue *r, unsigned parameter)
{
	struct process_node *start=NULL;
	struct queue *queue_node=NULL;

	time_t current_time;
	unsigned insertNext=1;

	for(start=p;insertNext==1 && start!=NULL;start=start->next)
	{
		if(start->isIncluded==0)
		{
			time(&current_time);
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
*/
/*
struct queue * poll_process_queue(struct process_node *p,struct queue *r)
{
	struct process_node *temp_process=NULL;
	struct 
	
	return r;
}
*/
//struct queue * 


struct queue * initialize_ready_queue(struct process_node *p, struct queue *r, unsigned parameter)
{
	struct process_node *start=p;
	struct queue *queue_node=NULL;

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

int display_options(void)
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
	
	q->number_of_pages=random_number(5);
	
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

	q->isIncluded=0;

	return q;
}

void add_process(struct process_node *p,struct process_node *q)
{
	struct process_node *r=NULL;
	
	if(!isEmpty(p))		//cannot be empty but still checkin  :)
	{
		r=p;
		while(r->next!=NULL)
			r=r->next;
		
		q->relative_time_of_arrival=(int)(q->time_of_arrival - r->time_of_arrival);
//printf("\n%u  -- %u   Rel %u",r->time_of_arrival,q->time_of_arrival,q->relative_time_of_arrival);
		
		r->next=q;
	}
//	else			process list cannot be empty as first process is added in main()
//		p=q;
}


//========================================================================================================
//========================================================================================================
//Memory Management Code starts here

void initialize_page(struct process_node *process,struct queue *ready)
{
	struct process_node *temp_process=NULL;
	struct queue *temp_queue=NULL;
	struct page_node *temp_page=NULL;
	struct page_node *temp_page_ptr=NULL;
	
	unsigned pages_to_be_allotted=0;
	unsigned counter=0;
	unsigned seed=1;
	unsigned pageExists=0;

	for(temp_queue=ready;temp_queue!=NULL;temp_queue=temp_queue->next)
	{
		for(temp_process=process;temp_process!=NULL;temp_process=temp_process->next)
		{
			if(temp_queue->pid==temp_process->pid)
			{
				if(NUMBER_OF_FRAMES>temp_process->number_of_pages)
					pages_to_be_allotted=temp_process->number_of_pages;
				else
					pages_to_be_allotted=NUMBER_OF_FRAMES;

				for(counter=0;counter<pages_to_be_allotted;counter++)
				{
					//srand(time(NULL));   UNCOMMENT LATER
					temp_page=(struct page_node *)calloc(1,sizeof(struct page_node));
					
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
					//Code for getting unique page numbers for each process
					while(1)
					{
						srand(++seed);		//NOTE THIS!!!
						temp_page->page_number=(rand()%(temp_process->number_of_pages));
						pageExists=0;
						for(temp_page_ptr=temp_process->page;temp_page_ptr!=NULL;temp_page_ptr=temp_page_ptr->next)
						{
							if(temp_page->page_number==temp_page_ptr->page_number)
							{
								pageExists=1;
								break;
							}
						}
						if(pageExists==1)
							continue;
						else
							break;
					}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

					temp_page->modified=rand()%2;		//can be 0 or 1
					seed=0;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
					//Code for mapping each page to a unique frame
					for(;;)
					{
						srand(++seed);		//NOTE THIS!!!
						temp_page->frame_index=(rand()%MAX_MEMORY);
					//printf("temp_page->frame_index : %d",temp_page->frame_index);
						if(MEMORY[temp_page->frame_index].occupied==1)
						{
					//printf("\nFrame occupied %d",seed);
							continue;
						}
						else
						{
							MEMORY[temp_page->frame_index].occupied=1;
							break;
						}
					}
					if(temp_process->page==NULL)
						temp_process->page=temp_page;
					else
					{
						//traversing to the end of page table
						for(temp_page_ptr=temp_process->page;temp_page_ptr->next!=NULL;temp_page_ptr=temp_page_ptr->next);
						temp_page_ptr->next=temp_page;
					}
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				}
			//?page number can be zero or needs to start from 1? here, page number starts from 1
				break;		//break from loop as process found
			}
		}
	}
}

void handle_disk_block(struct process_node *p,struct queue *d,unsigned pid,unsigned page_fault_code)
{
	//request new random page number
	//check if page is modified
	//if modified, double duration
	struct queue *temp_queue=NULL;
	struct process_node *temp_process=NULL;
	struct page_node *temp_page_ptr=NULL;
	struct page_node *ptr_to_page_to_be_changed=NULL;
	unsigned flag=0;
	unsigned new_page_number=0;
	
	for(temp_queue=d;temp_queue!=NULL;temp_queue=temp_queue->next)
	{
		for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
		{
			if(temp_queue->pid==pid && temp_process->pid==pid)	//found orrespponding blocked queue node and process node
			{

				if(page_fault_code==1)
					ptr_to_page_to_be_changed=temp_process->page->next;
				else if(page_fault_code==2)
					ptr_to_page_to_be_changed=temp_process->page->next->next;

				//srand(time(NULL)); 	UNCOMMENT LATER
				new_page_number=rand()%temp_process->number_of_pages;
				
				for(temp_page_ptr=temp_process->page;temp_page_ptr!=NULL;temp_page_ptr=temp_page_ptr->next)
				{
					//checking if old and new page numbers are equal
					if(new_page_number==temp_page_ptr->page_number)
					{
						new_page_number++;		//changing page number as new cannot be == to old
						temp_page_ptr=temp_process->page;
						continue;
					}
					temp_page_ptr->page_number=new_page_number;
					
					//checking whether current page modified or not
					//double the burst duration if modified
					if(temp_process->page->modified==1)
						temp_queue->burst->burst_duration*=2;
					//get new modified value
					temp_page_ptr->modified=rand()%2;
					flag=1;
					break;
				}
			}
			if(flag==1)
				break;
		}
	}
}


/*
int display_options_for_subsystems()
{
	int option=0;
	
	printf("\n1. Display Task Manager");
	printf("\n2. Display Memory Map");
	printf("\nEnter your option: ");
	fflush(stdin);
	scanf("%d",&option);
	return option;	
}
*/ 

void display_memory_map(struct process_node *p,struct queue *ready_queue)
{
	unsigned counter=0;
	unsigned total_frames_in_use=0;
	unsigned frames_per_process=0;
	unsigned breakFlag=0;
	struct process_node *temp_process=NULL;
	struct page_node *temp_page_ptr=NULL;
	struct queue *temp_queue=NULL;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	static unsigned anotherTerminal=0;		//flag to check if another terminal needed
	
//	if(anotherTerminal==0)
//	{
//		system("gnome-terminal");
//		anotherTerminal=1;
//	}
//	sleep(1);
//	system("exec 1> /dev/pts/2;echo \"HI\"");
//	system("exec 3>&1");
//	system("exec 1>output.txt");

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	for(counter=0;counter<MAX_MEMORY;counter++)
	{
		if(MEMORY[counter].occupied==1)
		{
			total_frames_in_use++;
		}
	}

	fprintf(stdout,"\n\n\t\t\t********** MEMORY MAP **********");

	printf("\n\n\t\tTotal Memory: %u\tTotal Memory in Use: %u",MAX_MEMORY,total_frames_in_use);
	printf("\n");
	
	for(temp_queue=ready_queue;temp_queue!=NULL;temp_queue=temp_queue->next)
	{
		for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
		{
			if(temp_queue->pid==temp_process->pid)
			{
				frames_per_process=0;
				for(temp_page_ptr=temp_process->page;temp_page_ptr!=NULL;temp_page_ptr=temp_page_ptr->next)
				{
					++frames_per_process;
				}
				
				printf("\n\t ___________________________________________________________________ ");
				printf("\n\t|  PID: %2u  |  Name: %-10s  |  Total Pages: %2u  |  Frames: %2u  |", \
					temp_process->pid,temp_process->name,temp_process->number_of_pages,frames_per_process);
				printf("\n\t|___________|____________________|___________________|______________|");
				printf("\n\t|        |  Page Number  |  Frame Index  |  Page Modified  |        |");
				printf("\n\t|        |---------------|---------------|-----------------|        |");
				for(temp_page_ptr=temp_process->page;temp_page_ptr!=NULL;temp_page_ptr=temp_page_ptr->next)
				{
					printf("\n\t|        |      %2u       |      %2u       |       %2u        |        |", \
						temp_page_ptr->page_number,temp_page_ptr->frame_index,temp_page_ptr->modified);
				}
				printf("\n\t|________|_______________|_______________|_________________|________|");
				
//				breakFlag=1;
//				break;
			}
		}
//		if(breakFlag==1)
//			break;
	}
	
//	system("exec 1>/dev/pts/1");???
}
//End of header.c

