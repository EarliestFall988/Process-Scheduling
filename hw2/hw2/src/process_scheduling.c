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
