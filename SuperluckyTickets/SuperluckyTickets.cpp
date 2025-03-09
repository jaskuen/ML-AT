#include <iostream>

// Шелеметев Михаил ПС-24
// 3.4. Суперсчастливые билеты (9)
// Известно,  что  «счастливым»  билетом   называется   билет,  в  номере   которого  сумма 
// цифр первой половины номера равна сумме цифр второй половины номера. А «суперсчастливым» 
// называется  билет,  у  которого  кроме  упомянутого  условия каждая цифра  отличается от
// соседней   не   более   чем   на   1   (например, 323233).  Номер  может начинаться с 0. 
// Найти количество «суперсчастливых» номеров среди всех 2N - значных  билетов(1 ≤ N ≤ 20).

const int MAX_N = 20;
const int MAX_SUM = 9 * MAX_N + 1;
long long dp[MAX_N * 2 + 1][MAX_SUM][MAX_SUM][10][2]; // [pos][sum1][sum2][prev_digit][is_start]
// long long - 64 бита на число

long long solve(int pos, int sum1, int sum2, int prev, int is_start, int n) 
{
    if (pos == 2 * n) 
    {
        return (sum1 == sum2) ? 1 : 0; 
    }

    long long& res = dp[pos][sum1][sum2][prev][is_start];
    if (res != -1) return res;

    res = 0;
    int limit = 9;
    for (int digit = 0; digit <= limit; digit++) 
    {
        if (!is_start || abs(digit - prev) <= 1)
        {
            if (pos < n) 
            {
                res += solve(pos + 1, sum1 + digit, sum2, digit, 1, n);
            }
            else 
            {
                if (sum1 >= sum2 + digit) 
                {
                    res += solve(pos + 1, sum1, sum2 + digit, digit, 1, n);   
                }
            }
        }
    }
    return res;
}

int main() 
{
    int n;
    std::cin >> n;

    memset(dp, -1, sizeof(dp));

    long long answer = solve(0, 0, 0, 0, 0, n);
    std::cout << answer << std::endl;

    return 0;
}