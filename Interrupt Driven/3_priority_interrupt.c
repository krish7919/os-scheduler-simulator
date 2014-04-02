
//Interrupt Driven Priority Scheduler

#ifndef _HEADER_INTERRUPT_H
#include"header_interrupt.c"
#endif

//#define STARVATION_LIMIT 15
/*
void check_starvation(struct process_node *p,struct queue *ready_queue)
{
	struct queue *temp_queue=ready_queue;
	struct process_node *temp_process=NULL;
	time_t current_time;

	for(temp_queue;temp_queue!=NULL;temp_queue=temp_queue->next)
	{
		time(&current_time);
		
		if(current_time-temp_queue->timestamp>STARVATION_LIMIT)	//process waiting>STARVATION_LIMIT
		{
			for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
			{
				if(temp_process->pid==temp_queue->pid)
					if(temp_process->priority>0)
						temp_process->priority--;		//increased prority
						////????rearrange ready queue??? later
					
			}
			temp_queue->timestamp=current_time;			//reset waiting time
			
			printf("\nSTARVATION ^o^\n###############################\n");
		}
	}
}
*/

struct interrupt
{
	unsigned pid;
//	time_t timestamp;
	unsigned duration;
	struct interrupt *next;
};


struct interrupt * insert_sort_by_interrupt(struct interrupt *interrupt_queue,struct interrupt *node)
{
	struct interrupt *temp_interrupt=NULL,*prev_node=NULL;
	int flag=0,interrupt1,interrupt2;
	
	if(!isEmpty(interrupt_queue))
	{
		temp_interrupt=interrupt_queue;
		
		interrupt1=temp_interrupt->duration;
		interrupt2=node->duration;
		
		if(temp_interrupt->next==NULL)
		{
			if(interrupt1>interrupt2)
			{
				node->next=temp_interrupt;
				interrupt_queue=node;
			}
			else
				temp_interrupt->next=node;
		}
		else
		{
			if(interrupt1>interrupt2)
			{
				node->next=temp_interrupt;
				interrupt_queue=node;
			}
			else
			{
				while(temp_interrupt->next!=NULL)	
				{
					prev_node=temp_interrupt;
					temp_interrupt=temp_interrupt->next;

					interrupt1=temp_interrupt->duration;

					if(interrupt1>interrupt2)
					{
						prev_node->next=node;
						node->next=temp_interrupt;
						flag=1;
						break;
					}
				}
				if(flag==0)
					temp_interrupt->next=node;
			}
		}
	}
	else
		interrupt_queue=node;

	return interrupt_queue;
}


struct queue * insert_process_in_blocked_queue(struct queue *q,struct queue *node,struct interrupt **interrupt_queue)
{
	struct queue *temp_queue=NULL;
	struct interrupt *temp_interrupt=NULL;
	struct burst_node *b=NULL;
	time_t current_time;
printf("\n3. interrupt_queue: %u",interrupt_queue);
printf("\n4. *interrupt_queue: %u",*interrupt_queue);
//printf("\n5. interrupt_queue: %u\n\n",**interrupt_queue);
	//creating new node for interrupt
	temp_interrupt=(struct interrupt *)calloc(1,sizeof(struct interrupt));
	temp_interrupt->pid=node->pid;

	b=node->burst->next;
	//b=b->next;

