//
//  signal_handler.c
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-01-16.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//

#include "../include/a1_shell.h"

/**
 * Function Name: child_signal_handler
 * Description: Handles signal for SIGCHILD
 * Inputs: int p
 * Returns: void
 */
void child_signal_handler(int p){
    // Wait for dead processes, use WNOHANG to ensure no blocking by signal handler
    // if child cleaned up
    while (waitpid(-1, NULL, WNOHANG) > 0) {
    }
    printf("\n");
}

/**
 * Function Name: int_signal_handler
 * Description: Handles signal for SIGINT
 * Inputs: int p
 * Returns: void
 */
void int_signal_handler(int p){
    // Child process receives a SIGTERM signal
    if (kill(pid,SIGTERM) == 0){
        no_reprint_prmpt = 1;
    }else{
        printf("\n");
    }
}
