#include <stdlib.h>
#include <wait.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <libgen.h>
#include <fcntl.h>

#define CMP_COUNT 101

pid_t zero_pid;
char *ModuleName;
int SIGUSR2counter = 0;
int SIGUSR1counter = 0;


void outputError(const char* path) {
    char strError[200];
    sprintf(strError, "%d '%s' %s", (int)getpid(), ModuleName, path);
    perror(strError);
}

long get_time() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_usec %1000L;
}

void __sigaction(int sig, void (*handler)(int)) {
	struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(sig, &action, NULL);
}


void first_handle() {
    SIGUSR1counter++;
    fprintf(stdout, "1 %d %d received signal SIGUSR2 %lu\n", getpid(), getppid(), get_time());
    if (SIGUSR1counter == CMP_COUNT)
    {
        kill(zero_pid + 2, SIGTERM);
        kill(zero_pid + 3, SIGTERM);
        kill(zero_pid + 4, SIGTERM);
        kill(zero_pid + 5, SIGTERM);
        kill(zero_pid + 6, SIGTERM);
        for (int i = 0; i < 5; ++i) 
        {
            wait(0);
        }
        fprintf(stdout, "1 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
                SIGUSR2counter);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
    else
    {
        fprintf(stdout, "1 %d %d sent signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
        kill(zero_pid + 8, SIGUSR1);
        fflush(stdout);
    }

}

void second_handle() {
    SIGUSR1counter++;
    fprintf(stdout, "2 %d %d received signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    fprintf(stdout, "2 %d %d sent signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    kill(zero_pid + 3, SIGUSR1);
    fflush(stdout);
}

void second_handleTERM() {
    fprintf(stdout, "2 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void third_handle() {
    SIGUSR1counter++;
    fprintf(stdout, "3 %d %d received signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    fprintf(stdout, "3 %d %d sent signal SIGUSR2 %lu\n", getpid(), getppid(), get_time());
    kill(zero_pid + 1, SIGUSR2);
    fflush(stdout);
}

void third_handleTERM() {
    fprintf(stdout, "3 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void fourth_handle() {
    SIGUSR2counter++;
    fprintf(stdout, "4 %d %d received signal SIGUSR2 %lu\n", getpid(), getppid(), get_time());
    fprintf(stdout, "4 %d %d sent signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    kill(zero_pid + 2, SIGUSR1);
    fflush(stdout);
}

void fourth_handleTERM() {
    fprintf(stdout, "4 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void fifth_handle() {
    SIGUSR2counter++;
    fprintf(stdout, "5 %d %d received signal SIGUSR2 %lu\n", getpid(), getppid(), get_time());
    fflush(stdout);
}

void fifth_handleTERM() {
    fprintf(stdout, "5 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void sixth_handle() {
    SIGUSR2counter++;
    fprintf(stdout, "6 %d %d received signal SIGUSR2 %lu\n", getpid(), getppid(), get_time());
    fflush(stdout);
}

void sixth_handleTERM() {
    kill(zero_pid + 7, SIGTERM);
    kill(zero_pid + 8, SIGTERM);
    for (int i = 0; i < 2; ++i) 
    {
        wait(0);
    }

    fprintf(stdout, "6 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void seventh_handle() {
    SIGUSR1counter++;
    fprintf(stdout, "7 %d %d received signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    fprintf(stdout, "7 %d %d sent signal SIGUSR2 %lu\n", getpid(), getppid(), get_time());
    killpg(zero_pid + 4, SIGUSR2);
    fflush(stdout);
}

void seventh_handleTERM() {
    fprintf(stdout, "7 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void eighth_handle() {
    SIGUSR1counter++;
    fprintf(stdout, "8 %d %d received signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    fprintf(stdout, "8 %d %d sent signal SIGUSR1 %lu\n", getpid(), getppid(), get_time());
    kill(zero_pid + 7, SIGUSR1);
    fflush(stdout);
}

void eighth_handleTERM() {
    fprintf(stdout, "8 %d %d ended after %d SIGUSR1 and %d SIGUSR2\n", getpid(), getppid(), SIGUSR1counter,
            SIGUSR2counter);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void create_hendler_1()
{
    __sigaction(SIGUSR2,first_handle);
    while (1) pause();
}


void create_hendler_2() {
    __sigaction(SIGUSR1, second_handle);
    __sigaction(SIGTERM, second_handleTERM);
    while (1) pause();
}

void create_hendler_3() {
    __sigaction(SIGUSR1, third_handle);
    __sigaction(SIGTERM, third_handleTERM);
    while (1) pause();

}

void create_hendler_4() {
    __sigaction(SIGUSR2, fourth_handle);
    __sigaction(SIGTERM, fourth_handleTERM);
    if (setpgid(zero_pid + 4, zero_pid + 4) == -1)
        outputError("");

    while (1) pause();
}

void create_hendler_5() {
    __sigaction(SIGUSR2, fifth_handle);
    __sigaction(SIGTERM, fifth_handleTERM);
    if (setpgid(zero_pid + 5, zero_pid + 4) == -1)
        outputError("");
    while (1) pause();
}

void create_hendler_6() {
    __sigaction(SIGUSR2, sixth_handle);
    __sigaction(SIGTERM, sixth_handleTERM);
    if (setpgid(zero_pid + 6, zero_pid + 4) == -1)
        outputError("");
    while(1) pause();
}

void create_hendler_7() {
    __sigaction(SIGUSR1, seventh_handle);
    __sigaction(SIGTERM, seventh_handleTERM);
    while (1) pause();
}

void create_hendler_8() {
    __sigaction(SIGUSR1, eighth_handle);
    __sigaction(SIGTERM, eighth_handleTERM);
    fprintf(stdout, "1 %d %d sent signal SIGUSR1 %lu\n", (int)(zero_pid + 1), (int)(zero_pid), get_time());
    kill(zero_pid + 8, SIGUSR1);
    while (1) pause();
}



void create_all_processes() {
	zero_pid = getpid();

	pid_t pid = fork(); 
	if (pid == 0) {
		pid = fork(); 
		if (pid > 0) {
			pid = fork(); 
			if (pid > 0) {
				pid = fork();                 
				if (pid > 0) {
					pid = fork();             
					if (pid > 0) {
						pid = fork();         
						if (pid == 0) {
							pid = fork();     
							if (pid > 0) {
								pid = fork(); 
							}
						}
					}
				}
			}
		}
	}

    if (pid == -1)
        exit(-1);

    switch(getpid() - zero_pid)
    {
        case 0: wait(0);            break;
        case 1: create_hendler_1(); break;
        case 2: create_hendler_2(); break;
        case 3: create_hendler_3(); break;
        case 4: create_hendler_4(); break;
        case 5: create_hendler_5(); break;
        case 6: create_hendler_6(); break;
        case 7: create_hendler_7(); break;
        case 8: create_hendler_8(); break;
    }
}


int main(int argc, char *argv[]) {
    ModuleName = basename(argv[0]);

    create_all_processes();

    return 0;
}
