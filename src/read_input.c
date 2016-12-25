#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <errno.h>
#include "./changedir.h"
#include "./execution.h"

int SPACEparse(char** eachRED, char* string, ssize_t MIN_BUFFSIZE) {
	int pos=0;
	ssize_t INPUT_BUFFSIZE=MIN_BUFFSIZE;
	char *temp;
	char delim[2] = {' ', '\t'};
	if(!eachRED) {
		fprintf(stderr, "Error reading input, failed to allocate memory");
		return 0;
	}
	temp = strtok(string, delim);
	while(temp != NULL) {
		eachRED[pos] = temp;
		pos++;
		if(pos >= INPUT_BUFFSIZE) {
			INPUT_BUFFSIZE += MIN_BUFFSIZE;
			eachRED = realloc(eachRED, INPUT_BUFFSIZE);
			if(!eachRED) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
				return 0;
			}
		}
		temp = strtok(NULL, delim);
	}
	eachRED[pos] = temp;
	return(pos);
}

int PIPEparse(char** eachRED, char* string, ssize_t MIN_BUFFSIZE) {
	int pos=0;
	ssize_t INPUT_BUFFSIZE=MIN_BUFFSIZE;
	char *temp;
	char delim[1] = {'|'};
	if(!eachRED) {
		fprintf(stderr, "Error reading input, failed to allocate memory");
		return 0;
	}
	temp = strtok(string, delim);
	while(temp != NULL) {
		eachRED[pos] = temp;
		pos++;
		if(pos >= INPUT_BUFFSIZE) {
			INPUT_BUFFSIZE += MIN_BUFFSIZE;
			eachRED = realloc(eachRED, INPUT_BUFFSIZE);
			if(!eachRED) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
				return 0;
			}
		}
		temp = strtok(NULL, delim);
	}
	eachRED[pos] = temp;
	return(pos);
}

int REDparse(char** eachRED, char* string, ssize_t MIN_BUFFSIZE) {
	int pos=0;
	ssize_t INPUT_BUFFSIZE=MIN_BUFFSIZE;
	char *temp;
	char delim[2] = {'>', '<'};
	if(!eachRED) {
		fprintf(stderr, "Error reading input, failed to allocate memory");
		return 0;
	}
	temp = strtok(string, delim);
	while(temp != NULL) {
		eachRED[pos] = temp;
		pos++;
		if(pos >= INPUT_BUFFSIZE) {
			INPUT_BUFFSIZE += MIN_BUFFSIZE;
			eachRED = realloc(eachRED, INPUT_BUFFSIZE);
			if(!eachRED) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
				return 0;
			}
		}
		temp = strtok(NULL, delim);
	}
	eachRED[pos] = temp;
	return(pos);
}


int SEMIparse(char** eachRED, char* string, ssize_t MIN_BUFFSIZE) {
	int pos=0;
	ssize_t INPUT_BUFFSIZE=MIN_BUFFSIZE;
	char *temp;
	char delim[1] = {';'};
	if(!eachRED) {
		fprintf(stderr, "Error reading input, failed to allocate memory");
		return 0;
	}
	temp = strtok(string, delim);
	while(temp != NULL) {
		eachRED[pos] = temp;
		pos++;
		if(pos >= INPUT_BUFFSIZE) {
			INPUT_BUFFSIZE += MIN_BUFFSIZE;
			eachRED = realloc(eachRED, INPUT_BUFFSIZE);
			if(!eachRED) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
				return 0;
			}
		}
		temp = strtok(NULL, delim);
	}
	eachRED[pos] = temp;
	return(pos);
}

int readInput(char *cwd, ssize_t MIN_BUFFSIZE) {
	int numOfChar=0;
	char *input = (char*)calloc(MIN_BUFFSIZE, sizeof(char));
	char each;
	int eachInt, i;
	ssize_t INPUT_BUFFSIZE=MIN_BUFFSIZE;
	if(input == NULL) {
		fprintf(stderr, "Error reading input, failed to allocate memory");
		return 0;
	}
	while(1) {
		each = getchar();
		eachInt = (int)each;
		if(eachInt == EOF || eachInt == '\n') {
			if(eachInt == EOF) {
				printf("\nExiting shell\n");
				exit(0);
			}
			input[numOfChar] = '\0';
			break;
		}
		else {
			input[numOfChar] = eachInt;
		}
		numOfChar++;
		if(numOfChar==INPUT_BUFFSIZE) {
			INPUT_BUFFSIZE+=MIN_BUFFSIZE;
			input = realloc(input, INPUT_BUFFSIZE);
			if (input == NULL) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
				return 0;
			}
		}
	}
	// It's better if we dont use strtok to tokenise the command? (escape character) : DO IT MANUALLY?
	numOfChar=0;
	char *temp, semi=';';
	char **eachInput = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	numOfChar = SEMIparse(eachInput, input, MIN_BUFFSIZE);
	// Splitting on the basis of space and tab
	int pos;
	for(i=0;i<numOfChar;i++) {

/*
		Parsing for < , > , >> , |
*/

		int totalCommands, total;
		char **splitCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
		totalCommands =  PIPEparse(splitCommand, eachInput[i], MIN_BUFFSIZE);
		if(totalCommands>1)	
			pipeCommand(splitCommand, totalCommands-1, MIN_BUFFSIZE, cwd);
		else {
			redirection(splitCommand, 1, MIN_BUFFSIZE, cwd);
		}
	}
	return(1);
}