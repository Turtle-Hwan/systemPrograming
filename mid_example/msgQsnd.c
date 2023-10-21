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
  struct
  {
    long id;
    int value;
  } mymsg;

  buf_len = sizeof(mymsg.value);

  ipckey = ftok("./tmp/foo", 1946);   //원하는 프로그램 project magic number 넣으면 된다.
  mqdes = msgget(ipckey, IPC_CREAT | 0600);  //IPC_CREAT : ipckey값 가진 queue가 존재하면 fd 넘겨주고, 없으면 만들기 | 0600 : queue의 권한 설정 (user가 같을때만 read, write)     //IPC_CREAT와 0600 or 연산
  if (mqdes < 0)
  {
    perror("msgget()");   //오류인 경우 strerr 찍어주기
    exit(0);
  }
  for (i = 0; i <= MAX_ID; i++)   //매번 다른 id의 message와 value 보낸다.
  {
    mymsg.id = i + 1;
    mymsg.value = i * 3;
    printf("Sending a message(val: %d, id: %ld)\n", mymsg.value, mymsg.id);
    if (msgsnd(mqdes, &mymsg, buf_len, 0) == -1)
    {
      perror("msgsnd()");
      exit(0);
    }
  }
  return 0;
}