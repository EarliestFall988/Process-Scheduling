#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"

// Using a C library requires extern "C" to prevent function managling
extern "C"
{
#include <dyn_array.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;

class GradeEnvironment : public testing::Environment
{
public:
    virtual void SetUp()
    {
        score = 0;
        total = 210;
    }

    virtual void TearDown()
    {
        ::testing::Test::RecordProperty("points_given", score);
        ::testing::Test::RecordProperty("points_total", total);
        std::cout << "SCORE: " << score << '/' << total << std::endl;
    }
};

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
    return RUN_ALL_TESTS();
}

// first come first serve tests

// Null result test

TEST(first_come_first_serve, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    bool result = false;
    result = first_come_first_serve(t, NULL);

    EXPECT_EQ(false, result);
}

// NULL Dynamic Array Test
TEST(first_come_first_serve, NULL_Dynamic_Array)
{
    ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);

    bool result = false;

    result = first_come_first_serve(t, &r);

    EXPECT_EQ(false, result);
}

// test valid pcb
TEST(first_come_first_serve, VALID_PCB)
{
    dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t r = {0, 0, 0};
    ProcessControlBlock_t pcb1 = {1, 0, 1, false};
    ProcessControlBlock_t pcb2 = {2, 0, 2, false};
    ProcessControlBlock_t pcb3 = {3, 0, 3, false};
    ProcessControlBlock_t pcb4 = {4, 0, 4, false};
    ProcessControlBlock_t pcb5 = {5, 0, 5, false};
    dyn_array_push_back(t, &pcb1);
    dyn_array_push_back(t, &pcb2);
    dyn_array_push_back(t, &pcb3);
    dyn_array_push_back(t, &pcb4);
    dyn_array_push_back(t, &pcb5);

    bool result = false;
    result = first_come_first_serve(t, &r);

    EXPECT_EQ(true, result);
    EXPECT_EQ(4, r.average_waiting_time);
    EXPECT_EQ(3, r.average_turnaround_time);
    EXPECT_EQ((unsigned long)15, r.total_run_time);
}

// shortest job first tests

TEST(shortest_job_first, NULL_Ready_Queue)
{
    ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);
    bool result = false;
    result = shortest_job_first(t, &r);
    EXPECT_EQ(false, result);
}

//NULL Result test
TEST(shortest_job_first, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, 5, NULL);
    bool result = false;
    result = shortest_job_first(t, NULL);

    EXPECT_EQ(false, result);
}

// Valid PCB Test
// TEST(shortest_job_first, VALID_PCB)
// {
//     dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);
//     ScheduleResult_t r = {0, 0, 0};
//     ProcessControlBlock_t pcb1 = {1, 0, 3, false};
//     ProcessControlBlock_t pcb2 = {4, 0, 1, false};
//     ProcessControlBlock_t pcb3 = {2, 0, 4, false};
//     ProcessControlBlock_t pcb4 = {6, 0, 0, false};
//     ProcessControlBlock_t pcb5 = {3, 0, 2, false};
//     dyn_array_push_back(t, &pcb1);
//     dyn_array_push_back(t, &pcb2);
//     dyn_array_push_back(t, &pcb3);
//     dyn_array_push_back(t, &pcb4);
//     dyn_array_push_back(t, &pcb5);

//     bool result = false;
//     result = shortest_job_first(t, &r);

//     EXPECT_EQ(true, result);
//     EXPECT_EQ(3.8, r.average_waiting_time);
//     EXPECT_EQ(7, r.average_turnaround_time);
//     EXPECT_EQ((unsigned long)16, r.total_run_time);
// }

//Second Valid PCB Test
TEST(shortest_job_first, VALID_PCB1)
{
    dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t r = {0, 0, 0};
    ProcessControlBlock_t pcb1 = {6, 0, 1, false};
    ProcessControlBlock_t pcb2 = {8, 0, 2, false};
    ProcessControlBlock_t pcb3 = {7, 0, 3, false};
    ProcessControlBlock_t pcb4 = {3, 0, 4, false};

    dyn_array_push_back(t, &pcb1);
    dyn_array_push_back(t, &pcb2);
    dyn_array_push_back(t, &pcb3);
    dyn_array_push_back(t, &pcb4);


    bool result = false;
    result = shortest_job_first(t, &r);

    EXPECT_EQ(true, result);
    EXPECT_EQ(7, r.average_waiting_time);
    EXPECT_EQ(6, r.average_turnaround_time);
    EXPECT_EQ((unsigned long)24, r.total_run_time);
}

