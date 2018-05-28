#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 256

ssize_t readln(int fildes, void *buffer, size_t nbyte);

size_t gatherArg(char* arg[], char* buffer, size_t size);

int analyse(char* buffer, ssize_t size);

void execute(char* arg[],  ssize_t num);

void executePipe(char* arg[], ssize_t num);

void printline(char* buffer, size_t n);