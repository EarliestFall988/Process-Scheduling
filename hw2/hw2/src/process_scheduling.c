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

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL)
    {
        return false;
    } // Check for invalid param
    if (!dyn_array_sort(ready_queue, cmpfuncArrival))
    {
        return false;
    }; // Sort array by arrival time

    float waittime = 0;
    unsigned long runtime = 0;
    int n = dyn_array_size(ready_queue);
    for (int i = 0; i < n; i++)
    {

        int x = n - i;
        runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
        if (i > 0)
        {
            waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x;
        }
    }

    for (int i = 0; i < n; i++)
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

        while (current->remaining_burst_time > 0)
        {
            virtual_cpu(current);
        }
    }

    dyn_array_destroy(ready_queue);

    result->average_waiting_time = waittime / n;
    result->average_turnaround_time = runtime / n;
    result->total_run_time = runtime;
    return true;
}

int cmpfuncShortest(const void *a, const void *b)
{
    if (((ProcessControlBlock_t *)a)->remaining_burst_time <= ((ProcessControlBlock_t *)b)->remaining_burst_time)
    {
        return 0;
    }
    return 1;
}

// shortest job first
bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL)
    {
        return false;
    } // Check for invalid param
    if (!dyn_array_sort(ready_queue, cmpfuncShortest))
    {
        return false;
    }; // Sort array by arrival time

    float waittime = 0;
    unsigned long runtime = 0;
    int n = dyn_array_size(ready_queue);
    for (int i = 0; i < n; i++)
    {

        int x = n - i;
        runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
        if (i > 0)
        {
            waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x;
        }
    }

    for (int i = 0; i < n; i++)
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

        while (current->remaining_burst_time > 0)
        {
            virtual_cpu(current);
        }
    }

    dyn_array_destroy(ready_queue);

    result->average_waiting_time = waittime / n;
    result->average_turnaround_time = runtime / n;
    result->total_run_time = runtime;
    return true;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

// bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
// {
//     if (ready_queue == NULL || result == NULL || quantum <= 0)
//     {
//         return false;
//     } // Check for invalid param
//     if (!dyn_array_sort(ready_queue, cmpfuncArrival))
//     {
//         return false;
//     }; // Sort array by arrival time

//     float total_run_time = 0;
//     float wait_time = 0;
//     size_t n = dyn_array_size(ready_queue);
//     size_t current_ready_queue_index = 0;
//     //Calculate the total run time
//     for(size_t i = 0;i < dyn_array_size(ready_queue);i++)
//     {
//         total_run_time += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
//     }
//     result->average_turnaround_time = total_run_time / n;
//     result->total_run_time = (unsigned long)total_run_time;
//     //loop through every unit time
//     while(total_run_time != 0)
//     {
//         //Loop back to beginning of ready queue if end has been reached
//         if(dyn_array_size(ready_queue) <= current_ready_queue_index)
//         {
//             current_ready_queue_index = 0;
//         }
//        for(size_t i = 0;i < quantum;i++)
//        {
//             //Remove the PCB if completed.
//             if(((ProcessControlBlock_t *)dyn_array_at(ready_queue, current_ready_queue_index))->remaining_burst_time <=0)
//             {
//                 dyn_array_erase(ready_queue, current_ready_queue_index);
//                 current_ready_queue_index--;//Compensate for the index incrementing that occurs wether PCB is completed or not
//                 break;
//             }
//             virtual_cpu((ProcessControlBlock_t *)dyn_array_at(ready_queue, current_ready_queue_index));//Process the PCB
//             total_run_time--;
//             wait_time += dyn_array_size(ready_queue) - 1;//EveryPCB in ready queue is waiting except the one that is running
//        }
//        current_ready_queue_index++;

//     }

//     result->average_waiting_time = (float)(wait_time / n);

//     // destroy dynamic arrays
//     dyn_array_destroy(ready_queue);

//     return true;
// }

