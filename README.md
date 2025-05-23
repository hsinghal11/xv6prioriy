# Modified xv6 Scheduler

This is a modified version of xv6 with the following changes:

## 1. Priority System Calls

Two new system calls have been implemented:

- `getPriority(pid)`: Gets the priority of a process with the given PID.
- `setPriority(pid, priority)`: Sets the priority of a process with the given PID. 
  - Priority ranges from 0 to 20, with 0 being the lowest.
  - Only a parent process can set its child's priority.

## 2. Lottery Scheduler

The default round-robin scheduler has been replaced with a lottery scheduler that:
- Distributes CPU time based on tickets assigned to processes
- Processes with more tickets have a higher chance of being scheduled
- A maximum of 100 tickets can be assigned across all processes
- An additional system call has been added:
  - `setTickets(pid, tickets)`: Sets the number of tickets for a process

## 3. Context Switch Counter

A context switch counter has been implemented that:
- Prints "330" after every three context switches
- Pauses for approximately one second after printing

## Using the Modified System

1. Build the system:
   ```
   make clean
   make
   ```

2. Run the system using QEMU:
   ```
   make qemu
   ```

3. Run the test programs:
   ```
   prioritytest   # Tests setPriority and getPriority syscalls
   lotterytest    # Tests the lottery scheduler with different ticket allocations
   ```

## Implementation Details

- Process priorities and tickets are initialized to defaults (0 for priority, 10 for tickets)
- Child processes inherit both priority and tickets from their parent
- The kernel process always has the highest priority
- The lottery scheduler uses a simple LFSR (Linear Feedback Shift Register) for random number generation
- The scheduler selects processes based on weighted probability according to tickets

## Switching Back to Round-Robin

To switch back to the original round-robin scheduler, modify the scheduler function in kernel/proc.c to use the original implementation instead of the lottery scheduler. 