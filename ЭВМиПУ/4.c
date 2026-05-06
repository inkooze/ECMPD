#include <stdio.h>
#include <windows.h>
#include <math.h>

// \\ // \\ //

// Кастомная структура ("тип данных") для floating point number - числа с плавающей точкой
typedef struct {
    int znak;
    unsigned int exponent;
    unsigned long long mantissa;
} fpn;

// Определение данных (структуры) для числа
fpn double_to_fpn(double db_num) {
    fpn fp_num = {0, 0, 0};

    if (db_num == 0.0) return fp_num;

    // Знак
    fp_num.znak = (db_num < 0.0) ? 1 : 0;

    //
    int exp = 0;
    double mant = fabs(db_num);

    while (mant >= 1.0) {
        mant /= 2.0;
        exp++;
    }

    while (mant < 0.5) {
        mant *= 2.0;
        exp--;
    }

    fp_num.exponent = (unsigned int)(exp - 1 + 1023);
    fp_num.mantissa = (unsigned long long)(mant * 2.0 * (1ull << 52) + 0.5);

    return fp_num;
}

double fpn_to_double(fpn fp_num) {
    if (fp_num.mantissa == 0) return 0.0;

    double znak = (double)fp_num.mantissa / (double)(1ull << 52);

    double db_num = (fp_num.znak == 1) ? -znak : znak;

    int exp = (int)fp_num.exponent - 1023;

    if (exp >= 0) {
        for (int i = 0; i < exp; i++) {
            db_num *= 2.0;
        }
    } else {
        int n = -exp;
        for (int i = 0; i < n; i++) {
            db_num /= 2.0;
        }
    }

    return db_num;
}

// \\ // \\ //

fpn umnozhenie(fpn A, fpn B) {
    if (A.mantissa == 0 || B.mantissa == 0) {
        printf("0 (S = 0, E = 0, M = 0)\n");
        fpn zero = {0, 0, 0};
        return zero;
        
    } else {
        int S3 = A.znak ^ B.znak;

        unsigned int E3 = A.exponent + B.exponent - 1023;

        unsigned long long M1 = A.mantissa;
        unsigned long long M2 = B.mantissa;

        unsigned long long M1_H = M1 >> 32;
        unsigned long long M1_L = M1 & 0xFFFFFFFFull;
        unsigned long long M2_H = M2 >> 32;
        unsigned long long M2_L = M2 & 0xFFFFFFFFull;

        unsigned long long ost = (M1_L * M2_L) >> 32; // Младшие 32 бита нужны для округления, но оно здесь не используется
        unsigned long long mid = M1_H * M2_L + M1_L * M2_H + ost; // Cредние 32 бита произведения
        unsigned long long high = M1_H * M2_H + (mid >> 32); // Cтаршие 64 бита произведения

        unsigned long long M3;

        // Проверка 105 бита всего произведения, если он 1, значит произведение >2^105, значит в норм. виде число >= 2
        if (high & (1ull << 41)) {
            E3++;
            M3 = (high << 11) | ((mid & 0xFFFFFFFFull) >> 21);
        } else {
            M3 = (high << 12) | ((mid & 0xFFFFFFFFull) >> 20);
        }

        fpn res = {S3, E3, M3};
        printf("%f (S = %d, E = %u, M = %llu)\n", fpn_to_double(res), S3, E3, M3);
        return res;
    }
}

// \\ // \\ //

fpn delenie(fpn A, fpn B) {
    if (B.mantissa == 0) {
        printf("Делить на ноль нельзя!\n");

    } else if (A.mantissa == 0) {
        printf("0 (S = 0, E = 0, M = 0)\n\n");
        fpn zero = {0, 0, 0};
        return zero;

    } else {
        int S3 = A.znak ^ B.znak;

        unsigned int E3 = A.exponent + B.exponent - 1023;

        unsigned long long M1 = A.mantissa;
        unsigned long long M2 = B.mantissa;

        unsigned long long M3_full = 0;
        unsigned long long rem = M1;

        for (int i = 0; i < 54; i++) {
            M3_full <<= 1;
            if (rem >= M2) {
                rem -= M2;
                M3_full |= 1;
            }
            rem <<= 1;
        }

        unsigned long long M3;

        if (M3_full & (1ull << 53)) {
            M3 = M3_full >> 1;
        } else {
            E3--;
            M3 = M3_full;
        }

        fpn res = {S3, E3, M3};
        printf("%f (S = %d, E = %u, M = %llu)\n", fpn_to_double(res), S3, E3, M3);
        return res;
    }
}

// \\ // \\ //

int main() {
    SetConsoleOutputCP(CP_UTF8);

    double a, b;
    printf("A:");
    scanf("%lf", &a);
    printf("B:");
    scanf("%lf", &b);

    fpn A = double_to_fpn(a);
    fpn B = double_to_fpn(b);

    printf("- A: %f (S = %d, E = %u, M = %llu)\n", fpn_to_double(A), A.znak, A.exponent, A.mantissa);
    printf("- B: %f (S = %d, E = %u, M = %llu)\n", fpn_to_double(B), B.znak, B.exponent, B.mantissa);

    printf("\nУмножение: ");
    umnozhenie(A, B);

    printf("\nДеление: ");
    delenie(A, B);

    return 0;
}