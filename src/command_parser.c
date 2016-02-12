//
//  command_parser.c
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-01-25.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//

#include "../include/a1_shell.h"
#include "../include/util.h"

/**
 * Function Name: shellPrompt
 * Description: Prints the prompt for the shell
 * Inputs: void
 * Returns: void
 */
static void shell_cmd_disp() {

    // Prints the prompt
    printf("\n%s > ", getcwd(currentDirectory, 1024));
}

/**
 * Function Name: change_directory
 * Description: Function to change directory
 * Inputs: char* tokens[]
 * Returns: int 
 */
static int change_directory(char* tokens[]) {

    // Go to HOME directory if user enters only 'cd'
    if (tokens[1] == NULL) {
        chdir(getenv("HOME"));
        return 1;
    }
    // Change to user specified directory if possible
    else{
        if (chdir(tokens[1]) == -1) {
            printf(" %s: no such directory\n", tokens[1]);
            return -1;
        }
    }
    return 0;
}

/**
 * Function Name: remove_char
 * Description: Function to remove first character of string
 * Inputs: char * char_buffer
 * Returns: char
 */
static char *remove_char(char *char_buffer) {
    char *str;
    
    if (strlen(char_buffer) == 0) {
        str = char_buffer;
    }else{
        str = char_buffer + 1;
    }
    return str;
}

/**
 * Function Name: manage_environ
 * Description: Function to manage environment variables
 * Inputs: char * tokens[], int option
 * Returns: int 
 */
static int manage_environ(char * tokens[], int option) {
    
    char *var_path;

    switch(option){
        // Case 0: Prints path for specified environment variable
        case 0:
            if(tokens[1] == NULL){
                printf("%s","not enought input arguments\n");
                return -1;
            }
            
            // Check if argument begins with '$' sign
            if(tokens[1][0] == '$') {
                var_path = remove_char(tokens[1]);
            } else {
                var_path = tokens[1];
            }
            
            // Check if the environment varirable is valid
            if(getenv(var_path) != NULL) {
                var_path = getenv(var_path);
            } else {
                printf("\n");
            }
            
            // Check if theres data in path
            if (var_path != NULL) {
                printf("%s\n", var_path);
            } else {
                printf("%s\n", tokens[1]);
            }
            
            break;
        // Case 1: Changing the Value of an Environment Variable
        case 1:
            // Check if enough input tokens
            if(tokens[1] == NULL){
                printf("%s","not enought input arguments\n");
                return -1;
            }
            
            if(getenv(tokens[1]) != NULL){
                printf("%s\n", tokens[1]);
                //putenv(tokens[1]);
            } else {
                printf("\n");
            }
            
            break;
    }
    return 0;
}

/**
 * Function Name: file_io_handler
 * Description: Manages input or ouput redirection
 * Inputs: char * tokens[], char* inputFile, char* outputFile, int option
 * Returns: void
 */
