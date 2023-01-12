#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <ctype.h>

#define STRUCT_SIZE 30
#define TRANSACTIONS 30

pthread_mutex_t lock;

// holds all client related information
struct Client {
    int account_number[TRANSACTIONS];
    char transaction_type[TRANSACTIONS];
    int num_transactions;
    int amount[TRANSACTIONS];
    int client_name;
} client[STRUCT_SIZE];

// holds balances for all accounts
int account_balances[STRUCT_SIZE];

// function that withdraws (removes) or deposits (adds) money from/into an account's balance
void *transaction(void *client){

    struct Client *curr_client = (struct Client*)client;

    int acc_num = 0;

    pthread_mutex_lock(&lock);

    // checks transaction type to conduct associated action
    for(int i = 0; i < TRANSACTIONS; i++){
        if(curr_client->transaction_type[i] == 'w'){
            acc_num = curr_client->account_number[i];
            // ensures amount in account is greater than requested withdrawl amount
            if(account_balances[acc_num] > curr_client->amount[i]){
                account_balances[acc_num] = account_balances[acc_num] - curr_client->amount[i];
            }
        }
        else if(curr_client->transaction_type[i] == 'd'){
            acc_num = curr_client->account_number[i];
            account_balances[acc_num] = account_balances[acc_num] + curr_client->amount[i];
        }
    }

    pthread_mutex_unlock(&lock);
}

int main(){
        
    FILE *f;

    char line[99];
    int line_length = sizeof(line);

    // supplying assignment_5_input.txt to the program
    f = fopen("assignment_5_input.txt", "r");

    if(f == 0 || f == NULL)
    {   
        perror("Cannot open assignment_5_input.txt, please try again.\n");
        exit(-1);
    }

    int num_acc = 0;
    int num_client = 1;

    char *split[STRUCT_SIZE];

    int t = 0;
        
    while (fgets(line, line_length, f))
    {
        // if line in file is for an account balance
        if(line[0] == 'a'){

            char *token = strtok(line, " ");
        
            while(token != NULL) {
                split[t++] = token;
                token = strtok(NULL, " ");
            }

            num_acc = num_acc + 1;
            account_balances[num_acc] = atoi(split[2]);
        }
        // if line in file is for a client transaction
        else if(line[0] == 'c'){

            char *token = strtok(line, " ");
            int transaction_num = 0;

            // checks each token for a specific case (account num, transaction type, amount, client) to add to client struct
            while(token != NULL) {
                if(token[0] == 'w'){
                    client[num_client].num_transactions++;
                    transaction_num = client[num_client].num_transactions;
                    client[num_client].transaction_type[transaction_num] = 'w'; 
                }
                else if(token[0] == 'd'){
                    client[num_client].num_transactions++;
                    transaction_num = client[num_client].num_transactions;
                    client[num_client].transaction_type[transaction_num] = 'd';
                }
                else if(token[0] == 'a'){
                    client[num_client].account_number[transaction_num] = atoi(&token[7]);
                }
                else if(isdigit(token[0])){
                    transaction_num = client[num_client].num_transactions;
                    client[num_client].amount[transaction_num] = atoi(token);
                }
                else if(token[0] == 'c'){
                    client[num_client].client_name = num_client;
                }
                
                token = strtok(NULL, " ");
            }
            num_client++;
        }
    }

    printf("Number of Clients: %d\n", num_client-1);
    printf("Number of Accounts: %d\n", num_acc);

    pthread_t threads[num_client];
    int err_thread;

    // creates threads for each client
    for (int i = 0; i < num_client; i++){

        err_thread = pthread_create(&threads[i], NULL, &transaction, (void *)&client[i+1]); 

        if (err_thread != 0){
            printf("\n Error creating thread %d", i);
        }
    }

    for (int i = 0; i < num_client; i++){
        
        pthread_join(threads[i], NULL);
    }

    // should print:
    // Account 1 Balance: $600
    // Account 2 Balance: $2200
    // Account 3 Balance: $3500
    // Account 4 Balance: $3800
    for(int i = 0; i < num_acc; i++){

        printf("Account %d Balance: $%d\n", i+1, account_balances[i+1]);
    }

    pthread_mutex_destroy(&lock); 

    return 0;
}
