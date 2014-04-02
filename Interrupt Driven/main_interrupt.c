
//#include"header.c"
#include"header_interrupt.c"
#include"3_priority_interrupt.c"
//#include"1_fcfs_system_done.c"
//#include"2_fcfs_ready_done.c"
//#include"3_priority_done.c"
//#include"4_sjf_done.c"
//#include"5_srtf_done.c"
//#include"6_round_robin_done.c"
//#include"7_round_robin_2_done.c"

int main(void)
{
	unsigned process_id=0;
	unsigned scheduler=0;
	unsigned total_time=0;
	unsigned time_quantum=0;
	unsigned fixed_response_time=0;
	char option;
	char *str=NULL;
	struct process_node *p=NULL,*q=NULL,*r=NULL,*start=NULL;

	system("clear");

	printf("STARTING SIMULATION: ");
	do
	{
		q=create_process(process_id++);
		if(p!=NULL)
			add_process(p,q);
		else
			p=q;

		printf("\nOne more process: (y/n)  ");
		fflush(stdin);
		scanf("%s",&option);
	}while(option!='n');
		
	display_process(p);
	printf("\nPress y to continue\n");
	fflush(stdin);
	scanf("%s",&option);

	do
	{
		scheduler=0;
		scheduler=display_options();
		str=NULL;
		switch(scheduler)
		{
			case 1:/*
				if(total_time=fcfs_process_arrives_in_system(p))
				{
					str="FCFS (Process Creation)";
				}*/
				break;
			case 2:/*
				if(total_time=fcfs_process_arrives_in_ready_queue(p))
				{
					str="FCFS (Arrival in Ready Queue)";
				}*/
				break;
			case 3:
				if(total_time=priority_scheduler(p))
				{
					str="Priority Scheduler";
				}
				break;
			case 4:/*
				if(total_time=sjf_scheduler(p))
				{
					str="Shortest Job First Scheduler";
				}*/
				break;
			case 5:/*
				if(total_time=srtf_scheduler(p))
				{
					str="Shortest Remaining Time First Scheduler";
				}*/
				break;
			case 6:/*
				printf("\nEnter time quantum: ");
				fflush(stdin);
				scanf("%u",&time_quantum);
				if(total_time=round_robin_scheduler(p,time_quantum))
				{
					str="Round Robin Scheduler";
				}*/
				break;
			case 7:/*
				printf("\nEnter fixed response duration: ");
				fflush(stdin);
				scanf("%u",&fixed_response_time);
				if(total_time=round_robin_scheduler_2(p,fixed_response_time))
				{
					str="Round Robin Scheduler 2";
				}*/
				break;
			case 8://display process details
				display_process(p);
				break;
			case 9://add another process
				q=create_process(process_id++);
				if(!isEmpty(p))
					add_process(p,q);
				else
					p=q;
				break;
			case 10:
				for(start=p;start!=NULL;)
				{
					r=start;
					start=start->next;
					free(r);
					r=NULL;
				}
				exit(0);
				break;
			default:
				printf("\nError: Pls try again\n");
		}
		
		if(scheduler>=1 && scheduler<=7)//!=8 && scheduler!=0)
		{
			if(total_time)
			{
				display_statistics(p,str,total_time);
				clear_statistics(p);
			}
			else
			{
				printf("\nScheduler failed\n");
			}
		}

	}while(1);
	return 0;
}

