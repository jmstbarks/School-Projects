#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>
#include <dyn_array.h>
#include <time.h>
#include "../include/process_scheduling.h"


 
const ScheduleStats_t first_come_first_served(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats = {0};
    size_t clock_time = 0, numProcs; 
    uint32_t latency = 0, wall_clock = 0;
    ProcessControlBlock_t* pcb = malloc(sizeof(ProcessControlBlock_t));
	dyn_array_t* newProcesses = dyn_array_create(16,sizeof(ProcessControlBlock_t),NULL);
	numProcs = dyn_array_size(futureProcesses);

	if(futureProcesses == NULL || dyn_array_empty(futureProcesses) == true){
		return stats;
	}

	rearranged_process_control_blocks_by_arrival_time(futureProcesses);

	while(dyn_array_empty(futureProcesses) == false) {
		if(fetch_new_processes(newProcesses, futureProcesses, clock_time) == true){
			while(dyn_array_empty(newProcesses) == false){
				dyn_array_extract_front(newProcesses, pcb);
				pcb->activated = 1;
				latency += clock_time;
					while(pcb->burstTime != 0){
						virtual_cpu(pcb);
						clock_time++;
					}
				wall_clock = latency+clock_time;	
			}
		
		}
		
	}

	

	stats.averageWallClockTime = (wall_clock/numProcs);
	stats.totalClockRuntime = clock_time;
	stats.averageLatencyTime = latency/numProcs;
	return stats;
}

int compare_burstTime(const void* a, const void *b) {
	const size_t aValue = ((ProcessControlBlock_t*)a)->burstTime;
	const size_t bValue = ((ProcessControlBlock_t*)b)->burstTime;
	return aValue - bValue;
}  

const ScheduleStats_t shortest_job_first(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats = {0};
    size_t clock_time = 0, numProcs; 
    uint32_t latency = 0, wall_clock = 0;
    ProcessControlBlock_t* pcb = malloc(sizeof(ProcessControlBlock_t));
	dyn_array_t* newProcesses = dyn_array_create(16,sizeof(ProcessControlBlock_t),NULL);
	numProcs = dyn_array_size(futureProcesses);

	if(futureProcesses == NULL || dyn_array_empty(futureProcesses) == true){
		return stats;
	}

	rearranged_process_control_blocks_by_arrival_time(futureProcesses);

	while(dyn_array_empty(futureProcesses) == false) {

		if(fetch_new_processes(newProcesses, futureProcesses, clock_time) == true){

			dyn_array_sort(newProcesses,&compare_burstTime);

			while(dyn_array_empty(newProcesses) == false){
				dyn_array_extract_front(newProcesses, pcb);
				pcb->activated = 1;
				latency += clock_time;
				while(pcb->burstTime != 0){
					virtual_cpu(pcb);
					clock_time++;
					}
				wall_clock = latency+clock_time;	
			}
		
		}
		
	}


	stats.averageWallClockTime = (wall_clock/numProcs);
	stats.totalClockRuntime = clock_time;
	stats.averageLatencyTime = latency/numProcs;
	return stats;
}

const ScheduleStats_t shortest_remaining_time_first(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats = {0};
    size_t clock_time = 0, numProcs; 
    uint32_t latency = 0, wall_clock = 0, totalArrivalTime = 0;
    ProcessControlBlock_t* pcb = malloc(sizeof(ProcessControlBlock_t));
	dyn_array_t* newProcesses = dyn_array_create(16,sizeof(ProcessControlBlock_t),NULL);
	numProcs = dyn_array_size(futureProcesses);

	if(futureProcesses == NULL || dyn_array_empty(futureProcesses) == true){
		return stats;
	}

	rearranged_process_control_blocks_by_arrival_time(futureProcesses);

	while(dyn_array_empty(futureProcesses) == false || (dyn_array_empty(newProcesses) == false)){

		if(dyn_array_empty(futureProcesses) == false){
			fetch_new_processes(newProcesses, futureProcesses, clock_time);
		}
	
		dyn_array_sort(newProcesses,&compare_burstTime);

		if(dyn_array_empty(newProcesses) == false){
			
			pcb = dyn_array_front(newProcesses);
			
			if(pcb->activated == 0){
				latency += clock_time;
				totalArrivalTime += pcb->arrivalTime;
			}
				pcb->activated = 1;{
					latency
				}
			
			virtual_cpu(pcb);
			clock_time++;	
			
			
			if(pcb->burstTime == 0){
				dyn_array_extract_front(newProcesses, pcb);
				wall_clock += clock_time;
			}
		}
	
	}
		
	
	stats.averageWallClockTime = ((wall_clock-totalArrivalTime)/numProcs);
	stats.totalClockRuntime = clock_time;
	stats.averageLatencyTime = ((float)latency-totalArrivalTime)/(float)numProcs;
	return stats;
}


