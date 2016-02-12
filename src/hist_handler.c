//
//  hist_handler.c
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-02-08.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//

#include "../include/a1_shell.h"

/**
 * Function Name: history
 * Description: Prints the history of user command inputs
 * Inputs: char *hist[], int current, int tot_len
 * Returns: int
 */
int history(char *hist[], int current, int tot_len) {
    
    if(tot_len <= current) {
        int k = current - tot_len;
        for(int j = k; j <= current; j++) {
            if (hist[j] != NULL) {
                printf("    %d %s", j+1, hist[j]);
            }
        }
    } else {
        for(int j = 0; j <= current; j++) {
            if (hist[j] != NULL) {
                printf("    %d %s", j+1, hist[j]);
            }
        }
    }
    
    return (0);
}

/**
 * Function Name: clear_history
 * Description: clears history list
 * Inputs: char *hist[]
 * Returns: int
 */
int clear_history(char *hist[]) {
    int i;
    
    for (i = 0; i < HISTORY_COUNT; i++) {
        free(hist[i]);
        hist[i] = NULL;
    }
    
    return (0);
}
