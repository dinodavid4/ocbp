#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define NO_OF_VCPUS			3	
#define LOW_CRITICALITY			0
#define HIGH_CRITICALITY		1

typedef long int s_time_t;


struct rtmixcrit_vcpu  {
	s_time_t period;
	s_time_t release; 
	s_time_t deadline; 
	bool criticality_vcpu;
	s_time_t budget_low; 
	s_time_t budget_high; 
};

struct vcpu_stack {
	struct rtmixcrit_vcpu vcpu_data;
	struct vcpu_stack * next_vcpu;
};

struct vcpu_stack *first_vcpu = NULL, *temp;

int updateQueue(struct rtmixcrit_vcpu vcpu_queue) 
{
	//LIFO is considered because the vcpu with lowest priority is pushed into the stack one by one
	//printf("Deadline Time = %ld\n",vcpu_queue.deadline);
	temp = (struct vcpu_stack *)malloc(sizeof(struct vcpu_stack));
	temp->vcpu_data = vcpu_queue;
	temp->next_vcpu = first_vcpu;
	first_vcpu = temp;
	return 0;
}

int getJobData(struct rtmixcrit_vcpu * vcpu_to_schedule) 
{
	vcpu_to_schedule[0].period 		= 8000;
	vcpu_to_schedule[0].release   		= 0;
	vcpu_to_schedule[0].deadline  		= 4000;
	vcpu_to_schedule[0].criticality_vcpu    = 0;
	vcpu_to_schedule[0].budget_low  	= 2000;
	vcpu_to_schedule[0].budget_high 	= 2000;

	vcpu_to_schedule[1].period 		= 10000;
	vcpu_to_schedule[1].release   		= 0;
	vcpu_to_schedule[1].deadline  		= 5000;
	vcpu_to_schedule[1].criticality_vcpu    = 1;
	vcpu_to_schedule[1].budget_low  	= 2000;
	vcpu_to_schedule[1].budget_high		= 4000;

	vcpu_to_schedule[2].period 		= 20000;
	vcpu_to_schedule[2].release   		= 0;
	vcpu_to_schedule[2].deadline  		= 10000;
	vcpu_to_schedule[2].criticality_vcpu    = 1;
	vcpu_to_schedule[2].budget_low  	= 2000;
	vcpu_to_schedule[2].budget_high 	= 4000;
	/*

	   vcpu_to_schedule[3].release   		= 0;//8;
	   vcpu_to_schedule[3].deadline  		= 12;
	   vcpu_to_schedule[3].criticality_vcpu    = 0;
	   vcpu_to_schedule[3].budget_low  	= 2;
	   vcpu_to_schedule[3].budget_high 	= 2;

	   vcpu_to_schedule[4].release   		= 0;//10;
	   vcpu_to_schedule[4].deadline  		= 15;
	   vcpu_to_schedule[4].criticality_vcpu    = 1;
	   vcpu_to_schedule[4].budget_low  	= 2;
	   vcpu_to_schedule[4].budget_high		= 4;

	   vcpu_to_schedule[5].release   		= 0;//16;
	   vcpu_to_schedule[5].deadline  		= 20;
	   vcpu_to_schedule[5].criticality_vcpu    = 0;
	   vcpu_to_schedule[5].budget_low  	= 2;
	   vcpu_to_schedule[5].budget_high 	= 2;

	   vcpu_to_schedule[6].release   		= 0;//20;
	   vcpu_to_schedule[6].deadline  		= 25;
	   vcpu_to_schedule[6].criticality_vcpu    = 1;
	   vcpu_to_schedule[6].budget_low  	= 2;
	   vcpu_to_schedule[6].budget_high 	= 4;

	   vcpu_to_schedule[7].release   		= 0;//20;
	   vcpu_to_schedule[7].deadline  		= 30;
	   vcpu_to_schedule[7].criticality_vcpu    = 1;
	   vcpu_to_schedule[7].budget_low  	= 2;
	   vcpu_to_schedule[7].budget_high		= 4;

	   vcpu_to_schedule[8].release   		= 0;//24;
	   vcpu_to_schedule[8].deadline  		= 28;
	   vcpu_to_schedule[8].criticality_vcpu    = 0;
	   vcpu_to_schedule[8].budget_low  	= 2;
	   vcpu_to_schedule[8].budget_high 	= 2;

	   vcpu_to_schedule[9].release   		= 0;//30;
	   vcpu_to_schedule[9].deadline  		= 35;
	   vcpu_to_schedule[9].criticality_vcpu    = 1;
	   vcpu_to_schedule[9].budget_low  	= 2;
	   vcpu_to_schedule[9].budget_high 	= 4;

	   vcpu_to_schedule[10].release   		= 0;//32;
	   vcpu_to_schedule[10].deadline  		= 36;
	   vcpu_to_schedule[10].criticality_vcpu   = 0;
	   vcpu_to_schedule[10].budget_low  	= 2;
	   vcpu_to_schedule[10].budget_high	= 2;
	 */

	return 0;
}

