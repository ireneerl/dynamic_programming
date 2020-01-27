#include <iostream>
#include<vector>


std::vector<int> cutRod(std::vector<int> price, int size){
  std::vector<int> r(size+1);
  std::vector<int> sol(size);
  r[0]=0;
  for(int i=1;i<=size;i++){
    int max_val = -2147483648  ;
    for (int j=1; j < i ; j++){
      if(max_val < price[j]+r[i-j]){
        max_val = price[j]+r[i-j];
        sol[i-1] = j;
      }
    }
    r[i]=max_val;
  }
  return sol;
}

int main(int argc, char const *argv[]) {
  std::vector<int> price {0, 1, 5, 8, 9, 10, 17, 17, 20} ;
  int size = 9;
  std::vector<int> sol = cutRod(price, size);
  std::cout << "cut n (max 8) = " << '\n';
  std::cin >> size;
  while (size > 0){
    std::cout << sol[size] << '\t';
    size -= sol[size];
  }
  return 0;
}
