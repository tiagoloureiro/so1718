#include "lib/helpers.h"

int main(int argc, char *argv[]) {

	int bug = 1;
	// Open file that is received as an argument by the program
	int fd = open(argv[1], O_RDWR, 0640);	
	// Creates pipeline to save last command result
	char dir[9];
	randomName(dir);
	mkdir(dir, 0777);
	char file[BUFSIZE];
	snprintf(file, BUFSIZE, "./%s/%s", dir, "Pipeline");
	mkfifoat(AT_FDCWD,file, 0640);
	snprintf(file, BUFSIZE, "./%s/%s", dir, "tmp.txt");
	open(file, O_CREAT | O_TRUNC, 0640);
	snprintf(file, BUFSIZE, "./%s/%s", dir, "error.txt");
	int fd2 = open(file, O_CREAT | O_RDWR | O_TRUNC, 0640);
	
	if(fd < 0){
		perror("The file does not exists\n");
		exit(1);
	}
	dup2(fd,0);
	close(fd);
	dup2(fd2, 2);
	close(fd2);
	char *buffer;
	buffer = (char*)malloc(BUFSIZE * sizeof(char));
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
		if(test == -3){
			flag = 1;
		}
		if(test == -4){
			flag = 0;
		}
		if(flag == 0){
			if(test == 0){
				printline(buffer, n, dir);
			}
			if(test == -1){
				num = gatherArg(arg, buffer, n);
				execute(arg, num, dir, execs);
				execs++;
			}
			if(test == -2){
				num = gatherArg(arg, buffer, n);
				executePipe(arg, num, dir, execs);
				execs++;
			}
			if(test > 0){
				if((execs - test) > 0){
					num = gatherArg(arg, buffer, n);
					executeNumPipe(arg, num, dir, execs, test);
					execs++;
				}
				else{
					perror("Comando não existente!\n");
					exit(0);
				}
			}	
		}
		snprintf(file, BUFSIZE, "./%s/%s", dir, "error.txt");
		int error = open(file, O_RDONLY);
		if(read(error, NULL, 1)!=0){
			dup(1);
			write(1, "Execution error!\nCheck errors on file error.txt.\n",61);
			snprintf(file, BUFSIZE, "./%s/%s", dir, "Pipeline");
			unlink(file);
			snprintf(file, BUFSIZE, "./%s/%s", dir, "tmp.txt");
			unlink(file);
			for(int i = 1; i < execs; i++){
				snprintf(file, BUFSIZE, "./%s/%d", dir, i);
				unlink(file);
			}
			return -1;
		}
	}
	snprintf(file, BUFSIZE, "./%s/%s", dir, "tmp.txt");
	int filetmp = open(file, O_RDONLY);
	int filenb = open(argv[1], O_WRONLY | O_TRUNC);
	dup2(filetmp, 0);
	dup2(filenb, 1);
	close(filetmp);
	close(filenb);
	char c;
	while((read(0, &c, 1)) >= 1){
		write(1, &c, 1);
	}
	snprintf(file, BUFSIZE, "./%s/%s", dir, "Pipeline");
	unlink(file);
	snprintf(file, BUFSIZE, "./%s/%s", dir, "tmp.txt");
	unlink(file);
	snprintf(file, BUFSIZE, "./%s/%s", dir, "error.txt");
	unlink(file);
	for(int i = 1; i < execs; i++){
		snprintf(file, BUFSIZE, "./%s/%d", dir, i);
		unlink(file);
	}
	rmdir(dir);
	return 0;
}