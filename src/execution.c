#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "./read_input.h"
#include "./changedir.h"
#include "global.h"
#include <math.h>

void fg(char **args) {
	int i, status, t, tmp;
	pid_t pid,pgid,child_pid;
	bgALL *temp =bg;
	for(i=0;i<strlen(args[1]);i++){
		tmp = args[1][i];
		t+=(tmp-48)*pow(10, strlen(args[1])-i-1);
	}
	i=0;
	while(args[i]!=NULL)
		i++;
	if(i==2){
		while(temp!=NULL && --t)
  			temp=temp->next;
		if(temp!=NULL)
  			pid=temp->pid;
		else {
  			pid=-1;
  			printf("Error : No such job exists\n");
		}
		if(pid>=0) {
			pgid=getpgid(pid);
			tcsetpgrp(STDIN_FILENO,pgid);
			child_pid=pgid;
			printf("Process %s is in fg\n", temp->name);
  			if(kill(pgid,SIGCONT)<0)
    			perror("Cannot Continue");
  			waitpid(pid,&status,WUNTRACED);
  			if(WIFSTOPPED(status))
    			fprintf(stderr, "\n[%d]+ stopped %s\n",child_pid,temp->name );
  			else {
			    bgREMOVE(pid);
			    child_pid=0;
  			}
  			tcsetpgrp(STDIN_FILENO,getpgrp());
		}
	}
	else
  		printf("Invalid arguments\n" );
}

void bgINSERT(char name[1024], pid_t pid) {
	bgALL *new=(bgALL*)malloc(sizeof(bgALL));
	strcpy(new->name,name);
	new->pid=pid;
	new->pgid=getpgid(pid);
	new->next=NULL;
	if(bg==NULL)
		bg=new;
	else {
		bgALL *temp=bg;
		while(temp->next!=NULL)
			temp=temp->next;
		temp->next=new;
	}
}

void bgREMOVE(pid_t pid) {
	if(bg!=NULL) {
		bgALL *temp=bg;
		if(bg->pid==pid){
	  		bg=bg->next;
	  		free(temp);
	  	} 
		else {
			bgALL *temp2;
			while(temp!=NULL && temp->pid!=pid) {
	    		temp2=temp;
	    		temp=temp->next;
	    	}
			if(temp!=NULL) {
	    		temp2->next=temp->next;
	    		free(temp);	
			}
		}
	}
}

void bgResponse(int signal) {
	pid_t pid;
	int status;
	while ((pid = waitpid(-1, &status, WNOHANG)) != -1) {
		printf("Child with pid %d has exited with exit status %d\n",  pid, WEXITSTATUS(status));
		break;
	}
	bgREMOVE(pid);
}

void pinfo(int pid, char** args, char* cwd) {
	FILE * fp;
	char name[1024], rev[100], path[1024];
	char *line = NULL;
	ssize_t read;
	size_t len=0, dest_length=1024;
	int delim='\n', i=6, temp=pid, j=0;
	name[0]='/'; name[1]='p'; name[2]='r'; name[3]='o'; name[4]='c'; name[5]='/'; name[6]='\0';
	if(args[1] != NULL)	{
		strcat(name, args[1]);
		i=i+strlen(args[1]);
	}
	else {
		printf("This is the current shell info (No arguments provided)\n");
		while(temp!=0) {
			rev[j]=(char)((temp%10) + '0');
			j++;
			temp=temp/10;
		}
		rev[j]='\0';
		j=strlen(rev)-1;
		while(j>=0) {
			name[i]=rev[j];
			j--; i++;
		}
	}
	name[i]='/'; name[i+1]='s'; name[i+2]='t'; name[i+3]='a'; name[i+4]='t'; name[i+5]='u'; name[i+6]='s'; name[i+7]='\0';
	fp = fopen(name, "r");	
	if(fp == NULL) {
		perror("Error(No such Process) ");
		return;
	}
	while(read=getdelim(&line, &len, delim, fp)!=-1) {
		if(line[0]=='N' && line[1]=='a' && line[2]=='m' && line[3]=='e')
			printf("%s", line);
		else if(line[0]=='S' && line[1]=='t' && line[2]=='a' && line[3]=='t' && line[4]=='e')
			printf("%s", line);
		else if(line[0]=='P' && line[1]=='i' && line[2]=='d')
			printf("%s", line);
		else if(line[0]=='V' && line[1]=='m' && line[2]=='S' && line[3]=='i')
			printf("(Virtual Memory allotted) %s", line);
	}
	name[i+1]='e'; name[i+2]='x'; name[i+3]='e'; name[i+4]='\0'; j=0;
	printf("Executable Path: ");
	if (readlink (name, path, dest_length) != -1){
		while(cwd[j] != '\0') {
			if(cwd[j] != path[j]) {
				fprintf(stdout, "%s\n", path);
				fclose(fp);
				if (line)
					free(line);
				return;
			}
			j++;
		}
		fprintf(stdout, "~");
		printf("%s\n", &path[j]);
    }
	fclose(fp);
    if (line)
        free(line);
}

