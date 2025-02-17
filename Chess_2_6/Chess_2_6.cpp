#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <stack>
#include <unordered_set>

// Шелеметев Михаил ПС-24
// 2.6.Шахматы(12)
// Шахматная доска состоит из N строк и M столбцов(1 ≤ N, M ≤ 15).Имеется единственный белый король.
// У черных в распоряжении кони, слоны и ладьи.По заданному расположению фигур требуется взять белым королем все 
// черные фигуры за минимальное число ходов, не находясь при этом под боем.
// Ввод.В первой строке находятся числа M и N.В следующих N строках по M символов задано расположение фигур.
// Имеются следующие обозначения:
// «.» – пустая клетка;
// «*» – белый король;
// «K» – черный конь;
// «B» – черный слон;
// «R» – черная ладья.
// Белый король в начальной позиции не атакован.Общее число черных фигур не более 14.
// Вывод.В единственной строке выводится минимальное число ходов, необходимое для взятия черных фигур.
// Вывести 0, если черных фигур нет, и - 1, если взять все черные фигуры невозможно.
// Ограничения.На одном тесте время счета не должно превышать 4 сек.

const std::string inputFileName = "input.txt";
const std::string outputFileName = "output.txt";

enum FieldState
{
    None = '.',
    King = '*',
    Knight = 'K',
    Bishop = 'B',
    Rook = 'R',
    Any,
};

const std::unordered_set<char> figures = { Knight, Rook, Bishop };

