#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char *find_in_path(const char *command);
void fork_and_exec_cmd(char *full_path, int argc, char **argv);
int is_executable(const char *path) { return access(path, X_OK) == 0; }

int main() {
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

  }else if(strncmp(input, "pwd", 4) == 0){
    system("pwd");
  }else{
    char *argv[10];
      int argc = 0;
      char *token = strtok(input, " ");
      while (token != NULL && argc < 10) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
      }
      argv[argc] = NULL;
      char *cmd_path = find_in_path(argv[0]);
      if (cmd_path) {
        fork_and_exec_cmd(cmd_path, argc, argv);
      } else
        printf("%s: command not found\n", argv[0]);
  }

  }

  return 0;
}



char *find_in_path(const char *command) {
  char *path_env = getenv("PATH");
  if (path_env == NULL)
    return NULL;
    char *path_copy = strdup(path_env);
    char *dir = strtok(path_copy, ":");
    static char full_path[1024];
    while (dir != NULL) {
      snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);
      if (is_executable(full_path)) {
        free(path_copy);
        return full_path;
      }
      dir = strtok(NULL, ":");
    }
    free(path_copy);
    return NULL;
  }

void fork_and_exec_cmd(char *full_path, int argc, char **argv) {
  pid_t pid = fork();
  if (pid == 0) {
    execv(full_path, argv);
    perror("execv");
    exit(1);
  } else if (pid < 0)
    perror("fork");
  else {
    int status;
    waitpid(pid, &status, 0);
  }
}
