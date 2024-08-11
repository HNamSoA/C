#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>



#define Max_Input_Length 100
#define Max_Arguments 10
#define History_Size 10  

char *history[History_Size]; 
int history_count = 0;

void addInputToHistory(char *input) {
    char *temp[History_Size];
    if (history_count < History_Size) {
        history[history_count++] = strdup(input);
    } else {
        for (int i = 0; i < History_Size - 1; i++) {
            temp[i] = history[i + 1]; 
        }
        temp[History_Size - 1] = strdup(input); 
        for (int i = 0; i < History_Size; i++) {
            history[i] = temp[i]; 
        }
    }
}

void printInputHistory() {
    for (int i = 0; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i]);
    }
}

int executeInput(char **args, int background) {
    int status;
    if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            char *home = getenv("HOME");
            if (home != NULL) {
                chdir(home);
            } else {
                fprintf(stderr, "cd: HOME not set\n");
            }
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
                return -1;  
            }
        }
        return 0;  
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
            return -1;
        }
        return 0;
    } else if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "history") == 0) {
        printInputHistory();
        return 0;
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("fork");
            return -1;
        } else {
            if (!background) {
                waitpid(pid, &status, WUNTRACED);
                return 0;
            } else {
                printf("%d\n", pid);
                return 0;
            }
        }
    }
    return 0; 
}

void executeInputConditionally(char *first_input[], char *second_input[]) {
    int first_result = executeInput(first_input, 0);
    if (first_result == 0) { 
        executeInput(second_input, 0);
    }
}

void executeInputWithPipe(char *first_input[], char *second_input[]) {
    int pipefd[2];
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]); 

        if (execvp(first_input[0], first_input) == -1) {
            perror("execvp first");
            exit(EXIT_FAILURE);
        }
    }

    pid2 = fork();
    if (pid2 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]); 

        if (execvp(second_input[0], second_input) == -1) {
            perror("execvp second");
            exit(EXIT_FAILURE);
        }
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void parseinput(char *input, char **args, char **first_input, char **second_input, int *background, int *has_pipe, int *has_and) {

    if (input[strlen(input) - 1] == '&') {
        *background = 1;
        input[strlen(input) - 1] = '\0'; 
    }

    int i = 0;
    char *token, *tmp = input;
    while ((token = strsep(&tmp, " ")) != NULL && i < Max_Arguments) {
        args[i++] = token;
    }
    args[i] = NULL;

    if (args[0] == NULL) {
        return;
    }

    *has_pipe = 0;
    *has_and = 0;
    int j = 0;
    for (j = 0; args[j] != NULL; j++) {
        first_input[j] = args[j];
        if (strcmp(args[j], "|") == 0) {
            first_input[j] = NULL; 
            int k = 0;
            j++; 
            while (args[j] != NULL) {
                second_input[k++] = args[j++];
            }
            second_input[k] = NULL;
            *has_pipe = 1;
            break;
        } else if (strcmp(args[j], "&&") == 0) {
            first_input[j] = NULL; 
            int k = 0;
            j++;  
            while (args[j] != NULL) {
                second_input[k++] = args[j++];
            }
            second_input[k] = NULL;
            *has_and = 1;
            break;
        }
    }
    if (!*has_pipe && !*has_and) {
        first_input[j] = NULL; 
    }
}

int main() {
    char input[Max_Input_Length];
    char *args[Max_Arguments];
    char *first_input[Max_Arguments];
    char *second_input[Max_Arguments];


    while (1) {
        int background = 0;
        int has_pipe = 0;
        int has_and = 0;
        printf("myshell> ");
        
        if (!fgets(input, Max_Input_Length, stdin)) {
            continue; 
        }

        input[strcspn(input, "\n")] = 0; 

        int end = strlen(input) - 1;
        while (end >= 0 && isspace((unsigned char) input[end])) {
            input[end] = '\0';
            end--;
        }

        if (strlen(input) > 0) {
            addInputToHistory(input); 
        }else{
            continue;
        }

        parseinput(input, args, first_input, second_input, &background, &has_pipe, &has_and);

        if (has_pipe) {
            executeInputWithPipe(first_input, second_input);
        } else if (has_and) {
            executeInputConditionally(first_input, second_input);
        } else {
            executeInput(first_input, background);
        }
    }
    return 0;
}