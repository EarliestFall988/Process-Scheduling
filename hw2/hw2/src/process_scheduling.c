#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/dyn_array.h"
#include "../include/processing_scheduling.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block)
{
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
    process_control_block->started = true;
}

int cmpfuncArrival(const void *a, const void *b)
{
    return (((ProcessControlBlock_t *)a)->arrival - ((ProcessControlBlock_t *)b)->arrival);
}

// first come first serve
bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL) // Check for invalud param
    {
        return false;
    }                                                 // Check for invalid param
    if (!dyn_array_sort(ready_queue, cmpfuncArrival)) // Sort array by arrival time
    {
        return false;
    };

    float waittime = 0;
    unsigned long runtime = 0;
    int n = dyn_array_size(ready_queue);
    for (int i = 0; i < n; i++) // loop through the ready queue
    {

        int x = n - i;                                                                            // set the wait time multiplier
        runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time; // add the burst time to the runtime
        if (i > 0)
        {
            waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x; // add the wait time to the waittime
        }
    }

    for (int i = 0; i < n; i++)
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

        while (current->remaining_burst_time > 0) // run the process until the burst time is 0
        {
            virtual_cpu(current);
        }
    }

    dyn_array_destroy(ready_queue); // cleanup

    result->average_waiting_time = waittime / n; // set the result
    result->average_turnaround_time = runtime / n;
    result->total_run_time = runtime;
    return true; // return true
}

int cmpfuncShortest(const void *a, const void *b) // compare function for shortest job first
{
    if (((ProcessControlBlock_t *)a)->remaining_burst_time < ((ProcessControlBlock_t *)b)->remaining_burst_time)
    {
        return -1;
    }
    else if (((ProcessControlBlock_t *)a)->remaining_burst_time == ((ProcessControlBlock_t *)b)->remaining_burst_time)
    {
        return 0;
    }

    return 1;
}

// shortest job first
bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL) // Check for invalud param
    {
        return false;
    } // Check for invalid param
    if (!dyn_array_sort(ready_queue, cmpfuncShortest))
    {
        return false;
    }; // Sort array by arrival time

    // float arrivalTime = 0;

    // unsigned long waittime = 0;                  // set the wait time
    unsigned long runtime = 0;           // set the runtime
    int n = dyn_array_size(ready_queue); // set the size of the ready queue

    unsigned long turnAroundTime = 0;
    unsigned long waiting = 0;

    float totalTurnAroundTime = 0;
    float totalWaitingTime = 0;

    // printf("turn around %f waiting %f\n", turnAroundTime, waiting);

    // for (int i = 0; i < n; i++)          // loop through the ready queue
    // {

    //     arrivalTime += i;

    //     int x = n - i;                                                                            // set the wait time multiplier
    //     runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time; // add the burst time to the runtime
    //     if (i > 0)
    //     {
    //         waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x; // add the wait time to the waittime
    //     }
    // }

    for (int i = 0; i < n; i++) // run the process until the burst time is 0
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i); // set the current process

        float totalBurst = current->remaining_burst_time;

        while (current->remaining_burst_time > 0) // run the process until the burst time is 0
        {
            virtual_cpu(current); // run the process through the virtual CPU
            runtime++;

            if (current->remaining_burst_time <= 0)
            {
                turnAroundTime = runtime - current->arrival;
                waiting = turnAroundTime - totalBurst;

                totalTurnAroundTime = turnAroundTime;
                totalWaitingTime = waiting;

                printf("runtime %ld arrival %d\n", runtime, current->arrival);

                printf("turn around %ld waiting %ld\n", turnAroundTime, waiting);
            }
        }

        // printf("waitTime %ld\n", waittime);
    }

    dyn_array_destroy(ready_queue); // cleanup

    // float sumExitTime = waittime + runtime;

    // float turnAroundTime = sumExitTime - arrivalTime;

    result->average_waiting_time = totalWaitingTime / n;
    result->average_turnaround_time = totalTurnAroundTime / n;
    result->total_run_time = runtime;
    return true; // return true
}

// priority not implemented as directed in the assignment
bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

// round robin
bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    if (ready_queue == NULL || result == NULL || quantum <= 0)
    {
        return false;
    } // Check for invalid param

    float runtime = 0; // set the runtime

    float n = dyn_array_size(ready_queue); // set the total number of processes

    float total = dyn_array_size(ready_queue); // set the total number of processes

    float completionTime = 0; // set the completion time
    float arrivalTime = 0;    // set the arrival time
    float waittime = 0;       // set the wait time
    float turnaroundTime = 0; // set the turnaround time
    float totalBurstTime = 0; // set the total burst time

    for (int i = 0; i < n; i++)
    {
        totalBurstTime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time; // add the burst time to the total burst time
    }

    while (total > 0) // <--| run the process until the ready queue is empty
    {
        for (size_t i = 0; i < dyn_array_size(ready_queue); i++) // <--| loop through the ready queue
        {
            ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i); // set the current process

            int q = quantum;                                            // set the quantum
            bool completedProcess = current->remaining_burst_time <= 0; // flag to check if the process is completed
            while (q > 0 && !completedProcess)                          // run the process until the quantum is 0 or the process is completed
            {

                virtual_cpu(current); // run the process

                runtime++;
                q--; // decrement the quantum

                if (current->remaining_burst_time <= 0) // check if the process is completed
                {

                    completedProcess = true;         // set the flag to true (no need to run the virtual CPU again because the process is completed)
                    completionTime += runtime;       // add the runtime to the completion time
                    arrivalTime += current->arrival; // add the arrival time to the arrival time

                    total--; // decrement the total number of processes
                }
            }
        }
    }

    dyn_array_destroy(ready_queue); // cleanup

    waittime = completionTime - arrivalTime - totalBurstTime; // calculate the total wait time

    turnaroundTime = completionTime - arrivalTime; // calculate the total turnaround time

    result->average_waiting_time = waittime / n;          // calculate the average wait time
    result->average_turnaround_time = turnaroundTime / n; // calculate the average turnaround time
    result->total_run_time = runtime;                     // set the total run time

    return true; // return true
}

