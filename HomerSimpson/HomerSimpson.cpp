#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

// Шелеметев Михаил ПС-24
// Гомер Симпсон пожертвовал N долларов на распространение дисков с фильмами о себе.
// Имеется 3 вида дисков, которые продаются по ценам C1, C2 и C3  долларов.
// Требуется купить как можно больше дисков так, чтобы осталась неизрасходованной как можно меньшая сумма денег.
// Ввод из файла INPUT.TXT.В первой строке задается значение N(1 ≤ N ≤ 2×10^9).
// Во второй строке C1, C2 и C3 через пробел(1 ≤ C1, C2, C3 ≤ 2×10^9). Все числа целые.
// Вывод в файл OUTPUT.TXT.В первой строке вывести общее количество покупаемых дисков.
// Во второй строке выводится остаток средств. В третьей строке через пробел выводится число покупаемых дисков каждого вида соответственно.

const std::string INPUT_FILE = "input.txt";
const std::string OUTPUT_FILE = "output.txt";

using llong = long long;
struct Result 
{
    llong totalDiscs;
    llong remainder;
    std::vector<llong> counts;
};

void ReadInput(llong& money, std::vector<llong>& prices) 
{
    std::ifstream fin(INPUT_FILE);
    fin >> money;
    prices.resize(3);
    fin >> prices[0] >> prices[1] >> prices[2];
    fin.close();
}

Result GetResult(llong money, std::vector<llong>& prices) 
{
    std::vector<llong> sortedPrices = prices;
    std::vector<int> indices = { 0, 1, 2 };
    std::sort(indices.begin(), indices.end(), [&sortedPrices](int a, int b) {
            return sortedPrices[a] < sortedPrices[b];
        });
    llong C1 = sortedPrices[indices[0]];
    llong C2 = sortedPrices[indices[1]]; 
    llong C3 = sortedPrices[indices[2]];

    llong maxCount = money / C1;
    llong minLeft = money - maxCount * C1;
    Result result = { maxCount, minLeft, {maxCount, 0, 0} };

    if (minLeft > 0)
    {
        //C2
        for (llong k1 = maxCount; k1 >= std::max(llong(0), maxCount - 1000); k1--) 
        {
            llong moneyLeft = money - k1 * C1;
            if (moneyLeft < 0) continue;

            llong maxC2 = moneyLeft / C2;
            llong remainder = moneyLeft - maxC2 * C2;
            llong totalDiscs = k1 + maxC2;

            if (totalDiscs > result.totalDiscs || (totalDiscs == result.totalDiscs && remainder < result.remainder)) 
            {
                result.totalDiscs = totalDiscs;
                result.remainder = remainder;
                result.counts = { k1, maxC2, 0 };
            }

            //C3
            for (llong k2 = maxC2; k2 >= std::max(llong(0), maxC2 - 1000); k2--) 
            {
                llong moneyLeft2 = moneyLeft - k2 * C2;
                if (moneyLeft2 < 0) continue;

                llong k3 = moneyLeft2 / C3;
                remainder = moneyLeft2 - k3 * C3;
                totalDiscs = k1 + k2 + k3;

                if (totalDiscs > result.totalDiscs || (totalDiscs == result.totalDiscs && remainder < result.remainder)) 
                {
                    result.totalDiscs = totalDiscs;
                    result.remainder = remainder;
                    result.counts = { k1, k2, k3 };
                }
            }
        }
    }
    
    std::vector<llong> originalCounts(3);
    for (int i = 0; i < 3; i++) 
    {
        originalCounts[indices[i]] = result.counts[i];
    }
    result.counts = originalCounts;

    return result;
}

void WriteOutput(const Result& result) 
{
    std::ofstream fout(OUTPUT_FILE);
    fout << result.totalDiscs << std::endl;
    fout << result.remainder << std::endl;
    fout << result.counts[0] << " " << result.counts[1] << " " << result.counts[2] << std::endl;
    fout.close();
}

int main() 
{
    llong money;
    std::vector<llong> prices;
    try
    {
        ReadInput(money, prices);
        Result result = GetResult(money, prices);
        WriteOutput(result);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}