const ScheduleStats_t round_robin(dyn_array_t* futureProcesses, const size_t quantum) {
	ScheduleStats_t stats = {0};
    size_t clock_time = 0, numProcs; 
    uint32_t latency = 0, wall_clock = 0, totalBurstTime = 0;
    ProcessControlBlock_t* pcb = malloc(sizeof(ProcessControlBlock_t));
	dyn_array_t* newProcesses = dyn_array_create(16,sizeof(ProcessControlBlock_t),NULL);
	numProcs = dyn_array_size(futureProcesses);

	if(futureProcesses == NULL || dyn_array_empty(futureProcesses) == true){
		return stats;
	}

	rearranged_process_control_blocks_by_arrival_time(futureProcesses);

	while(dyn_array_empty(futureProcesses) == false || (dyn_array_empty(newProcesses) == false)){

		if(dyn_array_empty(futureProcesses) == false){
			fetch_new_processes(newProcesses, futureProcesses, clock_time);
		}
		
		if(dyn_array_empty(newProcesses) == false){

			pcb = dyn_array_front(newProcesses);

			if(pcb->activated == 0){
			
				totalBurstTime += pcb->burstTime;
			}
				latency += clock_time;
				pcb->activated = 1;

				for(int i = 0; i<quantum; ++i){
					clock_time++;
					if(pcb->burstTime == 0){
						dyn_array_extract_front(newProcesses, pcb);
						wall_clock += clock_time;
						break;
					}

					virtual_cpu(pcb);

					
				}
					
				if(pcb->burstTime != 0){
					dyn_array_extract_front(newProcesses, pcb);
					dyn_array_push_back(newProcesses, pcb);
				} 
		}
	}
		
	stats.averageWallClockTime = (latency+totalBurstTime)/numProcs;
	stats.totalClockRuntime = clock_time;
	stats.averageLatencyTime = ((float)latency-totalBurstTime)/(float)numProcs;
	return stats;
} 

int compare_pid(const void* a, const void *b) {
	const size_t aValue = ((ProcessControlBlock_t*)a)->pid;
	const size_t bValue = ((ProcessControlBlock_t*)b)->pid;
	return aValue - bValue;
}

int compare_arrivalTime(const void* a, const void *b) {
	const size_t aValue = ((ProcessControlBlock_t*)a)->arrivalTime;
	const size_t bValue = ((ProcessControlBlock_t*)b)->arrivalTime;
	return bValue - aValue;
}  



const bool create_suspended_processes_and_assign_pcbs(dyn_array_t* futureProcesses){

	pid_t pid; 

	if(futureProcesses == NULL){
		return false;
	}

	for (size_t i = 0; i < dyn_array_size(futureProcesses); ++i) {
		pid = fork();
		ProcessControlBlock_t* pcb = dyn_array_at(futureProcesses,i);
		
		if(pid > 0){
			pcb->pid = pid;
			kill(pcb->pid,SIGSTOP);
		}
		kill(pcb->pid,SIGSTOP);
	}


	dyn_array_sort(futureProcesses, &compare_pid);

	return true; 
}


const bool fetch_new_processes(dyn_array_t* newProcesses, dyn_array_t* futureProcesses, const size_t currentClockTime) {

	if(newProcesses == NULL || futureProcesses == NULL){
		return false; 
	}
	

	for (int i = 0; i < dyn_array_size(futureProcesses); ++i) {

		ProcessControlBlock_t* pcb = dyn_array_at(futureProcesses,i);
	
		
		if(pcb->arrivalTime <= currentClockTime){
			dyn_array_push_back(newProcesses, pcb); 
			dyn_array_erase(futureProcesses, i);
			--i;
			}
	}

	if (dyn_array_size(newProcesses) == 0){
		return false;
	}
	
	return true;
}

void virtual_cpu(ProcessControlBlock_t* runningProcess) {
	
	if (runningProcess == NULL || !runningProcess->pid) {
		return;	
	}
	ssize_t err = 0;
	err = kill(runningProcess->pid,SIGCONT);
	if (err < 0) {
		fprintf(stderr,"VCPU FAILED to CONT process %d\n",runningProcess->pid);
	}
	--runningProcess->burstTime;
	err = kill(runningProcess->pid,SIGSTOP);
	if (err < 0) {
		fprintf(stderr,"VCPU FAILED to STOP process %d\n",runningProcess->pid);
	}
	
}

const bool load_process_control_blocks_from_file(dyn_array_t* futureProcesses, const char* binaryFileName)
{

	if(futureProcesses == NULL){
		return false;
	}

	uint32_t numProcs[1];

	int pcbFile = open(binaryFileName, O_RDONLY);
	
	if(pcbFile == -1)
   {
		return false;
   }

											
	read(pcbFile, numProcs, sizeof(uint32_t));//Get size of file
	


	ProcessControlBlock_t pcbData;


	for(uint32_t i=0; i<*numProcs; i++){//fill array with file contents

			read(pcbFile, &pcbData, sizeof(ProcessControlBlock_t));
			dyn_array_push_back(futureProcesses, &pcbData);
			
	}
	
	
	
	return true;
}

const bool rearranged_process_control_blocks_by_arrival_time(dyn_array_t* futureProcesses) {

	if(futureProcesses == NULL){
		return false; 
	}
	
	dyn_array_sort(futureProcesses,&compare_arrivalTime);


	return true;
} 

