#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

// 콘솔 인코딩 설정을 위한 함수
void SetConsoleEncoding()
{
    SetConsoleOutputCP(CP_UTF8); // 출력 인코딩을 UTF-8로 설정
    SetConsoleCP(CP_UTF8);       // 입력 인코딩을 UTF-8로 설정
}

#define MAPWIDTH 15
#define MAPHEIGHT 30

enum Block_Info
{
    BlockNull,
    BlockI,
    BlockO,
    BlockT,
    BlockZ,
    BlockS,
    BlockL,
    BlockJ
};

class Position
{
public:
    int X;
    int Y;
    Position(int x = 0, int y = 0) : X(x), Y(y) {}
};

class CBlock
{
public:
    int BlockShapes[7][5][5] = {
        {// IBlock
         {0, 0, 0, 0, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 1, 0, 0}},
        {// OBlock
         {0, 0, 0, 0, 0},
         {0, 1, 1, 0, 0},
         {0, 1, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0}},
        {// TBlock
         {0, 0, 0, 0, 0},
         {0, 1, 1, 1, 0},
         {0, 0, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0}},
        {// ZBlock
         {0, 0, 0, 0, 0},
         {0, 1, 1, 0, 0},
         {0, 0, 1, 1, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0}},
        {// SBlock
         {0, 0, 0, 0, 0},
         {0, 0, 1, 1, 0},
         {0, 1, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0}},
        {// LBlock
         {0, 1, 0, 0, 0},
         {0, 1, 0, 0, 0},
         {0, 1, 1, 0, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0}},
        {// JBlock
         {0, 0, 0, 1, 0},
         {0, 0, 0, 1, 0},
         {0, 0, 1, 1, 0},
         {0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0}}};

    int (*currentBlock)[5];
    Position blockPosition;

    CBlock()
    {
        srand((unsigned)time(0));
        GenerateNewBlock();
    }

    void GenerateNewBlock()
    {
        int randomIndex = rand() % 7;
        currentBlock = BlockShapes[randomIndex];
        blockPosition = Position(MAPWIDTH / 2 - 2, 0);
    }

    void MoveDown()
    {
        blockPosition.Y++;
    }

    void DrawBlock(char map[MAPHEIGHT][MAPWIDTH])
    {
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (currentBlock[i][j] == 1 &&
                    blockPosition.Y + i < MAPHEIGHT &&
                    blockPosition.X + j < MAPWIDTH)
                {
                    map[blockPosition.Y + i][blockPosition.X + j] = '1';
                }
            }
        }
    }
};

void InitMap(char map[MAPHEIGHT][MAPWIDTH])
{
    for (int i = 0; i < MAPHEIGHT; i++)
    {
        for (int j = 0; j < MAPWIDTH; j++)
        {
            map[i][j] = '0';
        }
    }
}

void DrawMap(char map[MAPHEIGHT][MAPWIDTH])
{
    system("cls");
    for (int i = 0; i < MAPHEIGHT; i++)
    {
        for (int j = 0; j < MAPWIDTH; j++)
        {
            if (map[i][j] == '0')
                cout << "  "; // 빈 공간을 두 칸으로 수정
            else if (map[i][j] == '1')
                cout << "■";
        }
        cout << endl;
    }
}

void Play()
{
    char map[MAPHEIGHT][MAPWIDTH];
    InitMap(map);
    CBlock block;

    while (true)
    {
        InitMap(map);
        block.DrawBlock(map);
        DrawMap(map);
        block.MoveDown();
        Sleep(500);
    }
}

int main()
{
    SetConsoleEncoding(); // 콘솔 인코딩 설정
    Play();
    return 0;
}