int calculatePriority(struct rtmixcrit_vcpu * vcpu_to_schedule) 
{
	int count;
	int loop_count = 0;
	int j_fxd_index;
	int vcpu_index;
	s_time_t total_wcet_time = 0; 

	for(j_fxd_index = 0; j_fxd_index < NO_OF_VCPUS ; j_fxd_index++) {
		for(vcpu_index = 0; vcpu_index < (NO_OF_VCPUS-loop_count) ; vcpu_index++) {

			if(vcpu_to_schedule[vcpu_index].criticality_vcpu == HIGH_CRITICALITY) {
				//printf("Entered HIGH Criticality\n");

				/* Check if the current vcpu can be assigned with LOW Priority*/
				for(count = vcpu_index + 1; count < (NO_OF_VCPUS-loop_count) ; count++) {
					total_wcet_time += vcpu_to_schedule[count].budget_high;
				}
				for(count = vcpu_index - 1; count >= 0; count--) {
					total_wcet_time += vcpu_to_schedule[count].budget_high;
				}

				if(total_wcet_time >= vcpu_to_schedule[vcpu_index].deadline) {
					//No time to execute the current vcpu
					//printf("Job %d cannot be assigned Low Priority\n", vcpu_index);	
				}
				else {
					if(((vcpu_to_schedule[vcpu_index].deadline - total_wcet_time) - vcpu_to_schedule[vcpu_index].release) 
							>= vcpu_to_schedule[vcpu_index].budget_high) {
						//printf("Job %d is assigned Low Priority\n", vcpu_index);	
						updateQueue(vcpu_to_schedule[vcpu_index]);
						break;
					}
					else {
						//printf("Job %d cannot be assigned Low Priority\n", vcpu_index);	
					}
				}
				//printf("Total WCET time = %ld\n", total_wcet_time);
				total_wcet_time = 0;

			}
			else {
				//printf("Entered LOW Criticality\n");

				/* Check if the current vcpu can be assigned with LOW Priority*/
				for(count = vcpu_index + 1; count < (NO_OF_VCPUS-loop_count) ; count++) {
					total_wcet_time += vcpu_to_schedule[count].budget_low;
				}
				for(count = vcpu_index - 1; count >= 0; count--) {
					total_wcet_time += vcpu_to_schedule[count].budget_low;
				}

				if(total_wcet_time >= vcpu_to_schedule[vcpu_index].deadline) {
					//No time to execute the current vcpu
					//printf("Job %d cannot be assigned Low Priority\n", vcpu_index);	
				}
				else {
					if(((vcpu_to_schedule[vcpu_index].deadline - total_wcet_time)- vcpu_to_schedule[vcpu_index].release) 
							>= vcpu_to_schedule[vcpu_index].budget_low) {
						//printf("Job %d is assigned Low Priority\n", vcpu_index);	
						updateQueue(vcpu_to_schedule[vcpu_index]);
						break;
					}
					else {
						//printf("Job %d cannot be assigned Low Priority\n", vcpu_index);	
					}
				}
				//printf("Total WCET time = %ld\n", total_wcet_time);
			}	
			total_wcet_time = 0;
		}
		/* Shifting the array index */
		if(vcpu_index == (NO_OF_VCPUS -1 -loop_count)) {
			/* No need to shift the arrays because of the last element*/
		}
		else {
			for(count = vcpu_index; count < (NO_OF_VCPUS-1-loop_count); count++) {
				vcpu_to_schedule[count] = vcpu_to_schedule[count+1];
			}
		}
		total_wcet_time = 0;
		loop_count++;
		printf("loop count %d\n", loop_count);
	}
	return 0;
}
s_time_t calculateLCM(struct rtmixcrit_vcpu * vcpu_to_schedule, int no_of_vcpu)
{
	s_time_t hyperperiod = 40;
	
	

	return hyperperiod;
}

