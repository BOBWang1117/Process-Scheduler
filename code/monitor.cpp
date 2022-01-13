#include<stdio.h>       //printf()
#include<stdlib.h>      //exit()
#include<string.h>      //strtok()
#include<unistd.h>      //getpid(), fork()
#include<signal.h>      //SIGALRM, signal()
#include <sys/wait.h>   // required by wait()
#include <sys/times.h>	//time counter
#include<iostream>

#define MAX_CMD_LENGTH 255 //The maximum command string for user

using namespace std;

int new_process_id; //global variable for monitor to control child process

/*
* Name: CONT_function
* Method: signal handler to CONT signal
* Description: use this function to control the child process
*/
void CONT_function(int sig)
{
    kill(new_process_id, SIGCONT);
    //printf("SIGCONT: The process %d is continue.\n", getpid());
}

/*
* Name: TSTP_function
* Method: signal handler to TSTP signal
* Description: use this function to control the child process
*/
void TSTP_function(int sig)
{
    kill(new_process_id, SIGTSTP);
    //printf("SIGTSTP: The process %d is stop.\n", getpid());
}

/*
* Name: TERM_function
* Method: signal handler to TERM signal
* Description: use this function to control child process
*/
void TERM_function(int sig)
{
    kill(new_process_id, SIGTERM);
    //printf("SIGTERM: The process %d is terminate.\n", getpid());
}

int main(int argc, char* argv[])
{
    clock_t start, end; //record the starting and ending time
    struct tms st_tms;
    long tck = sysconf(_SC_CLK_TCK);    //Clock ticks per second
    if (tck < 0) {
        printf("%s\n", "sysconf error");
        exit(1);
    }

    char* token[argc];  //store the command tokens
    int counter = 1;    //helps to iterate over the array 
    int cmd_length = 0; //count the length of command string
    int blank = argc - 2;   //count the length of space in command string

    while(counter < argc)   //interate to store the command tokens and count the total length of command string 
    {
        token[counter - 1] = argv[counter];
        cmd_length += strlen(token[counter - 1]);
	    counter++;
    }
    cmd_length += blank;

    token[counter - 1] = NULL;  //the last of token is NULL, for exec() family

    if(cmd_length > MAX_CMD_LENGTH)
    {
        printf("The Commond charactor is larger than 255\n");
	    exit(1);
    }

    start = times(&st_tms); //count starting time just before the birth of child process.
    new_process_id = fork();    //create a new process

    //This is the child process
    if(new_process_id == 0)
    {
        //using the exec() family to get it
       	execvp(token[0], token);
    }

    else {
        //signal() system call changes the signal handler
        signal(SIGCONT, CONT_function);
        signal(SIGTSTP, TSTP_function);
        signal(SIGTERM, TERM_function);

        wait(NULL); //wait for the termination of child process
        end = times(&st_tms);   //stop count time

        //printf("Process %d : ", getpid());
        //printf("time elapsed\t: %.4lf\n",(double) (end - start)/ (double)tck);
        //printf("\t\tuser time\t: %.4lf\n", ((double)(st_tms.tms_cutime) / (double)(tck)));
        //printf("\t\tsystem time\t: %.4lf\n", ((double)(st_tms.tms_cstime) / (double)tck));
    }

    return 0;
}