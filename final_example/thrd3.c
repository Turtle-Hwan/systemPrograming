#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

volatile int ncount;

void *do_loop(void *loop)
{
  while (1)
    ncount++;
  return NULL;
}

int main(int argc, char *argv[])
{
  int status, sec;
  pthread_t thread_id;

  ncount = 0;
  sec = atoi(argv[1]);

  status = pthread_create(&thread_id, NULL, do_loop, NULL); // do_loop thread main 함수
  if (status != 0)
  {
    perror("pthread_create");
    exit(1);
  }

  sleep(sec); // 인자로 들어온 시간만큼 잠들기
  pthread_cancel(thread_id);
  printf("counter = %d\n", ncount);

  return 0;
}