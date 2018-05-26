#include "main.h"

int main(int argc , char *argv[]){
	//Abre o ficheiro que o programa recebe como argumento.
	int fd = open(argv[1], O_RDWR, 0640);	
	//Cria um pipeline para guardar o resultado do último comando.
	mkfifo("Pipeline", 0640);
	open("execBefore.txt", O_CREAT | O_TRUNC, 0640);
	open("tmp.txt", O_CREAT | O_TRUNC, 0640);
	int fd2 = open("error.txt", O_CREAT | O_WRONLY | O_TRUNC, 0640);
	if(fd < 0){
		perror("Ficheiro inexistente.\n");
		exit(1);
	}
	dup2(fd,0);
	close(fd);
	dup2(fd2, 2);
	close(fd2);
	char *buffer;
	size_t bufsize = 256;
	buffer = (char*)malloc(bufsize * sizeof(char));
	if(buffer == NULL){
	 	perror("Não é possivel alocar o buffer\n");
	}
	size_t n;
	int flag = 0;
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
					execute(arg, num);
					break;
				}
				case 2:{
					num = gatherArg(arg, buffer, n);
					executePipe(arg, num);
					break;
				}
			}
			int error = open("error.txt", O_RDONLY);
			if(read(error, NULL, 1)!=0){
				dup(1);
				write(1, "Erro de execução!\nConsulte os erros no ficheiro erros.txt.\n",61);
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