int execCommand(char **args, int total) {
	pid_t pid, wpid;
	int status;
	if(args[total-1][0] == '&') {
		signal(SIGCHLD,bgResponse);
		pid = fork();
		args[total-1] = NULL;
		if (pid == 0) {
			if (execvp(args[0], args) < 0) {
				perror("Error: ");
			}
			exit(EXIT_FAILURE);
		}
		else if (pid < 0) {
			perror("Error: ");
		}
		else {
			bgINSERT(args[0], pid);
			//printf("%d\n", pid);
		}
	}
	else {
		PID = fork();
		strcpy(NAME, args[0]);
		if (PID == 0) {
			//printf("%d %d %d\n", getpid(), getppid(), getpgrp());
			if (execvp(args[0], args) < 0) {
				perror("Error: ");
			}
			exit(EXIT_FAILURE);
		}
		else if (PID < 0) {
			perror("Error: ");
		} 
		else {
			wpid = waitpid(PID, &status, WUNTRACED);
			while (!WIFEXITED(status) && !WIFSIGNALED(status) && !(WIFSTOPPED(status))) {
				wpid = waitpid(PID, &status, WUNTRACED);	
			}
		}
		return 1;
	}
}

int sendSIG(char* proc, char* sig) {
	int procLEN=strlen(proc), sigLEN=strlen(sig), i, procID=0, sigID=0,tmp;
	for(i=0;i<procLEN;i++){
		tmp = proc[i];
		procID+=(tmp-48)*pow(10, procLEN-i-1);
	}
	for(i=0;i<sigLEN;i++){
		tmp = sig[i];
		sigID+=(tmp-48)*pow(10, sigLEN-i-1);
	}
	if(kill(procID, sigID) != 0) {
		perror("Error (sending signal) 	");
	}
}

int listallBG() {
	bgALL *temp=bg;
	int i=1;
	if(bg==NULL)
		printf("No background process running\n");
	while(temp!=NULL) {
		printf("[%d] : %s : [%d]\n", i, temp->name, temp->pid);
		temp=temp->next;
		i++;
	}
}

int killallBG() {
	bgALL *prev;
	while(bg->next!=NULL) {
		prev=bg;
		if(kill(prev->pid, SIGKILL) < 0)
			perror("Error in putting it the process to bg ");
		bg=bg->next;
		free(prev);
	}
	if(bg!=NULL) {
		if(kill(bg->pid, SIGKILL) < 0)
			perror("Error in putting it the process to bg ");
		free(bg);
	}
	bg=NULL;
}

