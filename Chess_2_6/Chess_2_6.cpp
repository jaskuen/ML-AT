#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <stack>
#include <unordered_set>

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
    WaveBoard()
    {
        kingPos = Position(0, 0);
    }
};

std::istream& operator >> (std::istream& is, FieldState& fieldState)
{
    short in = 1;
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
        for (Position pos : std::vector<Position>{ {-2, -1}, {-2, 1}, {2, -1}, {2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2} })
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
            for (Position pos : std::vector<Position>{ {-i, -i}, {-i, i}, {i, -i}, {i, i} })
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
    for (Position pos : GetProtectedFields(board, position))
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

WaveBoard InitWaveBoard(const Board& board)
{
    WaveBoard newBoard;
    newBoard.board.resize(BoardHeight(board));
    for (int i = 0; i < BoardHeight(board); i++)
    {
        newBoard.board[i].resize(BoardWidth(board));
        for (int j = 0; j < BoardWidth(board); j++)
        {
            FieldState field = board[i][j];
            if (field == None)
            {
                newBoard.board[i][j] = 0;
            }
            else
            {
                if (field == King)
                {
                    newBoard.board[i][j] = -1;
                    newBoard.kingPos = Position(j, i);
                }
                newBoard.board[i][j] = field;
            }
        }
    }
    return newBoard;
}

WaveBoard GetWaveBoardWithBlockedFields(const Board& board)
{
    WaveBoard waveBoard = InitWaveBoard(board);
    for (int i = 0; i < BoardHeight(board); i++)
    {
        for (int j = 0; j < BoardWidth(board); j++)
        {
            for (Position blocked : GetProtectedFields(board, Position(j, i)))
            {
                waveBoard.board[blocked.y][blocked.x] = -1;
            }
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

std::pair<std::vector<Position>, int> GetClosestFiguresPositionsList(WaveBoard& waveBoard)
{
    std::vector<Position> closestFigures, wave;
    int stepsCount = 0;
    wave.push_back(waveBoard.kingPos);
    std::unordered_set<char> figures = { Knight, Rook, Bishop };
    while (closestFigures.size() == 0)
    {
        std::vector<Position> newWave;
        for (Position waveElem : wave)
        {
            for (Position newWaveElem : std::vector<Position>{
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
                            closestFigures.push_back(newWaveElem);
                        }
                        waveBoard.board[newWaveElem.y][newWaveElem.x] = -1;
                    }
                }
            }
        }
        if (newWave.size() > 0)
        {
            wave = newWave;
            stepsCount++;
        }
        else
        {
            break;
        }
    }
    if (closestFigures.size() == 0)
    {
        return { {}, 0 };
    }
    return { closestFigures, stepsCount };
}

std::vector<Position> GetUpdatedPassedFields(const std::vector<Position>& passedFields, const std::vector<Position>& newFields)
{
    std::vector<Position> newPassedFields = passedFields;
    for (Position pos : newFields)
    {
        if (std::find(passedFields.cbegin(), passedFields.cend(), pos) == passedFields.cend())
        {
            newPassedFields.push_back(pos);
        }
    }
    return newPassedFields;
}

std::vector<Position> GetClearedFields(const std::vector<Position>& passedFields, const std::vector<Position>& fieldsToClear)
{
    std::vector<Position> newPositions;
    for (Position pos : fieldsToClear)
    {
        if (std::find(passedFields.cbegin(), passedFields.cend(), pos) == passedFields.cend())
        {
            newPositions.push_back(pos);
        }
    }
    return newPositions;
}

int FindShortestWayStepsCount(const Board& initialBoard, const std::vector<Position>& pastContextPassedFields)
{
    WaveBoard waveBoard = GetWaveBoardWithBlockedFields(initialBoard);
    std::pair<std::vector<Position>, int> positionsAndStepsCount = GetClosestFiguresPositionsList(waveBoard);
    std::vector<Position> closestFiguresPoses = GetClearedFields(pastContextPassedFields, positionsAndStepsCount.first);
    int shortestWayStepsCount = positionsAndStepsCount.second;
    std::vector<Position> currentContextPassedFields = pastContextPassedFields;
    int childStepsCount = INT_MAX;
    if (closestFiguresPoses.size() == 0)
    {
        return 0;
    }
    for (Position source : closestFiguresPoses)
    {
        Board newBoard = GetUpdatedBoard(initialBoard, waveBoard.kingPos, source);
        int newValue = FindShortestWayStepsCount(newBoard, GetUpdatedPassedFields(currentContextPassedFields, {source}));
        if (newValue == 0)
        {
            childStepsCount = 0;
            break;
        }
        childStepsCount = std::min(newValue, childStepsCount);
        //std::cout << source.x << "-" << source.y << ":" << childStepsCount << " ";
    }
    if (childStepsCount == INT_MAX)
    {
        childStepsCount = 0;
    }
    return shortestWayStepsCount + childStepsCount;
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
        output << FindShortestWayStepsCount(board, fields) << std::endl;
    }
    catch (std::exception e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
