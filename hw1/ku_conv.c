#include "ku_input.h"
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void makeChild(int processNum, int filter[][3])
{
  // pipe 만들기
  int pipefd[2];
  int pipeBuf[3]; //{x, y, n} output[x][y] = n

  if (pipe(pipefd) == -1)
  {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
  fcntl(pipefd[1], F_SETFL, O_NONBLOCK);
  
  //process 개수는 최대 총 연산할 개수만큼 되도록 내부적으로 맞춰주기
  if (processNum > (N - 2) * (N - 2)) {
    processNum = (N - 2) * (N - 2);
  }

  // 자식 processNum개 만큼 fork하는 for문 돌리기 전에, 각 자식마다 계산을 최대한 일정하게 분배
  int now_start_idx = 0;
  int remainder = ((N - 2) * (N - 2)) % processNum; // 전체 계산량 / processNum 에서 나머지
  int idx_array[processNum][2];                     // 각각의 프로세스가 계산해야 할 start_idx, end_idx 저장한 배열

  for (int pn = 0; pn < processNum; pn++)
  {
    idx_array[pn][0] = now_start_idx;
    idx_array[pn][1] = now_start_idx + ((N - 2) * (N - 2)) / processNum - 1;
    if (remainder != 0)
    {
      idx_array[pn][1] += 1;
      remainder -= 1;
    }
    now_start_idx = idx_array[pn][1] + 1;
  }

  //fork() 해서 child process 생성
  for (int now_process_num = 0; now_process_num < processNum; now_process_num++)
  {
    pid_t child_pid = fork();

    if (child_pid == -1)
    {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    else if (child_pid == 0)
    { // child
      close(pipefd[0]);
      for (int i = idx_array[now_process_num][0]; i <= idx_array[now_process_num][1]; i++)
      {
        // 실제 계산할 행렬[][]의 idx로 변환
        int x = i / (N - 2);
        int y = i % (N - 2);
        int n = 0; // 계산 결과값

        for (int col = 0; col < 3; col++)
        {
          for (int row = 0; row < 3; row++)
          {
            n += input[col + x][row + y] * filter[col][row];
          }
        }

        pipeBuf[0] = x;
        pipeBuf[1] = y;
        pipeBuf[2] = n;

        while (1)
        {
          int return_write = write(pipefd[1], pipeBuf, sizeof(pipeBuf));
          if (return_write > 0)
            break;
        }
      }
      close(pipefd[1]);
      exit(EXIT_SUCCESS);
    }
  }

  // parent
  close(pipefd[1]);
  int output[N - 2][N - 2] = {0, };

  // pipe에서 data read 하는 부분
  while (1)
  {
    int return_read = read(pipefd[0], pipeBuf, sizeof(pipeBuf));
    if (return_read == 0)
      break;
    else if (return_read > 0)
      output[pipeBuf[0]][pipeBuf[1]] = pipeBuf[2];
  }
  close(pipefd[0]);

  // reaping
  while (waitpid(-1, NULL, 0) > 0);

  // print 하는 부분
  for (int col = 0; col < N - 2; col++)
  {
    for (int row = 0; row < N - 2; row++)
    {
      printf("%d", output[col][row]);
      if (row != N - 3)
        printf(" ");
    }
    printf("\n");
  }
}

int main(int argc, char **argv)
{
  // 201911560 김지환
  if (argc != 2)
  {
    printf("ERR: 인자 1개만 입력해 주세요. ex) ./ku_conv k\n");
    return 1;
  }
  int processNum = atoi(argv[1]);
  if (processNum <= 0)
  {
    printf("프로세스 개수는 양수여야 합니다.");
    return 1;
  }
  int filter[3][3] = {{-1, -1, -1},
                      {-1, 8, -1},
                      {-1, -1, -1}};

  makeChild(processNum, filter);
  return 0;
}