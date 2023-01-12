#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

int port[2];
int global_nums_len;

// start routine for thread 1
void *subtract(void *thread_id){

    char temp_for_read[50];
    char * thread_num = *(char *)thread_id;

    // reads from pipe
    int status;
    status = read(port[0], &temp_for_read, global_nums_len);
    if (status == -1) {
        perror("Error reading from pipe in thread 1.\n");
        exit(1);
    }
    
    // splits concatenated input
    char * first_num;
    char * second_num;
    char * token;
    int found_x = 0;

    token = strtok(temp_for_read, " ");
    while (token != NULL)
    {
        if(found_x == 0){
            first_num = token;
            found_x = 1;
        }
        if(found_x == 1){
            second_num = token;
        }
        token = strtok(NULL, " ");
    }

    int x = atoi(first_num);
    int y = atoi(second_num);

    printf("thread (TID %d) reads X = %d and Y = %d from the pipe.\n", thread_num, x, y);

    // does subtraction with values
    int s = x - y;

    // writes to pipe()
    char answer[50];
    sprintf(answer, "%d", s);
    int len_answer = strlen(answer);
    status = write(port[1], answer, len_answer);
    if (status == -1) {
        perror("Error writing to pipe for thread 1.\n");
        exit(1);
    }
    printf("thread (TID %d) writes X - Y = %d to the pipe.\n", thread_num, s);

    // updates global var
    global_nums_len = len_answer;
}

// start routine for thread 2
void *prime_check(void *thread_id){

    char temp_for_read[50];
    char * thread_num = *(char *)thread_id;

    // reads from pipe
    int status;
    status = read(port[0], &temp_for_read, global_nums_len);
    if (status == -1) {
        perror("Error reading from pipe in thread 2.\n");
        exit(1);
    }

    int s = atoi(temp_for_read);
    printf("thread (TID %d) reads X - Y = %d from the pipe.\n", thread_num, s);

    // determines whether value is a prime number
    int prime = 1;

    if(s == 0 || s == 1){
        prime = 0;
    }

    for (int i = 2; i <= s / 2; ++i) {
        if (s % i == 0){
            prime = 0;
        }
    }

    if(prime == 0){
        printf("thread (TID %d) identified that %d is not a prime number.\n", thread_num, s);
    }
    else if(prime == 1){
        printf("thread (TID %d) identified that %d is a prime number.\n", thread_num, s);
    }

    // writes to pipe
    int len_answer = strlen(temp_for_read);
    status = write(port[1], temp_for_read, len_answer);
    if (status == -1) {
        perror("Error writing to pipe for thread 2.\n");
        exit(1);
    }
    printf("thread (TID %d) writes %d to the pipe.\n", thread_num, s);

    // updates global var
    global_nums_len = len_answer;
}

// start routine for thread 3
void *reverse_num(void *thread_id){

    char temp_for_read[50];
    char * thread_num = *(char *)thread_id;

    // reads from pipe
    int status;
    status = read(port[0], &temp_for_read, global_nums_len);
    if (status == -1) {
        perror("Error reading from pipe in thread 3.\n");
        exit(1);
    }

    int s = atoi(temp_for_read);
    printf("thread (TID %d) reads X - Y = %d from the pipe.\n", thread_num, s);

    // reverses value
    int reversed = 0;
    int remainder;

    while(s != 0)    
    {    
        remainder = s % 10;    
        reversed = reversed * 10 + remainder;    
        s = s / 10;    
    }    

    printf("thread (TID %d) reversed number is %d.\n", thread_num, reversed);
}

int main(int argc, char *argv[]){

    // checks for correct number of arguments entered
    if(argc != 3){
        perror("Incorrect number of arguments. Must have 2 integer parameters.\n");
        return 1;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    // checks that the correct argument values entered
    if(x < 0 || y < 0){
        perror("Incorrect argument values; both integers must be positive.\n");
        return 1;
    }
    if(x <= y){
        perror("Incorrect argument values; the first entered integer must be greater than the second.\n");
        return 1;
    }

    pid_t parent_PID = getpid();

    printf("parent (PID %d) receives X = %d and Y = %d from the user.\n", parent_PID, x, y);

    // checks for pipe status
    int result;
    result = pipe(port);
    if (result < 0){
        perror("Pipe error.\n");
        exit(1);
    }
    
    // writes x and y to the pipe
    char nums[50];
    strcat(nums, argv[1]);
    strcat(nums, " ");
    strcat(nums, argv[2]);

    int nums_len = strlen(nums);
    global_nums_len = nums_len;

    int status;
    status = write(port[1], nums, nums_len);
    if (status == -1) {
        perror("Error writing to pipe in main.\n");
        exit(1);
    }
    printf("parent (PID %d) writes X = %d and Y = %d to the pipe.\n", parent_PID, x, y);

    // creates thread 1, thread 2, and thread 3
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;

    pthread_create(&thread_1, NULL, subtract, "1");
    pthread_join(thread_1, NULL);
    pthread_create(&thread_2, NULL, prime_check, "2");
    pthread_join(thread_2, NULL);
    pthread_create(&thread_3, NULL, reverse_num, "3"); 
    pthread_join(thread_3, NULL);

    return 0;
}
