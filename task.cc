#include "task.h"
#include <time.h>
#include <pthread.h>
#include <iostream>
#include <cstring> 

// Macro to check for errors in function calls
#define CHECK_ERROR(function_call)                                                   \
    {                                                                                \
        int return_code = (function_call);                                           \
        if (return_code)                                                             \
        {                                                                            \
            std::cout << #function_call << " failed with error code " << return_code \
                      << " (" << strerror(return_code) << ")" << std::endl;          \
            pthread_attr_destroy(&thread_attributes);                                \
            return return_code;                                                      \
        }                                                                            \
    }

// Constructor
Task::Task(std::string name, long int period_ns, int priority, int cpu_core, void *(*task_function)())
    : name(name),
      period_ns(period_ns),
      priority(priority),
      cpu_core(cpu_core),
      task_function(task_function)
{
    //check if period_ns is positive
    if (period_ns <= 0)
    {
        throw std::invalid_argument("period_ns must be positive");
    }

    //check if priority is in range
    if (priority < 1 || priority > 99)
    {
        throw std::invalid_argument("priority is out of range");
    }

    //check if cpu_core is available
    cpu_set_t available_cpus;
    CPU_ZERO(&available_cpus);
    //checks if the cpu_core is available
    if (sched_getaffinity(0, sizeof(cpu_set_t), &available_cpus) != 0)
    {
        throw std::runtime_error("Error: Failed to retrieve CPU affinity.");
    }
    if (!CPU_ISSET(cpu_core, &available_cpus))
    {
        throw std::invalid_argument("cpu_core is not available");
    }

    prepare_task();
}

// Static wrapper function to call the task loop
void *Task::task_loop_wrapper(void *context)
{
    return static_cast<Task *>(context)->task_loop();
}

// The main task loop that runs the task periodically
void *Task::task_loop()
{
    while (true)
    {
        measure_and_run_task();
        increment_period();
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_period, nullptr);
    }
    return nullptr;
}

// Measure the time it takes to run the task function and displays it in terminal
void Task::measure_and_run_task()
{
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    task_function();
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long int time_spent_ns = (end_time.tv_sec - start_time.tv_sec) * 1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    std::cout << "Task " << name << " took " << time_spent_ns / 1e6 << " ms to execute." << std::endl;
}

// Increments the period of the task
void Task::increment_period()
{
    next_period.tv_nsec += period_ns;
    while (next_period.tv_nsec >= 1e9)
    {
        next_period.tv_sec++;
        next_period.tv_nsec -= 1e9;
    }
}

// Prepare the task by setting the thread attributes and checking for errors
int Task::prepare_task()
{
    // reminder: Could lock memory and set stack size here

    CHECK_ERROR(pthread_attr_init(&thread_attributes));
    
    struct sched_param param;
    param.sched_priority = priority;
    CHECK_ERROR(pthread_attr_setschedpolicy(&thread_attributes, SCHED_FIFO));
    CHECK_ERROR(pthread_attr_setschedparam(&thread_attributes, &param));
    CHECK_ERROR(pthread_attr_setinheritsched(&thread_attributes, PTHREAD_EXPLICIT_SCHED));

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);
    CHECK_ERROR(pthread_attr_setaffinity_np(&thread_attributes, sizeof(cpu_set_t), &cpuset));

    return 0;
}

// Starts the task by creating a new thread and checking for errors
int Task::start_task()
{
    clock_gettime(CLOCK_MONOTONIC, &next_period);
    int return_code = pthread_create(&thread, &thread_attributes, task_loop_wrapper, this);
    if (return_code)
    {
        std::cout << "Create pthread failed with error code " << return_code
                  << " (" << strerror(return_code) << ")" << std::endl;
        if (return_code == EPERM)
            std::cout << "Did you run the program with sudo?" << std::endl;
        return return_code;
    }
    return 0;
}

// Waits for the task to finish
int Task::wait_for_task()
{
    int return_code = pthread_join(thread, nullptr);
    if (return_code)
    {
        std::cout << "Join pthread failed with error code " << return_code
                  << " (" << strerror(return_code) << ")" << std::endl;
        return return_code;
    }
    return 0;
}