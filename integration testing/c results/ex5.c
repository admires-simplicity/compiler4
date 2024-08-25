#include <stdio.h>
int f(int x){
return (() ? ({
printf("roger roger, it's zero");
x}
) : ({
x*2}
));
}
int main(){
printf("%d\n", f(0));
printf("%d\n", f(1));
printf("%d\n", f(2));
}
;