int execution(char** eachCommand, int pos, char* next, int ttt, char* cwd) {
	int MIN_BUFFSIZE=1024;
	if(!(strcmp(eachCommand[0], "cd")))
		changeDir(next, ttt, cwd, MIN_BUFFSIZE);
	else if(!(strcmp(eachCommand[0], "pwd")))
		currentDir(eachCommand, pos, MIN_BUFFSIZE);
	else if(!(strcmp(eachCommand[0], "exit")))
		exit(0);
	else if(!(strcmp(eachCommand[0], "fg")))
		fg(eachCommand);
	else if(!(strcmp(eachCommand[0], "listjobs")))
		listallBG();
	else if(!(strcmp(eachCommand[0], "killallbg")))
		killallBG();
	else if(!(strcmp(eachCommand[0], "kjob"))) {
		if(eachCommand[1]!=NULL&&eachCommand[2]!=NULL)
			sendSIG(eachCommand[1], eachCommand[2]);
		else
			printf("Error: Invalid args\n");
	}
	else if(!(strcmp(eachCommand[0], "echo"))) {
		echoSTDOUT(next, ttt, MIN_BUFFSIZE);
	}
	else if(!(strcmp(eachCommand[0], "pinfo"))) {
		pinfo(getpid(), eachCommand, cwd);
	}
	else{
		execCommand(eachCommand, pos);
	}
}