// load process controll blocks from a given file and return a dynamic array of process control blocks
dyn_array_t *load_process_control_blocks(const char *input_file)
{

    if (input_file == NULL) // Check for invalid param
    {
        return NULL;
    }
    FILE *fp;
    fp = fopen(input_file, "r"); // open the file
    if (fp == NULL)
    {
        return NULL; // return null if the file is not found
    }
    uint32_t num_PCB[1];
    if (fread(num_PCB, sizeof(uint32_t), 1, fp) != 1) // read the number of PCBs
    {
        return NULL; // return null if the file is not found
    }
    uint32_t buffer[num_PCB[0] * 3];                // create a buffer to hold the PCBs
    if (fseek(fp, sizeof(uint32_t), SEEK_SET) != 0) // seek to the beginning of the PCBs
    {
        return NULL;
    }
    if (fread(buffer, sizeof(uint32_t), (size_t)(num_PCB[0] * 3), fp) != (num_PCB[0] * 3)) // read the PCBs
    {
        return false;
    }
    dyn_array_t *PCB_array = dyn_array_create((size_t)num_PCB[0], sizeof(uint32_t), NULL); // create a dynamic array to hold the PCBs
    if (PCB_array == NULL)                                                                 // check if the dynamic array was created
    {
        return NULL;
    }

    // Loop through the file creating dynamic array of specified PCBs
    for (uint32_t i = 0; i < num_PCB[0] * 3; i += 3)
    {
        ProcessControlBlock_t *PCB = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t)); // create a PCB
        PCB->remaining_burst_time = buffer[i];                                                       // set the PCB's remaining burst time
        PCB->priority = buffer[i + 1];                                                               // set the PCB's priority
        PCB->arrival = buffer[i + 2];                                                                // set the PCB's arrival time
        PCB->started = false;                                                                        // set the PCB's started flag to false
        dyn_array_push_back(PCB_array, PCB);                                                         // add the PCB to the dynamic array
    }
    fclose(fp);       // close the file
    return PCB_array; // return the dynamic array
}

int cmpfuncRemainingTime(const void *a, const void *b)
{
    return (((ProcessControlBlock_t *)a)->remaining_burst_time - ((ProcessControlBlock_t *)b)->remaining_burst_time); // compare the remaining burst time
}

// Shortest Remaining Time First
bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL)
    {
        return false;
    } // Check for invalid param
    if (!dyn_array_sort(ready_queue, cmpfuncArrival))
    {
        return false;
    }; // Sort array by arrival time

    float total_run_time = 0;               // set the total run time to 0
    float wait_time = 0;                    // set the wait time to 0
    size_t n = dyn_array_size(ready_queue); // set the number of processes to the size of the ready queue
    size_t current_ready_queue_index = 0;   // set the current ready queue index to 0
    dyn_array_t *run_que = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    // Calculate the total run time
    for (size_t i = 0; i < dyn_array_size(ready_queue); i++) // loop through the ready queue
    {
        total_run_time += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time; // add the remaining burst time to the total run time
    }
    // loop through every unit time
    for (size_t i = 0; i < (size_t)total_run_time; i++) // loop through the total run time
    {
        // Add every element to the run que that has an arrival time less or equal to current time
        if (dyn_array_size(ready_queue) > current_ready_queue_index)
        {
            if (((ProcessControlBlock_t *)dyn_array_at(ready_queue, current_ready_queue_index))->arrival <= (uint32_t)i) // check if the arrival time is less than or equal to the current time
            {
                dyn_array_push_back(run_que, dyn_array_at(ready_queue, current_ready_queue_index)); // add the PCB to the run queue
                current_ready_queue_index++;                                                        // increment the current ready queue index
            }
        }
        // Process any PCBs in the run queue
        if (dyn_array_size(run_que) != 0)
        {
            if (!dyn_array_sort(run_que, cmpfuncRemainingTime))
            {
                return false; // sort the run queue by remaining burst time
            }
            wait_time += dyn_array_size(run_que) - 1;                                           // add the number of PCBs in the run queue to the wait time
            virtual_cpu((ProcessControlBlock_t *)dyn_array_at(run_que, 0));                     // run the PCB at the front of the run queue
            if (((ProcessControlBlock_t *)dyn_array_at(run_que, 0))->remaining_burst_time == 0) // check if the PCB is done
            {
                dyn_array_erase(run_que, 0); // remove the PCB from the run queue
            }
        }
    }

    // printf("wait time: %f", wait_time / (float)n);

    result->average_waiting_time = wait_time / n;           // calculate the average waiting time
    result->average_turnaround_time = (total_run_time + wait_time) / n;   // calculate the average turnaround time
    result->total_run_time = (unsigned long)total_run_time; // set the total run time

    dyn_array_destroy(ready_queue); // cleanup
    dyn_array_destroy(run_que);

    return true; // return true
}
