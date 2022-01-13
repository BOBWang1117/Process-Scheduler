# Process Scheduler

-----

>  Team: 
>
>  [@Hongbo Wang ](https://github.com/BOBWang1117) 
>
>  @Yichu Li

>  

## **Environment:**

- System: Linux Ubuntu 

- Language: C++

- Compiler: GNU g++

  

## **Details**:

- Type: term project
- Teacher: 戴弘宁
- Computer Language: C
- Project Name: Process Scheduler
- Time: 2021/5/5
- Description: This scheduler can complement a scheduler with four scheduling policy, First-In First-Out, Round Robin, Shortest Job First and preemptive SJF respectively.



## How to Run:

### Compile

Open Terminal in Linux system(recommended system is Linux Ubuntu), enter:

```
$ g++ -o monitor monitor.cpp
$ make
```

or:

```
$ g++ -o monitor monitor.cpp
$ g++ -o scheduler scheduler.cpp iotxt.cpp
```

### Run

You can write your own file as the format:

| Arrival Time 1 | \t   | Command 1 | \t   | Duration 1 | \n   |
| -------------- | ---- | --------- | ---- | ---------- | ---- |
| Arrival Time 2 | \t   | Command 2 | \t   | Duration 2 | \n   |
| Arrival Time 3 | \t   | Command 3 | \t   | Duration 3 | \n   |

Then, put the file to the same path as the other compiled executable file.

Enter this command in the Terminal (scheduler policy you can choose "FIFO", "RR", "SJF" or "PSJF"):

```
$ ./scheduler [Job Description File Name] [Scheduler Policy]
```
