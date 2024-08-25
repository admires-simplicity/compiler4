#include <stdio.h>
int f(int x, int y){
return x*x+y*y;
}
;
int main(){
printf("f(%d, %d) = %d\n", 0, 0, f(0, 0));
printf("f(%d, %d) = %d\n", 0, 1, f(0, 1));
printf("f(%d, %d) = %d\n", 1, 0, f(1, 0));
printf("f(%d, %d) = %d\n", 1, 1, f(1, 1));
printf("f(%d, %d) = %d\n", 1, 2, f(1, 2));
printf("f(%d, %d) = %d\n", 2, 0, f(2, 0));
printf("f(%d, %d) = %d\n", 2, 1, f(2, 1));
printf("f(%d, %d) = %d\n", 2, 2, f(2, 2));
printf("f(%d, %d) = %d\n", 2, 3, f(2, 3));
printf("f(%d, %d) = %d\n", 3, 0, f(3, 0));
printf("f(%d, %d) = %d\n", 3, 1, f(3, 1));
printf("f(%d, %d) = %d\n", 3, 2, f(3, 2));
printf("f(%d, %d) = %d\n", 3, 3, f(3, 3));
}
;
