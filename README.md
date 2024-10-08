# Icarus Task Manager

This is a C++ proof of concept program designed to demonstrate task management on a multi-core system running a real-time operating system. The program uses **CPU affinity** to bind tasks (threads) to specific cores and applies **real-time scheduling policies** to assign priorities to each task. It is specifically designed for systems running **Raspberry Pi OS with the PREEMPT_RT patch**, which supports real-time scheduling.

It is based on the [Real Time Linux project documentation](https://wiki.linuxfoundation.org/realtime/documentation/start) and the [Avionics Software Diagram](https://rocket-team.epfl.ch/en/icarus/avionics/2024_I_AV_OVERVIEW)

![Architecure diagram](architecture.png)

The aim of this program will be to manage all of the tasks running on the Raspberry Pi.