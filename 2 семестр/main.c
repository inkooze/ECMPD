#include <stdio.h>

int first(int x, int Y) {
    int S = 0, X = x << 16;

    do {
        X >>= 1;
        if (Y & (1 << 15)) S += X; // Y & (1 << 7) - вернёт число либо 0x0 (восьмого бита нет), либо 10x0 (восьмой бит имеется)
        Y <<= 1;
    } while (Y != 0);

    return S;
}

int main() {
    int A = 12, B = 4;
    printf("1. %i * %i = %i", A, B, first(A, B));

    return 0;
}