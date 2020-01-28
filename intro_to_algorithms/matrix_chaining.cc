#include <iostream>
#include <vector>

void print_matrix(int i, int j, int n, int *bracket, char &name){
  if (i==j){
    std::cout << name++;
        return;
  }
  std::cout << "(";
  print_matrix(i, *((bracket+i*n)+j), n, bracket, name);
  print_matrix(*((bracket+i*n)+j) + 1, j,  n, bracket, name);
  std::cout << ")";
}

void min_chaining(std::vector<int> arr, int number_arr){
  std::vector<std::vector<int> > memo (number_arr,std::vector<int> (number_arr,-1));
  int state[number_arr][number_arr];
  int current = 0;

  for (int i = 1; i < number_arr; i++) {
    memo[i][i] = 0;
  }
  for (int i = 2; i <= number_arr; i++) {
    for (int j = 1; j <= number_arr - i; j++) {
      int idx = j + i - 1; //-1 so the K will not be out of the boundaries
      memo[j][idx] = 2147483647;
      for (int k = j; k < idx; k++) {
        current = memo[j][k] + memo[k+1][idx] + arr[j-1]*arr[k]*arr[idx];
        if(current < memo[j][idx]){
          memo[j][idx] = current; //insertion complexity time O(1)
          state[j][idx] = k;
        }
      }
    }
  }

  for (int i = 1; i < number_arr; i++) {
    for (int j = 1; j < number_arr; j++) {
      std::cout << memo[i][j] << '\t';
    }
    std::cout << '\n';
  }

  char arr_name = 'A';
  print_matrix(1, number_arr-1, number_arr, (int *)state, arr_name);
  std::cout << '\n';
}




int main(int argc, char const *argv[]) {
  std::vector<int> arr {4, 3, 5, 1, 2, 7};
  int number_arr = arr.size();
  std::cout << "number" << number_arr << '\n';
  min_chaining(arr, number_arr);
  return 0;
}
