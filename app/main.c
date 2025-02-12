#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


typedef void (*builtin_func_t)();
// Structure to map builtin commands to functions
typedef struct {
  const char *name;
  const char *text;
} builtin_map;

// Create the mapping table
builtin_map builtins[] = {
  {"echo", "echo is a shell builtin\n"},
  {"type", "type is a shell builtin\n"},
  {"exit", "exit is a shell builtin\n"},
  {"pwd",  "pwd is a shell builtin\n"}
};

#define NUM_BUILTINS (sizeof(builtins)/sizeof(builtins[0]))
// Define the program functions
int is_executable(const char *path) { return access(path, X_OK) == 0; }

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

void type_command(char *input){
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

void cd_command(char *input){
  char *path = input + 2;
  while(*path == ' ') path++;
  if (strncmp(path, "~", 1) == 0)
    chdir(getenv("HOME"));
  else if (chdir(input + 3) < 0)
    printf("cd: %s: No such file or directory\n", input + 3);
}

void echo_command(char *input){
  printf("%s\n", input + 5);
}

void run_program(char *input) {
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

void handle_type_builtin(char *input) {
  char *cmd = input + 5;
  // Loop through builtins table
  for (size_t i = 0; i < NUM_BUILTINS; i++) {
      // Compare using strcmp to the length of the command string stored in the table
      if (strncmp(cmd, builtins[i].name, strlen(builtins[i].name)) == 0) {
          printf("%s",builtins[i].text);
          return;
      }
  }
  // If no builtin matched, run the type_command function
  type_command(input);
}




int main() {
  // Flush after every printf
  setbuf(stdout, NULL);
  system("clear");


  while (1) {
    printf("$ ");

  // Wait for user input and delete newline character
    char input[100];
    fgets(input, 100, stdin);
    input[strcspn(input, "\n")] = '\0';
  
  //Check user input
    if(strstr(input, "exit 0") != NULL)
      break;
    else if(strncmp(input, "echo", 4) == 0)
      echo_command(input);
    else if(strncmp(input, "type", 4) == 0)
      handle_type_builtin(input);
    else if(strncmp(input, "pwd", 4) == 0)
      system("pwd");
    else if(strncmp(input, "cd", 2) == 0)
      cd_command(input);
    else
      run_program(input);
    

  }

  return 0;
}



