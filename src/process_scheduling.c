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
	ScheduleStats_t stats;
	int i;
	double wallClock;

	struct timespec now, tmstart;
    clock_gettime(CLOCK_REALTIME, &tmstart);
    clock_t start, end;


start = clock();


	for (i = 0; i < dyn_array_data_size(futureProcesses); ++i) {

		ProcessControlBlock_t* pcb = dyn_array_at(futureProcesses,i);
		virtual_cpu(pcb);

		}

		end = clock();
	wallClock = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
	stats.averageWallClockTime = wallClock;
	stats.totalClockRuntime = ((double) (end - start)) / CLOCKS_PER_SEC;

	int latencyTime = stats.totalClockRuntime - wallClock;
	stats.averageLatencyTime = latencyTime;
			return stats;
}


const ScheduleStats_t shortest_job_first(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats;
	int shortestJob = 100, i;
	ProcessControlBlock_t* shortestProc = malloc(sizeof(ProcessControlBlock_t ));
	double wallClock;

	struct timespec now, tmstart;
    clock_gettime(CLOCK_REALTIME, &tmstart);
    clock_t start, end;


start = clock();

	for (i = 0; i < dyn_array_data_size(futureProcesses); ++i) {

		ProcessControlBlock_t* pcb = dyn_array_at(futureProcesses,i);

			if(shortestJob < pcb->burstTime){
				shortestJob = pcb->burstTime;
				shortestProc = pcb;
			}

		
		virtual_cpu(shortestProc);
		

		}

	end = clock();
	wallClock = (double)((now.tv_sec+now.tv_nsec*1e-9) - (double)(tmstart.tv_sec+tmstart.tv_nsec*1e-9));
	stats.averageWallClockTime = wallClock;
	stats.totalClockRuntime = ((double) (end - start)) / CLOCKS_PER_SEC;

	int latencyTime = stats.totalClockRuntime - wallClock;
	stats.averageLatencyTime = latencyTime;

	

		return stats;
}

const ScheduleStats_t shortest_remaining_time_first(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats;
	return stats;
}


const ScheduleStats_t round_robin(dyn_array_t* futureProcesses, const size_t quantum) {
	ScheduleStats_t stats;
	return stats;
} 


const bool create_suspended_processes_and_assign_pcbs(dyn_array_t* futureProcesses){

	int pid; 


	for (size_t i = 0; i < dyn_array_data_size(futureProcesses); ++i) {
		pid = fork();
		ProcessControlBlock_t* pcb = dyn_array_at(futureProcesses,i);
		pcb->pid = pid;
		kill(pcb->pid,SIGSTOP);
	}	

	return false; 
}


const bool fetch_new_processes(dyn_array_t* newProcesses, dyn_array_t* futureProcesses, const size_t currentClockTime) {

	int i;


	for (i = 0; i < dyn_array_data_size(futureProcesses); ++i) {
		ProcessControlBlock_t* pcb = dyn_array_at(futureProcesses,i);
		
			if(pcb->arrivalTime <= currentClockTime){
				dyn_array_push_back(newProcesses, pcb);
				dyn_array_extract_back(futureProcesses, pcb);
			}

			
	}

	


	return false;
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

	FILE *pcbFile;
	int i, numProcs[1], pcbData[100];
	ProcessControlBlock_t *newPCB = malloc(sizeof(ProcessControlBlock_t ));


	pcbFile = fopen(binaryFileName,"rb");

	fread(numProcs, sizeof(size_t), 1, pcbFile);
	
		for(i=0; i<numProcs[0]; i++){

			fread(&pcbData, sizeof(int), 4, pcbFile);
			
			newPCB->burstTime = pcbData[0];
			newPCB->arrivalTime = pcbData[1];
			newPCB->pid = pcbData[2];
			newPCB->activated = pcbData[3];

			dyn_array_push_back(futureProcesses,&newPCB);
			
			
			memset(pcbData, 0, 4*sizeof(*pcbData));

			  	}

	if(futureProcesses != NULL){
		return true;

	}
	
		return false;
}

int compare(const void* a, const void *b) {
	const size_t aValue = ((ProcessControlBlock_t*)a)->pid;
	const size_t bValue = ((ProcessControlBlock_t*)b)->pid;
	return bValue - aValue;
} 

const bool rearranged_process_control_blocks_by_arrival_time(dyn_array_t* futureProcesses) {

	ProcessControlBlock_t pcb;	
	dyn_array_sort(futureProcesses,&compare);


	dyn_array_insert_sorted(futureProcesses,&pcb,&compare);



	return true;
} 


int main(int argc, char*argv[]) {

int currentClockTime = 0;
ScheduleStats_t *statsFCFS = malloc(sizeof(ScheduleStats_t));
//ScheduleStats_t *statsSJF = malloc(sizeof(ScheduleStats_t));

dyn_array_t* futureProcesses = dyn_array_create(0,sizeof(ProcessControlBlock_t),NULL);
dyn_array_t* newProcesses = dyn_array_create(16,sizeof(ProcessControlBlock_t),NULL); 

if(!load_process_control_blocks_from_file(futureProcesses, argv[1])){
	return 0;
}

if(create_suspended_processes_and_assign_pcbs(futureProcesses)){
	return 0;
}

if(!rearranged_process_control_blocks_by_arrival_time(futureProcesses)){
	return 0;
}

while(dyn_array_empty(futureProcesses) == false){
	++currentClockTime;
	 fetch_new_processes(newProcesses,futureProcesses, currentClockTime);
	 *statsFCFS = first_come_first_served(newProcesses);
	 //*statSJF = shortest_job_first(newProcesses);



}


return 0;
}
