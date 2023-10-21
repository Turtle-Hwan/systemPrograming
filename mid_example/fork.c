#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void fork1()
{
  int x = 1;
  pid_t pid = fork();
  if (pid == 0)
  {
    printf("Child pid:%d has x = %d\n", getpid(), ++x);
  }
  else
  {
    printf("Parent pid:%d has x = %d\n", getpid(), --x);
  }
  printf("Bye from process %d with x = %d\n", getpid(), x);
}

void fork2()
{
  printf("L0\n");
  fork();
  printf("L1\n");
  fork();
  printf("Bye\n");
}

void fork3()
{
  printf("L0\n");
  fork();
  printf("L1\n");
  fork();
  printf("L2\n");
  fork();
  printf("Bye\n");
}

void fork4()
{
  printf("L0\n");
  if (fork() != 0)
  {
    printf("L1\n");
    if (fork() != 0)
    {
      printf("L2\n");
      fork();
    }
  }
  printf("Bye\n");
}

void fork5()
{
  printf("L0\n");
  if (fork() == 0)
  {
    printf("L1\n");
    if (fork() == 0)
    {
      printf("L2\n");
      fork();
    }
  }
  printf("Bye\n");
}

void cleanup(void)
{
  printf("cleaning up\n");
}
void fork6()
{
  atexit(cleanup);
  fork();
  exit(0);
}

void fork7()
{
  if (fork() == 0)
  {
    /* Child */
    printf("Terminating Child, PID = %d\n", getpid());
    exit(0);
  }
  else
  {
    printf("Running Parent, PID = %d\n",getpid());
    while (1)
      ; /* Infinite loop */
  }
}

void fork8()
{
  if (fork() == 0)
  {
    /* Child */
    printf("Running Child, PID = %d\n", getpid());
    while (1)
      ; /* Infinite loop */
  }
  else
  {
    printf("Terminating Parent, PID = %d\n",getpid());
    exit(0);
  }
}

void fork9() {
   int child_status;  

   if (fork() == 0) {
      printf("HC: hello from child\n");
   }
   else {
      printf("HP: hello from parent\n");
      wait(&child_status);
      printf("CT: child has terminated\n");
   }
   printf("Bye\n");
   exit();
}

#define N 10
void fork10()
{
  pid_t pid[N];
  int i, child_status;
  for (i = 0; i < N; i++)
    if ((pid[i] = fork()) == 0)
      exit(100 + i); /* Child */
  for (i = 0; i < N; i++)
  {
    pid_t wpid = wait(&child_status);
    if (WIFEXITED(child_status))
      printf("Child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
    else
      printf("Child %d terminate abnormally\n", wpid);
  }
}

void fork11()
{
  pid_t pid[N];
  int i;
  int child_status;
  for (i = 0; i < N; i++)
    if ((pid[i] = fork()) == 0)
      exit(100 + i); /* Child */
  for (i = 0; i < N; i++)
  {
    pid_t wpid = waitpid(pid[i], &child_status, 0);
    if (WIFEXITED(child_status))
      printf("Child %d terminated with exit status %d\n",
             wpid, WEXITSTATUS(child_status));
    else
      printf("Child %d terminated abnormally\n", wpid);
  }
}

int main(int argc, char **argv)
{
  if (argc == 1)
    printf("인수 입력 필요\n");
  if (argc >= 2)
  {
    char *functionNum = argv[1];

    switch (*functionNum)
    {
    case '1':
      fork1();
      break;
    case '2':
      fork2();
      break;
    case '3':
      fork3();
      break;
    case '4':
      fork4();
      break;
    case '5':
      fork5();
      break;
    case '6':
      fork6();
      break;
    case '7':
      fork7();
      break;
    case '8':
      fork8();
      break;
    case '9':
      fork9();
      break;
    case '10':
      fork10();
      break;
    case '11':
      fork11();
      break;
    case '12':
      fork12();
      break;
    case '13':
      fork13();
      break;
    case '14':
      fork14();
      break;
    default:
      printf("번호 에러\n");
      break;
    }
  }
}