int redirectWRITE(char **args, ssize_t MIN_BUFFSIZE, char* next, int ttt, char* cwd) {
	int in, out, total;
	char **nextCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	SPACEparse(nextCommand, args[1], MIN_BUFFSIZE);
	out = open(nextCommand[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	char **eachCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	total = SPACEparse(eachCommand, args[0], MIN_BUFFSIZE);
	int saved_stdout;
	saved_stdout = dup(1);
	dup2(out, 1);
	execution(eachCommand, total, next, ttt, cwd);
	dup2(saved_stdout, 1);
	close(saved_stdout);
	close(out);
}

int redirectREAD(char **args, ssize_t MIN_BUFFSIZE, char* next, int ttt, char* cwd) {
	int in, out, total;
	char **nextCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	SPACEparse(nextCommand, args[1], MIN_BUFFSIZE);
	in = open(nextCommand[0], 'r');
	char **eachCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	total = SPACEparse(eachCommand, args[0], MIN_BUFFSIZE);
	int saved_stdin;
	saved_stdin = dup(0);
	dup2(in, 0);
	execution(eachCommand, total, next, ttt, cwd);
	dup2(saved_stdin, 0);
	close(saved_stdin);
	close(in);
}

int redirectAPPEND(char **args, ssize_t MIN_BUFFSIZE, char* next, int ttt, char* cwd) {
	int in, out, total;
	char **nextCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	SPACEparse(nextCommand, args[1], MIN_BUFFSIZE);
	FILE *test=fopen(nextCommand[0], "a");
    out=fileno(test);
	//out = open(nextCommand[0], 'a');
	char **eachCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	total = SPACEparse(eachCommand, args[0], MIN_BUFFSIZE);
	int saved_stdout;
	saved_stdout = dup(1);
	dup2(out, 1);
	execution(eachCommand, total, next, ttt, cwd);
	dup2(saved_stdout, 1);
	close(saved_stdout);
	close(out);
}

int redirectSERIES(char **args, ssize_t MIN_BUFFSIZE, char append, char* next, int ttt, char* cwd) {
	int in, out, total;
	char **nextCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	SPACEparse(nextCommand, args[2], MIN_BUFFSIZE);
	if(append == 'a') {	
		FILE *test=fopen(nextCommand[0], "a");
    	out=fileno(test);
	}
	else
		out = open(nextCommand[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	in = open(args[1], 'r');
	char **eachCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
	total = SPACEparse(eachCommand, args[0], MIN_BUFFSIZE);
	int saved_stdout, saved_stdin;
	saved_stdout = dup(1);
	saved_stdin = dup(0);
	dup2(out, 1);
	dup2(in, 0);
	execution(eachCommand, total, next, ttt, cwd);
	dup2(saved_stdin, 0);
	dup2(saved_stdout, 1);
	close(saved_stdout);
	close(saved_stdin);
	close(out);
}

int redirection(char** splitCommand, int totalCommands, ssize_t MIN_BUFFSIZE, char* cwd) {
	int p=0, q=0, pos, l, total;
	int ttt=0, eachInt;
	ssize_t INPUT_BUFFSIZE=MIN_BUFFSIZE;
	for(p=0;p<(totalCommands);p++) {
		char delim[2] = {'>', '<'};
		char red;
		l=0;
		while(splitCommand[p][l] != '\0') {
			if(splitCommand[p][l]=='>') {
				if(splitCommand[p][l+1] != '>')
					red='>';
				else {
					red='a';
					splitCommand[p][l]=' ';
					l++;
				}
			}
			else if(splitCommand[p][l]=='<')
				red='<';
			l++;
		}
		char **eachRED = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
		pos = REDparse(eachRED, splitCommand[p], MIN_BUFFSIZE); 
		INPUT_BUFFSIZE=MIN_BUFFSIZE;
		ttt=0;
		char *next = (char*)malloc(MIN_BUFFSIZE*sizeof(char));
		if(next == NULL) {
			fprintf(stderr, "Error reading input, failed to allocate memory");
		}
		while(1) {
			eachInt = (int)eachRED[0][ttt];
			if(eachInt == EOF || eachInt == '\n' || eachInt == '\0') {
				next[ttt] = '\0';
				break;
			}
			else {
				next[ttt] = eachInt;
			}
			ttt++;
			if(ttt==INPUT_BUFFSIZE) {
				INPUT_BUFFSIZE+=MIN_BUFFSIZE;
				next = realloc(next, INPUT_BUFFSIZE);
				if (next == NULL) {
					fprintf(stderr, "Error reading input, failed to allocate memory");
				}
			}
		}
		if(pos==2 && red=='>') {
			redirectWRITE(eachRED, MIN_BUFFSIZE, next, ttt, cwd);
		}						
		else if(pos==2 	&& red=='<') {
			redirectREAD(eachRED, MIN_BUFFSIZE, next, ttt, cwd);
		}
		else if(pos==2 && red=='a') {
			redirectAPPEND(eachRED, MIN_BUFFSIZE, next, ttt, cwd);
		}
		else if(pos==3) {
			if(red=='a')
				redirectSERIES(eachRED, MIN_BUFFSIZE, 'a', next, ttt, cwd);
			else
				redirectSERIES(eachRED, MIN_BUFFSIZE, '>', next, ttt, cwd);
		}
		else {
			char **eachCommand = (char**)calloc(MIN_BUFFSIZE, sizeof(char*));
			total = SPACEparse(eachCommand, eachRED[0], MIN_BUFFSIZE);
			execution(eachCommand, total, next, ttt, cwd);
		}
	}
}

int pipeCommand(char **args, int numOfPipes, ssize_t MIN_BUFFSIZE, char* cwd) {
	int status=0;
	pid_t pid, wpid;
	int i, j, total;
	int pipes[2*numOfPipes];
	char *cat_args[] = {"cat", "scores", NULL};
	char *grep_args[] = {"grep", "Villanova", NULL};
	char *cut_args[] = {"cut", "-b", "1-10", NULL};
	char *ls_args[] = {"ls", NULL};
	char *grep2_args[] = {"grep", "a", NULL};
	for(i=0;i<(2*numOfPipes);i+=2) {
		pipe(pipes + i);
	}
	// that the 1st and last only deal with the end of one pipe.
	int k;
	for(i=-2;i<(2*numOfPipes);i+=2) {
		//for(k=0;k<(2*numOfPipes);k+=1)
			//printf("%d ", pipes[k]);
			//printf("\n");
		if ((pid = fork()) == 0) {
			//printf("reading from %d writing on %d\n", i, i+3);
			if((i+3)<(2*numOfPipes))
				dup2(pipes[i+3], 1);
			if(i!=-2)
				dup2(pipes[i], 0);
			// close all pipes (very important!); end we're using was safely copied
			for(j=0;j<(2*numOfPipes);j++)
				close(pipes[j]);
			// execvp
			char** eachCommand = &args[(i+2)/2];
			redirection(eachCommand, 1, MIN_BUFFSIZE, cwd);
			exit(EXIT_FAILURE);
		}
		else {
			/*
			wpid = waitpid(pid, &status, WUNTRACED);
			while (!WIFEXITED(status) && !WIFSIGNALED(status)) {
				wpid = waitpid(pid, &status, WUNTRACED);	
			}*/
		}
	}
	for(i=0;i<(2*numOfPipes);i++)
		close(pipes[i]);
	while ((wpid = wait(&status)) > 0);
	return 0;
}