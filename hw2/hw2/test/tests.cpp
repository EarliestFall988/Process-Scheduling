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

// priority tests (not apart of the milestones)

// TEST(priority, NULL_Ready_Queue)
// {
//     ScheduleResult_t r = {0,0,0};
//     dyn_array_t *t = dyn_array_create(32, 32, NULL);
//     bool result = false;
//     result = priority(t, &r);
//     EXPECT_EQ(false, result);
// }

// TEST(priority, NULL_Schedule_Result)
// {
//     dyn_array_t *t = dyn_array_create(5, 5, NULL);
//     bool result = false;
//     result = priority(t, NULL);

//     EXPECT_EQ(false, result);
// }

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

// loading process control block tests

TEST(load_process_control_blocks, NULL_File_Name)
{
    dyn_array_t *t = dyn_array_create(248, 248, NULL);
    t = load_process_control_blocks(NULL);
    EXPECT_TRUE(t == NULL);
}

TEST(load_process_control_blocks, NULL_File)
{
    dyn_array_t *t = dyn_array_create(248, 248, NULL);
    t = load_process_control_blocks("test.txt");
    EXPECT_TRUE(t == NULL);
}

TEST(load_process_control_blocks, Valid_File)
{
    dyn_array_t *correct = dyn_array_create(248, 248, NULL); // no idea what correct is yet.

    dyn_array_t *t = dyn_array_create(248, 248, NULL);
    t = load_process_control_blocks("../pcb.bin");
    EXPECT_TRUE(t == correct);
}
