
unsigned fcfs_process_arrives_in_ready_queue(struct process_node *p)
{
	struct queue *i=NULL,*o=NULL,*d=NULL,*r=NULL;
	struct queue *temp_queue=NULL;
	struct process_node *temp_process=NULL;	
	struct burst_node *b=NULL;
	time_t current_time,start_time,end_time;
	unsigned flag=0;

	time(&start_time);
	
	r=initialize_ready_queue(p,r,2);

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

			r=switch_process_to_ready_queue(p,r,&i,2);
			r=switch_process_to_ready_queue(p,r,&o,2);
			r=switch_process_to_ready_queue(p,r,&d,2);			
			temp_queue=r;
		}
		
		display_task_manager(-1,p,i,o,d,r,0);

		if(!isEmpty(i)||!isEmpty(o)||!isEmpty(d)||!isEmpty(r))
			flag=1;
		else
			flag=0;
		
		if(flag)
		{
			r=switch_process_to_ready_queue(p,r,&i,2);
			r=switch_process_to_ready_queue(p,r,&o,2);
			r=switch_process_to_ready_queue(p,r,&d,2);
		}

	}while(flag);

	time(&end_time);

	return((int)end_time-start_time);
}

