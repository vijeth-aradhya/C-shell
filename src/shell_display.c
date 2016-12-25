#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/wait.h>
#include <errno.h>

int displayShell(char *cwd, char* hostname, char* username, ssize_t MIN_BUFFSIZE) {
	struct passwd *user;
	int ret_int, i=0;
	user = getpwuid(getuid());
	if(!user) {
		perror("Error: ");
		return 0;
	}
	strcpy(username, user->pw_name);
	ret_int = gethostname(hostname, MIN_BUFFSIZE);
	if(ret_int<0) {
		perror("Error: ");
		return 0;
	}
	char* ret_char;
	char thisDir[1024];
	ret_char = getcwd(thisDir, MIN_BUFFSIZE);
	if(!ret_char) {
		perror("Error: ");
		return 0;
	}
	while(cwd[i] != '\0') {
		if(cwd[i] != thisDir[i]) {
			fprintf(stdout, "\033[32;1m<%s@%s:%s> \033[0m", username, hostname, thisDir);
			return(1);
		}
		i++;
	}
	fprintf(stdout, "\033[32;1m<%s@%s:~\033[0m", username, hostname);
	while(thisDir[i] != '\0') {
		fprintf(stdout, "\033[32;1m%c\033[0m", thisDir[i]);
		i++;
	}
	fprintf(stdout, "\033[32;1m> \033[0m");
	return(1);
}
