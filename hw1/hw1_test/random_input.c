#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void random_input(int n, int input_array[][n]) {
  //srand((unsigned int)time(NULL));
  srand(1);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      input_array[i][j] = 1000000000 - rand();
    }
  }
}


// int main () {
//   int N = 50;
//   int input_array[N][N];
//   random_input(N, input_array);
//   printf("%d\n", input_array[0][0]);
// }