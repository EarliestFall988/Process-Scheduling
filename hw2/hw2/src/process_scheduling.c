#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block)
{
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    // Sort array by arrival time first

    /*
    float completion_time[dyn_array_size(ready_queue)];
    float waiting_time[dyn_array_size(ready_queue) + 1];
    wating_time[0] = 0;
    for(int i = 0;i < dyn_array_size(ready_queue);i++)
    {
        ProcessControlBlock_t * current = dyn_array_at(ready_queue, 0);
        completion_time[i] = current->remaining_burst_time;
        wating_time[i+1] = wating_time[i] + completion_time[i];
        while(current->remaining_burst_time != 0)
        {
            virtual_cpu(current);
        }
        dyn_array_pop_front(dyn_array_t *const dyn_array);
    }

    //Calculate values for result
    float average_waiting_time;
    float average_turnaround_time;
    unsigned long total_run_time = 0;
    for(int i = 0;i < sizeof(waiting_time);i++)
    {
        average_wating_time += waiting_time[i];
        total_run_time += completion_time[i];
    }

    result->average_waiting_time = average_waiting_time/(sizeof(waiting_time) - 1);
    result->average_turnaround_time = (float)total_run_time/(sizeof(waiting_time) - 1);
    result->total_run_time = total_run_time;


    */

    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    UNUSED(ready_queue);
    UNUSED(result);
    UNUSED(quantum);
    return false;
}

dyn_array_t *load_process_control_blocks(const char *input_file)
{
    UNUSED(input_file);
    return NULL;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}
