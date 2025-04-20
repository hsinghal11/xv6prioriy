#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  int i, n, pid;
  
  printf("Lottery Scheduler Test\n");
  sleep(1); // Add delay to prevent output interleaving
  
  // Number of child processes to create
  n = 3;
  
  // Create n child processes with different tickets
  for(i = 0; i < n; i++){
    pid = fork();
    if(pid < 0){
      printf("fork failed\n");
      exit(1);
    }
    
    if(pid == 0){ // Child process
      int my_pid = getpid();
      int ticket_count = 0;
      
      // Assign different ticket counts to children
      if(i == 0){
        ticket_count = 10; // First child gets 10 tickets
      } else if(i == 1){
        ticket_count = 30; // Second child gets 30 tickets
      } else {
        ticket_count = 60; // Third child gets 60 tickets
      }
      
      // Set the tickets for this process
      if(setTickets(my_pid, ticket_count) == 0){
        printf("Child %d (PID: %d) set tickets to %d\n", i, my_pid, ticket_count);
        sleep(1); // Add delay after each print
      } else {
        printf("Failed to set tickets for Child %d (PID: %d)\n", i, my_pid);
        sleep(1); // Add delay after each print
      }
      
      // Each child does some work proportional to its ticket count
      for(int j = 0; j < 20; j++){ // Reduced from 50 to 20 iterations
        printf("Child %d (PID: %d) running iteration %d\n", i, my_pid, j);
        sleep(1); // Add delay after each print
        
        // Do some CPU-intensive work
        for(volatile int k = 0; k < 1000000; k++);
        
        // Yield to let other processes run
        sleep(5);
      }
      
      exit(0);
    }
  }
  
  // Parent waits for all children to complete
  for(i = 0; i < n; i++){
    wait(0);
  }
  
  printf("All child processes have completed\n");
  exit(0);
} 