# Icarus RPI main code

## Introduction
This project contains a task management library as well as main code running on the CM4 of the avionics of project Icarus. The program uses **CPU affinity** to bind tasks (threads) to specific cores and applies **real-time scheduling policies** to assign priorities to each task. It is specifically designed for systems running **Raspberry Pi OS with the PREEMPT_RT patch**, which supports real-time scheduling.

The SDD associated with this program can be found here:
https://rocket-team.epfl.ch/en/icarus/avionics/software/2024_I_AV_SW_MAIN_CODE_RPI_SDD

## Installation
> [!NOTE]  
> This project i intended to be run on a raspberry pi running Linux with the PREEMPT RT patchset. However, it still works to a certain extent on a regular Linux machine like Ubuntu.

### Cloning the repository
> [!CAUTION]
> This repository uses git submodules! To clone it, use the --recurse-submodules flag like so:
> ```bash
> git clone --recurse-submodules https://github.com/EPFLRocketTeam/2024_I_AV_SW_MAIN_CODE_RPI.git
> ```

### Configuration of the raspberry pi
See https://rocket-team.epfl.ch/icarus/avionics/software/2025_I_AV_SW_RTLinux for instructions on  how to install Linux with the PREEMPT RT patch on a Raspberry Pi

### Connection to the raspberry pi
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

And this is the configuration of the CM4:
* WIFI uname: `ert`
* WIFI pwd: `ERT123ert`
* RPI  uname: `pi`
* RPI  pwd: `raspberry`

### Cross-compilation *(Recommended)*
Cross-compilation means compiling the project on a machine different from the one on which it will run. This is useful when the target machine has different architecture or OS. In this case, the target machine is a Raspberry Pi 4 running Linux with the PREEMPT RT patch. This is the recommended way to compile the project. 

#### Requirements
The following packages are required to compile this project: `build-essential cmake gcc-aarch64-linux-gnu g++-aarch64-linux-gnu`
You can install them on your machine by running these commands:
```bash
sudo apt update
sudo apt install -y build-essential cmake gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

#### Compilation
From the root of the project, run the following commands:
```bash
mkdir build_cm4
cd build_cm4
cmake .. -DCROSS_COMPILE_CM4=ON
make
```
This will compile the project for the Raspberry Pi 4. The resulting executable can be found in the build directory.

#### Copying the executable to the Raspberry Pi
You can easily copy files to the RPI by using `scp` from your computer:
```bash
scp build_cm4/src/rocket username@raspberrypi.local:
```
Replace `username` with the RPI username of the RPI you are using. See [Connection to the raspberry pi](#connection-to-the-raspberry-pi) for more information. This will copy the executable to the home directory on the RPI. You will be prompted for the password of the user `username` on the RPI.

### Direct compilation
If you intend to run the program on the same machine on which you are compiling it, you can compile it directly on the machine. 

#### Requirements
The packages `gcc-aarch64-linux-gnu` and `g++-aarch64-linux-gnu` are not needed in this case. The only packages required are the following: `build-essential cmake`
You can install them on your machine by running these commands:
```bash
sudo apt update
sudo apt install -y build-essential cmake
```

#### Compilation
From the root of the project, run the following commands:
```bash
mkdir build_local
cd build_local
cmake ..
make
```
The resulting executable can be found in `local_build/src/rocket`.

## Usage
### Main code
The executable can be found in the build directory. It needs to be run with sudo in order to set scheduling policies:
```bash
sudo ./build_dir/src/rocket
```
Replace `build_dir` with the name of the build directory you used. The program will start and run until it is stopped by the user.

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