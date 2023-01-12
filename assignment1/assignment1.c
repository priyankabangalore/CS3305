#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t child_1;
    pid_t child_2;
    pid_t child_1_1;
    int status;

    pid_t parent_PID = getppid();

    printf("\nparent (PID %d): process started \n", parent_PID);

    // creates child process 1
    printf("parent (PID %d): forking child_1 \n", parent_PID);
    child_1 = fork();
    wait(NULL);

    // if child 1 forking is successful
    if(child_1 == 0)
    {
        pid_t child_1_PID = getpid();
        printf("parent (PID %d): fork successful for child_1 (PID %d) \n", parent_PID, child_1_PID);

        printf("parent (PID %d): waiting for child_1 to complete (PID %d) \n", parent_PID, child_1_PID);

        // creates child process 1.1
        printf("child_1 (PID %d): forking child_1.1 \n", child_1_PID);
        child_1_1 = fork();
        wait(NULL);

        // if child 1.1 forking is successful
        if(child_1_1 == 0){
            pid_t child_1_1_PID = getpid();
            printf("child_1 (PID %d): fork successful for child_1.1 (PID %d) \n", child_1_PID, child_1_1_PID);
          
            printf("child_1 (PID %d): waiting for child_1.1 (PID %d) to complete \n", child_1_PID, child_1_1_PID);

            // concatenating
            char ID_1_1[7];
            sprintf(ID_1_1, "%d", child_1_1_PID);
            strcat(ID_1_1, " for child 1.1");

            // external program call
            printf("child_1.1 (PID %d): calling an external program [external_program.out] \n", child_1_1_PID);
            status = execl("external_program.out", ID_1_1, NULL);
            if(status < 0){
                printf("child_1.1 (PID %d): external call unsuccessful \n", child_1_1_PID);
            }
        }
        else if(child_1_1 < 0){
            printf("child_1 (PID %d): fork unsuccessful for child 1.1 \n", child_1_PID);
        }
        printf("child_1 (PID %d): completed \n", child_1_PID);

        // creates child process 2
        printf("parent (PID %d): forking child_2 \n", parent_PID);
        child_2 = fork();
        wait(NULL);

        // if child 2 forking is successful
        if(child_2 == 0){
            pid_t child_2_PID = getpid();
            printf("parent (PID %d): fork successful for child_2 (PID %d) \n", parent_PID, child_2_PID);
            printf("parent (PID %d): waiting for child_2 (PID %d) to complete \n", parent_PID, child_2_PID);

            // concatenating
            char ID_2[7];
            sprintf(ID_2, "%d", child_2_PID);
            strcat(ID_2, " for child 2");

            // external program call
            printf("child_2 (PID %d): calling an external program [external_program.out] \n", child_2_PID);
            status = execl("external_program.out", ID_2, NULL);
            if(status < 0){
                printf("child_2 (PID %d): external call unsuccessful \n", child_2_PID);
            }
        }
        else if(child_2 < 0){
            printf("parent (PID %d): fork unsuccessful for child 2 \n", parent_PID);
        }
        // parent process terminates
        printf("parent (PID %d): completed \n", parent_PID);
    }
    else if(child_1 < 0){
        printf("parent (PID %d): fork unsuccessful for child_1 \n", parent_PID);
    }

    return 0;
}
