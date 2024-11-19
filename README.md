# Icarus RPI main code
## Introduction
This project contains a task management library as well as main code running on the CM4 of the avionics of project Icarus. The program uses **CPU affinity** to bind tasks (threads) to specific cores and applies **real-time scheduling policies** to assign priorities to each task. It is specifically designed for systems running **Raspberry Pi OS with the PREEMPT_RT patch**, which supports real-time scheduling.

The SDD associated with this program can be found here:
https://rocket-team.epfl.ch/en/icarus/avionics/software/2024_I_AV_SW_MAIN_CODE_RPI_SDD

## Installation
> [!NOTE]  
> This project should obviously be complied and run on a raspberry pi running Linux with the PREEMPT RT patchset. However, it still works to a certain extent on a regular Linux machine like Ubuntu.

### Configuration of the raspberry pi
TODO

### Connect to the raspberry pi
As of the 7.11.2024, the Raspberry Pi 4 of avionics is has this specific configuration:

* WIFI uname: `ert`
* WIFI pwd: `ERT123ert`
* RPI  uname: `ert`
* RPI  pwd: `ERT123ert`

This means that, on startup, the RPI will try to connect to a network named `ert` with the password `ERT123ert`. You can create this network by sharing your phone connection for example.

Then, you can connect to it using ssh:

    ssh ert@pi.local

Here, `ert` is the RPI uname. You will be then be prompted for a password. This is the RPI password, `ERT123ert` in this case. You  should then be connected to the RPI.

Additionally, here is the configuration of Daniel's RPI 3B+

* WIFI uname: `ert`
* WIFI pwd: `ERT123ert`
* RPI  uname: `daniel`
* RPI  pwd: `ert2024`

### Copy files to the raspberry pi
You can easily copy files to the RPI by using `rsync` from your computer:

    rsync * ert@pi.local: --exclude='/.git' --exclude='/build' -tvr

This will copy your current directory to the home directory of the RPI, assuming you are connected to the same network and the pi name is `ert`.

### Requirements
The following packages are required to compile this project: `build-essential cmake`

You can install them by running these commands:

    sudo apt update
    sudo apt install -y build-essential cmake

### Compilation
From the root of the project, run the following commands:

    mkdir build
    cd build
    cmake ..
    make

## Usage
### Main code
The executable can be found in the build directory. It needs to be run with sudo in order to set scheduling policies:

    sudo ./build/src/rocket

## Debugging
The `ps` utility can be use to list running threads and processes.
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
Finally, to only show certain threads, one can pipe the output of `ps` into `grep`:

    ps -eLo start,pid,user,policy,rtprio,psr,command | grep 'rocket'

## CPU Isolation
To paraphrase [the wiki](https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cpu-partitioning/start), the `isolcpus` kernel parameter can be used to specify CPUs to be isolated from the scheduler algorithms. The argument is a CPU core list:

    isolcpus=<cpu number>,….,<cpu number>

Some other system threads may still be started on the core. See https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cpu-partitioning/start#cpu_affinity_and_kworkers and https://forums.raspberrypi.com/viewtopic.php?t=228727 for potential solutions. Those could potentially be isolated if needed, but the process may involve recompiling the kernel...

## Sources:
- https://shuhaowu.com/blog/2022/04-linux-rt-appdev-part4.html
- https://wiki.linuxfoundation.org/realtime/documentation/start
- https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html
- Tune turbo and system cpu limits: https://forums.raspberrypi.com/viewtopic.php?t=228727