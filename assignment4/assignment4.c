#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(){

    FILE *f;

    char line[40];
    int line_length = sizeof(line);

    int test_case = 0;

    char *split[32];

    // supplying rr_input.txt to the program
    f = fopen("rr_input.txt", "r");

    if(f == 0 || f == NULL)
    {   
        perror("Cannot open rr_input.txt, please try again.\n");
        exit(-1);
    }
    
    while (fgets(line, line_length, f))
    {
        int quantum = 0;
        int t = 0;

        test_case = test_case + 1;
        printf("Test Case #%d: %s\n", test_case, line);

        // parsing the input file correctly
        char *token = strtok(line, " ");
        
        while(token != NULL) {
            split[t++] = token;
            token = strtok(NULL, " ");
        }

        quantum = atoi(split[t-1]);

        int num_processes = (t-1) / 3;

        int arrivals[num_processes];
        int bursts[num_processes];
        int ctr = 0;
        
        // process names are at 0 3 6 etc.
        // arrival times are at 1 4 7 etc.
        // burst times are at 2 5 8 etc.
        for(int i = 0; i < num_processes; i++){
            arrivals[i] = atoi(split[ctr+1]);
            bursts[i] = atoi(split[ctr+2]);
            ctr = ctr + 3;
        }

        // printing the number of processes and quantum for every test case
        printf("Number of Processes: %d, Quantum: %d\n", num_processes, quantum);
        printf("Process Scheduling Started:\n");

        int time = 0;
        int arrived = 0;
        int sum = 0;
        int turnaround = 0;
        int waiting = 0;
        int process;
        int burst;
        int arrival;
        int completed;
        int burst_size = num_processes;
        int avg_waiting = 0;
        int avg_turnaround = 0;
        int x = 0;

        for(int i = 0; i < num_processes; i++){
            sum = sum + bursts[i];
        }

        // ensures the time is never more than the sum of all burst times
        while(time <= sum){
            printf("CPU Time %d: ", time);
            // prints the process once the CPU time = the arrival time
            if(time == arrivals[x]){
                process = (x+1);
                burst = bursts[x];
                arrival = arrivals[x];
                printf("[p%d Arrived] ", process);
                arrived = 1;
            }
            // if the burst time is less than the quantum time, the process is completed and its info is removed
            // from the bursts and arrivals lists
            if(burst <= quantum){
                for(int i = 0; i <= burst; i++){
                    printf("p%d [%d/%d]\n", process, i, burst);
                    waiting = time - arrival - burst;
                    turnaround = waiting + burst;
                    if(i == burst){
                        printf("Process p%d completed with Turn Around Time: %d, Waiting Time: %d\n", process, turnaround, waiting);
                        for(int y=process-1; y<sizeof(bursts)-1; y++)
                        {
                            bursts[y] = bursts[y + 1];
                        }
                        for(int y=process-1; y<sizeof(bursts)-1; y++)
                        {
                            arrivals[y] = arrivals[y + 1];
                        }
                        burst_size = burst_size - 1;
                        // if the burst list is empty, time is set to be above the sum of all burst times to exit the while loop
                        if(burst_size == 0){
                            time = sum + 1;
                        }
                    }
                }
            }
            // if the burst time is more than the quantum time, after the quantum amount is completed,
            // the process' burst time is sent to the end of the list with an updated burst time
            else if(burst > quantum){
                for(int i = 1; i <= quantum; i++){
                    printf("p%d [%d/%d]\n", process, i, burst);
                }
                waiting = time - arrival - burst;
                turnaround = waiting + burst;
                completed = quantum;

                for(int r=process-1; r<sizeof(bursts)-1; r++)
                {
                    bursts[r] = bursts[r + 1];
                }
                bursts[num_processes-1] = (burst-quantum);
            }
            // if time is 0 and no process has an arrival time of 0, "None" is printed
            if(time == 0 && arrived == 0){
                printf("None\n");
            }
            x = x + 1;
            if(x > num_processes){
                time = sum + 1;
            }
            time = time + 1;
        }

        printf("\n");

        avg_waiting = waiting/num_processes;
        avg_turnaround = turnaround/num_processes;

        printf("Process scheduling completed with Avg Turn Around Time: %d, Avg Waiting Time: %d\n", avg_turnaround, avg_waiting);
    }

    fclose(f);

    return 0;
}
