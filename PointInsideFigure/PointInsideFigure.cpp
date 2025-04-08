#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

struct Point 
{
    int x, y;
};

bool IsOnEdge(const Point& P1, const Point& P2, const Point& P) 
{
    int dx = P2.x - P1.x;
    int dy = P2.y - P1.y;
    int dxp = P.x - P1.x;
    int dyp = P.y - P1.y;

    if (dy * dxp != dx * dyp) return false;

    int minx = std::min(P1.x, P2.x);
    int maxx = std::max(P1.x, P2.x);
    int miny = std::min(P1.y, P2.y);
    int maxy = std::max(P1.y, P2.y);
    return minx <= P.x && P.x <= maxx && miny <= P.y && P.y <= maxy;
}

bool Return(std::ofstream& fout, const std::string& result)
{
    fout << result << std::endl;
    fout.close();
    exit(0);
}

bool IsPointMatchingOthers(const std::vector<Point> points, const Point& point)
{
    for (const auto& pt : points)
    {
        if (point.x == pt.x && point.y == pt.y)
        {
            return true;
        }
    }
}

int main() 
{
    try
    {
        // Открываем файлы ввода и вывода
        std::ifstream fin("input.txt");
        std::ofstream fout("output.txt");

        std::vector<Point> points(4);
        for (auto& pt : points) 
        {
            fin >> pt.x >> pt.y;
        }

        Point point;
        fin >> point.x >> point.y;

        if (IsPointMatchingOthers(points, point))
        {
            Return(fout, "In");
        }

        // Алгоритм трассировки луча
        int crossings = 0;
        for (int i = 0; i < 4; ++i) 
        {
            Point P1 = points[i];
            Point P2 = points[(i + 1) % 4];
            if (IsOnEdge(P1, P2, point))
            {
                Return(fout, "In");
            }
            if ((P1.y < point.y && P2.y > point.y) || (P1.y > point.y && P2.y < point.y))
            {
                // Вычисляем t и x-координату пересечения
                double t = static_cast<double>(point.y - P1.y) / (P2.y - P1.y);
                double x = P1.x + t * (P2.x - P1.x);
                // Пересечение считается, если x строго больше P.x
                if (x > point.x)
                {
                    crossings++;
                }
            }
        }

        // Определяем результат по четности пересечений
        if (crossings % 2 == 1) 
        {
            Return(fout, "In");
        }
        else 
        {
            Return(fout, "Out");
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    

    return 0;
}