//
//  init_shell.c
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-01-16.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//


#include "../include/a1_shell.h"
#include "../include/util.h"

static pid_t SHELL_PID;
static pid_t SHELL_PROCESS_GROUP_ID;
static int SHELL_INTERACTIVE;
static struct termios SHELL_TERMINAL_MODES;

struct sigaction child_action;
struct sigaction int_action;


/**
 * Function Name: init_shell
 * Description: initalizes the shell in order it to be used
 * Inputs: void
 * Returns: void
 */
void init_shell() {
    // Check if shell is running interactively
    SHELL_PID = getpid();
    // Shell is interactive if STDIN is the terminal
    SHELL_INTERACTIVE = isatty(STDIN_FILENO);
    
    if (SHELL_INTERACTIVE) {
        while (tcgetpgrp(STDIN_FILENO) != (SHELL_PROCESS_GROUP_ID = getpgrp()))
            kill(SHELL_PID, SIGTTIN);
       
        child_action.sa_handler = child_signal_handler;
        int_action.sa_handler = int_signal_handler;
        
        sigaction(SIGCHLD, &child_action, 0);
        sigaction(SIGINT, &int_action, 0);
        
        // Placed in own process group
        setpgid(SHELL_PID, SHELL_PID); // Make shell process -> new process group leader
        SHELL_PROCESS_GROUP_ID = getpgrp();
        if (SHELL_PID != SHELL_PROCESS_GROUP_ID) {
            exit(EXIT_FAILURE);
        }
        // Get control of terminal
        tcsetpgrp(STDIN_FILENO, SHELL_PROCESS_GROUP_ID);
        tcgetattr(STDIN_FILENO, &SHELL_TERMINAL_MODES);
        
        // Get current directory (needed for different functions)
        currentDirectory = (char*) calloc(1024, sizeof(char));
    } else {
        exit(EXIT_FAILURE);
    }
}

/**
 * Function Name: welcome_screen
 * Description: Creates a welcome screen initially
 * Inputs: void
 * Returns: void
 */
void welcome_screen() {
    printf("\n\t==============================================\n");
    printf("\t               CIS*3110 - C Shell\n");
    printf("\t       Developed by: Vithursan Thangarasa\n");
    printf("\t               Date: 2016-02-12            \n");
    printf("\t==============================================\n");
    printf("\n\n");
}
