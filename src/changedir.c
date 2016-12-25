#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <errno.h>

void currentDir(char **args, int total, ssize_t MIN_BUFFSIZE){
	char* ret_char;
	char buffer[1024];
	ret_char = getcwd(buffer, MIN_BUFFSIZE);
	if(!ret_char) {
		perror("Error: ");
		return;
	}
	else
		fprintf(stdout, "%s", buffer);
	fprintf(stdout, "\n");
}

void changeDir(char *args, int total, char *cwd, ssize_t MIN_BUFFSIZE){
	int i=0, SEP=0, k=0;
	char sep;
	char tempStore[1024];
	int length;
	while(1) {
		if(args[i] == 'c')
			break;
		i++;
	}
	i=i+2;
	while(args[i]==' '){
		i++;
	}
	if(args[i] == '~') {
		tempStore[0]='\0';
		strcat(tempStore, cwd);
		k=k+strlen(cwd);
		i++;
	}
	while(args[i]!='\0') {
		if(SEP == 1) {
			if(sep != args[i]) {
				tempStore[k]=args[i];
				k++;
			}
			else
				SEP=0;
		}
		else {
			if(args[i] == '"') {
				SEP=1;
				sep='"';
			}
			else if(args[i] == '\'') {
				SEP=1;
				sep='\'';
			}
			else if(args[i] == ' ')
				break;
			else {
				tempStore[k]=args[i];
				k++;
			}
		}
		i++;
	}
	if(SEP == 1) {
		int numOfChar;
		char each;
		int eachInt, j;
		ssize_t INPUT_BUFFSIZE;
		while(SEP == 1) {
			numOfChar=0;
			fprintf(stdout, " >");
			INPUT_BUFFSIZE=MIN_BUFFSIZE;
			char *input = (char*)malloc(MIN_BUFFSIZE*sizeof(char));
			if(input == NULL) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
			}
			while(1) {
				each = getchar();
				eachInt = (int)each;
				if(eachInt == EOF || eachInt == '\n') {
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
					}
				}
			}
			i=0;
			while(input[i]!='\0') {
				if(SEP == 1) {
					if(sep != input[i]) {
						tempStore[k]=input[i];
						k++;
					}
					else
						SEP=0;
				}
				else {
					if(input[i] == '"') {
						SEP=1;
						sep='"';
					}
					else if(input[i] == '\'') {
						SEP=1;
						sep='\'';
					}
					else if(args[i] == ' ')
						break;
					else {
						tempStore[k]=args[i];
						k++;
					}		
				}
				i++;
			}
			free(input);
		}
	}
	length=k;
	if(length == 0) {
		tempStore[0]='\0';
		strcat(tempStore, cwd);
		k=k+strlen(cwd);
	}
	tempStore[k]='\0';
	printf("%s\n", tempStore);
	if(chdir(tempStore) < 0) {
		perror("Error: ");
	}
}

void echoSTDOUT(char *args, int total, ssize_t MIN_BUFFSIZE){
	int i=0, space=1, SEP=0, k=0;
	char sep;
	char *tempStore = (char*)malloc((total+1)*sizeof(char));
	while(1) {
		if(args[i] == 'e')
			break;
		i++;
	}
	i=i+4;
	while(args[i]!='\0') {
		if(SEP == 1) {
			if(sep != args[i]) {
				tempStore[k]=args[i];
				k++;
			}
			else
				SEP=0;
		}
		else {
			if(args[i] == '"') {
				SEP=1;
				sep='"';
			}
			else if(args[i] == '\'') {
				SEP=1;
				sep='\'';
			}
			else if(space == 1) {
				if(args[i] != ' ') {
					tempStore[k]=args[i];
					k++;
					space=0;
				}
			}
			else {
				if(args[i] != ' ') {
					tempStore[k]=args[i];
					k++;
				}
				else {
					tempStore[k]=args[i];
					k++;
					space=1;
				}
			}
		}
		i++;
	}
	if(SEP == 1) {
		int numOfChar;
		char each;
		int eachInt, j;
		ssize_t INPUT_BUFFSIZE;
		while(SEP == 1) {
			tempStore[k]='\n';
			k++;
			numOfChar=0;
			fprintf(stdout, " >");
			INPUT_BUFFSIZE=MIN_BUFFSIZE;
			char *input = (char*)malloc(MIN_BUFFSIZE*sizeof(char));
			if(input == NULL) {
				fprintf(stderr, "Error reading input, failed to allocate memory");
			}
			while(1) {
				each = getchar();
				eachInt = (int)each;
				if(eachInt == EOF || eachInt == '\n') {
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
					}
				}
			}
			i=0;
			while(input[i]!='\0') {
				if(SEP == 1) {
					if(sep != input[i]) {
						tempStore[k]=input[i];
						k++;
					}
					else
						SEP=0;
				}
				else {
					if(input[i] == '"') {
						SEP=1;
						sep='"';
					}
					else if(input[i] == '\'') {
						SEP=1;
						sep='\'';
					}
					else if(space == 1) {
						if(input[i] != ' ') {
							tempStore[k]=input[i];
							k++;
							space=0;
						}
					}
					else {
						if(input[i] != ' ') {
							tempStore[k]=input[i];
							k++;
						}
						else {
							tempStore[k]=input[i];
							k++;
							space=1;
						}
					}
				}
				i++;
			}
			free(input);
		}
	}
	tempStore[k]='\0';
	printf("%s\n", tempStore);
}