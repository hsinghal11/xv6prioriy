#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// A simplified test program that avoids console output interleaving
// by collecting statistics and printing them at the end

#define MAX_ITERATIONS 1000
#define RUNTIME_MS 3000 // Run for approximately 3 seconds

int
main(int argc, char *argv[])
{
  int i, pid;
  int child_count = 3;
  int ticket_allocations[3] = {10, 30, 60}; // 10%, 30%, 60% of tickets
  
  printf("Improved Lottery Scheduler Test\n");
  printf("-------------------------------\n");
  printf("This test will run 3 processes with different ticket allocations:\n");
  printf("Process 0: 10 tickets (10%%)\n");
  printf("Process 1: 30 tickets (30%%)\n");
  printf("Process 2: 60 tickets (60%%)\n");
  printf("Each process will increment a counter as fast as possible.\n");
  printf("If the lottery scheduler is working correctly, the counter values\n");
  printf("should be roughly proportional to the ticket allocations.\n\n");
  
  // Shared region for reporting results - we'll use a file for this
  // Create the file and initialize it with zeros
  int fd = open("results", O_CREATE | O_RDWR);
  if(fd < 0) {
    printf("Failed to create results file\n");
    exit(1);
  }
  
  // Initialize the file with zeros
  for(i = 0; i < child_count; i++) {
    int zero = 0;
    write(fd, &zero, sizeof(int));
  }
  close(fd);
  
  // Create child processes
  for(i = 0; i < child_count; i++) {
    pid = fork();
    if(pid < 0) {
      printf("Fork failed\n");
      exit(1);
    }
    
    if(pid == 0) { // Child process
      int my_pid = getpid();
      int tickets = ticket_allocations[i];
      int counter = 0;
      int start_time;
      
      // Set tickets for this process
      if(setTickets(my_pid, tickets) < 0) {
        printf("Failed to set tickets for child %d\n", i);
        exit(1);
      }
      
      // Record start time
      start_time = uptime();
      
      // Increment counter as fast as possible for the specified duration
      while(uptime() - start_time < RUNTIME_MS / 10) { // Convert to ticks
        counter++;
        
        // Occasionally yield to give other processes a chance
        if(counter % 10000 == 0) {
          sleep(0); // Yield
        }
      }
      
      // Write result to file
      fd = open("results", O_RDWR);
      if(fd < 0) {
        printf("Child %d: Failed to open results file\n", i);
        exit(1);
      }
      
      // Seek to the right position
      for(int j = 0; j < i; j++) {
        int dummy;
        read(fd, &dummy, sizeof(int));
      }
      
      // Write the counter value
      write(fd, &counter, sizeof(int));
      close(fd);
      
      exit(0);
    } else {
      // Parent continues
    }
  }
  
  // Parent waits for all children
  for(i = 0; i < child_count; i++) {
    wait(0);
  }
  
  // Read and display results
  printf("\nTest completed. Results:\n");
  
  fd = open("results", O_RDONLY);
  if(fd < 0) {
    printf("Failed to open results file\n");
    exit(1);
  }
  
  int results[3];
  int total = 0;
  
  // Read the results
  for(i = 0; i < child_count; i++) {
    read(fd, &results[i], sizeof(int));
    total += results[i];
  }
  close(fd);
  
  // Display results and percentages
  for(i = 0; i < child_count; i++) {
    float percentage = (float)results[i] / total * 100;
    printf("Process %d (%d tickets): %d iterations (%.2f%%)\n", 
           i, ticket_allocations[i], results[i], percentage);
  }
  
  printf("\nExpected percentages: 10%%, 30%%, 60%%\n");
  printf("If the actual percentages are close to the expected ones,\n");
  printf("the lottery scheduler is working correctly.\n");
  
  // Clean up
  unlink("results");
  
  exit(0);
} 