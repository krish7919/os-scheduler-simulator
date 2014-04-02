
#ifndef _HEADER_MEM_MGMT_C
#include"header_mem_mgmt.c"
#endif
#define STARVATION_LIMIT 15


int calculate_burst_duration(struct queue *ready_queue,unsigned fixed_response_time)
{
	int count=0;
	struct queue *temp_queue=ready_queue;
	
	if(!isEmpty(ready_queue))
	{
		for(temp_queue;temp_queue!=NULL;temp_queue=temp_queue->next)
			count++;

		return (int)(fixed_response_time/count);
	}
	else
		return 0;
}

unsigned round_robin_scheduler_2(struct process_node *p,unsigned fixed_response_time)
{
	struct queue *i=NULL,*o=NULL,*d=NULL,*r=NULL;
	struct process_node *temp_process=NULL;	
	struct queue *temp_queue=NULL;
	struct burst_node *b=NULL;
	time_t current_time,start_time,end_time;
	unsigned flag=0;
	int burst_duration_for_ready_queue=0;

	time(&start_time);

	r=initialize_ready_queue(p,r,7);
	initialize_page(p,r); //initialize page number for the processes to request first page number

	printf("\nReady Queue Initialized as follows: ");
	display_queue(p,r);
	printf("\n\nInitial Memory Map is as follows: ");
	display_memory_map(p,r);

	printf("\n\n**********  Fixed time slice: %d  **********\n",fixed_response_time);
	
	do
	{
		temp_queue=r;
		while(temp_queue!=NULL)
		{
			burst_duration_for_ready_queue=calculate_burst_duration(r,fixed_response_time);
			printf("\n1. Burst Duration allocated: %d",burst_duration_for_ready_queue);

			for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
				if(temp_queue->pid==temp_process->pid)
					break;
			
			b=temp_queue->burst;

			if(b->next!=NULL)
			{
				//CPU burst
				time(&current_time);
				temp_process->waiting_time+=(current_time-temp_queue->timestamp);
				r=r->next;			//changed ready queue

				temp_queue->timestamp=current_time;
				time(&current_time);				
				temp_queue->next=NULL;

				if(burst_duration_for_ready_queue!=0)
				{
					if(b->burst_duration>burst_duration_for_ready_queue)
					{
						temp_process->execution_time+=burst_duration_for_ready_queue;
						
						temp_queue->burst=create_burst_node(b,burst_duration_for_ready_queue);

						display_task_manager(temp_process->pid,p,i,o,d,r,burst_duration_for_ready_queue);
						sleep(burst_duration_for_ready_queue);
						//insert process back into ready queue
						r=insert_in_ready_queue(p,r,temp_queue,7);
					}
					else
					{
						temp_process->execution_time+=b->burst_duration;
						temp_queue->burst=b->next;
						display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
						sleep(b->burst_duration);
		
						//push node to blocked queue
						switch(b->exit_code)
						{
							case 0:			//input blocked queue
								i=insert_process_at_end_of_queue(i,temp_queue);
								break;
							case 1: 		//output blocked queue
								o=insert_process_at_end_of_queue(o,temp_queue);
								break;
							case 2:			//disk blocked queue
								d=insert_process_at_end_of_queue(d,temp_queue);
						handle_disk_block(p,d,temp_queue->pid, random_number(6));
						display_memory_map(p,r);

								break;
						}
					}
				}
			}
			else
			{

				time(&current_time);
				temp_process->waiting_time+=(current_time-temp_queue->timestamp);
				r=r->next;		//changed ready queue

				temp_queue->timestamp=current_time;
				time(&current_time);
				temp_queue->next=NULL;
				if(burst_duration_for_ready_queue!=0)
				{			
					if(b->burst_duration>burst_duration_for_ready_queue)
					{
						temp_process->execution_time+=burst_duration_for_ready_queue;
						temp_queue->burst=create_burst_node(b,burst_duration_for_ready_queue);
						display_task_manager(temp_process->pid,p,i,o,d,r,burst_duration_for_ready_queue);
						sleep(burst_duration_for_ready_queue);
						//insert process back into ready queue
						r=insert_in_ready_queue(p,r,temp_queue,7);
					}
					else
					{
						temp_process->execution_time+=b->burst_duration;
						display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
						sleep(b->burst_duration);
						printf("\n*****************  ");
						printf("PID: %u\tProcess %s  finished execution",temp_queue->pid,temp_process->name);
						printf("  *****************\n");
						sleep(3);
						//free memory
						free(temp_queue);
						temp_queue=NULL;
					}
				}
			}

			r=switch_process_to_ready_queue(p,r,&i,7);
			r=switch_process_to_ready_queue(p,r,&o,7);
			r=switch_process_to_ready_queue(p,r,&d,7);

			temp_queue=r;
		}
		
		display_task_manager(-1,p,i,o,d,r,0);

		if(!isEmpty(i)||!isEmpty(o)||!isEmpty(d)||!isEmpty(r))//implement this line of code in other ALGOS
			flag=1;
		else
			flag=0;

		if(flag)
		{
			r=switch_process_to_ready_queue(p,r,&i,7);
			r=switch_process_to_ready_queue(p,r,&o,7);
			r=switch_process_to_ready_queue(p,r,&d,7);
		}

	}while(flag);

	time(&end_time);
	return((int)end_time-start_time);
}
