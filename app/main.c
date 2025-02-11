#include <stdio.h>

#include <string.h>

int main() {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage
  while (1) {
   printf("$ ");

  // Wait for user input
  char input[100];
  fgets(input, 100, stdin);
  input[strcspn(input, "\n")] = '\0';
  if(strstr(input, "exit 0") != NULL){
    break;
  }
  if(strncmp(input, "echo", 4) == 0){
    printf("%s\n", input + 5);
  }else{
  printf("%s: command not found\n", input);
  }
  }
  return 0;
}
