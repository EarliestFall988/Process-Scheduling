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

    // ScheduleResult_t * results = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    ScheduleResult_t results = {0, 0, 0};

    // printf("Loading PCBs from %s \n", argv[1]);

    dyn_array_t *PCB_Array = load_process_control_blocks(argv[1]);

    // printf("Number of PCBs: %lu \n", dyn_array_size(PCB_Array));

    // printf("args 2: %s \n", argv[2]);

    if (strcmp(argv[2], FCFS) == 0)
    {
        first_come_first_serve(PCB_Array, &results);
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }
    if (strcmp(argv[2], P) == 0)
    {

        EXIT_FAILURE;

        priority(PCB_Array, &results);
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }
    if (strcmp(argv[2], RR) == 0)
    {
        // printf("args 2: %s \n", argv[2]);
        // printf("args 3: %s \n", argv[3]);

        size_t quantum = (size_t)atoi(argv[3]);
        // printf("quantum created: %lu \n", quantum);
        round_robin(PCB_Array, &results, quantum);

        // printf("args 2: %s \n", argv[2]);
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }
    if (strcmp(argv[2], SJF) == 0)
    {
        shortest_job_first(PCB_Array, &results);
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }
    if (strcmp(argv[2], SRT) == 0)
    {
        shortest_remaining_time_first(PCB_Array, &results);
        printf("Average Waiting Time: %f \n", results.average_waiting_time);
        printf("Average Turnaround Time: %f \n", results.average_turnaround_time);
        printf("Total Run Time: %lu \n", results.total_run_time);
    }

    // dyn_array_destroy(PCB_Array);

    return EXIT_SUCCESS;
}
