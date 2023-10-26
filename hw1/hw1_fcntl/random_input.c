#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void random_input(int n, int input_array[][n]) {
  //srand((unsigned int)time(NULL));
  srand(7);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      input_array[i][j] = 100 - (rand() % 200);
      if (input_array[i][j] == 0)
        input_array[i][j] += 1;
    }
  }
}


// int main () {
//   int N = 50;
//   int input_array[N][N];
//   random_input(N, input_array);
//   printf("%d\n", input_array[0][0]);
// }