using Board = std::vector<std::vector<FieldState>>;
struct Position
{
public:
    int x;
    int y;
    Position()
    {
        x = 0;
        y = 0;
    }
    Position(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    Position(const Position& pos, int deltaX, int deltaY)
    {
        this->x = pos.x + deltaX;
        this->y = pos.y + deltaY;
    }
};

struct WaveBoard
{
public:
    std::vector<std::vector<char>> board;
    Position kingPos;
    int width, height;
    WaveBoard()
    {
        kingPos = Position(0, 0);
    }
};

std::istream& operator >> (std::istream& is, FieldState& fieldState)
{
    char ch;
    is >> ch;
    switch (ch)
    {
    case None: fieldState = None; break;
    case King: fieldState = King; break;
    case Knight: fieldState = Knight; break;
    case Bishop: fieldState = Bishop; break;
    case Rook: fieldState = Rook; break;
    default: fieldState = None;
    }
    return is;
}

bool operator == (const Position& pos1, const Position& pos2)
{
    return pos1.x == pos2.x && pos1.y == pos2.y;
}

int BoardHeight(const Board& board)
{
    return board.size();
}

int BoardWidth(const Board& board)
{
    if (board.size() > 0)
    {
        return board[0].size();
    }
    return 0;
}


Board ReadBoard(std::istream& input)
{
    int x, y;
    input >> y >> x;
    Board board(y);
    std::string line;
    std::getline(input, line);
    for (int i = 0; i < y; i++)
    {
        std::getline(input, line);
        std::istringstream stream(line);
        board[i].resize(x);
        for (int j = 0; j < x; j++)
        {
            stream >> board[i][j];
        }
    }
    return board;
}

bool IsPositionInsideBoard(const Board& board, const Position& position)
{
    return position.x >= 0 &&
        position.x < BoardWidth(board) &&
        position.y >= 0 &&
        position.y < BoardHeight(board);
}

bool IsPositionInsideBoard(const WaveBoard& waveBoard, const Position& position)
{
    return position.y >= 0 &&
        position.y < waveBoard.board.size() &&
        position.x >= 0 &&
        position.x < waveBoard.board[0].size();
}

bool IsCertainFigure(const Board& board, const FieldState figure, const Position& position)
{
    if (IsPositionInsideBoard(board, position))
    {
        if (figure == Any)
        {
            return true;
        }
        return board[position.y][position.x] == figure;
    }
    return false;
}

std::vector<Position> GetProtectedFields(const Board& board, const Position& position)
{
    std::vector<Position> protectedFields;
    FieldState figure = board[position.y][position.x];
    int boardMax = std::max(BoardWidth(board), BoardHeight(board));
    // For bishop: Left up, Left down, Right up, Right down
    // For rook: Left, right, up, down
    std::unordered_set<int> remainingDirections = { 0, 1, 2, 3 };
    switch (figure)
    {
    case Knight:
        for (const Position& pos : std::vector<Position>{ {-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2} })
        {
            if (IsCertainFigure(board, Any, Position(position, pos.x, pos.y)))
            {
                protectedFields.push_back(Position(position, pos.x, pos.y));
            }
        }
        break;
    case Bishop:
        for (int i = 1; i < boardMax; i++)
        {
            int counter = 0;
            for (const Position& pos : std::vector<Position>{ {-i, -i}, {-i, i}, {i, -i}, {i, i} })
            {
                if (remainingDirections.contains(counter) && IsCertainFigure(board, Any, Position(position, pos.x, pos.y)))
                {
                    protectedFields.push_back(Position(position, pos.x, pos.y));
                    if (IsCertainFigure(board, Knight, Position(position, pos.x, pos.y)) || 
                        IsCertainFigure(board, Rook, Position(position, pos.x, pos.y)))
                    {
                        remainingDirections.erase(counter);
                    }
                }
                counter++;
            }
        }
        break;
    case Rook:
        for (int i = 1; i < BoardWidth(board); i++)
        {
            if (remainingDirections.contains(0) && IsCertainFigure(board, Any, Position(position, -i, 0)))
            {
                protectedFields.push_back(Position(position, -i, 0));
                if (IsCertainFigure(board, Knight, Position(position, -i, 0)) ||
                    IsCertainFigure(board, Bishop, Position(position, -i, 0)))
                {
                    remainingDirections.erase(0);
                }
            }
            else
            {
                remainingDirections.erase(0);
            }
            if (remainingDirections.contains(1) && IsCertainFigure(board, Any, Position(position, i, 0)))
            {
                protectedFields.push_back(Position(position, i, 0));
                if (IsCertainFigure(board, Knight, Position(position, i, 0)) ||
                    IsCertainFigure(board, Bishop, Position(position, i, 0)))
                {
                    remainingDirections.erase(1);
                }
            }
            else
            {
                remainingDirections.erase(1);
            }
            if (!remainingDirections.contains(0) && !remainingDirections.contains(1))
            {
                break;
            }
        }
        for (int i = 1; i < BoardHeight(board); i++)
        {
            if (remainingDirections.contains(2) && IsCertainFigure(board, Any, Position(position, 0, -i)))
            {
                protectedFields.push_back(Position(position, 0, -i));
                if (IsCertainFigure(board, Knight, Position(position, 0, -i)) ||
                    IsCertainFigure(board, Bishop, Position(position, 0, -i)))
                {
                    remainingDirections.erase(2);
                }
            }
            else
            {
                remainingDirections.erase(2);
            }
            if (remainingDirections.contains(3) && IsCertainFigure(board, Any, Position(position, 0, i)))
            {
                protectedFields.push_back(Position(position, 0, i));
                if (IsCertainFigure(board, Knight, Position(position, 0, i)) ||
                    IsCertainFigure(board, Bishop, Position(position, 0, i)))
                {
                    remainingDirections.erase(3);
                }
            }
            else
            {
                remainingDirections.erase(3);
            }
            if (!remainingDirections.contains(2) && !remainingDirections.contains(3))
            {
                break;
            }
        }
        break;
    default:
        break;
    }
    return protectedFields;
}

bool IsFigureProtectingSame(const Board& board, const Position& position)
{
    FieldState figure = board[position.y][position.x];
    for (const Position& pos : GetProtectedFields(board, position))
    {
        if (board[pos.y][pos.x] == figure)
        {
            return true;
        }
    }
    return false;
}

bool IsUnsolvable(const Board& board)
{
    bool isUnsolvable = false;
    for (int i = 0; i < BoardHeight(board); i++)
    {
        for (int j = 0; j < BoardWidth(board); j++)
        {
            isUnsolvable = IsFigureProtectingSame(board, Position(j, i));
            if (isUnsolvable)
            {
                return true;
            }
        }
    }
    return false;
}

std::pair<WaveBoard, std::vector<Position>> InitWaveBoard(const Board& board)
{
    WaveBoard newBoard;
    std::vector<Position> figuresPositions;
    newBoard.width = BoardWidth(board);
    newBoard.height = BoardHeight(board);
    newBoard.board.resize(BoardHeight(board));
    for (int j = 0; j < newBoard.height; j++)
    {
        newBoard.board[j].resize(newBoard.width);
        for (int i = 0; i < newBoard.width; i++)
        {
            FieldState field = board[j][i];
            if (field == None)
            {
                newBoard.board[j][i] = 0;
            }
            else
            {
                if (field == King)
                {
                    newBoard.board[j][i] = -1;
                    newBoard.kingPos = Position(i, j);
                    continue;
                }
                figuresPositions.push_back(Position(i, j));
                newBoard.board[j][i] = field;
            }
        }
    }
    return { newBoard, figuresPositions };
}

WaveBoard GetWaveBoardWithBlockedFields(const Board& board)
{
    std::pair<WaveBoard, std::vector<Position>> initWaveBoardResult = InitWaveBoard(board);
    WaveBoard waveBoard = initWaveBoardResult.first;
    std::vector<Position> figuresPoses = initWaveBoardResult.second;
    for (const Position& figure : figuresPoses)
    {
        for (const Position& blocked : GetProtectedFields(board, figure))
        {
            waveBoard.board[blocked.y][blocked.x] = -1;
        }
    }
    
    return waveBoard;
}

Board GetUpdatedBoard(const Board& board, Position oldKingPos, Position newKingPos)
{
    Board newBoard = board;
    newBoard[oldKingPos.y][oldKingPos.x] = None;
    newBoard[newKingPos.y][newKingPos.x] = King;
    return newBoard;
}

int GetMaxWaveRadius(const WaveBoard& waveBoard)
{
    return std::max(
        std::min(waveBoard.kingPos.x, std::abs(waveBoard.kingPos.x - waveBoard.width) - 1),
        std::min(waveBoard.kingPos.y, std::abs(waveBoard.kingPos.y - waveBoard.height) - 1)
    );
}

void RemoveUnoptimalFields(WaveBoard& waveBoard, const Position& figureSource, int deltaX, int deltaY)
{
    int removedFiguresCount = 0;
    if (deltaX == 0)
    {
        if (deltaY > 0) // bottom center
        {
            for (int j = figureSource.y; j < waveBoard.height; j++)
            {
                for (int i = figureSource.x - (j - figureSource.y); i < figureSource.x + (j - figureSource.y) + 1; i++)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        if (IsCertainFigure)
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
        else // top center
        {
            for (int j = figureSource.y; j >= 0; j--)
            {
                for (int i = figureSource.x - (figureSource.y - j); i < figureSource.x + (figureSource.y - j) + 1; i++)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
    }
    if (deltaY == 0)
    {
        if (deltaX > 0) // right center
        {
            for (int i = figureSource.x; i < waveBoard.width; i++)
            {
                for (int j = figureSource.y - (i - figureSource.x); j < figureSource.y + (i - figureSource.x) + 1; j++)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
        else // left center
        {
            for (int i = figureSource.x; i >= 0; i--)
            {
                for (int j = figureSource.y - (figureSource.x - i); j < figureSource.y + (figureSource.x - i) + 1; j++)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
    }
    if (deltaX > 0)
    {
        if (deltaY > 0) // bottom right
        {
            for (int j = figureSource.y; j < waveBoard.height; j++)
            {
                for (int i = figureSource.x; i < waveBoard.width; i++)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
        else // top right
        {
            for (int j = figureSource.y; j >= 0; j--)
            {
                for (int i = figureSource.x; i < waveBoard.width; i++)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
    }
    else
    {
        if (deltaY > 0) // bottom left
        {
            for (int j = figureSource.y; j < waveBoard.height; j++)
            {
                for (int i = figureSource.x; i >= 0; i--)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
        else // top left
        {
            for (int j = figureSource.y; j >= 0; j--)
            {
                for (int i = figureSource.x; i >= 0; i--)
                {
                    if (IsPositionInsideBoard(waveBoard, Position(i, j)))
                    {
                        waveBoard.board[j][i] = -1;
                    }
                }
            }
        }
    }
}

std::vector<std::pair<Position, int>> GetFiguresPositionsList(WaveBoard& waveBoard)
{
    std::vector<std::pair<Position, int>> closestFigures;
    std::vector<Position> wave;
    int stepsCount = 0;
    wave.push_back(waveBoard.kingPos);
    waveBoard.board[waveBoard.kingPos.y][waveBoard.kingPos.x] = -1;
    while (wave.size() > 0)
    {
        stepsCount++;
        std::vector<Position> newWave;
        for (auto waveElem : wave)
        {
            std::vector<Position> figuresToEat;
            for (const Position& newWaveElem : std::vector<Position>{
                {waveElem.x + 1, waveElem.y},
                {waveElem.x - 1, waveElem.y},
                {waveElem.x, waveElem.y + 1},
                {waveElem.x, waveElem.y - 1},
                {waveElem.x + 1, waveElem.y + 1},
                {waveElem.x + 1, waveElem.y - 1},
                {waveElem.x - 1, waveElem.y + 1},
                {waveElem.x - 1, waveElem.y - 1} })
            {
                if (IsPositionInsideBoard(waveBoard, newWaveElem))
                {
                    if (waveBoard.board[newWaveElem.y][newWaveElem.x] >= 0)
                    {
                        newWave.push_back(newWaveElem);
                        if (figures.contains(waveBoard.board[newWaveElem.y][newWaveElem.x]))
                        {
                            closestFigures.push_back({ newWaveElem, stepsCount });
                            RemoveUnoptimalFields(waveBoard, newWaveElem, newWaveElem.x - waveElem.x, newWaveElem.y - waveElem.y);
                            figuresToEat.push_back({ newWaveElem.x, newWaveElem.y });
                        }
                        else
                        {
                            waveBoard.board[newWaveElem.y][newWaveElem.x] = -1;
                        }
                    }
                }
            }
            for (const Position& figure : figuresToEat)
            {
                waveBoard.board[figure.y][figure.x] = -1;
            }
        }
        wave = newWave;
    }
    return closestFigures;
}

int GetFiguresCount(const Board& board)
{
    int count = 0;
    for (int i = 0; i < BoardHeight(board); i++)
    {
        for (int j = 0; j < BoardWidth(board); j++)
        {
            if (board[i][j] == Rook || board[i][j] == Bishop || board[i][j] == Knight)
            {
                count++;
            }
        }
    }
    return count;
}

int GetFiguresCount(const WaveBoard& waveBoard)
{
    int count = 0;
    for (int i = 0; i < waveBoard.height; i++)
    {
        for (int j = 0; j < waveBoard.width; j++)
        {
            if (waveBoard.board[i][j] == Rook || waveBoard.board[i][j] == Bishop || waveBoard.board[i][j] == Knight)
            {
                count++;
            }
        }
    }
    return count;
}

int FindShortestWayStepsCount(const Board& initialBoard, int figuresLeft, int& minPathStepsCount, int currentPathStepsCount)
{
    if (currentPathStepsCount + figuresLeft >= minPathStepsCount)
    {
        return 0;
    }
    WaveBoard waveBoard = GetWaveBoardWithBlockedFields(initialBoard);
    std::vector<std::pair<Position, int>> figuresPoses = GetFiguresPositionsList(waveBoard);
    int childStepsCount = INT_MAX,
        shortestWayFigureStepsCount = INT_MAX,
        stepsSum = 0;
    if (figuresPoses.size() == 0 && figuresLeft == 0)
    {
        if (minPathStepsCount > currentPathStepsCount)
        {
            minPathStepsCount = currentPathStepsCount;
        }

        return 0;
    }
    for (std::pair<Position, int> source : figuresPoses)
    {
        int newValue = FindShortestWayStepsCount(
            GetUpdatedBoard(initialBoard, waveBoard.kingPos, source.first), 
            figuresLeft - 1, 
            minPathStepsCount,
            currentPathStepsCount + source.second);
        if (newValue == 0)
        {
            childStepsCount = 0;
            shortestWayFigureStepsCount = source.second;
            break;
        }
        if (childStepsCount == INT_MAX || (childStepsCount - newValue) + (shortestWayFigureStepsCount - source.second) > 0)
        {
            childStepsCount = newValue;
            shortestWayFigureStepsCount = source.second;
        }
        //std::cout << source.x << "-" << source.y << ":" << childStepsCount << " ";
    }
    if (childStepsCount == INT_MAX)
    {
        if (figuresLeft > 0)
        {
            return -1;
        }
        childStepsCount = 0;
    }
    else
    {
        if (childStepsCount == -1 && figuresLeft > 0)
        {
            return -1;
        }
    }
    return childStepsCount + shortestWayFigureStepsCount;
}

int main(int argc, char* argv[])
{
    try
    {
        std::ifstream input(inputFileName);
        if (!input.is_open())
        {
            throw std::runtime_error("Failed to open " + inputFileName + " for reading");
        }
        std::ofstream output(outputFileName);
        if (!output.is_open())
        {
            throw std::runtime_error("Failed to open/create " + outputFileName + " for writing");
        }
        Board board = ReadBoard(input);
        if (IsUnsolvable(board))
        {
            output << -1 << std::endl;
            return EXIT_SUCCESS;
        }
        std::vector<Position> fields;
        int pathStepsCount = INT_MAX;
        FindShortestWayStepsCount(board, GetFiguresCount(board), pathStepsCount, 0);
        output << (pathStepsCount == INT_MAX ? -1 : pathStepsCount) << std::endl;
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
