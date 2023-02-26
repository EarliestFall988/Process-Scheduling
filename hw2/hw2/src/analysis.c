#include <stdio.h>
#include <stdlib.h>


#include "../include/dyn_array.h"
#include "../include/processing_scheduling.h"


#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
    if (argc < 3) 
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        return EXIT_FAILURE;
    }
    ScheduleResult_t * results = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    dyn_array_t * PCB_Array = load_process_control_blocks("argv[0]");

    if(strcmp(argv[1],FCFS))
    {
        first_come_first_serve(PCB_Array,results);
        printf("Average Waiting Time: %f \n", results->average_waiting_time);
        printf("Average Turnaround Time: %f \n", results->average_turnaround_time);
        printf("Total Run Time: %lu \n", results->total_run_time);
    }
    if(strcmp(argv[1],P))
    {
        priority(PCB_Array,results);
        printf("Average Waiting Time: %f \n", results->average_waiting_time);
        printf("Average Turnaround Time: %f \n", results->average_turnaround_time);
        printf("Total Run Time: %lu \n", results->total_run_time);
    }
    if(strcmp(argv[1],RR))
    {
        round_robin(PCB_Array,results,(size_t)atoi(argv[2]));
        printf("Average Waiting Time: %f \n", results->average_waiting_time);
        printf("Average Turnaround Time: %f \n", results->average_turnaround_time);
        printf("Total Run Time: %lu \n", results->total_run_time);
    }
    if(strcmp(argv[1],SJF))
    {
        shortest_job_first(PCB_Array,results);
        printf("Average Waiting Time: %f \n", results->average_waiting_time);
        printf("Average Turnaround Time: %f \n", results->average_turnaround_time);
        printf("Total Run Time: %lu \n", results->total_run_time);
    }
    free(results);
    free(PCB_Array);


    return EXIT_SUCCESS;
}
