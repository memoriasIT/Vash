#include "config.h"
#include "internalCommands.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


// Counts the number of argumments passed as a parameter
int numofargs(const char *command[]){
   int i = 0;
   while (command[i] != NULL){
        i++;
   }

   return i;
}


// Check if command is internal, if internal execute, else return -1 to main.c
int isInternal(const char *command[]) {
    // Use pointer to go through function map
    vash_functionmap * ptr;
    for (ptr = internal_funcs; ptr->name != NULL; ptr++) {
        if (!strcmp(ptr->name, command[0])){ 
          // Command found execute and return 0
          ptr->func(numofargs(command), command);
          
          return 0;
        }
        
    }
    // Command is not internal, return error
    return -1;
}


void cd(int nargs, const char* args[]){
    if (nargs == 1){
        // Only cd command specified, goto home folder
            chdir(getenv("HOME"));
    } else {
        // More data specified
        // chdir returns -1 if error ocurred and erno is set
        if (chdir(args[1]) == -1)
                fprintf(stderr, ERRORSTR" - Error in cd: %s\n", strerror(errno));
    }

}


void jobs(int nargs, const char* args[]) {
    print_job_list(job_list);

}


void bg (int nargs, const char* args[]) {
    // Critical section for jobs
    block_SIGCHLD();

    int jobpos;
    if (nargs != 1){
        jobpos = atoi(args[1]);
    } else {
        // Get to bg last job
        jobpos = 1;
    }
    
    job* job = get_item_bypos(job_list, jobpos);
    if (job != NULL) {
        // Change job state and send SIGCONT to job pgid
        job->state = BACKGROUND;
        killpg(job->pgid, SIGCONT);
    } else {
        fprintf(stderr, ERRORSTR" - Error while putting into bg: %s\n", strerror(errno));
    }

    // End the critical section for jobs
    unblock_SIGCHLD();


}


void fg (int nargs, const char* args[]) {
    int status, info; // for waitpid and analyze status
    enum status status_res; // to analyze_status

    // Critical structure due to jobs data structure
    block_SIGCHLD();

    // Set jobpos to 1 in case not specified
    int jobpos;
    if (nargs != 1){
        jobpos = atoi(args[1]);
    } else {
        jobpos = 1;
    }

    // Get job by position
    job* job = get_item_bypos(job_list, jobpos);
    
    if (job == NULL){
        // Error while getting item
        fprintf(stderr, ERRORSTR" - Error while putting job to fg: %s\n", strerror(errno));
    } else {
        // Item was sucessfully gotten
        
        // Set terminal to group pid and notify
        set_terminal(job->pgid);
        killpg(job->pgid, SIGCONT);

        pid_t pid = waitpid(job->pgid, &status, WUNTRACED);
        set_terminal(getpid());

        status_res = analyze_status(status, &info);
        if (status_res == SUSPENDED || status_res == SIGNALED){
            // Notify
            fprintf(stderr, ERRORSTR" - Command %s was suspended or signaled.\n", job->command);
            job->state = STOPPED;
        } else {
            // Job was exited notify and delete job
            fprintf(stderr, ERRORSTR" - Command %s was exited.\n", job->command);
            delete_job(job_list, job);
        }

        // End of critical section due to job structure
        unblock_SIGCHLD();
    }


    unblock_SIGCHLD();
}

