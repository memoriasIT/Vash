#include "job_control.h"
#include "vash_globals.h"

#include <sys/types.h>  // pid_t
#include <errno.h>      // Signal exit status


void vash_signalHandler(int signal) {
   /* int exitStatus;         // Status at exit, from waitpid()
    int toDelete;           // Bool True if job needs to be deleted
   
    // analyze_status()
    enum status status_res; // Status result
    int info;               // changed in analyze_status
    
    pid_t temppid;
    job* job = job_list->next;

    // Advance linearly untill object is NULL
    while (job != NULL){
        toDelete = 0;

        temppid = waitpid(job->pgid, &exitStatus, WNOHANG | WUNTRACED);
        if (temppid == job->pgid){
            status_res = analyze_status(exitStatus, &info);
            
            // if exited, then delete, call f delete_job with job to be deleted
            // if stoped notify, job->state = STOPPED
            // if continued, 
            
        }

        // advance job->next
    }
*/
}
