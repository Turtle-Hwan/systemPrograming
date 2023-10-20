#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_ID 5

int main(void)
{
  key_t ipckey;
  int mqdes, i;
  size_t buf_len;
  struct  //같은 message 구조로 통신!
  {
    long id;
    int value;
  } mymsg;

  buf_len = sizeof(mymsg.value);

  ipckey = ftok("./tmp/foo", 1946); //동일한 key값을 가져야 동일한 큐로 통신하므로, ftok의 인자값을 똑같이 맞추어야 함.
  mqdes = msgget(ipckey, IPC_CREAT | 0600);   //sender가 먼저 실행되든, receiver가 먼저 실행되든 msgget으로 
  if (mqdes < 0)
  {
    perror("msgget()");
    exit(0);
  }
  for (i = 0; i <= MAX_ID; i++)
  {
    if (msgrcv(mqdes, &mymsg, buf_len, i + 1, 0) == -1)
    {
      perror("msgrcv()");
      exit(0);
    }
    else
      printf("Received a message(val: %d, id: %ld)\n", mymsg.value, mymsg.id);
  }
  return 0;
}