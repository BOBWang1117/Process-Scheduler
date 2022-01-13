#include <iostream>     
#include <string.h>         //strtok(), strcpy()
#include <unistd.h>         //getpid(), fork()
#include <signal.h>         //SIGALRM, signal()
#include <sys/wait.h>       //wait()
#include <queue>            // For queue container
#include <sys/times.h>	    //time counter
#include <limits.h>
#include <vector>
#include "iotxt.h"

using namespace std;

#define ROUND_ROBIN_QUANTUM 2   //The Round Robin scheduler with quantum set to 2 seconds
#define MAX_CMD 255             //Maximum command length for a line
#define MAX_JOB 9               //Maxumum amount of job 
#define NOT_ARRIVED 0
#define ARRIVED 1
#define EXECUTING 2
#define FINISHED 3

#define SIGKILL 9               //Forced-process termination
#define SIGALRM 14              //Real-timerclock
#define SIGCHLD 17              //Child process stopped or terminated, or got signal if traced
#define SIGCONT 18              //Resume execution, if stopped
#define SIGTSTP 20              //Stop process issued from tty

string scheduleNumber = "";

class Job {
public:
    int pid;
    int jid;
    char cmdline[MAX_CMD];
    int arr_time;
    int dur_time;
    int remain_time;
    int situation;
    string printGantt;
};

int processID;

void handler(int sig)
{
    if (sig == SIGCHLD)
    {
        wait(NULL);
    }
}

int getShortestJob (vector<Job> now)
{
    int shortestNum = 0;
    for (int i = 1; i < now.size(); i++) {
        if (now[i].remain_time < now[shortestNum].remain_time) {
            shortestNum = i;
        }
    }

    return shortestNum;
}

int strtok_counter_function(char cmdline[])
{
    int tokenLength = 0;
    char* cmdLineToken;
    char str[strlen(cmdline)];
    strcpy(str, cmdline);
    char* outer_ptr = NULL;
    cmdLineToken = strtok_r(str, " ", &outer_ptr);

    while (cmdLineToken != NULL)
    {
        cmdLineToken = strtok_r(NULL, " ", &outer_ptr);
        tokenLength++;
    }

    return (tokenLength + 2); //one plus for "./monitor"
}

void strtok_function(char cmdline[], int tokenArrayLength, char* tokenArray[])
{
    int counter = 1;
    char* cmdLineToken;
    char str[strlen(cmdline)];
    string startMonitor = "./monitor";
    strcpy(str, cmdline);
    tokenArray[0] = (char*)malloc(sizeof(char) * startMonitor.length());
    strcpy(tokenArray[0], startMonitor.c_str());    //changed
    cmdLineToken = strtok(str, " ");
    while (counter < tokenArrayLength - 1)
    {
        tokenArray[counter] = (char*)malloc(sizeof(char) * strlen(cmdLineToken));
        strcpy(tokenArray[counter], cmdLineToken);
        cmdLineToken = strtok(NULL, " ");
        counter++;
    }

}

void setJobProcess(Job process[], int line, char** store)
{

    for (int i = 0; i < line; i++)
    {
        int count = 0;
        char* jobToken;
        jobToken = strtok(store[i], "\t");
        process[i].arr_time = atoi(jobToken);
        process[i].situation = NOT_ARRIVED;
        process[i].pid = i;
        while (jobToken != NULL)
        {
            jobToken = strtok(NULL, "\t");
            if (count == 0)
            {
                strcpy(process[i].cmdline, jobToken);
            }
            if (count == 1)
            {
                process[i].dur_time = atoi(jobToken);
                if (atoi(jobToken) == -1)
                {
                    process[i].remain_time = INT_MAX;
                }
                else
                {
                    process[i].remain_time = atoi(jobToken);
                }
            }
            count++;
        }

    }
}

void beReadyProcess(Job process[], int line)
{
    for (int i = 0; i < line; i++)
    {
        int tokenArrayLength = strtok_counter_function(process[i].cmdline);
        char* tokenArray[tokenArrayLength];
        strtok_function(process[i].cmdline, tokenArrayLength, tokenArray);
        tokenArray[tokenArrayLength - 1] = NULL;

        int new_process_id = fork();

        if (new_process_id == 0)//Child
        {
            process[i].jid = getpid();
            kill(process[i].jid, SIGTSTP);
            execvp(tokenArray[0], tokenArray);
        }
        else    //Parent
        {
            process[i].jid = new_process_id;
        }
    }
}

