//
//  main.c
//  A1_Shell
//
//  Created by Vithursan Thangarasa on 2016-01-16.
//  Copyright © 2016 Vithursan Thangarasa. All rights reserved.
//

#include "../include/a1_shell.h"
#include "../include/util.h"

int main(int argc, char *argv[]) {
    
    init_shell();     // Initalizes the shell
    welcome_screen(); // Displays the welcome screen
    
    setenv("shell", getcwd(currentDirectory, 1024), 1);
    
    command_parser();
    
    return (0);
}
