#include "task.h"
#include "logging.h"
#include "configure.h"
#include <unistd.h>

#include <unistd.h>
#include <iostream>
using namespace std;

void *low_prio_function()
{
    cout << "Hello from low priority task!" << endl;
    return nullptr;
}

void *high_prio_function()
{
    cout << "Hello from high priority task!" << endl;
    return nullptr;
}

// Define control function
void *control_function(double setpoint, double measured_value)
{
    cout << "Executing control task..." << endl;
    static double error = 0.0;
    static double integral = 0.0;
    static double previous_error = 0.0;

    // PID Constants (proportional, integral, derivative)
    double kp = 1.0;
    double ki = 0.1;
    double kd = 0.01;

    // Calculate error
    error = setpoint - measured_value;

    // Proportional term
    double p_term = kp * error;

    // Integral term
    integral += error;
    double i_term = ki * integral;

    // Derivative term
    double d_term = kd * (error - previous_error);

    // PID output
    double output = p_term + i_term + d_term;

    // Update previous error
    previous_error = error;

    // Print the output for demonstration purposes
    cout << "Control output: " << output << endl;

    // Send the output to actuators (placeholder)
    // This would be replaced by actual actuator control code
    return nullptr;
}


int main()
{
    check_cpuisolation(3);

    // Testing that a low priority task can be preempted by a high priority task
    // Initialize tasks with name, period, priority, CPU core, and function.
    Task low_priority_task = Task("low priority task", 0, 80, 0, low_prio_function);
    Task high_priority_task = Task("high priority task", 0, 90, 0, high_prio_function);

    low_priority_task.start_task();
    sleep(10);
    high_priority_task.start_task();

    low_priority_task.wait_for_task();
    high_priority_task.wait_for_task();

    return 0;
}

