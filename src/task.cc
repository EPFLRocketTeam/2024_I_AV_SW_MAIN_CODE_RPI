#include "task.h"
#include <time.h>
#include <pthread.h>
#include <iostream>
#include <cstring>


// Constructor
Task::Task(std::string name, long int period_ns, int priority, int cpu_core)
    : name(name),
      period_ns(period_ns),
      priority(priority),
      cpu_core(cpu_core)
{
    // check if period_ns is positive
    if (period_ns <= 0)
    {
        throw std::invalid_argument("period_ns must be positive");
    }

    // check if priority is in range
    if (priority < 1 || priority > 99)
    {
        throw std::invalid_argument("priority is out of range");
    }

    // check if cpu_core is available
    cpu_set_t available_cpus;
    CPU_ZERO(&available_cpus);
    // checks if the cpu_core is available
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

// Wrapper function to call the task loop
// TODO: Should this be static?
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

    // Todo: We could pass elapsed time to the loop function
    loop();

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
void Task::prepare_task()
{
    // Initialize the pthread attribute
    int ret = pthread_attr_init(&thread_attributes);
    if (ret)
    {
        throw std::runtime_error(std::strerror(ret));
    }

    // Set the scheduler policy
    ret = pthread_attr_setschedpolicy(&thread_attributes, SCHED_FIFO);
    if (ret)
    {
        throw std::runtime_error(std::strerror(ret));
    }

    // Set the scheduler priority
    struct sched_param param;
    param.sched_priority = priority;
    ret = pthread_attr_setschedparam(&thread_attributes, &param);
    if (ret)
    {
        throw std::runtime_error(std::strerror(ret));
    }

    // Make sure threads created using the thread_attr_ takes the value
    // from the attribute instead of inherit from the parent thread
    ret = pthread_attr_setinheritsched(&thread_attributes, PTHREAD_EXPLICIT_SCHED);
    if (ret)
    {
        throw std::runtime_error(std::strerror(ret));
    }

    // Assign a core to the task
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);
    ret = pthread_attr_setaffinity_np(&thread_attributes, sizeof(cpu_set_t), &cpuset);
    if (ret)
    {
        throw std::runtime_error(std::strerror(ret));
    }
}

// Starts the task by creating a new thread and checking for errors
void Task::start_task()
{
    clock_gettime(CLOCK_MONOTONIC, &next_period);
    int ret = pthread_create(&thread, &thread_attributes, task_loop_wrapper, this);
    if (ret)
    {
        if (ret == EPERM)
        {
            std::cout << "Did you run the program with sudo?" << std::endl;
        }
        throw std::runtime_error(std::strerror(ret));
    }
}

// Waits for the task to finish
void Task::wait_for_task()
{
    int ret = pthread_join(thread, nullptr);
    if (ret)
    {
        throw std::runtime_error(std::strerror(ret));
    }
}