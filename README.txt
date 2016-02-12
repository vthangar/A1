************************************************************************************
* Assignment 1: Shell in C                                                         *
* Developed by: Vithursan Thangarasa                                               *
* Student ID:   0821795                                                            *
* Date:         2016-02-16                                                         *
* Professor:    Dr. Deborah Stacey                                                 *
* Course:       CIS*3110 - Operating Systems                                       *
* References:                                                                      *
*                                                                                  *
*    [1] http://www.cs.uleth.ca/~holzmann/C/system/pipeforkexec.html               *
*          - used for developing the pipe_handler and signal_handler               *
*    [2] http://www.cise.ufl.edu/class/cop4600sp12/dsslides/shell(6).pdf           *
*          - used for developing the file input/output redirection                 *
*    [3] http://www.sarathlakshman.com/2012/09/24/implementation-overview-of-      *
*        redirection-and-pipe-operators-in-shell/                                  *
*          - used as a resource for piping, redirection, and processes             *
*                                                                                  *
************************************************************************************

Overview
------------------------------------------------------------------------------------
In this assignment I implemented a simple UNIX shell program. A shell is simply 
a program that conveniently allows the user to run other programs. 


Installation
------------------------------------------------------------------------------------
Use make
It will create a binary called CIS3110sh
execute using ./CIS3110sh

Example:
make
./CIS3110sh


Working
------------------------------------------------------------------------------------

— terminates shell (‘exit’)
- command:
     with no arguments (’ls’)
     with arguments (’ls -l’)
     with/without arguments, executed in the background using & (xemacs &)
     with/without arguments, whose output is redirected to a file (ls -l > foo)
     with/without arguments, whose input is redirected from a file (sort < test file)
     with/without arguments, whose output is piped to the input of another command (ls -l | more)
- print environment variables (echo $PATH)
- history, history -c, history n 
- change directory (cd)

Not Working
------------------------------------------------------------------------------------
- export command
- reading in profile file on initialization

Known Issues
------------------------------------------------------------------------------------

1. When you use the ls command, sometimes this error comes up:

ls: cannot access : No such file or directory