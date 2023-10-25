#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_STR 100

/* arguments */
typedef struct{
  int i;
  char str[MAX_STR];
}my_arg;

/* return value */
typedef struct{
  int i;
  char c;
}my_ret;

void *thread_routine(void *arg)
{
  my_arg *targ = (my_arg *)arg;
  my_ret *ret = malloc(sizeof(my_ret));

  if( strlen(targ->str) > targ->i ){  //string 범위 벗어나는지 확인
    ret->i = targ->i;
    ret->c = targ->str[ret->i];
  }
  else
    ret->i = -1;
  return ret;
}

int main (void)
{
  pthread_t thread_id;
  int status;
  my_arg arg;
  my_ret *ret;

  arg.i = rand() % MAX_STR; //시드 고정되어 있음.
  fgets(arg.str, MAX_STR, stdin);   //scanf는 buffer의 길이 명시 불가해서 overflow 가능성 때문에 fgets 쓰는 습관 들이자.
  status = pthread_create(&thread_id, NULL, thread_routine, &arg);

  if (status != 0){
    perror("create"); exit(1);
  }
  status = pthread_join(thread_id, (void **)&ret);
  if (status !=0){
    perror("join"); exit(1);
  }
  if( ret->i > -1 )
    printf("%dth character of %s is %c\n", ret->i, arg.str, ret->c);
  else
    printf("out of range\n");
  return 0;
}