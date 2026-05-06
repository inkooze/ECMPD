#include <stdio.h>
#include <windows.h>

// Для вывода в двоичной системе
void printbinary(unsigned long x, int bits) {
    for (int i = bits - 1; i >= 0; i--)
        printf("%lu%s", (x >> i) & 1, (i % 4) ? "" : "");
}

int main(void) {
    SetConsoleOutputCP(CP_UTF8);

    int n = 646;
    int m = -8386608;
    int i = 0;

    // printf("%10i: ", m);
    // printbinary(-m, 24);
    // printf("\n");
    while (i <= 16) {
        printf("%10i: ", n);
        printbinary(n, 24);
        printf("\n");

        m += n;
        n *= 2;
        i++;
    }

    printbinary(7602176, 24);
    printf("\n");
    printbinary(6815744, 24);
    printf("\n");
    printbinary(5242880, 24);
    printf("\n");

    return 0;
}
