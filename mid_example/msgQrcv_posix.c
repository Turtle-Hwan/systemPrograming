#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>

#define MSG_SIZE 4
#define NAME "/m_queue"
#define MAX_PRIO 5
int main(void)
{
  struct mq_attr attr;
  int value = 0, num_msg = 0;
  unsigned int prio;
  mqd_t mqdes;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MSG_SIZE;
  mqdes = mq_open(NAME, O_CREAT | O_RDWR, 0600, &attr);
  if (mqdes < 0)
  {
    perror("mq_open()");
    exit(0);
  }
  while (num_msg <= MAX_PRIO)
  {
    if (mq_receive(mqdes, (char *)&value, MSG_SIZE, &prio) == -1)
    {
      perror("mq_receive()");
      break;
    }
    else
    {
      printf("Received a message (val: %d, prio: %d)\n", value, prio);
      num_msg++;
    }
  }
  mq_close(mqdes);
  mq_unlink(NAME);
  return 0;
}