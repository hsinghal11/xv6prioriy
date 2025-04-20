#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  int pid, parent_pid;
  
  printf("Priority and Lottery Scheduler Test\n");
  sleep(1); // Add delay to prevent output interleaving
  
  parent_pid = getpid();
  printf("Parent process PID: %d\n", parent_pid);
  sleep(1); // Add delay to prevent output interleaving
  printf("Initial priority: %d\n", getPriority(parent_pid));
  sleep(1); // Add delay to prevent output interleaving
  
  // Fork a child process
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }
  
  if(pid == 0){ // Child process
    int my_pid = getpid();
    printf("Child process PID: %d\n", my_pid);
    sleep(1); // Add delay to prevent output interleaving
    printf("Child initial priority: %d\n", getPriority(my_pid));
    sleep(1); // Add delay to prevent output interleaving
    
    // Child process runs a simple loop
    for(int i = 0; i < 2; i++) { // Reduced from 100 to 20 iterations
      printf("Child process running... %d\n", i);
      sleep(1); // Add delay to prevent output interleaving
    }
    
    exit(0);
  } else { // Parent process
    // Set priority of child
    printf("Parent setting child priority to 15\n");
    sleep(1); // Add delay to prevent output interleaving
    if(setPriority(pid, 15) == 0){
      printf("Successfully set child priority\n");
      sleep(1); // Add delay to prevent output interleaving
    } else {
      printf("Failed to set child priority\n");
      sleep(1); // Add delay to prevent output interleaving
    }
    
    printf("Child's new priority: %d\n", getPriority(pid));
    sleep(1); // Add delay to prevent output interleaving
    
    // Create a second child to demonstrate lottery scheduling
    int pid2 = fork();
    if(pid2 < 0){
      printf("second fork failed\n");
      exit(1);
    }
    
    if(pid2 == 0){ // Second child process
      int my_pid = getpid();
      printf("Second child process PID: %d\n", my_pid);
      sleep(1); // Add delay to prevent output interleaving
      
      // Second child process runs another loop
      for(int i = 0; i < 2; i++) { // Reduced from 100 to 20 iterations
        printf("Second child process running... %d\n", i);
        sleep(1); // Add delay to prevent output interleaving
      }
      
      exit(0);
    } else {
      // Wait for both children
      wait(0);
      wait(0);
      printf("Both children have completed\n");
      sleep(1); // Add delay to prevent output interleaving
    }
  }
  
  exit(0);
} 