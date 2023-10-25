#include "ku_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  int processNum = atoi(argv[1]);
  
  for (int i = 0; i < processNum; i++) {
    if (fork() == 0) {
      int i = 3282;
      i += 1;
      i *= 2;
      exit(EXIT_SUCCESS);
    }
  }
}