#include <stdio.h>
#include <windows.h>

void printbinary(unsigned long x, int bits) {
    for (int i = bits - 1; i >= 0; i--)
        printf("%d%s", (x >> i) & 1, (i % 4) ? "" : ".");
}

void division(int RG1, int RG2) {
    printf("Вычисляем выражение %i / %i\n\n", RG1, RG2);

    // Знак
    int minus = (RG1 < 0) ^ (RG2 < 0);

    unsigned long u_dividend = (RG1 >= 0) ? RG1 : -RG1;
    unsigned short u_divisor  = (RG2 >= 0) ? RG2 : -RG2;

    if (u_divisor == 0) {
        printf("Деление на ноль запрещено!");
        return;
    }

    int CT1 = 16 + 1;
    int CT2 = 16 + 1;
    unsigned long RG1_1 = u_dividend;
    unsigned long RG1_2 = u_dividend;
    unsigned short RG3_1 = 0;
    unsigned short RG3_2 = 0;

    // --- Первый алгоритм (с восстановлением остатка) ---
    printf("Первый алгоритм (С восстановлением остатка)\nЗначения перед началом цикла: RG1 = %i, RG2 = %i, RG3 = %i, CT = %i\n", RG1_1, u_divisor, RG3_1, CT1);
    int overflow = 0;

    while (CT1 > 0) {
        unsigned short high = (unsigned short)((RG1_1 >> 16) & 0xFFFF);
        int diff = (int)high - (int)u_divisor;

        // Проверка переполнения после первого такта
        if (CT1 == 17 && diff >= 0) {
            overflow = 1;
            printf("- ОШИБКА: переполнение разрядной сетки частного (первый остаток >= 0)\n");
            break;
        }

        int bit;
        if (diff >= 0) {
            bit = 1;
            RG1_1 = (RG1_1 & 0xFFFF) | ((unsigned long)diff << 16);
        } else {
            bit = 0;
            // Восстановление: старшие биты остаются неизменными
            RG1_1 = (RG1_1 & 0xFFFF) | ((unsigned long)high << 16);
        }

        RG3_1 = (RG3_1 << 1) | bit;   // сдвиг частного и добавление бита

        RG1_1 <<= 1;                   // сдвиг делимого влево
        RG1_1 &= 0xFFFFFFFF;           // ограничение 32 битами

        printf("- diff = %i, RG1 = %i, RG3 = %i, CT = %i\n", diff, RG1_1, RG3_1, CT1);

        CT1--;
    }

    if (overflow) {
        printf("Операция не может быть выполнена: частное не помещается в 16 бит.\n\n");
    } else {
        unsigned short remainder = (unsigned short)((RG1_1 >> 16) & 0xFFFF);
        printf("Результат: частное = %d, остаток = %d\n\n", (minus) ? -RG3_1 : RG3_1, remainder);
    }

    // --- Второй алгоритм (без восстановления остатка) ---
    printf("Второй алгоритм (Без восстановления остатка)\nЗначения перед началом цикла: RG1 = %i, RG2 = %i, RG3 = %i, CT = %i\n", RG1_2, u_divisor, RG3_2, CT2);
    int first = 1;

    while (CT2 > 0) {
        // Используем знаковые типы для корректного вычисления разности
        short high = (short)((RG1_2 >> 16) & 0xFFFF);
        short divisor_s = (short)u_divisor;
        int diff;

        if (first) {
            // первый такт: проверяем знак исходного делимого
            if (high >= 0)
                diff = (int)high - (int)divisor_s;
            else
                diff = (int)high + (int)divisor_s;
            first = 0;
        } else {
            // последующие такты: знак остатка (high)
            if (high >= 0)
                diff = (int)high - (int)divisor_s;
            else
                diff = (int)high + (int)divisor_s;
        }

        RG1_2 = (RG1_2 & 0xFFFF) | ((unsigned long)(diff & 0xFFFF) << 16);

        int bit = (diff >= 0) ? 1 : 0;   // бит частного
        RG3_2 = (RG3_2 << 1) | bit;

        RG1_2 <<= 1;
        RG1_2 &= 0xFFFFFFFF;

        // Исправлено: выводим актуальные переменные для второго алгоритма
        printf("- diff = %i, RG1 = %i, RG3 = %i, CT = %i\n", diff, RG1_2, RG3_2, CT2);

        CT2--;
    }

    // коррекция остатка
    short final_rem = ((short)((RG1_2 >> 16) & 0xFFFF) < 0) ? 0 : (short)((RG1_2 >> 16) & 0xFFFF);
    if (final_rem < 0)
        final_rem += (short)u_divisor;

    // Применяем знак к частному
    int result = (minus) ? -(int)RG3_2 : (int)RG3_2;
    printf("Результат: частное = %d, остаток = %d\n\n", result, final_rem);
}

int main(void) {
    SetConsoleOutputCP(CP_UTF8);

    division(65000, 1000);
    return 0;
}