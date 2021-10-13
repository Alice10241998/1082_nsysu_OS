#include <stdio.h>
#include <string.h> // strtok(), strcmp()
#include <stdlib.h> // exit()
#include <unistd.h> // dup(), getcwd(), gethostname(), execlp()
#include <sys/types.h> // getuid(), geteuid()
#include <limits.h> // PATH_MAX, HOST_NAME_MAX
#include <pwd.h> // getpwuid(), struct passwd
#include <fcntl.h> // open(), close()...

#define STD_INPUT 0
#define STD_OUTPUT 1
#define TRUE 1
#define	MAXARG 30

void printPath();
struct passwd *user;

int main(void){
	pid_t pid1, pid2;
	int in_fd, out_fd;

	while(TRUE){
		printPath();

		char *command = malloc(sizeof(char) * MAX_INPUT);
		int cd = 0; // 0: false, 1: ture
		int background = 0, pipeline = 0, INredirect = 0, OUTredirect = 0;
		int OUTredirectA = 0; // IO Redirect append
		int fd[2]; // fd[0]: read from pipe,  fd[1]: write from pipe
		
		/* read command on the shell */
		if(fgets(command, MAX_INPUT, stdin) == NULL){
			printf("\n");
			exit(0);
		}
		if(strcmp(command,"exit\n") == 0 || strcmp(command,"quit\n") == 0) {
			exit(0);
		}
		command[strcspn(command, "\n")] = 0; // remove '\n'
		
		/* parsing command */
		char **parameters = malloc(sizeof(char*)*(MAXARG));
		char **parameters2 = malloc(sizeof(char*)*(MAXARG));
		int paraNum = 0; // number of paramter
		int paraNum2 = 0; // number of parameter 2 for pipeline

		// split string
		char delim[] = " "; // delimiter
		char *p = strtok(command, delim); // split string of command
		if(p == NULL){
			printf("\n");
			exit(0);
		}
		while(p != NULL){
			if(strcmp(p, ">") == 0){
				OUTredirect = 1;
				p = strtok(NULL, delim);
				continue;
			}
			else if(strcmp(p, ">>") == 0){
				OUTredirectA = 1;
				p = strtok(NULL, delim);
				continue;
			}
			else if(strcmp(p, "<") == 0) INredirect = 1;
			else if(strcmp(p, "|") == 0){
				pipeline = 1;
				p = strtok(NULL, delim);
				continue;
			}
			else if(strcmp(p, "&") == 0) background = 1;
			else if(strcmp(p, "cd") == 0) cd = 1;
			

			if(pipeline || OUTredirect || OUTredirectA){
				parameters2[paraNum2] = p;
				paraNum2++;
			}else{
				parameters[paraNum] = p;
				paraNum++;
			}

			p = strtok(NULL, delim);
		}

		/* Execuation */
		if(pipeline == 1){
			if(pipe(fd) < 0){
				printf("Error: Pipe failed.\n");
				exit(0);
			}
		}
		int status;
		if((pid1 = fork()) != 0){ /* parent process */
			if(pipeline == 1){
				pid2 = fork();
				if(pid2 != 0){
					/* parent process */
					close(fd[0]);
					close(fd[1]);
					waitpid(pid2, &status, 0); // wait child execute command2
				}else{
					/* child process */
					close(fd[1]);
					close(STD_INPUT);
					dup(fd[0]);
					close(fd[0]);
					execvp(parameters2[0], parameters2);
				}
			}
			if(background != 1) waitpid(pid1, &status, 0); // wait child until child terminate
		}
		/* child process */
		else{
			if(pipeline == 1){
				if(OUTredirect != 1 && OUTredirectA != 1){
					close(fd[0]);
					close(STD_OUTPUT);
					dup(fd[1]);
					close(fd[1]);
				}else{
					close(fd[0]);
					close(fd[1]);
					if(OUTredirect == 1){
						out_fd = open(parameters[paraNum], O_WRONLY|O_CREAT|O_TRUNC, 0666);
					}else if(OUTredirectA == 1){
						out_fd = open(parameters[paraNum-1], O_WRONLY|O_CREAT|O_APPEND, 0666);
					}
					close(STD_INPUT);
					dup2(out_fd, STD_OUTPUT);
					close(out_fd);
				}
			}

			// 0666 : -rw-rw-rw-
			if(OUTredirect == 1){
				out_fd = open(parameters2[0], O_WRONLY|O_CREAT|O_TRUNC, 0666);
				//close(STD_OUTPUT);
				dup2(out_fd, STD_OUTPUT);
				close(out_fd);
			}
			if(OUTredirectA == 1){
				out_fd = open(parameters2[0], O_WRONLY|O_CREAT|O_APPEND, 0666);
				close(STD_OUTPUT);
				dup2(out_fd, STD_OUTPUT);
				close(out_fd);
			}
			if(INredirect == 1){
				in_fd = open(parameters[2], O_RDONLY, 0666);
				close(STD_INPUT);
				dup2(in_fd, STD_INPUT);
				close(in_fd);
			}
			//if(cd == 1) chdir(parameters[1]);
			execvp(parameters[0], parameters);
			free(parameters);
			free(command);
		}
	}
	return 0;
}

void printPath(){
	const int hostLen = HOST_NAME_MAX;
	const int pathLen = PATH_MAX;
	/* print user path of message */
	char hostname[hostLen], path[pathLen];
	user = getpwuid(getuid());  // get user information

	// print host name 
	if(gethostname(hostname, hostLen) == 0) {
		// gethostname success
		printf("[HW1]%s@%s:", user->pw_name, hostname);
	}else{
		// gethostname fail
		printf("[HW1]%s@unknown:", user->pw_name);
	}
	getcwd(path, pathLen);
	printf("%s",path);
	(geteuid() == 0)? printf("# ") : printf("$ ");
}