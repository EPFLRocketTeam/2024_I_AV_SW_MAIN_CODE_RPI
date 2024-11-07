#ifndef TASK_H
#define TASK_H

#include <time.h>
#include <pthread.h>
#include <string>

// The Task class represents a periodic task that can be run on a specific CPU core with a given priority.
class Task
{
public:
    // Starts the task by creating a thread and running the task loop.
    void start_task();
    // Waits for the task to complete.
    void wait_for_task();

protected:
    // Constructor to initialize the task with a name, period in nanoseconds, priority, CPU core, and the task function.
    Task(std::string name,
         long int period_ns,
         int priority,
         int cpu_core);

    // This function is called every period of the task.
    // Derived classes should implement this function to define the task behavior.
    virtual void loop() = 0;

    // TODO: Add a destructor to clean up resources.

private:
    std::string name;               // Name of the task.
    long int period_ns;             // Period of the task in nanoseconds.
    int priority;                   // Priority of the task.
    int cpu_core;                   // CPU core on which the task should run.

    pthread_attr_t thread_attributes; // Thread attributes for the task.
    pthread_t thread;                 // Thread handle for the task.
    struct timespec next_period;      // Time for the next period of the task.

    // Prepares the task by setting thread attributes and other necessary configurations.
    void prepare_task();

    // Static wrapper function to call the task loop.
    static void *task_loop_wrapper(void *context);

    // The main task loop that runs the task periodically.
    void *task_loop();

    // Measures the time and runs the task function.
    void measure_and_run_task();

    // Increments the period to the next cycle.
    void increment_period();
};

#endif