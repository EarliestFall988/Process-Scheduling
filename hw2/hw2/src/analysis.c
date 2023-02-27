#include <stdio.h>
#include <stdlib.h>

#include "../include/dyn_array.h"
#include "../include/processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRT "SRT"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        return EXIT_FAILURE;
    }

    
    ScheduleResult_t results = {0, 0, 0};
    
    dyn_array_t *PCB_Array = load_process_control_blocks(argv[1]);

    //Run First Come First Serve Algorithm    
    if (strcmp(argv[2], FCFS) == 0)  //checks algorithm requested
    {
        first_come_first_serve(PCB_Array, &results); //enters FCFS algorithm

        //prints results from algorithms
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }

    //Run Priority First Algorithm
    if (strcmp(argv[2], P) == 0) //checks algorithm requested
    {

        EXIT_FAILURE;

        priority(PCB_Array, &results); //enters P algorithm

        //prints results from algorithms
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }

    //Run Round Robin Algorithm
    if (strcmp(argv[2], RR) == 0) //checks algorithm requested
    {
        size_t quantum = (size_t)atoi(argv[3]);
        round_robin(PCB_Array, &results, quantum); //enters RR algorithm

        //prints results from algorithms
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }

    //Run Shortest Job First Algorithm
    if (strcmp(argv[2], SJF) == 0) //checks algorithm requested
    {
        shortest_job_first(PCB_Array, &results); //enters SJF algorithm

        //prints results from algorithms
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }

    //Run Shortest Remaining Time First Algorithm
    if (strcmp(argv[2], SRT) == 0) //checks algorithm requested
    {
        shortest_remaining_time_first(PCB_Array, &results); //enters SRT algorithm

        //prints results from algorithms
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }

    return EXIT_SUCCESS;
}
