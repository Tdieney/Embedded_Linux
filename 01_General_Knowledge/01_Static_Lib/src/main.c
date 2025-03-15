#include <stdio.h>
#include "math.h"

int main() {
    int a, b;
    printf("Input a, b: ");
    scanf("%d %d", &a, &b);

    printf("Addition: %d + %d = %d\n", a, b, add(a, b));
    printf("Subtraction: %d - %d = %d\n", a, b, subtract(a, b));
    printf("Multiplication: %d * %d = %d\n", a, b, multiply(a, b));
    printf("Division: %d / %d = %d\n", a, b, divide(a, b));

    return 0;
}
