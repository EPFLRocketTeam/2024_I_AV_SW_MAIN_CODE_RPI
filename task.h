#ifndef TASK_H
#define TASK_H

#include <time.h>
#include <pthread.h>
#include <string>

class Task
{
public:
    Task(std::string name, long int period_ns, int priority, int cpu_core, void *(*task_functions)(void));
    int start_task();
    int wait_for_task();
    // TODO: destructor

private:
    std::string name;
    long int period_ns;
    int priority;
    int cpu_core;
    void *(*task_function)();

    pthread_attr_t thread_attributes;
    pthread_t thread;
    struct timespec next_period;

    int prepare_task();
    static void *task_loop_wrapper(void *context);
    void *task_loop();
    void measure_and_run_task();
    void increment_period();
};

#endif