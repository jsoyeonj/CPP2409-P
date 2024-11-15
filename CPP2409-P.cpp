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

enum MAIN_MENU
{
    MENU_START = 1,
    MENU_EXIT,
    MENU_NONE
};

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

class CMenu
{
public:
    int OutputMenu()
    {
        while (true)
        {
            system("cls");
            CreateLogo();
            gotoxy(22, 17);
            cout << "======================= 1. 시작 =======================" << endl;
            gotoxy(22, 18);
            cout << "======================= 2. 종료 =======================" << endl;

            gotoxy(22, 22);
            cout << "메뉴를 선택하세요 : ";
            int iInput = InputInt();
            if (iInput < MENU_START || iInput >= MENU_NONE)
            {
                cout << "다시 입력하세요." << endl;
                system("pause");
                continue;
            }
            return iInput;
        }
    }

private:
    void CreateLogo() // Tetris 출력
    {
        gotoxy(20, 7);
        cout << "    ######    ###     ######   ###      ###       ##     " << endl;
        gotoxy(20, 8);
        cout << "     ###    ##        ####    ## ##      ##     ## #    " << endl;
        gotoxy(20, 9);
        cout << "      ##    ##          ##    ##  ##     ##     ##      " << endl;
        gotoxy(20, 10);
        cout << "      ##   #####        ##   #####       ##     ####   " << endl;
        gotoxy(20, 11);
        cout << "      ##   ##           ##   ####       ###         ## " << endl;
        gotoxy(20, 12);
        cout << "      ###  ###          ###  ## ##      ###    ###  ## " << endl;
        gotoxy(20, 13);
        cout << "      ###    #####      ###  ##   ##    ###     #####  " << endl;
    }

    int InputInt()
    {
        int iInput;
        cin >> iInput;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1024, '\n');
            return 0;
        }

        return iInput;
    }
};

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

void DrawMap(char map[MAPHEIGHT][MAPWIDTH]) // 맵 생성
{
    // 1. 30x15 맵을 에워싸는 틀 생성 및 출력
    gotoxy(10, 5);
    for (int i = 0; i <= MAPHEIGHT; i++)
    {
        for (int j = 0; j < MAPWIDTH + 2; j++)
        {
            gotoxy(10 + j * 2, 5 + i);
            if (j == MAPWIDTH + 1 || j == 0 || i == MAPHEIGHT)
                cout << "▩";
        }
    }
    // 2. 틀 안에 30x15 맵 생성 및 출력
    gotoxy(12, 5);
    for (int i = 0; i < MAPHEIGHT; i++)
    {
        for (int j = 0; j < MAPWIDTH; j++)
        {
            gotoxy(12 + j * 2, 5 + i);
            if (map[i][j] == '0')
                cout << "  ";
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
    CMenu Menu;
    int iInput = Menu.OutputMenu(); // 구조체, Switch-Case문을 통한 메뉴 선택지 입력

    switch (iInput)
    {
    case MENU_START:
        system("cls");
        Play(); // 게임 시작
        break;
    case MENU_EXIT:
        system("cls");
        break;
    }
    return 0;
}