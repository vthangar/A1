//
//  pipe_handler.c
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-01-22.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//

#include "../include/a1_shell.h"

/**
 * Function Name: pipe_handler
 * Description: Manages the pipe handling
 * Inputs: char * args[]
 * Returns: void
 */
void pipe_handler(char * args[]){
    // File descriptors
    int file_descriptor_odd[2];
    int file_descriptor_even[2];
    
    int num_cmds = 0;
    
    char *command[256];
    
    pid_t pid;
    
    int error = -1;
    int end = 0;
    
    // Loop variables
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    
    // Get number of commands seperated by '|'
    while (args[l] != NULL){

        if (strcmp(args[l], "|") == 0){
            num_cmds++;
        }
        l++;
    }
    num_cmds++;
    
    // Configure for each command between pipes and replace STDOUT/STDIN
    while (args[j] != NULL && end != 1){

        k = 0;
        // Store each command in array
        while (strcmp(args[j],"|") != 0){
            command[k] = args[j];
            j++;
            if (args[j] == NULL){
                // Terminates the while loop
                end = 1;
                k++;
                break;
            }
            k++;
        }
        // Last index is set to NULL
        command[k] = NULL;
        j++;
        
        // Different file descripters for pipe inputs and outputs.
        if (i % 2 != 0){
            pipe(file_descriptor_odd);
        }else{
            pipe(file_descriptor_even);
        }
        
        pid=fork();
        
        if(pid==-1){
            if (i != num_cmds - 1){
                if (i % 2 != 0){
                    close(file_descriptor_odd[1]);
                }else{
                    close(file_descriptor_even[1]);
                }
            }
            // Return if the child process cannot be created
            return;
        }
        if(pid==0){
            if (i == 0){
                dup2(file_descriptor_even[1], STDOUT_FILENO);
            }
            // Replace STDIN if last command, leave STDOUT as is
            else if (i == num_cmds - 1){
                if (num_cmds % 2 != 0){
                    dup2(file_descriptor_odd[0], STDIN_FILENO);
                }else{
                    dup2(file_descriptor_even[0], STDIN_FILENO);
                }
            // Use two pipes (i.e. one for input and another for output) 
            // if middle command
            }else{
                if (i % 2 != 0){
                    dup2(file_descriptor_even[0], STDIN_FILENO);
                    dup2(file_descriptor_odd[1], STDOUT_FILENO);
                }else{
                    dup2(file_descriptor_odd[0], STDIN_FILENO);
                    dup2(file_descriptor_even[1], STDOUT_FILENO);
                }
            }
            
            if (execvp(command[0],command) == error){
                kill(getpid(), SIGTERM);
            }
        }
        
        // Related to Parent -> close all file descriptors
        if (i == 0){
            close(file_descriptor_even[1]);
        }
        else if (i == num_cmds - 1){
            if (num_cmds % 2 != 0){
                close(file_descriptor_odd[0]);
            }else{
                close(file_descriptor_even[0]);
            }
        }else{
            if (i % 2 != 0){
                close(file_descriptor_even[0]);
                close(file_descriptor_odd[1]);
            }else{
                close(file_descriptor_odd[0]);
                close(file_descriptor_even[1]);
            }
        }
        
        waitpid(pid, NULL, 0);
        
        i++;
    }
}
