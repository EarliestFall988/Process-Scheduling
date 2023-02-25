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
    float runtime = 0;
    int n = dyn_array_size(ready_queue);
    for (int i = 0; i < n; i++)
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);
        while (current->remaining_burst_time > 0)
        {
            virtual_cpu(current);
        }
        int x = n - i;
        runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
        if (i > 0)
        {
            waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x;
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
// bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
// {
//     if (ready_queue == NULL || result == NULL)
//     {
//         return false;
//     } // Check for invalid param
//     if (!dyn_array_sort(ready_queue, cmpfuncShortest))
//     {
//         return false;
//     }; // Sort array by arrival time

//     float waittime = 0;
//     float runtime = 0;
//     int n = dyn_array_size(ready_queue);
//     for (int i = 0; i < n; i++)
//     {
//         ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);
//         while (current->remaining_burst_time > 0)
//         {
//             virtual_cpu(current);
//         }
//         int x = n - i;
//         runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
//         if (i > 0)
//         {
//             waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x;
//         }
//     }

//     dyn_array_destroy(ready_queue);

//     result->average_waiting_time = waittime / n;
//     result->average_turnaround_time = runtime / n;
//     result->total_run_time = runtime;
//     return true;
// }

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
    float runtime = 0;

    // create dynamic array
    dyn_array_t *run_que = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);

    // completed array
    dyn_array_t *completed = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);

    int n = dyn_array_size(ready_queue);
    uint32_t longestArraySize = 0;
    for (int i = 0; i < n; i++)
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
        if (current->arrival > longestArraySize)
        {
            longestArraySize = current->arrival;
        }

        current->priority = 0; // double checking process priority is 0
    }

    uint32_t k = 0;

    while (k < longestArraySize || dyn_array_size(run_que) > 0) // while loop, increment with k and check if run_que is empty
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, k);
        if (current->arrival == k)
        {
            dyn_array_push_back(run_que, dyn_array_at(ready_queue, k));
            dyn_array_sort(run_que, cmpfuncShortest);
        }

        // vcp here in order to compute the jobs that are already in the run_que

        if (dyn_array_size(run_que) > 0)
        {
            virtual_cpu((ProcessControlBlock_t *)dyn_array_at(run_que, 0));
            runtime += 1;
            waittime += runtime; // TODO: Wait time does not seem correct theoretically

            if (dyn_array_size(run_que) > 1)
            {
                for (size_t i = 1; i < dyn_array_size(run_que); i++)
                {
                    ((ProcessControlBlock_t *)dyn_array_at(run_que, i))->priority += 1;
                }
            }

            if (((ProcessControlBlock_t *)dyn_array_at(run_que, 0))->remaining_burst_time == 0) // if the job is done, remove it from the run_que
            {
                dyn_array_push_back(completed, dyn_array_at(run_que, 0));
                dyn_array_erase(run_que, 0); // good bye priority
            }
        }

        if (k < longestArraySize)
            k++;
    }

    result->average_waiting_time = waittime / n;
    result->average_turnaround_time = runtime / n;
    result->total_run_time = runtime;

    // destroy dynamic arrays
    dyn_array_destroy(ready_queue);
    dyn_array_destroy(run_que);
    dyn_array_destroy(completed);

    return true;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    if (ready_queue == NULL || result == NULL || quantum <= 0)
    {
        return false;
    } // Check for invalid param
    float completion_time[dyn_array_size(ready_queue)];
    float waiting_time[dyn_array_size(ready_queue)];
    size_t completed_PCB = dyn_array_size(ready_queue);
    while (completed_PCB != 0)
    {
        for (size_t i = 0; i < dyn_array_size(ready_queue); i++)
        {
            ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
            for (size_t j = 0; j < quantum; j++)
            {
                if (current->remaining_burst_time == 0)
                {
                    break;
                }
                virtual_cpu(current);
                if (current->remaining_burst_time == 0)
                {
                    --completed_PCB;
                }
                for (size_t l = 0; l < dyn_array_size(ready_queue); l++)
                {
                    if (current->started)
                    {
                        completion_time[l]++;
                    }
                    else
                    {
                        waiting_time[l]++;
                    }
                }
            }
        }
    }
    dyn_array_destroy(ready_queue);

    float average_waiting_time = 0;
    unsigned long total_run_time = 0;
    for (int i = 0; i < (int)sizeof(waiting_time); i++)
    {
        average_waiting_time += waiting_time[i];
        total_run_time += completion_time[i];
    }
    // Put results in result structure
    result->average_waiting_time = average_waiting_time / (sizeof(waiting_time));
    result->average_turnaround_time = (float)total_run_time / (sizeof(waiting_time));
    result->total_run_time = total_run_time;
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
    printf("%d \n", num_PCB[1]);
    if (fread(num_PCB, sizeof(uint32_t), 1, fp) != 1)
    {
        return NULL;
    }
    uint32_t buffer[num_PCB[1] * 3];
    if (fseek(fp, sizeof(uint32_t), SEEK_SET) != 0)
    {
        return NULL;
    };
    if (fread(buffer, sizeof(uint32_t), num_PCB[1] * 3, fp) != (num_PCB[1] * 3))
    {
        return false;
    };
    dyn_array_t *PCB_array = dyn_array_create((size_t)num_PCB[1], sizeof(uint32_t), NULL);
    if (PCB_array == NULL)
    {
        return NULL;
    }
    // Loop through the file creating dynamic array of specified PCBs
    for (uint32_t i = 0; i < num_PCB[1] * 3; i += 3)
    {
        ProcessControlBlock_t *PCB = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t));
        PCB->remaining_burst_time = buffer[i];
        PCB->priority = buffer[i + 1];
        PCB->arrival = buffer[i + 2];
        PCB->started = false;
        dyn_array_push_back(PCB_array, PCB);
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
    if (!dyn_array_sort(ready_queue, cmpfuncRemainingTime))
    {
        return false;
    }; // Sort array by arrival time

    float waittime = 0;
    float runtime = 0;
    int n = dyn_array_size(ready_queue);
    for (int i = 0; i < n; i++)
    {
        ProcessControlBlock_t *current = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);
        while (current->remaining_burst_time > 0)
        {
            virtual_cpu(current);
        }
        int x = n - i;
        runtime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i))->remaining_burst_time;
        if (i > 0)
        {
            waittime += ((ProcessControlBlock_t *)dyn_array_at(ready_queue, i - 1))->remaining_burst_time * x;
        }
    }

    dyn_array_destroy(ready_queue);

    result->average_waiting_time = waittime / n;
    result->average_turnaround_time = runtime / n;
    result->total_run_time = runtime;
    return true;
}
