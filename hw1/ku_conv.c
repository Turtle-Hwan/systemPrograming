#include "ku_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void makeChild(int processNum, int filter[][3]) {
  //pipe 만들기
  int pipefd[2];
  int pipeBuf[3];   //{x, y, n} output[x][y] = n

  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  
  //자식 processNum개로 생성하기 for 돌릴 때, 자식에게 계산 몇 개씩 분배
  int now_start_idx = 0;
  int remainder = ((N-2)*(N-2)) % processNum; //전체 계산량 / processNum 에서 나머지 개수
  int idx_array[processNum][2];   //각각의 프로세스가 계산해야 할 startIdx, endIdx 저장한 배열

  for (int pn = 0; pn < processNum; pn++) {
    idx_array[pn][0] = now_start_idx;
    idx_array[pn][1] = now_start_idx + ((N-2)*(N-2))/processNum - 1;
    if (remainder != 0) {
      idx_array[pn][1] += 1;
      remainder -= 1;
    }
    now_start_idx = idx_array[pn][1] + 1;

    // printf("idx_array : %d, %d\n", idx_array[pn][0], idx_array[pn][1]);
  }

  for (int now_process_num = 0; now_process_num < processNum; now_process_num++) {
    pid_t child_pid = fork();
    //printf("cpid ; %d\n", child_pid);
    if (child_pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    else if (child_pid == 0) {  //child
      close(pipefd[0]); //child에서는 read 안함.
      for (int i = idx_array[now_process_num][0]; i <= idx_array[now_process_num][1]; i++) {
        //실제 계산할 행렬[][]의 idx로 변환

        int x = i/(N-2);  
        int y = i%(N-2);
        int n = 0;  //계산 결과값
        
        for (int col = 0; col < 3; col++) {
          for (int row = 0; row < 3; row++) {
            n += input[col + x][row + y] * filter[col][row];
          }
        }

        pipeBuf[0] = x;
        pipeBuf[1] = y;
        pipeBuf[2] = n;

        //printf("x, y, n : %d %d %d\n", x, y, n);
        write(pipefd[1], pipeBuf, sizeof(pipeBuf));
      }
      close(pipefd[1]);

      exit(EXIT_SUCCESS);
    }
      //while (wait() != 0)
    }

      close(pipefd[1]);
      int output[N-2][N-2];

      //pipe에서 받아오는 부분
      while (waitpid(-1, NULL, 0) > 0);

      while (read(pipefd[0], pipeBuf, sizeof(pipeBuf)) > 0) {
        output[pipeBuf[0]][pipeBuf[1]] = pipeBuf[2];
      }
      close(pipefd[0]);
      
      //print 하는 부분
      for (int col = 0; col < N-2; col++) {
        for (int row = 0; row < N-2; row++) {
          printf("%d", output[col][row]);
          if (row != N-3)
            printf(" ");
        }
        printf("\n");
  }

  /* 
    총 계산량 : (N-2)*(N-2) N-2 by N-2 행렬 a11, a12, ....a1N-2, a21... aN-2N-2  (0, 1, 2, 3, ...)
  
  파이프에 계산해야 할 결과행렬 좌표 (a11, a12, a13, a14, a15... 를 넣어놓기)
    child에서 pipe(필요 없을수도 있음)에서 값 꺼내가서, 
    해당 값을 연산해서 다시 pipe에 값 int value[3] = {행번호,열번호,실제값}을 넣는다.
  */
}


int main(int argc, char** argv) { //201911560 김지환
  if (argc != 2) {
    //printf("ERR: 인자 1개만 입력해 주세요. ex) ./ku_conv k\n");
    return 1;
  }
  int processNum = atoi(argv[1]);
  if (processNum <= 0) {
    //printf("프로세스 개수는 양수여야 합니다.")
    return 1;
  }
  int filter[3][3] = {{-1, -1, -1},
                      {-1, 8, -1},
                      {-1, -1, -1}};

  makeChild(processNum, filter);
  return 0;
}