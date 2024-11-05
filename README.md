# Icarus main code
*Also known as Abraracourcix*

This is a C++ proof of concept program designed to demonstrate task management on a multi-core system running a real-time operating system. The program uses **CPU affinity** to bind tasks (threads) to specific cores and applies **real-time scheduling policies** to assign priorities to each task. It is specifically designed for systems running **Raspberry Pi OS with the PREEMPT_RT patch**, which supports real-time scheduling.

It is based on the [Real Time Linux project documentation](https://wiki.linuxfoundation.org/realtime/documentation/start) and the [Avionics Software Diagram](https://rocket-team.epfl.ch/en/icarus/avionics/2024_I_AV_OVERVIEW)

![Architecure diagram](architecture.png)

The aim of this program will be to manage all of the tasks running on the Raspberry Pi.


## Copy files to the raspberry pi
You can easily copy the entire project to the raspberry pi by running:

    rsync * username@pi.local:destination/path --exclude='/.git' -v

Be sure to modify the username and destination path to correspond to the device you are using.

## Compilation
First, you need to update dependencies:

    make depend


Then, once connected to the rpi, compile the executable by running:

    make rocket


## Execution
You need sudo privileges in order to set a scheduling policy to a thread. Don't forget to use sudo!

    sudo ./rocket


## Debugging
The `ps` utility can be use to list running processes.
By default, `ps` only shows proccesses associated with the current user and terminal.
This is not very useful, as it is usually run from another terminal (like a second ssh connexion). In addition, `ps` doesn't show threads and realtime attributes if run without arguments. Therefore, I suggest using the following command:
```
ps -eLo start,pid,user,policy,rtprio,psr,command
```
Explanation:
- `-e` List all running processes
- `-L` Include threads
- `-o` Format according to the format string

Format:
- **start**: time the command started
- **pid**: a number representing the process ID
- **user**: effective user name
- **policy**: scheduling policy ("**TS**": SCHED_OTHER, "**FF**": SCHED_FIFO, "**DLN**": SCHED_DEADLINE, "**-**": not reported, "**?**": unknown)
- **rtprio**: Real-time priority (if applicable)
- **psr**: processor that process is currently assigned to
- **command**: command name

Additional information can be found in the manual: `man ps`  
Finally, to only show our threads, one can pipe the output of `ps` into `grep`:

    ps -eLo start,pid,user,policy,rtprio,psr,command | grep '[r]ocket'

We use brackets to avoid matching the grep program itself.

## CPU Isolation
To paraphrase [the wiki](https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cpu-partitioning/start), the `isolcpus` kernel parameter can be used to specify CPUs to be isolated from the scheduler algorithms. The argument is a CPU core list:

    isolcpus=<cpu number>,….,<cpu number>

Some other system threads may still be started on the core. See https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cpu-partitioning/start#cpu_affinity_and_kworkers and https://forums.raspberrypi.com/viewtopic.php?t=228727 for potential solutions. Those could potentially be isolated if needed, but the process may involve recompiling the kernel...

## Possible ameliorations
- Error handling
- Watchdog: logging, stopping unresponsive tasks
- Pre-set stack size and memory
- Test latencies!
- Tune turbo and system cpu limits: https://forums.raspberrypi.com/viewtopic.php?t=228727

https://shuhaowu.com/blog/2022/04-linux-rt-appdev-part4.html