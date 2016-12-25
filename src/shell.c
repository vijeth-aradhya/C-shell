#include "global.h"
#include "read_input.h"
#include "shell_display.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <errno.h>

void procINT(int signal);

void procSTOP(int signal);

int main() {
	//Global Initilizations
	char username[1024];
	char hostname[1024];
	char cwd[1024];
	ssize_t MIN_BUFFSIZE;
	MIN_BUFFSIZE = 1024;
	char* ret_char;
	ret_char = getcwd(cwd, MIN_BUFFSIZE);
	if(!ret_char) {
		perror("Error: ");
		return 0;
	}
	signal(SIGINT,procINT);
	signal(SIGTSTP,procSTOP);
	while(1) {
		if(!displayShell(cwd, hostname, username, MIN_BUFFSIZE)) {
			return 0;
		}
		if(!readInput(cwd, MIN_BUFFSIZE)) {
			return 0;	
		}
	}
	return 0;
}

void procINT(int signal) {
	printf("\nProcess Interrupted\n");
}

void procSTOP(int signal) {
	if(kill(PID, SIGSTOP) < 0)
		perror("Error in putting it the process to bg ");
	else {
		printf("\nProcess %d has been stopped and sent to bg\n", PID);
		bgINSERT(NAME, PID);
	}
}