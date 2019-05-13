#include "job_control.h"
#include "vash_globals.h"
#include "config.h"

#include <sys/types.h>  // pid_t
#include <errno.h>      // Signal exit status


void vash_signalHandler(int signal) {
    int status;             // Status from waitpid()
    enum status status_res; // Status result
    int info;               // changed in analyze_status
    
    pid_t temppid;          // Temporal pid 
    int counter = 0;        // counter for job list
    job* job = get_item_bypos(job_list, 1); // First job used for traversing

    // Advance linearly untill object is NULL
    while (job != NULL){
        // Job in background or stopped 
        if (job->state != FOREGROUND){
            // Wait for the job pgid
            temppid = waitpid(job->pgid, &status, WNOHANG | WUNTRACED);
       
            // If it's the pid that caused the signal analyze
            if (temppid == job->pgid) {
                status_res = analyze_status(status, &info);
            
                //--------------------------
                // ANALISIS OF STATUS RES
                //--------------------------
                if (status_res == EXITED || status_res == SIGNALED){
                    // JOB IS EITHER EXITED OR SIGNALED 
                    
                    // Anounce end
                    fprintf(stderr, ERRORSTR" - Job '%s' was finished.\n", job->command);
                    
                    // Change state and delete
                    job->state = STOPPED;
                    delete_job(job_list, job); // from job_control.h
               
                    // Anounce end

                } else if (status_res == SUSPENDED){
                    // JOB IS EITHER SUSPENDED
                    // Change state to stop and announce
                    job->state = STOPPED;

                    // Anounce suspension
                    fprintf(stderr, ERRORSTR" - Job '%s' was suspended.\n", job->command);
                } // end of status res analysis
            
            } // temppid did not cause the signal
            
        } // end of background job

        // After routine or foreground:
        // Advance counter and get next job
        ++counter; 
        job = get_item_bypos(job_list, counter);

    } //end of while

} // end of signal handler function
