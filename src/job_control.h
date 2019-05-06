#ifndef _JOB_CONTROL_H
#define _JOB_CONTROL_H


// [-------------------------------- IMPORTS ---------------------------------]
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// [--------------------------------- ENUMS ----------------------------------]
enum status { SUSPENDED, SIGNALED, EXITED};
enum job_state { FOREGROUND, BACKGROUND, STOPPED };
static char* status_strings[] = { "Suspended","Signaled","Exited" };
static char* state_strings[] = { "Foreground","Background","Stopped" };


// [------------------------- JOB TYPE FOR JOB LIST --------------------------]
typedef struct job_ {
    pid_t pgid;             // Group ID
    char *command;          // Program name
    enum job_state state;   // State of the job
    struct job_ *next;      // Next job in list
} job;


// +--------------------------------------------------------------------------+
// |                            PUBLIC FUNCTIONS                              |
// +--------------------------------------------------------------------------+

void get_command(char inputBuffer[], int size, char *args[],int *background);

job * new_job(pid_t pid, const char * command, enum job_state state);

void add_job (job * list, job * item);

int delete_job(job * list, job * item);

job * get_item_bypid  (job * list, pid_t pid);

job * get_item_bypos( job * list, int n);

enum status analyze_status(int status, int *info);

// +--------------------------------------------------------------------------+
// |                           PRIVATE FUNCTIONS                              |
// |                          (Use macros bellow)                             |
// +--------------------------------------------------------------------------+

void print_item(job * item);

void print_list(job * list, void (*print)(job *));

void terminal_signals(void (*func) (int));

void block_signal(int signal, int block);


// +--------------------------------------------------------------------------+
// |                            PUBLIC MACROS                                 |
// +--------------------------------------------------------------------------+

// JOB LIST
//      Number of jobs in the list
#define list_size(list) 	 list->pgid 
//      Return 1 (true) if the list is empty
#define empty_list(list) 	 !(list->pgid)
//      Make new list, name must be const char*
#define new_list(name) 		 new_job(0,name,FOREGROUND) 
#define print_job_list(list) 	 print_list(list, print_item)

// SIGNALS
#define restore_terminal_signals()   terminal_signals(SIG_DFL)
#define ignore_terminal_signals() terminal_signals(SIG_IGN)

#define set_terminal(pid)        tcsetpgrp (STDIN_FILENO,pid)
#define new_process_group(pid)   setpgid (pid, pid)

#define block_SIGCHLD()   	 block_signal(SIGCHLD, 1)
#define unblock_SIGCHLD() 	 block_signal(SIGCHLD, 0)

// DEBUG MACRO----------------------------------------------------
// to debug integer i, use:    debug(i,%d);
// it will print out:  current line number, 
//                     function name and file name, 
//                     and also variable name, value and type
#define debug(x,fmt) fprintf(stderr,"\"%s\":%u:%s(): --> %s= " #fmt " (%s)\n", __FILE__, __LINE__, __FUNCTION__, #x, x, #fmt)

#endif // _JOB_CONTROL_H not defined
