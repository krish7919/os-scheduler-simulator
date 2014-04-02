
#ifndef _HEADER_MEM_MGMT_C
#include"header_mem_mgmt.c"
#endif
#define STARVATION_LIMIT 15

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
			sleep(1);
		}
	}
}

unsigned priority_scheduler(struct process_node *p)
{
	struct queue *i=NULL,*o=NULL,*d=NULL,*r=NULL;
	struct queue *temp_queue=NULL;
	struct process_node *temp_process=NULL;	
	struct burst_node *b=NULL;
	time_t current_time,start_time,end_time;
	unsigned flag=0;
//	unsigned display_option=0;
	unsigned isProcessPresent=0;

	time(&start_time);

	r=initialize_ready_queue(p,r,3);
//	r=initialize_ready_queue_modded_as_per_maam(p,r,3);
	
	initialize_page(p,r); //initialize page number for the processes to request first page number

	printf("\nReady Queue Initialized as follows: ");
	display_queue(p,r);
	printf("\n\nInitial Memory Map is as follows: ");
	display_memory_map(p,r);

	do
	{
		temp_queue=r;
		while(temp_queue!=NULL)
		{
			for(temp_process=p;temp_process!=NULL;temp_process=temp_process->next)
			{
				if(temp_queue->pid==temp_process->pid)
				{
					break;
				}
			}
			
			b=temp_queue->burst;

			if(b->next!=NULL)
			{
				//CPU burst
				time(&current_time);
				temp_process->waiting_time+=(current_time-temp_queue->timestamp);
				temp_process->execution_time+=b->burst_duration;
				r=r->next;			//changed ready queue
				temp_queue->timestamp=current_time;
				time(&current_time);
				temp_queue->burst=b->next;
				temp_queue->next=NULL;
				
/*				display_option=display_options_for_subsystems();
				switch(display_option)
				{
					case 1:
						display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
						sleep(b->burst_duration);
						break;
					case 2:
						display_memory_map(p,r);
						break;
				}
*/

				display_task_manager(temp_process->pid,p,i,o,d,r,b->burst_duration);
				sleep(b->burst_duration);
				
				//push node to blocked queue - 3 cases
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
			else
			{
				time(&current_time);
				temp_process->waiting_time+=(current_time-temp_queue->timestamp);
				temp_process->execution_time+=b->burst_duration;
				r=r->next;
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

			r=switch_process_to_ready_queue(p,r,&i,3);
			r=switch_process_to_ready_queue(p,r,&o,3);
			r=switch_process_to_ready_queue(p,r,&d,3);

			//check ready queue for starving blocked processes
			
			check_starvation(p,r);
			
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
		sleep(1);

	}while(flag);

	time(&end_time);

	return((int)end_time-start_time);

}

