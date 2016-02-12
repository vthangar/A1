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
static void shellPrompt() {

    char hostn[1204] = "";

    gethostname(hostn, sizeof(hostn));
    // Prints the prompt
    printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
}

/**
 * Function Name: change_directory
 * Description: Function to change directory
 * Inputs: char* args[]
 * Returns: int 
 */
static int change_directory(char* args[]) {

    // Go to HOME directory if user enters only 'cd'
    if (args[1] == NULL) {
        chdir(getenv("HOME"));
        return 1;
    }
    // Change to user specified directory if possible
    else{
        if (chdir(args[1]) == -1) {
            printf(" %s: no such directory\n", args[1]);
            return -1;
        }
    }
    return 0;
}

/**
 * Function Name: manage_environ
 * Description: Function to manage environment variables
 * Inputs: char * args[], int option
 * Returns: int 
 */
static int manage_environ(char * args[], int option) {
    char **env_aux;
    switch(option){
        // Case 0: Prints the environment variables with their values
        case 0:
            for(env_aux = environ; *env_aux != 0; env_aux ++){
                printf("%s\n", *env_aux);
            }
            break;
        // Case 1: Sets an environment variable to a value
        case 1:
            if((args[1] == NULL) && args[2] == NULL){
                printf("%s","Not enought input arguments\n");
                return -1;
            }
            
            // Use different output for new and overwritten variables
            if(getenv(args[1]) != NULL){
                printf("%s", "The variable has been overwritten\n");
            }else{
                printf("%s", "The variable has been created\n");
            }
            
            // If no value is specified for the variable, set it to ""
            if (args[2] == NULL){
                setenv(args[1], "", 1);
            // Set the variable to the given value
            }else{
                setenv(args[1], args[2], 1);
            }
            break;
        // Case 2: Deletes an environment variable
        case 2:
            if(args[1] == NULL){
                printf("%s","Not enought input arguments\n");
                return -1;
            }
            if(getenv(args[1]) != NULL){
                unsetenv(args[1]);
                printf("%s", "The variable has been erased\n");
            }else{
                printf("%s", "The variable does not exist\n");
            }
            break;       
    }
    return 0;
}

/**
 * Function Name: file_io_handler
 * Description: Manages input or ouput redirection
 * Inputs: char * args[], char* inputFile, char* outputFile, int option
 * Returns: void
 */
static void file_io_handler(char * args[], char* inputFile, char* outputFile, int option){
    
    int err = -1;
    
    int f_descriptor; // between 0 and 19, describing the output or input file
    
    // Checks if child process could be created
    if((pid=fork()) == -1){
        printf("Child process could not be created\n");
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
        
        if (execvp(args[0],args) == err) {
            printf("err");
            kill(getpid(), SIGTERM);
        }
    }
    waitpid(pid, NULL, 0);
}

/**
 * Function Name: program_launcher
 * Description: Launches program in background or foreground
 * Inputs: char **args, int background
 * Returns: void
 */
static void program_launcher(char **args, int background){
    int err = -1;
    
    if((pid=fork()) == -1) {
        printf("Child process could not be created\n");
        return;
    }

    // Related to child process
    if(pid == 0){
        // Sets the child to ignore SIGINT signals
        signal(SIGINT, SIG_IGN);
        
        setenv("parent", getcwd(currentDirectory, 1024), 1);
        
        // End process if unknown commands are launched
        if (execvp(args[0], args) == err) {
            printf("Command not found");
            kill(getpid(), SIGTERM);
        }
    }
    
    // Related to parent process
    if (background == 0) {
        waitpid(pid, NULL, 0);
    } else {
        // Skips call to wait, SIGCHILD handler takes care of 
        // child's return values
        printf("Process created with PID: %d\n",pid);
    }
}

/**
 * Function Name: command_handler
 * Description: Parses the arguments of the command entered by user
 * Inputs: char * args[], char * hist[], int current
 * Returns: int
 */
