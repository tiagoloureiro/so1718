#include "lib/helpers.h"

int main(int argc, char *argv[]) {

	// Open file that is received as an argument by the program
	int fd = open(argv[1], O_RDWR, 0640);	
	// Creates pipeline to save last command result
	mkfifo("Pipeline", 0640);
	open("execBefore.txt", O_CREAT | O_TRUNC, 0640);
	open("tmp.txt", O_CREAT | O_TRUNC, 0640);
	char dir[8];
	randomName(dir);
	int fd2 = open("error.txt", O_CREAT | O_WRONLY | O_TRUNC, 0640);
	if(fd < 0){
		perror("The file does not exists\n");
		exit(1);
	}
	dup2(fd,0);
	close(fd);
	dup2(fd2, 2);
	close(fd2);
	char *buffer;
	size_t bufsize = BUFSIZE;
	buffer = (char*)malloc(bufsize * sizeof(char));
	if(buffer == NULL){
	 	perror("It is not possible to alloc buffer\n");
	}
	size_t n;
	int flag = 0;
	int execs = 1;
	while((n = readln(0, buffer, 1)) >= 1){
		char *arg[32];
		size_t num;
		int test;
		test = analyse(buffer, n);
		if(test == 3){
			flag = 1;
		}
		if(test == 4){
			flag = 0;
		}
		if(flag == 0){
			switch(test){
				case 0:{
					printline(buffer, n);
					break;
				}
				case 1:{
					num = gatherArg(arg, buffer, n);
					execute(arg, num, execs);
					execs++;
					break;
				}
				case 2:{
					num = gatherArg(arg, buffer, n);
					executePipe(arg, num, execs);
					execs++;
					break;
				}
			}
			int error = open("error.txt", O_RDONLY);
			if(read(error, NULL, 1)!=0){
				dup(1);
				write(1, "Execution error!\nCheck errors on file error.txt.\n",61);
				unlink("Pipeline");
				unlink("tmp.txt");
				unlink("execBefore.txt");
				return -1;
			}
		}
	}
	int filetmp = open("tmp.txt", O_RDONLY);
	int filenb = open(argv[1], O_WRONLY | O_TRUNC);
	dup2(filetmp, 0);
	dup2(filenb, 1);
	close(filetmp);
	close(filenb);
	char c;
	while((read(0, &c, 1)) >= 1){
		write(1, &c, 1);
	}
	unlink("Pipeline");
	unlink("tmp.txt");
	unlink("execBefore.txt");
	unlink("error.txt");

	return 0;
}