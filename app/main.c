#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>


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


// Define the program functions
int is_executable(const char *path) { return access(path, X_OK) == 0; }

char *quotes_handle(char *input) {
  int i = 0, j = 0, in_quotes = 0;
  char quote_char = '\0';
    while (input[i] != '\0') {
        // If not in quotes, check for quote start.
        if (!in_quotes && (input[i] == '"' || input[i] == '\'')) {
            in_quotes = 1;
            quote_char = input[i];
            i++; // Skip the opening quote.
        }
        // If in quotes and we find the matching quote.
        else if (in_quotes && input[i] == quote_char) {
            in_quotes = 0;
            quote_char = '\0';
            i++; // Skip the closing quote.
        }
        // If outside quotes and we encounter spaces, compress them.
        else if (!in_quotes && input[i] == ' ') {
            input[j++] = ' ';
            while (input[i] == ' ')
                i++;
        }
        // Otherwise, just copy the character.
        else {
            input[j++] = input[i++];
        }
    }
    input[j] = '\0';
    return input;
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

char *removeSpacesFromStr(char *string) {
  int i = 0, j = 0;
    
  while (string[i] != '\0') {
      // Copy non-space characters
      if (string[i] != ' ') {
          string[j++] = string[i++];
      } else {
          // If a space is found, insert one and skip any additional spaces.
          string[j++] = ' ';
          while (string[i] == ' ')
              i++;
      }
  }
  
  string[j] = '\0';
  return string;
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

char **parse_input(const char *input, int *argc) {
  int capacity = 10, count = 0;
  char **tokens = malloc((capacity + 1) * sizeof(char *));
  int i = 0, len = strlen(input);

  while (i < len) {
      // Skip leading whitespace.
      while (i < len && isspace((unsigned char)input[i]))
          i++;
      if (i >= len)
          break;

      char token[1024];
      int j = 0;

      // If token starts with a quote, take everything until the matching quote.
      if (input[i] == '"' || input[i] == '\'') {
          char quote = input[i++];
          while (i < len && input[i] != quote)
              token[j++] = input[i++];
          if (i < len && input[i] == quote)
              i++; // Skip closing quote.
      } else {
          // Otherwise, build token until next whitespace.
          while (i < len && !isspace((unsigned char)input[i])) {
              if (input[i] == '\\' && i + 1 < len) {  // Handle backslash escape in unquoted tokens
                  token[j++] = input[i + 1];
                  i += 2;
              } else {
                  token[j++] = input[i++];
              }
          }
      }
      token[j] = '\0';
      tokens[count++] = strdup(token);
      if (count >= capacity) {
          capacity *= 2;
          tokens = realloc(tokens, (capacity + 1) * sizeof(char *));
      }
  }
  tokens[count] = NULL;
  *argc = count;
  return tokens;
}

void echo_command(char *input) {
  int argc;
  char **argv = parse_input(input, &argc);

  // Print tokens[1]..tokens[argc-1] joined with a space.
  for (int k = 1; k < argc; k++) {
      printf("%s", argv[k]);
      if (k < argc - 1)
          printf(" ");
  }
  printf("\n");

  for (int k = 0; k < argc; k++)
      free(argv[k]);
  free(argv);
}

void run_program(char *input) {
  char **argv;
  int argc;
  argv = parse_input(input, &argc);
  char *cmd_path = find_in_path(argv[0]);
  if (cmd_path) {
    fork_and_exec_cmd(cmd_path, argc, argv);
  } else
    printf("%s: command not found\n", argv[0]);
}

void handle_type_builtin(char *input, const int NUM_BUILTINS) {
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
  
  const int NUM_BUILTINS = (sizeof(builtins)/sizeof(builtins[0]));
  // Flush after every printf
  setbuf(stdout, NULL);
  //system("clear");


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
      handle_type_builtin(input, NUM_BUILTINS);
    else if(strncmp(input, "pwd", 4) == 0)
      system("pwd");
    else if(strncmp(input, "cd", 2) == 0)
      cd_command(input);
    else
      run_program(input);
    

  }

  return 0;
}



