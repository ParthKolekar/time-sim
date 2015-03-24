#include <sys/time.h>
#include <sys/times.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

typedef void (*sighandler)(int);

struct times_val_t
{
	double utime;
	double stime;
	double rtime;
} times_val;

#ifndef HARAKIRI
#define HARAKIRI 143
#endif

char *commands[100];

void parseArgs (const int argc , char ** argv) 
{
	if (argc >= 2)
	{
		int i;
		int flag = 0;
		int j = 0;
		for (i = 1; i < argc; ++i)
		{
			if (argv[i][0] == '-' && !flag)
			{	
				fprintf(stderr, "Argument %s Not Supported\n" , argv[i]);
				continue;
			}
			else 
			{
				flag = 1;
				commands[j++] = argv[i];
			}
		}
		commands[j++] = NULL;
	}
	else 
	{
		fprintf(stderr, "Insufficent Arguments\n");
		exit(-1);
	}
}

int runChild ()
{
	sighandler inturupt, quit;
    int pid = fork();
    int status = 0;
    struct timeval start_time, end_time; // rtime
    struct rusage kernStat; // utime stime
    if (pid != 0)
    {
    	inturupt = signal(SIGINT , SIG_IGN);
        quit = signal(SIGQUIT , SIG_IGN);
        gettimeofday(&start_time , NULL); //No need for time-zones.
        wait4 (pid , &status , WUNTRACED , &kernStat);
		gettimeofday(&end_time , NULL);
        signal(SIGINT , inturupt);
        signal(SIGQUIT , quit);
        if ((status & 0xFF) == HARAKIRI) 
        {
        	kill (pid , SIGKILL);
        	return HARAKIRI; // Because Kill Yourself.
        }
        if ((status & 0xFF) != EXIT_SUCCESS) 
        {
        	fprintf(stderr, "Child Killed By Code %d\n" , status);
        }
        times_val.stime = kernStat.ru_stime.tv_sec + (kernStat.ru_stime.tv_usec / 10e6); //s
        times_val.utime = kernStat.ru_stime.tv_sec + (kernStat.ru_utime.tv_usec / 10e6); //s 
        times_val.rtime = (end_time.tv_sec + (end_time.tv_usec / 10e6)) - (start_time.tv_sec + (start_time.tv_usec / 10e6));//us
        return 0;
    }
    else
    {
 		if (execvp ( commands[0] , commands ) < 0)
		{
            fprintf ( stderr , "COULD NOT FIND COMMAND : %s\n" , commands[0] );
		    kill (getpid() , SIGTERM);
		    return HARAKIRI;  // Because Kill Yourself.
		}       
    }
    return 0;
}

void printData() 
{
	fprintf(stderr , "Real Time :%.3lf\n", times_val.rtime);
	fprintf(stderr , "User Time :%.3lf\n", times_val.utime);
	fprintf(stderr , "System Time :%.3lf\n", times_val.stime);
}

int main(const int argc, char ** argv)
{
	parseArgs(argc, argv);
	runChild();
	printData();
	return 0;
}