	temp_interrupt->duration=b->burst_duration;	
	temp_interrupt->next=NULL;
	interrupt_queue=insert_sort_by_interrupt(*(interrupt_queue),temp_interrupt);//
	
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


void subtract_burst_duration_from_interrupt_queue(struct interrupt *iq,unsigned dur)
{
	struct interrupt *temp_interrupt=NULL;
	if(!isEmpty(iq))
	{
		for(temp_interrupt=iq;temp_interrupt!=NULL;temp_interrupt=temp_interrupt->next)
		{
			temp_interrupt->duration-=dur;
		}
	}
	else	///shud never come to else ^o^
		printf("\n$$$$$$$$$$$ MAHAN ERROR HERE $$$$$$$$$$$\n");sleep(60);
}

void display_interrupt_queue(struct interrupt *interrupt_ptr)
{
	struct interrupt *temp_interrupt=NULL;
	printf("\n\nInterrupt Queue:   %u",interrupt_ptr);
	for(temp_interrupt=interrupt_ptr;temp_interrupt!=NULL;temp_interrupt=temp_interrupt->next)
	{
		printf("\n\tPID: %u\tDuration: %u",temp_interrupt->pid,temp_interrupt->duration);
	}
}

unsigned priority_scheduler(struct process_node *p)
{
	struct queue *i=NULL,*o=NULL,*d=NULL,*r=NULL;
	struct queue *temp_queue=NULL;
	struct process_node *temp_process=NULL;	
	struct burst_node *b=NULL,*new_burst=NULL;
	time_t current_time,start_time,end_time;
	unsigned flag=0;

	struct interrupt *interrupt_queue=NULL,*temp_interrupt=NULL;

	time(&start_time);

	r=initialize_ready_queue(p,r,3);
	display_queue(p,r);

	do
	{
		temp_queue=r;
		while(temp_queue!=NULL)
		{
			for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
				if(temp_queue->pid==temp_process->pid)
				{
					b=temp_queue->burst;
					break;
				}

			if(b->next!=NULL)
			{
				//CPU burst
				time(&current_time);
				temp_process->waiting_time+=(current_time-temp_queue->timestamp);
				r=r->next;			//changed ready queue
				temp_queue->timestamp=current_time;

				if(interrupt_queue!=NULL)
				{
					//execute only till interrupt duration
					//burst_duration=burst_duration - interrupt_duration
					//and if interrupt_queue->duration==0 then 
					//free that node and interrupt_queue=interrupt_queue->next
					
					if(b->burst_duration>interrupt_queue->duration)
					{
						display_task_manager(temp_process->pid,p,i,o,d,r,interrupt_queue->duration);
						temp_process->execution_time+=interrupt_queue->duration;
						sleep(interrupt_queue->duration);
						subtract_burst_duration_from_interrupt_queue(interrupt_queue,interrupt_queue->duration);
						//create burst too!!!
						new_burst=(struct burst_node *)calloc(1,sizeof(struct burst_node));
						new_burst->burst_duration=b->burst_duration-interrupt_queue->duration;
						new_burst->exit_code=b->exit_code;
						new_burst->next=b->next;
						temp_queue->burst=new_burst;
						r=insert_in_ready_queue(p,r,temp_queue,3);
						//check for interrupts ie check blocked queues now
					}
					else
					{
						//execute burst and (interrupt_queue->duration - temp_queue->burst_duration)
						//for each node in interrupt_queue
						temp_process->execution_time+=b->burst_duration;
						display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
						sleep(b->burst_duration);
						subtract_burst_duration_from_interrupt_queue(interrupt_queue,b->burst_duration);
						temp_queue->burst=b->next;
						if(interrupt_queue->duration==0) //handles the case of b->burst_duration==interrupt_queue->duration
						{
							temp_interrupt=interrupt_queue;
							interrupt_queue=interrupt_queue->next;
							free(temp_interrupt);
							temp_interrupt=NULL;
						}
					}
				}
				else	//interrupt queue is null; let the first burst execute as in nonpremptive algo
				{
					display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
					sleep(b->burst_duration);
					//interrupt_queue=insert_sort_by_interrupt(interrupt_queue,temp_interrupt);
					//display_interrupt_queue(interrupt_queue);
				}
printf("\n1. interrupt_queue: %u",interrupt_queue);
printf("\n2. interrupt_queue: %u",&interrupt_queue);
				//push node to blocked queue - 3 cases
				switch(b->exit_code)
				{
					case 0:			//input blocked queue
						i=insert_process_in_blocked_queue(i,temp_queue,&interrupt_queue);
						break;
					case 1: 		//output blocked queue
						o=insert_process_in_blocked_queue(o,temp_queue,&interrupt_queue);
						break;
					case 2:			//disk blocked queue
						d=insert_process_in_blocked_queue(d,temp_queue,&interrupt_queue);
						break;
				}
display_interrupt_queue(interrupt_queue);
			}
			else
			{
printf("\nLast burst in process");
				time(&current_time);
				temp_process->waiting_time+=(current_time-temp_queue->timestamp);
				temp_process->execution_time+=b->burst_duration;
				r=r->next;
				display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
			////use interrupts here too
				sleep(b->burst_duration);
				printf("\n*****************  ");
				printf("PID: %u\tProcess %s  finished execution",temp_queue->pid,temp_process->name);
				printf("  *****************\n");
				sleep(3);
				//free memory
				free(temp_queue);
				temp_queue=NULL;
			}

			r=switch_process_to_ready_queue(p,r,&i,3);
			r=switch_process_to_ready_queue(p,r,&o,3);
			r=switch_process_to_ready_queue(p,r,&d,3);

			//check ready queue for starving blocked processes
			
//			check_starvation(p,r);
			
			temp_queue=r;
		}
		
		display_task_manager(-1,p,i,o,d,r,0);

		if(!isEmpty(i)||!isEmpty(o)||!isEmpty(d)||!isEmpty(r))
			flag=1;
		else
			flag=0;
		
		if(flag)
		{
			r=switch_process_to_ready_queue(p,r,&i,3);
			r=switch_process_to_ready_queue(p,r,&o,3);
			r=switch_process_to_ready_queue(p,r,&d,3);
		}

	}while(flag);

	time(&end_time);

	return((int)end_time-start_time);

}

