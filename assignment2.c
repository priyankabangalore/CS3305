#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main (int argc, char *argv[]) {

    if(argc != 4){
        perror("Incorrect number of arguements. Must have 3 message parameters.");
        return 1;
    }
        
    int port[2];
    if (pipe(port) < 0){
        perror("Pipe error.");
        return 1;
    }

    char xyz[50];
    char xy[50];
    char temp_for_read[50];

    pid_t parent_PID = getppid();
    
    pid_t pid;
    pid = fork();
    pid_t child_PID = getpid();

    // in child process
    if(pid == 0){

        printf("child (PID %d): received X = \"%s\"\n", child_PID, argv[1]);

        printf("child (PID %d): writing \"%s\" into pipe\n", child_PID, argv[1]);
        int len_x = strlen(argv[1]) + 1;
        write(port[1], argv[1], len_x);

        // move to parent
        sleep(1);
        // come back from parent

        int len_xyz = strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) + 3;
        read(port[0], &temp_for_read, len_xyz);
        printf("child (PID %d): read from pipe \"%s\"\n", child_PID, temp_for_read);

        printf("child (PID %d): received Z = \"%s\"\n", child_PID, argv[3]);

        // concatenates x, y and z
        strcat(xy, argv[1]);
        strcat(xy, " ");
        strcat(xy, argv[2]);
        strcat(xyz, argv[1]);
        strcat(xyz, " ");
        strcat(xyz, argv[2]);
        strcat(xyz, " ");
        strcat(xyz, argv[3]);
        printf("child (PID %d): \"%s\" + Z = \"%s\"\n", child_PID, xy, xyz);

        printf("child (PID %d): writing into pipe \"%s\"\n", child_PID, xyz);
        write(port[1], xyz, len_xyz);

        printf("child (PID %d): all tasks completed\n", child_PID);

        // move to parent
    }
    // in parent process
    else if(pid > 0){

        printf("parent (PID %d): created child (PID %d)\n", parent_PID, child_PID);

        int len_x = strlen(argv[1]) + 1;
        read(port[0], &temp_for_read, len_x);
        printf("parent (PID %d): read from pipe \"%s\"\n", parent_PID, temp_for_read);

        printf("parent (PID %d): received Y = \"%s\"\n", parent_PID, argv[2]);

        // concatenates x and y
        strcat(xy, argv[1]);
        strcat(xy, " ");
        strcat(xy, argv[2]);
        printf("parent (PID %d): \"%s\" + Y = \"%s\"\n", parent_PID, argv[1], xy);

        printf("parent (PID %d): writing into pipe \"%s\"\n", parent_PID, xy);
        int len_xy = strlen(argv[1]) + strlen(argv[2]) + 2;
        write(port[1], xy, len_xy);

        // move to child
        wait(NULL);
        // come back from child

        int len_xyz = strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) + 3;
        read(port[0], &temp_for_read, len_xyz);
        printf("parent (PID %d): read from pipe \"%s\"\n", parent_PID, temp_for_read);
        
        printf("parent (PID %d): all tasks completed\n", parent_PID);

        // finish entire process here
    }

    return 0;
}
