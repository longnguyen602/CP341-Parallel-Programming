#include <stdio.h>

int main()
{
  for(int i = 0; i < 5; i++){
    printf("%d\n", fib(i));
  }
  return 0;
}

int fib(int num){
  if (num ==0){
    return 0;
  }
  if (num ==1){
    return 1;
  }
  return fib(num-1) + fib(num-2);

}