static void file_io_handler(char * tokens[], char* inputFile, char* outputFile, int option){
    
    int err = -1;
    
    int f_descriptor; // between 0 and 19, describing the output or input file
    
    // Checks if child process could be created
    if((pid=fork()) == -1) {
        return;
    }

    if(pid == 0){
        // Option 0: redirects the output
        if (option == 0) {
            // Opens file truncating it at 0, for WRITE only
            f_descriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            // Replaces the STDOUT with appropriate file
            dup2(f_descriptor, STDOUT_FILENO);
            close(f_descriptor);
        // Option 1: redirects the input and output
        } else if (option == 1) {
            // Opens file for READ only
            f_descriptor = open(inputFile, O_RDONLY, 0600);
            // Replaces the STDIN with appropriate file
            dup2(f_descriptor, STDIN_FILENO);
            close(f_descriptor);

            f_descriptor = open(outputFile, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            dup2(f_descriptor, STDOUT_FILENO);
            close(f_descriptor);
        }
        
        setenv("parent", getcwd(currentDirectory, 1024), 1);
        
        if (execvp(tokens[0],tokens) == err) {
            kill(getpid(), SIGTERM);
        }
    }
    waitpid(pid, NULL, 0);
}

/**
 * Function Name: program_launcher
 * Description: Launches program in background or foreground
 * Inputs: char **tokens, int background
 * Returns: void
 */
static void program_launcher(char **tokens, int background){
    int err = -1;
    
    if((pid=fork()) == -1) {
        return;
    }

    // Related to child process
    if(pid == 0){
        // Sets the child to ignore SIGINT signals
        signal(SIGINT, SIG_IGN);
        
        setenv("parent", getcwd(currentDirectory, 1024), 1);
        
        // End process if unknown commands are launched
        if (execvp(tokens[0], tokens) == err) {
            printf("command not found");
            kill(getpid(), SIGTERM);
        }
    }
    
    // Related to parent process
    if (background == 0) {
        waitpid(pid, NULL, 0);
    } else {
        // Skips call to wait, SIGCHILD handler takes care of 
        // child's return values
        printf("process created with PID: %d\n",pid);
    }
}

/**
 * Function Name: command_handler
 * Description: Parses the arguments of the command entered by user
 * Inputs: char * tokens[], char * hist[], int current
 * Returns: int
 */
static int command_handler(char * tokens[], char * hist[], int current) {
    
    int i = 0;
    int j = 0;
    
    int f_descriptor;
    int std_out;
    int hist_len = current;

    int background = 0;
    
    char *new_args[256];
    
    // Look for special characters and create new array of tokens
    while ( tokens[j] != NULL){
        if ( (strcmp(tokens[j], ">") == 0) || (strcmp(tokens[j], "<") == 0) || (strcmp(tokens[j], "&") == 0)){
            break;
        }
        new_args[j] = tokens[j];
        j++;
    }
    
    // 'exit' -> quits the shell
    if(strcmp(tokens[0], "exit") == 0) {
        printf("logout\n\n");
        printf("[Process Completed]\n");
        exit(0);
    }
    // 'pwd' -> prints the current directory
    else if (strcmp(tokens[0], "pwd") == 0){
        if (tokens[j] != NULL){
            // Checks if file output requested
            if ( (strcmp(tokens[j],">") == 0) && (tokens[j+1] != NULL) ){
                f_descriptor = open(tokens[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                // Replaces the STDOUT with appropriate file
                std_out = dup(STDOUT_FILENO); 	// Makes a copy of STDOUT
                dup2(f_descriptor, STDOUT_FILENO);
                close(f_descriptor);
                printf("%s\n", getcwd(currentDirectory, 1024));
                dup2(std_out, STDOUT_FILENO);
            }
        }else{
            printf("%s\n", getcwd(currentDirectory, 1024));
        }
    }
    // 'clear' -> clears screen
    else if (strcmp(tokens[0],"clear") == 0) {
        system("clear");
    }
    // 'cd' -> changes the directory
    else if (strcmp(tokens[0],"cd") == 0) {
        change_directory(tokens);
    }
    // 'echo' -> print environment variables
    else if (strcmp(tokens[0], "echo") == 0) {
        manage_environ(tokens, 0);
    }
    // 'export' -> undefine environment variables
    else if (strcmp(tokens[0], "export") == 0) {
        manage_environ(tokens, 1);
    }
    // 'history' -> displays history of input commands
    else if (strcmp(tokens[0], "history") == 0) {
        int k = 0;
        int num_index = 0;
        
        while(tokens[k] != NULL){
            k++;
            num_index++;
        }
        
        // Prints entire history
        if (num_index == 1) {
            history(hist, current, hist_len);
        }else{
            // Clear history
            if (strcmp(tokens[1], "-c") == 0) {
                clear_history(hist, current);
            // Prints history up to user specified entries
            } else if ((atoi(tokens[1]) > 0) && ((atoi(tokens[1])) <= HISTORY_COUNT)) {
                history(hist, current, (atoi(tokens[1])));
            } else {
                printf("invalid parameters\n");
            }
        }
    }
    else{
        // Checks for input/output direction, piped/background execution
        while (tokens[i] != NULL && background == 0) {
            // Checks if background execution
            if (strcmp(tokens[i], "&") == 0) {
                background = 1;
            // Checks for piping i.e. '|'
            }else if (strcmp(tokens[i], "|") == 0) {
                pipe_handler(tokens);
                return 1;
            // Checks for input redirection i.e. '<'
            }else if (strcmp(tokens[i], "<") == 0) {
                file_io_handler(new_args, tokens[i+1], NULL, 1);
                return 1;
            }
            // Checks for output redirection i.e. '>'
            else if (strcmp(tokens[i], ">") == 0) {
                if (tokens[i+1] == NULL) {
                    printf("Not enough input arguments\n");
                    return -1;
                }
                file_io_handler(new_args, NULL, tokens[i+1], 0);
                return 1;
            }
            i++;
        }
        // Launches program in the background
        program_launcher(new_args, background);
    }
    return 1;
}

/**
 * Function Name: command_parser
 * Description: Parases the command at command line
 * Inputs: void
 * Returns: void
 */
void command_parser() {
    
    char user_input[MAXLEN];
    char * tokens[MAXTOKENS];
    int num_tokens;
    char *hist[HISTORY_COUNT];
    
    int i, current = 0;
    no_redisplay = 0; 	// Prevents shell being reprinted
    
    pid = -10;
    
    for (i = 0; i < HISTORY_COUNT; i++) {
        hist[i] = NULL;
    }
  
    // Prints the shell prompt and user command is read
    while(TRUE){

        // Prints shell if needed
        if (no_redisplay == 0) {
            shell_cmd_disp();
        }
        
        no_redisplay = 0;
        
        // Emptys the user_input buffer
        memset (user_input, '\0', MAXLEN);
        
        // Waits for user_input
        fgets (user_input, MAXLEN, stdin);
        
	// Free history at current index
        free(hist[current]);
        
        hist[current] = strdup(user_input);
        current = (current + 1) % HISTORY_COUNT;
        
        // Execte loop if nothing is written
        if((tokens[0] = strtok(user_input," \n\t")) == NULL) {
            continue;
        }
        
        // Read all tokens from input and pass it to command_handler
        num_tokens = 1;
        while((tokens[num_tokens] = strtok(NULL, " \n\t")) != NULL) {
            num_tokens++;
        }
        
        command_handler(tokens, hist, current);     // Parses the tokens
        
    }
    
    clear_history(hist);
}