// shortest remaining time first tests

//NULL Dynamic Array Test
TEST(shortest_remaining_time_first, NULL_Ready_Queue)
{
    ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);
    bool result = false;
    result = shortest_remaining_time_first(t, &r);
    EXPECT_EQ(false, result);
}

TEST(shortest_remaining_time_first, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    bool result = false;
    result = shortest_remaining_time_first(t, NULL);

    EXPECT_EQ(false, result);
}

//Valid PCB Test
TEST(shortest_remaining_time_first, Valid_PCB)
{
    dyn_array_t *t = dyn_array_create(4, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t r = {0, 0, 0};
    ProcessControlBlock_t pcb1 = {6, 0, 2, false};
    ProcessControlBlock_t pcb2 = {2, 0, 5, false};
    ProcessControlBlock_t pcb3 = {8, 0, 1, false};
    ProcessControlBlock_t pcb4 = {3, 0, 0, false};
    ProcessControlBlock_t pcb5 = {4, 0, 4, false};

    dyn_array_push_back(t, &pcb1);
    dyn_array_push_back(t, &pcb2);
    dyn_array_push_back(t, &pcb3);
    dyn_array_push_back(t, &pcb4);
    dyn_array_push_back(t, &pcb5);

    bool result = false;
    result = shortest_remaining_time_first(t, &r);

    EXPECT_EQ(true, result);
    EXPECT_EQ((float)4.6, r.average_waiting_time);
    EXPECT_TRUE((float)9.2 == (float)r.average_turnaround_time);
    EXPECT_EQ((unsigned long)23, r.total_run_time);
}

// round robin tests

//NULL Dynamic Array
TEST(round_robin, NULL_Ready_Queue)
{
    // ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, sizeof(ProcessControlBlock_t), NULL);
    bool result = false;
    result = round_robin(t, NULL, QUANTUM);
    EXPECT_EQ(false, result);
}

//NULL Result Test
TEST(round_robin, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    bool result = false;
    result = round_robin(t, NULL, QUANTUM);

    EXPECT_EQ(false, result);
}

TEST(round_robin, Valid_PCB)
{
    dyn_array_t *t = dyn_array_create(4, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t r = {0, 0, 0};
    ProcessControlBlock_t pcb0 = {8, 0, 0, false};
    ProcessControlBlock_t pcb1 = {5, 0, 1, false};
    ProcessControlBlock_t pcb2 = {10, 0, 2, false};
    ProcessControlBlock_t pcb3 = {11, 0, 3, false};
    dyn_array_push_back(t, &pcb0);
    dyn_array_push_back(t, &pcb1);
    dyn_array_push_back(t, &pcb2);
    dyn_array_push_back(t, &pcb3);

    bool result = false;
    int quantum = 6;
    result = round_robin(t, &r, quantum);

    EXPECT_EQ(true, result);
    EXPECT_EQ(14.75, r.average_waiting_time);
    EXPECT_EQ(23.25, r.average_turnaround_time);
    EXPECT_EQ((unsigned long)34, r.total_run_time);
}





// loading process control block tests

// NULL File Name
TEST(load_process_control_blocks, NULL_File_Name)
{
    dyn_array_t *t = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    t = load_process_control_blocks(NULL);
    EXPECT_TRUE(t == NULL);
    dyn_array_destroy(t);
}

//NULL FILE
TEST(load_process_control_blocks, NULL_File)
{
    dyn_array_t *t = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    t = load_process_control_blocks("test.txt");
    EXPECT_TRUE(t == NULL);
}

// Valid file test comparing size
TEST(load_process_control_blocks, Valid_File)
{

    dyn_array_t *t = load_process_control_blocks("../pcb.bin");
    EXPECT_EQ((size_t)4, dyn_array_size(t));
}