void printGanttChart(Job process[], int line)
{
    cout << "GanttChart" << endl;
    cout << "===========" << endl;
    cout << "  Time " << "  |";
    for (int i = 0; i < process[line - 1].printGantt.size() / 2; i++)
    {
        if (i % 10 == 0)
        {
            cout << " " << i / 10;
        }
        else
        {
            cout << "  ";
        }
    }
    cout << endl;
    cout << "         |";
    for (int i = 0; i < (process[line - 1].printGantt.size()) / 2; i++)
    {
        cout << " " << i % 10;
    }
    cout << endl;
    for (int i = 0; i < line; i++)
    {
        cout << "---------+";
        for (int j = 0; j < process[line - 1].printGantt.size(); j++)
        {
            cout << "-";
        }
        cout << endl;
        cout << "  Job " << (i + 1) << "  |" << process[i].printGantt << endl;
    }
    cout << "---------+";
    for (int j = 0; j < process[line - 1].printGantt.size(); j++)
    {
        cout << "-";
    }
    cout << endl;
    cout << "  Mixed " << " | " << scheduleNumber << endl;
}

int timer()
{
    int addTime = 1;
    sleep(1);

    return addTime;
}

void record(Job process[], int line)
{
    scheduleNumber += " ";
    for (int i = 0; i < line; i++) {
        if (process[i].situation == NOT_ARRIVED) {
            process[i].printGantt += " ";
            process[i].printGantt += " ";
        }
        else if (process[i].situation == ARRIVED) {
            process[i].printGantt += " ";
            process[i].printGantt += ".";
        }
        else if (process[i].situation == EXECUTING) {
            process[i].printGantt += " ";
            process[i].printGantt += "#";
            char c[20];
            sprintf(c, "%d", process[i].pid + 1);
            scheduleNumber += c;
            //scheduleNumber += to_string(process[i].pid + 1);
        }
        else if (process[i].situation == FINISHED) {
            process[i].printGantt += " ";
            process[i].printGantt += " ";
        }
    }
}


void FIFO(Job process[], int line)
{
    queue<Job> fifo;

    int time = 0;
    int counter = 0;
    while (1)
    {
        for (int i = 0; i < line; i++) {    //when job arrives, push it to the queue
            if (process[i].arr_time == time) {
                process[i].situation = ARRIVED;
                fifo.push(process[i]);
            }
        }
        if (counter == line)    //finished all jobs
        {
            break;
        }

        if (fifo.empty())   //no job needs to be executed
        {
            record(process, line);
            timer();
            time++;
            continue;
        }
        process[fifo.front().pid].situation = EXECUTING;
        kill(fifo.front().jid, SIGCONT);   //execute the first job
        record(process, line);
        timer();
        time++;
        (process[fifo.front().pid].remain_time)--;//reduce the remain_time

        int status;
        if (waitpid(fifo.front().jid, &status, WNOHANG) > 0)//situation 2: child process is complete the process.
        {
            process[fifo.front().pid].situation = FINISHED;
            fifo.pop();
            counter++;
            continue;
        }
        if (process[fifo.front().pid].remain_time == 0)//situation 1: remaining time =0
        {
            kill(fifo.front().jid, SIGTERM);
            process[fifo.front().pid].situation = FINISHED;
            fifo.pop();
            counter++;
            continue;
        }

        kill(fifo.front().jid, SIGTSTP);
    }
}

void SJF(Job process[], int line)   //Shortest-Job-First
{
    vector<Job> sjf;

    int time = 0;
    int now;
    bool HAS_EXCUTING_PROCESS = false;
    int counter = 0;
    while (1) 
    {
        if (counter == line) {
            break;
        }
        for (int i = 0; i < line; i++) {
            if (process[i].arr_time == time) {  //any process arrived
                sjf.push_back(process[i]);
                process[i].situation = ARRIVED;
            }
        }
        if (sjf.size() == 0) {  //no process arrived, wait 1 second
            timer();
            time++;
            record(process, line);
            continue;
        }
        if (!HAS_EXCUTING_PROCESS) {
            now = getShortestJob(sjf);  //find the shortest job, execute it

            process[sjf[now].pid].situation = EXECUTING;
        }
        HAS_EXCUTING_PROCESS = true;
        record(process, line);
        kill(sjf[now].jid, SIGCONT);
        timer();//stop 1 second
        time++;
        (process[sjf[now].pid].remain_time)--;//reduce the remain_time
        int status;

        if (waitpid(sjf[now].jid, &status, WNOHANG) > 0)//situation 2: child process is complete the process.
        {
            process[sjf[now].pid].situation = FINISHED;
            sjf.erase(sjf.begin() + now);
            counter++;
            HAS_EXCUTING_PROCESS = false;
            continue;
        }

        if (process[sjf[now].pid].remain_time == 0)//situation 1: remaining time =0
        {
            process[sjf[now].pid].situation = FINISHED;
            kill(sjf[now].jid, SIGTERM);
            sjf.erase(sjf.begin() + now);
            counter++;
            HAS_EXCUTING_PROCESS = false;
            continue;
        }
        kill(sjf[now].jid, SIGTSTP);
    }
}

