#include <stdio.h>
#include <windows.h>
#include <math.h>

// \\ // \\ //

// Кастомная структура ("тип данных") для floating point number - числа с плавающей точкой
typedef struct {
    int znak;
    unsigned int exponent;
    double mantissa;
} fpn;

// Определение данных (структуры) для числа
fpn double_to_fpn(double db_num) {
    fpn fp_num = {0, 0, 0};

    if (db_num == 0) return fp_num;

    // Знак
    fp_num.znak = (db_num < 0) ? 1 : 0;

    int exp = 0;
    double mant = fabs(db_num);

    while (mant >= 1) {
        mant /= 2;
        exp++;
    }

    while (mant < 0.5) {
        mant *= 2;
        exp--;
    }

    fp_num.exponent = exp - 1 + 1023;
    fp_num.mantissa = mant * 2 * (1ull << 52);

    return fp_num;
}

double fpn_to_double(fpn fp_num) {
    if (fp_num.mantissa == 0) return 0;

    double znak = fp_num.mantissa / (1ull << 52);
    
    double db_num = (fp_num.znak == 1) ? -znak : znak;
    
    int exp = fp_num.exponent - 1023;
    
    if (exp >= 0) {
        for (int i = 0; i < exp; i++) {
            db_num *= 2;
        }
    } else {
        int n = -exp;
        for (int i = 0; i < n; i++) {
            db_num /= 2;
        }
    }

    return db_num;
}

// \\ // \\ //

fpn slozhenie(fpn A, fpn B) {
    unsigned long long M1 = A.mantissa;
    unsigned long long M2 = B.mantissa;

    // Если один из операндов 0, значит возвращаем другой
    if (A.mantissa == 0) return B;
    if (B.mantissa == 0) return A;

    unsigned int E3 = (A.exponent > B.exponent) ? A.exponent : B.exponent;
    long dE = A.exponent - B.exponent;

    if (dE != 0) {
        if (dE > 0) {
            M2 >>= dE;
        } else {
            M1 <<= dE;
        }
        printf("    - Уравнивание: dE = %ld\n", dE);
    } else {
        printf("    - Уравнивания не требуется\n");
    }
    printf("    - M1 = %llu\n    - M2 = %llu\n", M1, M2);

    unsigned long long M3;
    int S3;

    if (A.znak == B.znak) {
        M3 = M1 + M2;
        S3 = A.znak;

        if (M3 & (1ull << 53)) {
            M3 >>= 1;
            E3++;
            printf("    - Переполнение мантиссы, сдвиг вправо, E3 + 1 = %i\n", E3);
        }
    } else {
        if (M1 >= M2) {
            M3 = M1 - M2;
            S3 = A.znak;
        } else {
            M3 = M2 - M1;
            S3 = B.znak;
        }

        if (M3 != 0) {
            int k = 0;
            while ((M3 & (1ull << 52)) == 0 || k > 60) {
                M3 <<= 1;
                k++;
            }
            E3 -= k;
            printf("    - Нормализация влево на %d разрядов, E3 = %i\n", k, E3);
        } else {
            S3 = 0;
        }
    }

    if (M3 == 0) {
        S3 = 0;
        E3 = 0;
    }

    fpn res = {S3, E3, M3};
    printf("%f (S = %d, E = %u, M = %llu)\n", fpn_to_double(res), S3, E3, M3);
    return res;
}

// \\ // \\ //

fpn vichitanie(fpn A, fpn B) {
    B.znak = (B.mantissa != 0) ? 1 - B.znak : 0; // Учитываем, чтобы 0 не был "отрицательным" (через sign)

    return slozhenie(A, B);
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

    printf("- A: %f (S = %d, E = %u, M = %lf)\n", fpn_to_double(A), A.znak, A.exponent, A.mantissa);
    printf("- B: %f (S = %d, E = %u, M = %lf)\n", fpn_to_double(B), B.znak, B.exponent, B.mantissa);

    printf("Сложение:\n");
    slozhenie(A, B);

    printf("\nВычитание:\n");
    vichitanie(A, B);

    return 0;
}