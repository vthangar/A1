//
//  a1_shell.h
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-01-16.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//

#ifndef a1_shell_h
#define a1_shell_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>

#define MAXTOKENS 256 // Max number of tokens for a command
#define MAXLEN 1024 // Max number of characters from user input
#define HISTORY_COUNT 40

#define TRUE 1
#define FALSE 0

pid_t pid;
int no_redisplay;

/**
 * Implicit Declarations
 */
void init_shell();
void welcome_screen();
void command_parser();
void pipe_handler();
int history();
int clear_history();
void sort_file();

/**
 * Signal Handlers
 */
void child_signal_handler(int p);
void int_signal_handler(int p);

#endif /* a1_shell_h */