void RR(Job process[], int line)
{
    vector<Job> rr;

    int time = 0;
    int RR_time = 0;
    int counter = 0;
    int now = 0;
    bool JOB_NOT_FINISHED = false;
    int again;
    bool switchProcess = false;

    while (1)
    {
        if (counter == line)
        {
            break;
        }
        for (int i = 0; i < line; i++)
        {
            if (process[i].arr_time == time) 
            {
                rr.push_back(process[i]);
                process[i].situation = ARRIVED;
            }
        }

        if (rr.size() == 0) {  //no process arrived, wait 1 second
            record(process, line);
            timer();
            time++;
            continue;
        }

        if (process[rr[now].pid].situation == FINISHED) {
            while (1) {
                now++;
                if (now > (rr.size() - 1)) {
                    now = 0;
                }
                if (process[rr[now].pid].situation != FINISHED) {
                    break;
                }
            }
        }
        else if (RR_time == ROUND_ROBIN_QUANTUM) {    //switch to another job
            process[rr[now].pid].situation = ARRIVED;
            RR_time = 0;    //reset it to zero

            while (1) {
                now++;
                if (now > (rr.size() - 1)) {
                    now = 0;
                }
                if (process[rr[now].pid].situation != FINISHED) {
                    break;
                }
            }
        }

        process[rr[now].pid].situation = EXECUTING;
        record(process, line);
        kill(rr[now].jid, SIGCONT);
        timer();//stop 1 second
        time++;
        RR_time++;
        (process[rr[now].pid].remain_time)--;   //reduce the remain_time
        int status;

        if (waitpid(rr[now].jid, &status, WNOHANG) > 0) //situation 2: child process is complete the process.
        {
            process[rr[now].pid].situation = FINISHED;
            counter++;
            RR_time = 0;
            continue;
        }

        if (process[rr[now].pid].remain_time == 0)  //situation 1: remaining time =0
        {
            process[rr[now].pid].situation = FINISHED;
            kill(rr[now].jid, SIGTERM);
            counter++;
            RR_time = 0;
            continue;
        }

        kill(rr[now].jid, SIGTSTP);
    }
}

void PSJF(Job process[], int line)
{
    vector<Job> p_sjf;
    int time = 0;
    int now;
    int counter = 0;

    while (1)
    {
        if (counter == line) {  //Finished all process
            break;
        }
        for (int i = 0; i < line; i++) {
            if (process[i].arr_time == time) {  //any process arrived
                p_sjf.push_back(process[i]);
                process[i].situation = ARRIVED;
            }
        }
        if (p_sjf.size() == 0) {  //no process arrived, wait 1 second
            timer();
            time++;
            record(process, line);
            continue;
        }
        now = getShortestJob(p_sjf);  //find the shortest job, execute it
        process[p_sjf[now].pid].situation = EXECUTING;
        record(process, line);
        kill(p_sjf[now].jid, SIGCONT);
        timer();//stop 1 second
        time++;
        (process[p_sjf[now].pid].remain_time)--;//reduce the remain_time
        int status;

        if (waitpid(p_sjf[now].jid, &status, WNOHANG) > 0)//situation 2: child process is complete the process.
        {
            process[p_sjf[now].pid].situation = FINISHED;
            p_sjf.erase(p_sjf.begin() + now);
            counter++;
            continue;
        }

        if (process[p_sjf[now].pid].remain_time == 0)//situation 1: remaining time =0
        {
            process[p_sjf[now].pid].situation = FINISHED;
            kill(p_sjf[now].jid, SIGTERM);
            p_sjf.erase(p_sjf.begin() + now);
            counter++;
            continue;
        }
        process[p_sjf[now].pid].situation = ARRIVED;
        kill(p_sjf[now].jid, SIGTSTP);
    }
}

void policyRule(char* policy, Job process[], int line)
{
    int policyNumber;
    if (strcmp(policy, "FIFO") == 0)
    {
        policyNumber = 0;
    }
    else if (strcmp(policy, "SJF") == 0)
    {
        policyNumber = 1;
    }
    else if (strcmp(policy, "RR") == 0)
    {
        policyNumber = 2;
    }
    else if (strcmp(policy, "PSJF") == 0)//preemptive SJF
    {
        policyNumber = 3;
    }
    else
    {
        cout << "No Policy Rule in this program!" << endl;
        policyNumber = 4;
    }

    switch (policyNumber)
    {
    case 0: FIFO(process, line); break;
    case 1: SJF(process, line);  break;
    case 2: RR(process, line);   break;
    case 3: PSJF(process, line); break;
    default: cout << "WRONG!" << endl;
    }

}



int main(int argc, char* argv[])
{
    int line;
    char* inputfilename = argv[1];
    char* policy = argv[2];
    IoTxt IoTxt(inputfilename);  //get name
    line = IoTxt.txtLine(); //get number of line
    IoTxt.ioOut();  //execute io

    char** store;
    store = IoTxt.stringArrayCopy();    //get store from class IoTxt

    Job process[line];  //Notice that line is smaller or equal to 9
    setJobProcess(process, line, store);
    beReadyProcess(process, line);
    policyRule(policy, process, line);
    printGanttChart(process, line);
    wait(NULL);

    return 0;
}