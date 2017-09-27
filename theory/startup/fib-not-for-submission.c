#include <stdio.h>

int main()
{
  int size = 5;
  int nums[size];
  nums[0]=0;
  nums[1]=1;
  int *p;
  int * fib(int size, int nums[], int counter);
  p = fib(size, nums, 0);
  printf(*p);
  return 0;;
}

int * fib(int size, int nums[], int counter)
{
  if (counter==size-1)
  {
    return nums;
  }
  nums[counter+2]= nums[counter]+nums[counter+1];
  nums[counter+3]= nums[counter+1]+nums[counter+2];
  counter+=1;
  return fib(size, nums, counter);
}