void findInstance(struct rtmixcrit_vcpu * vcpu_to_schedule, int no_of_vcpu)
{
	s_time_t hyperperiod = 0;
	s_time_t vcpu_instance = 0;
	int count = 0;
	int i;
	int lclcnt;
	int eachvcpu = no_of_vcpu;
	//struct rtmixcrit_vcpu * final_vcpu_to_schedule;

	/* Calculate HyperPeriod (LCM) */
	hyperperiod = calculateLCM(vcpu_to_schedule, no_of_vcpu);

	/*
	printf("DEBUG\n");
	vcpu_instance = hyperperiod / vcpu_to_schedule[count].period;
	printf("HYPERPERIOD %ld, VCPU_INSTANCE %ld\n", hyperperiod, vcpu_instance);
	printf("%d\n", eachvcpu);
	*/

	for(count = 0; count < no_of_vcpu; count++){
		vcpu_instance = hyperperiod / vcpu_to_schedule[count].period;
		vcpu_instance -=1;
		
		//printf("%ld", vcpu_instance);
		for(lclcnt = 0; lclcnt < vcpu_instance; lclcnt++) {

			vcpu_to_schedule[eachvcpu+lclcnt].release   		= vcpu_to_schedule[count].release ;
			vcpu_to_schedule[eachvcpu+lclcnt].deadline  		= (vcpu_to_schedule[count].deadline +
										  ((lclcnt + 1)*vcpu_to_schedule[count].period) +
										    vcpu_to_schedule[count].release );
			vcpu_to_schedule[eachvcpu+lclcnt].criticality_vcpu    	= vcpu_to_schedule[count].criticality_vcpu    ;
			vcpu_to_schedule[eachvcpu+lclcnt].budget_low  		= vcpu_to_schedule[count].budget_low  	;
			vcpu_to_schedule[eachvcpu+lclcnt].budget_high 		= vcpu_to_schedule[count].budget_high 	;
		}
		eachvcpu += vcpu_instance;
	}

	/*
	eachvcpu = 0;

	for(count = 0; count < no_of_vcpu; count++){
		vcpu_instance = hyperperiod / vcpu_to_schedule[count].period;
		
		//printf("%ld", vcpu_instance);
		for(lclcnt = 0; lclcnt < vcpu_instance; lclcnt++) {

			final_vcpu_to_schedule = (struct rtmixcrit_vcpu *)malloc(sizeof(struct rtmixcrit_vcpu));
			final_vcpu_to_schedule[eachvcpu+lclcnt].release   		= vcpu_to_schedule[count].release ;
			final_vcpu_to_schedule[eachvcpu+lclcnt].deadline  		= (vcpu_to_schedule[count].deadline +
										  ((lclcnt)*vcpu_to_schedule[count].period) +
										    vcpu_to_schedule[count].release );
			final_vcpu_to_schedule[eachvcpu+lclcnt].criticality_vcpu    	= vcpu_to_schedule[count].criticality_vcpu    ;
			final_vcpu_to_schedule[eachvcpu+lclcnt].budget_low  		= vcpu_to_schedule[count].budget_low  	;
			final_vcpu_to_schedule[eachvcpu+lclcnt].budget_high 		= vcpu_to_schedule[count].budget_high 	;
			//printf("final %ld\n", final_vcpu_to_schedule[eachvcpu+lclcnt].deadline);
		}
		eachvcpu += vcpu_instance;
	}
	for(i = 0; i< 12; i++){
		//printf("final %ld\n", final_vcpu_to_schedule[i].deadline);
	}	
	vcpu_to_schedule = &final_vcpu_to_schedule[0];
	*/
	printf("%d\n", eachvcpu);
}

int main()
{
	int loop_count = 0;
	int no_of_vcpu = 3;
	struct rtmixcrit_vcpu vcpu_to_schedule[NO_OF_VCPUS]; 	

	struct rtmixcrit_vcpu vcpu_prioritized[NO_OF_VCPUS]; 	

	getJobData(vcpu_to_schedule);	

	//findInstance(vcpu_to_schedule, no_of_vcpu);

	calculatePriority(vcpu_to_schedule);//Computes the priority and updates the Queue as per the priority
	while(first_vcpu != NULL)
	{
		//memcpy(&vcpu_prioritized[loop_count], first_vcpu, sizeof(struct vcpu_stack));
		vcpu_prioritized[loop_count] = first_vcpu->vcpu_data;
		printf("Deadline Time = %ld Deadline Time from Prioritized Job %ld\n",first_vcpu->vcpu_data.deadline, vcpu_prioritized[loop_count].deadline);
		first_vcpu = first_vcpu->next_vcpu;
		loop_count++;
	}
	return 0;
}
