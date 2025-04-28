#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>

// Шелеметев Михаил ПС-24
// Два бегемота, Вася и Эдик, живут в зоопарке.Каждый живет в отдельной прямоугольной клетке, 
// три стены которой каменные, а четвертая образована решеткой.Через решетки бегемоты видят друг друга.
// Однажды ночью обоих мучила бессонница, и Вася предложил сыграть в морской бой, чтобы скоротать время.
// Чтобы сообщать о своем ходе как можно тише(ночь все - таки, все остальные звери спят), Эдик предложил 
// найти такие положения около решеток, расстояние между которыми минимально.
// Ввод из файла INPUT.TXT.В единственной строке задаются через пробел восемь чисел : 
// сначала описание первой решетки, затем второй. Каждая решетка задается координатами концов(x1, y1, x2, y2).
// Все числа целые, по модулю не больше 10000.
// Вывод в файл OUTPUT.TXT. В единственной строке вывести минимальное расстояние, которое может быть 
// между Васей и Эдиком, если каждый будет стоять около своей решетки. Расстояние должно быть выведено 
// с тремя знаками после запятой.

const std::string INPUT_FILE = "input.txt";
const std::string OUTPUT_FILE = "output.txt";

struct Point 
{
    double x, y;
};

// Положение точки C относительно отрезка AB
double PointPosition(Point a, Point b, Point c) 
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

// Проверка пересечения отрезков
bool DoJailsIntersect(Point p1, Point p2, Point p3, Point p4) 
{
    double d1 = PointPosition(p3, p4, p1);
    double d2 = PointPosition(p3, p4, p2);
    double d3 = PointPosition(p1, p2, p3);
    double d4 = PointPosition(p1, p2, p4);
    if (((d1 * d2 < 0) && (d3 * d4 < 0)) ||
        (d1 == 0 && std::min(p3.x, p4.x) <= p1.x && p1.x <= std::max(p3.x, p4.x) && std::min(p3.y, p4.y) <= p1.y && p1.y <= std::max(p3.y, p4.y)) ||
        (d2 == 0 && std::min(p3.x, p4.x) <= p2.x && p2.x <= std::max(p3.x, p4.x) && std::min(p3.y, p4.y) <= p2.y && p2.y <= std::max(p3.y, p4.y)) ||
        (d3 == 0 && std::min(p1.x, p2.x) <= p3.x && p3.x <= std::max(p1.x, p2.x) && std::min(p1.y, p2.y) <= p3.y && p3.y <= std::max(p1.y, p2.y)) ||
        (d4 == 0 && std::min(p1.x, p2.x) <= p4.x && p4.x <= std::max(p1.x, p2.x) && std::min(p1.y, p2.y) <= p4.y && p4.y <= std::max(p1.y, p2.y))) 
    {
        return true;
    }
    return false;
}

double GetDistation(Point p, Point a, Point b) 
{
    double len2 = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
    // Точки совпали, возвращаем расстояние до этой точки
    if (len2 == 0)
    {
        return std::sqrt((p.x - a.x) * (p.x - a.x) + (p.y - a.y) * (p.y - a.y));
    }

    // Ищем проекцию нашей точки на отрезок, возвращаем расстояние между точкой и проекцией
    double t = std::max(0.0, std::min(1.0, ((p.x - a.x) * (b.x - a.x) + (p.y - a.y) * (b.y - a.y)) / len2));
    Point proj = { a.x + t * (b.x - a.x), a.y + t * (b.y - a.y) };
    return std::sqrt((p.x - proj.x) * (p.x - proj.x) + (p.y - proj.y) * (p.y - proj.y));
}

int main() 
{
    std::ifstream fin(INPUT_FILE);
    std::ofstream fout(OUTPUT_FILE);

    Point p1, p2, p3, p4;
    fin >> p1.x >> p1.y >> p2.x >> p2.y >> p3.x >> p3.y >> p4.x >> p4.y;

    double ans = 0;

    // Проверка на пересечение
    if (!DoJailsIntersect(p1, p2, p3, p4))
    {
        ans = std::min({
            GetDistation(p1, p3, p4),
            GetDistation(p2, p3, p4),
            GetDistation(p3, p1, p2),
            GetDistation(p4, p1, p2)
        });
    }

    fout << std::fixed << std::setprecision(3) << ans << std::endl;

    return EXIT_SUCCESS;
}