#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

#define MAPWIDTH 15  // 맵 Row 크기 , 양쪽 벽이 있으므로 최종적으로 Row + 2
#define MAPHEIGHT 30 // 맵 Col 크기, 맨 아래 벽이 있으므로 최종적으로는 Col + 1
// 블록은 1, 빈 공간은 0, 벽은 2

// 콘솔 인코딩 설정을 위한 함수
void SetConsoleEncoding()
{
    SetConsoleOutputCP(CP_UTF8); // 출력 인코딩을 UTF-8로 설정
    SetConsoleCP(CP_UTF8);       // 입력 인코딩을 UTF-8로 설정
}

typedef struct _currentPosition // 블록 좌표 제어를 위한 구조체 자료형 선언
{
    int X;
    int Y;
} Position;

void PositionInit(Position *BlockPos) // 블록 좌표 초기값 지정
{
    BlockPos->X = 5;
    BlockPos->Y = 0;
}

void ConsoleInit() // 콘솔 커서 숨김 설정 및 콘솔 창 크기 지정
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);

    system("mode con cols=100 lines=40"); // 콘솔 창 크기 지정
}

void gotoxy(int x, int y) // 커서 좌표 이동 함수
{
    COORD pos = {(SHORT)x, (SHORT)y}; // 명시적 형변환

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

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

Block_Info BlockInfo = BlockNull;

class CBlock
{
public:
    int IBlock[5][5] =
        {
            {0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 1, 0, 0}};

    int OBlock[5][5] =
        {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0},
            {0, 1, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}};

    int TBlock[5][5] =
        {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}};

    int ZBlock[5][5] =
        {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0},
            {0, 0, 1, 1, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}};

    int SBlock[5][5] =
        {
            {0, 0, 0, 0, 0},
            {0, 0, 1, 1, 0},
            {0, 1, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}};

    int LBlock[5][5] =
        {
            {0, 1, 0, 0, 0},
            {0, 1, 0, 0, 0},
            {0, 1, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}};

    int JBlock[5][5] =
        {
            {0, 0, 0, 1, 0},
            {0, 0, 0, 1, 0},
            {0, 0, 1, 1, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}};
    // 블록 모양을 배열로 관리하는 BlockShapes 멤버 변수 선언
    int (*BlockShapes[7])[5] = {IBlock, OBlock, TBlock, ZBlock, SBlock, LBlock, JBlock};
    void DrawNextBlock(int blockShape[5][5]) // 다음 블록 출력
    {
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                gotoxy(57 + j * 2, 6 + i);
                cout << "  ";
            }
        }
        switch (BlockInfo)
        {
        case BlockI:
            for (int i = 0; i < 4; i++)
            {
                gotoxy(62, 8 + i);
                cout << "■";
            }
            break;
        case BlockO:
            for (int i = 0; i < 2; i++)
                for (int j = 0; j < 2; j++)
                {
                    gotoxy(61 + j * 2, 8 + i);
                    cout << "■";
                }
            break;
        case BlockT:
            for (int j = 0; j <= 2; j++)
            {
                gotoxy(60 + j * 2, 8);
                cout << "■";
            }
            gotoxy(62, 9);
            cout << "■";
            break;
        case BlockZ:
            for (int j = 0; j < 2; j++)
            {
                gotoxy(61 + j * 2, 9);
                cout << "■";
                gotoxy(61 + j * 2 + 2, 10);
                cout << "■";
            }
            break;
        case BlockS:
            for (int j = 0; j < 2; j++)
            {
                gotoxy(63 + j * 2, 9);
                cout << "■";
                gotoxy(63 + j * 2 - 2, 10);
                cout << "■";
            }
            break;
        case BlockL:
            for (int i = 0; i < 3; i++)
            {
                gotoxy(61, 8 + i);
                cout << "■";
                if (i == 2)
                {
                    gotoxy(63, 8 + i);
                    cout << "■";
                }
            }
            break;
        case BlockJ:
            for (int i = 0; i < 3; i++)
            {
                gotoxy(63, 8 + i);
                cout << "■";
                if (i == 2)
                {
                    gotoxy(61, 8 + i);
                    cout << "■";
                }
            }
            break;
        default:
            break;
        }
    }

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
        blockPosition.X = MAPWIDTH / 2 - 2;
        blockPosition.Y = 0;
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
    ConsoleInit();        // 커서 숨기기 및 콘솔창 크기 초기화 함수
    Play();
    return 0;
}