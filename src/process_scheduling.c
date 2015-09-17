#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <fcntl.h>
#include <stdbool.h>

#include "../include/process_scheduling.h"
#include <dyn_array.h>


const ScheduleStats_t first_come_first_served(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats;
	return stats;
}


const ScheduleStats_t shortest_job_first(dyn_array_t* futureProcesses) {
	ScheduleStats_t stats;
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
	return false; 
}


const bool fetch_new_processes(dyn_array_t* newProcesses, dyn_array_t* futureProcesses, const size_t currentClockTime) {
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
	return false;

}

const bool rearranged_process_control_blocks_by_arrival_time(dyn_array_t* futureProcesses) {
	return true;
}  