static int command_handler(char * args[], char * hist[], int current) {
    
    int i, j = 0;
    
    int f_descriptor;
    int standardOut;
    int hist_len = current;

    int background = 0;
    
    char *args_aux[256];
    
    // Look for special characters and create new array of args
    while ( args[j] != NULL){
        if ( (strcmp(args[j], ">") == 0) || (strcmp(args[j], "<") == 0) || (strcmp(args[j], "&") == 0)){
            break;
        }
        args_aux[j] = args[j];
        j++;
    }
    
    // 'exit' -> quits the shell
    if(strcmp(args[0], "exit") == 0) {
        printf("logout\n\n");
        printf("[Process Completed]\n");
        exit(0);
    }
    // 'pwd' -> prints the current directory
    else if (strcmp(args[0], "pwd") == 0){
        if (args[j] != NULL){
            // Checks if file output requested
            if ( (strcmp(args[j],">") == 0) && (args[j+1] != NULL) ){
                f_descriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                // Replaces the STDOUT with appropriate file
                standardOut = dup(STDOUT_FILENO); 	// Makes a copy of STDOUT
                dup2(f_descriptor, STDOUT_FILENO);
                close(f_descriptor);
                printf("%s\n", getcwd(currentDirectory, 1024));
                dup2(standardOut, STDOUT_FILENO);
            }
        }else{
            printf("%s\n", getcwd(currentDirectory, 1024));
        }
    }
    // 'clear' -> clears screen
    else if (strcmp(args[0],"clear") == 0) {
        system("clear");
    }
    // 'cd' -> changes the directory
    else if (strcmp(args[0],"cd") == 0) {
        change_directory(args);
    }
    // 'environ' -> lists the environment variables
    else if (strcmp(args[0],"environ") == 0) {
        if (args[j] != NULL){
            // Checks if file output requested
            if ( (strcmp(args[j], ">") == 0) && (args[j+1] != NULL) ){
                f_descriptor = open(args[j+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                // Replaces the STDOUT with appropriate file
                standardOut = dup(STDOUT_FILENO); 	// Makes a copy of STDOUT
                dup2(f_descriptor, STDOUT_FILENO);
                close(f_descriptor);
                manage_environ(args, 0);
                dup2(standardOut, STDOUT_FILENO);
            }
        } else {
            manage_environ(args,0);
        }
    }
    // 'setenv' -> set environment variables
    else if (strcmp(args[0], "setenv") == 0) {
        manage_environ(args, 1);
    }
    // 'unsetenv' -> undefine environment variables
    else if (strcmp(args[0], "unsetenv") == 0) {
        manage_environ(args, 2);
    }
    // 'history' -> displays history of input commands
    else if (strcmp(args[0], "history") == 0) {
        int k = 0;
        int num_index = 0;
        
        while(args[k] != NULL){
            k++;
            num_index++;
        }
        
        // Prints entire history
        if (num_index == 1) {
            history(hist, current, hist_len);
        }else{
            // Clear history
            if (strcmp(args[1], "-c") == 0) {
                clear_history(hist, current);
            // Prints history up to user specified entries
            } else if ((atoi(args[1]) > 0) && ((atoi(args[1])) <= HISTORY_COUNT)) {
                history(hist, current, (atoi(args[1])));
            } else {
                printf("Invalid paramters\n");
            }
        }
    }
    else{
        // Checks for input/output direction, piped/background execution
        while (args[i] != NULL && background == 0) {
            // Checks if background execution
            if (strcmp(args[i], "&") == 0) {
                background = 1;
            // Checks for piping i.e. '|'
            }else if (strcmp(args[i], "|") == 0) {
                pipe_handler(args);
                return 1;
            // Checks for input redirection i.e. '<'
            }else if (strcmp(args[i], "<") == 0) {
                file_io_handler(args_aux, args[i+1], NULL, 1);
                return 1;
            }
            // Checks for output redirection i.e. '>'
            else if (strcmp(args[i], ">") == 0) {
                if (args[i+1] == NULL) {
                    printf("Not enough input arguments\n");
                    return -1;
                }
                file_io_handler(args_aux, NULL, args[i+1], 0);
                return 1;
            }
            i++;
        }
        // Launches program in the background
        program_launcher(args_aux, background);
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
    no_reprint_prmpt = 0; 	// Prevents shell being reprinted
    
    pid = -10;
    
    for (i = 0; i < HISTORY_COUNT; i++) {
        hist[i] = NULL;
    }
  
    // Prints the shell prompt and user command is read
    while(TRUE){

        // Prints shell if needed
        if (no_reprint_prmpt == 0) {
            shellPrompt();
        }
        
        no_reprint_prmpt = 0;
        
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
