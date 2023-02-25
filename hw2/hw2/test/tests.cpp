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

TEST(first_come_first_serve, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, 5, NULL);
    bool result = false;
    result = first_come_first_serve(t, NULL);

    EXPECT_EQ(false, result);
}

TEST(first_come_first_serve, NULL_Dynamic_Array)
{
    ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, 32, NULL);

    bool result = false;

    result = first_come_first_serve(t, &r);

    EXPECT_EQ(false, result);
}

// test valid pcb
TEST(first_come_first_serve, VALID_PCB)
{
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
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
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
    bool result = false;
    result = shortest_job_first(t, &r);
    EXPECT_EQ(false, result);
}

TEST(shortest_job_first, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, 5, NULL);
    bool result = false;
    result = shortest_job_first(t, NULL);

    EXPECT_EQ(false, result);
}



// shortest remaining time first tests

TEST(shortest_remaining_time_first, NULL_Ready_Queue)
{
    ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
    bool result = false;
    result = shortest_remaining_time_first(t, &r);
    EXPECT_EQ(false, result);
}

TEST(shortest_remaining_time_first, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, 5, NULL);
    bool result = false;
    result = shortest_remaining_time_first(t, NULL);

    EXPECT_EQ(false, result);
}

TEST(shortest_remaining_time_first, Valid_PCB)
{
    dyn_array_t *t = dyn_array_create(4, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t r = {0, 0, 0};
    ProcessControlBlock_t pcb1 = {8, 0, 0, false};
    ProcessControlBlock_t pcb2 = {4, 0, 1, false};
    ProcessControlBlock_t pcb3 = {9, 0, 2, false};
    ProcessControlBlock_t pcb4 = {5, 0, 3, false};
    dyn_array_push_back(t, &pcb1);
    dyn_array_push_back(t, &pcb2);
    dyn_array_push_back(t, &pcb3);
    dyn_array_push_back(t, &pcb4);

    bool result = false;
    result = shortest_remaining_time_first(t, &r);

    EXPECT_EQ(true, result);
    EXPECT_EQ(6.5, r.average_waiting_time);
    EXPECT_EQ(26 / 4, r.average_turnaround_time);
    EXPECT_EQ((unsigned long)26, r.total_run_time);
}

// round robin tests

TEST(round_robin, NULL_Ready_Queue)
{
    ScheduleResult_t r = {0, 0, 0};
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
    bool result = false;
    result = round_robin(t, &r, QUANTUM);
    EXPECT_EQ(false, result);
}

TEST(round_robin, NULL_Schedule_Result)
{
    dyn_array_t *t = dyn_array_create(5, 5, NULL);
    bool result = false;
    result = round_robin(t, NULL, QUANTUM);

    EXPECT_EQ(false, result);
}

TEST(round_robin, Valid_PCB)
{
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
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
// loading process control block tests

TEST(load_process_control_blocks, NULL_File_Name)
{
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
    t = load_process_control_blocks(NULL);
    EXPECT_TRUE(t == NULL);
}

TEST(load_process_control_blocks, NULL_File)
{
    dyn_array_t *t = dyn_array_create(32, 32, NULL);
    t = load_process_control_blocks("test.txt");
    EXPECT_TRUE(t == NULL);
}

TEST(load_process_control_blocks, Valid_File)
{
    dyn_array_t *correct = dyn_array_create(248, sizeof(ProcessControlBlock_t), NULL); // no idea what correct is yet.

    // ProcessControlBlock_t pcb1 = {1, 0, 0, 10};
    // ProcessControlBlock_t pcb2 = {2, 0, 0, 5};

    // dyn_array_push_back(correct, &pcb1);
    // dyn_array_push_back(correct, &pcb2);

    dyn_array_t *t = dyn_array_create(248, sizeof(ProcessControlBlock_t), NULL);
    t = load_process_control_blocks("../pcb.bin");
    EXPECT_TRUE(dyn_array_size(t) == dyn_array_size(correct));

    // for (int i = 0; i < dyn_array_size(t); i++)
    // {
    //     ProcessControlBlock_t *pcb = dyn_array_at(t, i);
    //     ProcessControlBlock_t *pcb_correct = dyn_array_at(correct, i);
    //     EXPECT_TRUE(pcb->pid == pcb_correct->pid);
    //     EXPECT_TRUE(pcb->arrival_time == pcb_correct->arrival_time);
    //     EXPECT_TRUE(pcb->priority == pcb_correct->priority);
    //     EXPECT_TRUE(pcb->cpu_burst == pcb_correct->cpu_burst);
    // }
    // dyn_array_destroy(t);
    // dyn_array_destroy(correct);
}