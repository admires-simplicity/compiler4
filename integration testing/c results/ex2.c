#include <stdio.h>
int fib(int x){
return ((x<2) ? (1) : (fib(x-1)+fib(x-2)));
}
;
int main(){
printf("%dth fibonacci number is %d\n", 1, fib(1));
printf("%dth fibonacci number is %d\n", 2, fib(2));
printf("%dth fibonacci number is %d\n", 3, fib(3));
printf("%dth fibonacci number is %d\n", 4, fib(4));
printf("%dth fibonacci number is %d\n", 5, fib(5));
printf("%dth fibonacci number is %d\n", 6, fib(6));
printf("%dth fibonacci number is %d\n", 7, fib(7));
printf("%dth fibonacci number is %d\n", 8, fib(8));
}
;
