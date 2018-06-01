#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>

#define BUFSIZE 256

ssize_t readln(int fildes, void *buffer, size_t nbyte){
	char *cbuffer = (char*) buffer;
	char c;
	int i=0;
	while(read(fildes, &c, nbyte) && c != '\n'){
		cbuffer[i] = c;
		i++;
	}
	if(c == '\n')
		cbuffer[i++] = '\n';
	cbuffer[i] = '\0';
	return i;
}

size_t gatherArg(char* arg[], char* buffer, size_t size){
	int i, j, l = 0, x;
	for(i = 0; i < size ; i++){
		arg[l] = (char*)malloc(32 *sizeof(char));
		char *elem = (char*) arg[l];
		for(j = i ,x = 0; j < size && buffer[j] != ' ' && buffer[j] != '\n'; j++, x++){
			elem[x] = buffer[j];
		}
		elem[x] = '\0';
		l++;
		i = j;
	}
	arg[l] = NULL;
	return l;
}

void randomName(char* dir){
	srand(time(0));
	int ascii;
	char c;
	for(int i = 0; i<8; i++){
		ascii = (rand() % 26) + 97;
		c = ascii;
		dir[i] = c;
	}
	dir[8] = '\0';
}

int analyse(char* buffer, ssize_t size){
	if(size > 2){
		if(buffer[0]== '$'){
			if(buffer[1] =='|'){
				return -2;
			}
			if(buffer[1] == ' '){
				return -1;
			}
			char num[1000];
			int res = -1;
			for(int i = 1, j = 0; buffer[i]!='\0' && buffer[i]!='|' ; i++, j++){
				int test = buffer[i];
				if(test > 47 && test < 58)
					num[j] = buffer[i];
				else
					return 0;
			}
			return atoi(num);
		}
		if(buffer[0] == '>'){
			if(buffer[1] == '>'){
				if(buffer[2] == '>'){
					return -3;
				}
			}
		}
		if(buffer[0] == '<'){
			if(buffer[1] == '<'){
				if(buffer[2] == '<'){
					return -4;
				}
			}
		}
	}
	return 0;
}

void execute(char* arg[],  ssize_t num, char* dir, int execs){
	size_t size = BUFSIZE;
	if(!fork()){
		if(!fork()){
			char file[size];
			snprintf(file, size , "./%s/%s", dir, "Pipeline");
			int fd = open(file, O_WRONLY, 0640);
			dup2(fd,1);
			close(fd);
			int x = execvp(arg[1], &arg[1]);
			if(x == -1){
				perror("Execução falhada!\n");
				exit(0);
			}
		}
		else{
			char file[size];
			snprintf(file, size, "./%s/%s", dir, "Pipeline");
			int fd = open(file, O_RDONLY | O_TRUNC, 0640);
			dup2(fd,0);
			close(fd);
			char file2[size];
			snprintf(file2, size, "./%s/%s", dir, "tmp.txt");
			int fd2 = open(file2, O_WRONLY | O_APPEND);
			dup2(fd2, 1);
			close(fd2);
			char file3[size];
			snprintf(file3, size, "./%s/%d", dir, execs);
			int fd3 = open(file3, O_RDWR | O_CREAT, 777);
			char c;
			write(1,">>>\n",4);
			while(read(0, &c, 1) > 0){
				write(1, &c, 1);
				write(fd3, &c, 1);
			}
			close(fd3);
			write(1,"<<<\n",4);
			exit(0);
		}
	}
	else{
		wait(0);
	}
}

void executeNumPipe(char* arg[], ssize_t num, char* dir, int execs, int numexec){
	size_t size = BUFSIZE;
	if(!fork()){
		if(!fork()){
			int p[2];
			pipe(p);
			if(!fork()){
				dup2(p[0],0);
				char file[size];
				snprintf(file, size, "./%s/%s", dir, "Pipeline");
				int fd = open(file, O_WRONLY | O_TRUNC);
				dup2(fd, 1);
				close(p[1]);
				close(p[0]);
				close(fd);
				int x = execvp(arg[1], &arg[1]);
				if(x == -1){
					perror("Execução falhada!\n");
					exit(0);
				}			
			}
			else{
				dup2(p[1], 1);
				char file[size];
				int ex = execs - numexec;
				snprintf(file, size, "./%s/%d", dir, ex);
				int fd = open(file, O_RDONLY);
				dup2(fd, 0);
				close(p[0]);
				close(fd);
				char c;
				while(read(0, &c, 1) > 0){
					write(1, &c, 1);
				}
				close(p[1]);
				exit(0);

			}
		}
		else{
			wait(0);
			char file[size];
			snprintf(file, size, "./%s/%s", dir, "Pipeline");
			int fd = open(file, O_RDONLY);
			dup2(fd,0);
			close(fd);
			char file2[size];
			snprintf(file2, size, "./%s/%s", dir, "tmp.txt");
			int fd2 = open(file2, O_WRONLY | O_APPEND);
			dup2(fd2, 1);
			close(fd2);
			char file3[size];
			snprintf(file3, size, "./%s/%d", dir, execs);
			int fd3 = open(file3, O_RDWR | O_CREAT, 777);
			char c;
			write(1,">>>\n",4);
			while(read(0, &c, 1) > 0){
				write(1, &c, 1);
				write(fd3, &c, 1);
			}
			close(fd3);
			write(1,"<<<\n",4);
			exit(0);
		}
	}
	else{
		wait(0);
	}
}

void printline(char* buffer, size_t n, char* dir){
	size_t size = BUFSIZE;
	if(!fork()){
		char file[size];
		snprintf(file, size, "./%s/%s", dir, "tmp.txt");
		int fd = open(file, O_WRONLY | O_APPEND);
		dup2(fd, 1);
		close(fd);
		write(1, buffer, n);
		if(buffer[n-1] != '\n')
			write(1, "\n", 1);
		exit(0);
	}
	else{
		wait(0);
	}
}