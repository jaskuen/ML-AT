#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>

// Шелеметев Михаил ПС-24
// 4.5. Маршрут (6)
// Дана матрица N × N, заполненная целыми положительными числами.Путь по матрице начинается в левом верхнем углу.За один ход можно пройти в соседнюю по вертикали или горизонтали клетку(если она существует).Нельзя ходить по диагонали, нельзя оставаться на месте.Требуется найти максимальную сумму чисел, стоящих в клетках по пути длиной K клеток(клетку можно посещать несколько раз).
// Ограничения: 2 ≤ N ≤ 20, элементы матрицы имеют значения от 1 до 9999, 1 ≤ K ≤ 20.
// Ввод из файла INPUT.TXT.В первой строке находятся разделенные пробелом числа N и K.Затем идут N строк по N чисел в каждой.
// Вывод в файл OUTPUT.TXT.Вывести в первой строке максимальную сумму.В следующих K строках – соответствующий маршрут в виде координат клеток.

const std::string inputFileName = "input.txt";
const std::string outputFileName = "output.txt";
const int MAX_N = 20;
const int dx[4] = { 0, 0, 1, -1 }; // Направления: вправо, влево, вниз, вверх
const int dy[4] = { 1, -1, 0, 0 };

int N, K;
std::vector<std::vector<int>> matrix(MAX_N, std::vector<int>(MAX_N));
int allStates[MAX_N][MAX_N][MAX_N + 1]; // Макс. сумма для позиции (x,y) и steps шагов
int prevX[MAX_N][MAX_N][MAX_N + 1]; // Для восстановления пути
int prevY[MAX_N][MAX_N][MAX_N + 1];

bool IsInsideField(int x, int y) 
{
    return x >= 0 && x < N && y >= 0 && y < N;
}

int Step(int x, int y, int steps)
{
    if (steps == 0) return 0;
    if (allStates[x][y][steps] != -1) return allStates[x][y][steps];

    int max_sum = -1;
    for (int d = 0; d < 4; d++) 
    {
        int nx = x + dx[d];
        int ny = y + dy[d];
        if (IsInsideField(nx, ny)) {
            int current = Step(nx, ny, steps - 1);
            if (current != -1 && current + matrix[x][y] > max_sum) {
                max_sum = current + matrix[x][y];
                prevX[x][y][steps] = nx;
                prevY[x][y][steps] = ny;
            }
        }
    }
    return allStates[x][y][steps] = max_sum;
}

int main() 
{
    try
    {
        std::ifstream input(inputFileName);
        if (!input.is_open())
        {
            throw std::runtime_error("Falied to open " + inputFileName + " for reading");
        }

        input >> N >> K;
        for (int i = 0; i < N; i++) 
        {
            for (int j = 0; j < N; j++) 
            {
                input >> matrix[i][j];
            }
        }
        input.close();

        memset(allStates, -1, sizeof(allStates));
        memset(prevX, -1, sizeof(prevX));
        memset(prevY, -1, sizeof(prevY));

        int result = Step(0, 0, K);

        std::vector<std::pair<int, int>> path;
        int x = 0, y = 0, steps = K;
        while (steps > 0) 
        {
            path.push_back({ x + 1, y + 1 });
            int nx = prevX[x][y][steps];
            int ny = prevY[x][y][steps];
            x = nx;
            y = ny;
            steps--;
        }

        std::ofstream output(outputFileName);
        if (!output.is_open())
        {
            throw std::runtime_error("Falied to open " + outputFileName + " for reading");
        }
        output << result << "\n";
        for (auto [px, py] : path) 
        {
            output << px << " " << py << "\n";
        }
        output.close();
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return EXIT_SUCCESS;
}