// round robin, rewritten for the purpose of testing and comparing with the original round robin
bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    if (ready_queue == NULL || result == NULL || quantum <= 0)
    {
        return false;
    } // Check for invalid param

    float runtime = 0;

    float n = dyn_array_size(ready_queue);

    float total = dyn_array_size(ready_queue);

    float completionTime = 0;
    float arrivalTime = 0;
    float waittime = 0;
    float turnaroundTime = 0;
    float totalBurstTime = 0;

    for (int i = 0; i < n; i++)
    {
        totalBurstTime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
        // printf("totalburstTime: %f\n", totalBurstTime);
    }

    while (total > 0) // <--| run the process until the ready queue is empty
    {
        for (size_t i = 0; i < dyn_array_size(ready_queue); i++) // <--| loop through the ready queue
        {
            ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

            int q = quantum;                                            // set the quantum
            bool completedProcess = current->remaining_burst_time <= 0; // flag to check if the process is completed
            while (q > 0 && !completedProcess)                          // run the process until the quantum is 0 or the process is completed
            {
                // printf("%ld) completedTime: %d -> %f\n", i, current->remaining_burst_time, current->remaining_burst_time - (double)quantum);
                virtual_cpu(current); // run the process

                runtime++;
                q--; // decrement the quantum

                if (current->remaining_burst_time <= 0)
                {

                    completedProcess = true;
                    completionTime += runtime;
                    arrivalTime += current->arrival;

                    total--;
                }

                // waittime = waittime + dyn_array_size(ready_queue) - 1;
            }
        }
    }

    dyn_array_destroy(ready_queue);

    // printf("%f: %f: %f", completionTime, arrivalTime, totalBurstTime);

    waittime = completionTime - arrivalTime - totalBurstTime;

    turnaroundTime = completionTime - arrivalTime;

    result->average_waiting_time = waittime / n;
    result->average_turnaround_time = turnaroundTime / n;
    result->total_run_time = runtime;

    return true;
}

dyn_array_t *load_process_control_blocks(const char *input_file)
{

    if (input_file == NULL)
    {
        return NULL;
    }
    FILE *fp;
    fp = fopen(input_file, "r");
    if (fp == NULL)
    {
        return NULL;
    }
    uint32_t num_PCB[1];
    if (fread(num_PCB, sizeof(uint32_t), 1, fp) != 1)
    {
        return NULL;
    }
    uint32_t buffer[num_PCB[0] * 3];
    if (fseek(fp, sizeof(uint32_t), SEEK_SET) != 0)
    {
        return NULL;
    }
    if (fread(buffer, sizeof(uint32_t), (size_t)(num_PCB[0] * 3), fp) != (num_PCB[0] * 3))
    {
        return false;
    }
    dyn_array_t *PCB_array = dyn_array_create((size_t)num_PCB[0], sizeof(uint32_t), NULL);
    if (PCB_array == NULL)
    {
        return NULL;
    }
    printf("%d \n", (int)(num_PCB[0]));
    // Loop through the file creating dynamic array of specified PCBs
    for (uint32_t i = 0; i < num_PCB[0] * 3; i += 3)
    {
        ProcessControlBlock_t *PCB = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t));
        PCB->remaining_burst_time = buffer[i];
        PCB->priority = buffer[i + 1];
        PCB->arrival = buffer[i + 2];
        PCB->started = false;
        dyn_array_push_back(PCB_array, PCB);
        printf("%u ", PCB->remaining_burst_time);
        printf("%u ", PCB->priority);
        printf("%u\n", PCB->arrival);
    }
    fclose(fp);
    return PCB_array;
}

int cmpfuncRemainingTime(const void *a, const void *b)
{
    return (((ProcessControlBlock_t *)a)->remaining_burst_time - ((ProcessControlBlock_t *)b)->remaining_burst_time);
}

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

    float total_run_time = 0;
    float wait_time = 0;
    size_t n = dyn_array_size(ready_queue);
    size_t current_ready_queue_index = 0;
    dyn_array_t *run_que = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    // Calculate the total run time
    for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
    {
        total_run_time += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
    }
    // loop through every unit time
    for (size_t i = 0; i < (size_t)total_run_time; i++)
    {
        // Add every element to the run que that has an arrival time less or equal to current time
        if (dyn_array_size(ready_queue) > current_ready_queue_index)
        {
            if (((ProcessControlBlock_t *)dyn_array_at(ready_queue, current_ready_queue_index))->arrival <= (uint32_t)i)
            {
                dyn_array_push_back(run_que, dyn_array_at(ready_queue, current_ready_queue_index));
                current_ready_queue_index++;
            }
        }
        // Process any PCBs in the run queue
        if (dyn_array_size(run_que) != 0)
        {
            if (!dyn_array_sort(run_que, cmpfuncRemainingTime))
            {
                return false;
            }
            wait_time += dyn_array_size(run_que) - 1;
            virtual_cpu((ProcessControlBlock_t *)dyn_array_at(run_que, 0));
            if (((ProcessControlBlock_t *)dyn_array_at(run_que, 0))->remaining_burst_time == 0)
            {
                dyn_array_erase(run_que, 0);
            }
        }
    }

    result->average_waiting_time = wait_time / n;
    result->average_turnaround_time = total_run_time / n;
    result->total_run_time = (unsigned long)total_run_time;

    // destroy dynamic arrays
    dyn_array_destroy(ready_queue);
    dyn_array_destroy(run_que);

    return true;
}
