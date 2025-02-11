#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  FILE *fp;
  char path[128];
  // Flush after every printf
  setbuf(stdout, NULL);
  system("clear");

  // Uncomment this block to pass the first stage
  while (1) {
   printf("$ ");

  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);
  input[strcspn(input, "\n")] = '\0';
  if(strstr(input, "exit 0") != NULL){
    break;
  }else if(strncmp(input, "echo", 4) == 0){
    printf("%s\n", input + 5);
  }else if(strncmp(input, "type", 4) == 0){

    if (strncmp(input + 5, "echo", 4) == 0){
      printf("echo is a shell builtin\n");
    }else if(strncmp(input + 5, "type", 4) == 0){
      printf("type is a shell builtin\n");
    }else if(strncmp(input + 5, "exit", 4) == 0){
      printf("exit is a shell builtin\n");
    }else{
      char command[256];
      sprintf(command, "which %s > /dev/null 2>&1", input + 5);
      if(system(command) == 0){
          printf("%s is ",input + 5);
          sprintf(command, "which %s", input + 5);
          system(command);
      }else{
        printf("%s: not found\n", input + 5);
      }
    }

  }else{
  printf("%s: command not found\n", input);
  }

  }

  return 0;
}
