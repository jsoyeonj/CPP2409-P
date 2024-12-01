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
    void SetBlock(int blockShape[5][5]) // 현재 블록, 다음 블록 제어
    {
        srand((unsigned int)(time(NULL)));
        switch (rand() % 7)
        { // switch문으로 블럭 추출 후 Blockshape에 저장

        case 0:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = IBlock[i][j];
            BlockInfo = BlockI;
            break;
        case 1:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = OBlock[i][j];
            BlockInfo = BlockO;
            break;
        case 2:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = TBlock[i][j];
            BlockInfo = BlockT;
            break;
        case 3:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = ZBlock[i][j];
            BlockInfo = BlockZ;
            break;
        case 4:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = SBlock[i][j];
            BlockInfo = BlockS;
            break;
        case 5:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = LBlock[i][j];
            BlockInfo = BlockL;
            break;
        case 6:
            for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                    blockShape[i][j] = JBlock[i][j];
            BlockInfo = BlockJ;
            break;
        default:
            break;
        }
    }

    void LimitBottom(int blockShape[5][5], int BottomArray[], int *BottomRow)
    {
        // 각 열마다 가장 아래에 있는 행을 BottomArray에 저장하고, 그 열은 BottomRow에 저장한다.
        for (int i = 0; i <= 4; i++)
        {
            for (int j = 0; j <= 4; j++)
            {
                if (blockShape[i][j] == 1)
                {
                    BottomArray[j] = i;
                    if ((*BottomRow) < BottomArray[j])
                        (*BottomRow) = BottomArray[j];
                }
            }
        }
    }

    int LimitLeft(int blockShape[5][5], int i, int LeftArray[], int *LeftRow, int *LeftCol)
    {
        // 각 행마다 가장 왼쪽에 있는 열을 LeftArray에 저장하고, LeftCol은 가장 왼쪽에 있는 블럭의 열, LeftRow는 가장 왼쪽에 있는 블럭의 행이다.
        for (int j = 4; j >= 0; j--)
        {
            if (blockShape[i][j] == 1)
            {
                LeftArray[i] = j;
                if ((*LeftCol) > j)
                {
                    (*LeftCol) = j;
                    (*LeftRow) = i;
                }
            }
        }
        return LeftArray[i];
    }

    int LimitRight(int blockShape[5][5], int i, int RightArray[], int *RightRow, int *RightCol)
    {
        for (int j = 0; j <= 4; j++)
        {
            if (blockShape[i][j] == 1)
            {
                RightArray[i] = j;
                if ((*RightCol) < j)
                {
                    (*RightCol) = j;
                    (*RightRow) = i;
                }
            }
        }
        return RightArray[i];
    }
};
class CMap
{

public:
    CBlock Block;

    void InitMap(char map[MAPHEIGHT][MAPWIDTH]) // 맵 초기화
    {
        for (int i = 0; i < MAPHEIGHT; i++)
        {
            for (int j = 0; j < MAPWIDTH; j++)
                map[i][j] = '0';
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
    void OutputBlock(char Map[MAPHEIGHT][MAPWIDTH], int blockShape[5][5], Position BlockPos) // 블록 출력 : 2차원 배열 Map에 표현
    {
        // 블럭 모양에 해당하는 좌표를 Map상의 좌표에 뿌려줌 ('1' 부여)
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (blockShape[i][j] == 1)
                    Map[BlockPos.Y + i][BlockPos.X + j] = '1';
            }
        }
    }

    bool GoDown(char Map[MAPHEIGHT][MAPWIDTH], int blockShape[5][5], Position *BlockPos)
    {
        int BottomArray[5] = {}; // 각 열마다 제일 아래에 있는 행을 저장하는 배열
        int BottomRow = 0;       // 제일 아래의 행

        Block.LimitBottom(blockShape, BottomArray, &BottomRow); // botRow에 블럭의 제일 아래 행이 저장, Array는 각 열에 마지막 행을 저장
        // Left, Right와 다르게 Array가 각 행이 아닌, 각 열을 기준으로 저장됐다.
        // 블록 제일 아랫부분의 y좌표가 blocky에 저장, 각 x축에 가장 아랫부분이 bottomArray에 저장
        for (int i = 0; i <= 4; i++)
        {
            if (BottomArray[i] != 0)
            {
                if (Map[(BlockPos->Y) + BottomArray[i] + 1][(BlockPos->X) + i] == '1' || (BlockPos->Y) + BottomRow + 1 == MAPHEIGHT)
                { // 각 열의 제일 아래 블록 한 칸 아래에 블록이 있거나 한칸 아래가 벽일 경우, true를 반환하여 바닥에 닿았다는 것을 알려준다.
                    return true;
                }
            }
        }
        RemoveShape(Map, blockShape, BlockPos);
        Sleep(80);
        (BlockPos->Y)++;
        return false; // 닿지 않았으면 블록을 한 칸 아래로 내린다.
    }

    void GoLeft(char Map[MAPHEIGHT][MAPWIDTH], int blockShape[5][5], Position *BlockPos)
    {
        int LeftArray[5] = {};
        bool goLeft = true;
        int LeftRow = 0;
        int LeftCol = 4;

        for (int i = 0; i <= 4; i++)
        {
            LeftArray[i] = Block.LimitLeft(blockShape, i, LeftArray, &LeftRow, &LeftCol);
            // LeftArray는 각 행에 가장 왼쪽에 있는 블록의 열을 저장하고, LeftRow는 가장 왼쪽에 있는 블록의 행을 저장한다.
        }

        if ((BlockPos->X) + LeftArray[LeftRow] - 1 < 0) // 가장 왼쪽에 있는 블럭의 한 칸 왼쪽이 벽이면 더 이상 가지 못하게 false로 바꾼다.
            goLeft = false;
        for (int j = 4; j >= 0; j--)
        {
            if (LeftArray[j] != 0)
            {
                if (Map[(BlockPos->Y) + j][(BlockPos->X) + LeftArray[j] - 1] == '1') // 각 열에 제일 왼쪽에 있는 블럭 한 칸 왼쪽에 블록이 있다면, 더 이상 가지 못하게 false로 바꾼다.
                {
                    goLeft = false;
                }
            }
        }
        if (goLeft == true)
        {
            RemoveShape(Map, blockShape, BlockPos);
            (BlockPos->X)--;
        }
    }

    void GoRight(char Map[MAPHEIGHT][MAPWIDTH], int blockShape[5][5], Position *BlockPos)
    {
        int RightArray[5] = {};
        int RightRow = 0;
        int RightCol = 0;
        bool goRight = true;

        for (int i = 0; i <= 4; i++)
        {
            RightArray[i] = Block.LimitRight(blockShape, i, RightArray, &RightRow, &RightCol);
            // Left와 같은 방식으로 동작.
        }

        if ((BlockPos->X) + RightArray[RightRow] + 1 >= MAPWIDTH)
            goRight = false;
        for (int j = 0; j <= 4; j++)
        {

            if (RightArray[j] != 0)
            {
                if (Map[(BlockPos->Y) + j][(BlockPos->X) + RightArray[j] + 1] == '1')
                {
                    goRight = false;
                }
            }
        }
        if (goRight == true)
        {
            RemoveShape(Map, blockShape, BlockPos);
            (BlockPos->X)++;
        }
    }
    void Rotate(char Map[MAPHEIGHT][MAPWIDTH], int blockShape[5][5], Position *BlockPos)
    {
        int temp_arr[5][5] = {};
        int RightArray[5] = {}, LeftArray[5] = {}, BottomArray[5] = {};
        int RightRow = 0, RightCol = 0;
        int LeftRow = 0, LeftCol = 4;
        int BottomRow = 0;
        int count = 0;

        bool rotate = true;
        bool goLeft = true;
        bool goRight = true;

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                temp_arr[i][j] = blockShape[5 - j - 1][i];
            }
        }
        // 회전된 모양이 가능한지 확인
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (temp_arr[i][j] == 1)
                {
                    // 회전된 위치가 맵 범위를 벗어나거나 다른 블록과 충돌하면 회전 불가
                    if ((BlockPos->X + j) >= MAPWIDTH ||
                        (BlockPos->X + j) < 0 ||
                        (BlockPos->Y + i) >= MAPHEIGHT ||
                        Map[BlockPos->Y + i][BlockPos->X + j] == '1')
                    {
                        rotate = false;
                        break;
                    }
                }
            }
            if (!rotate)
                break;
        }

        // 회전이 가능하면 블록 모양을 회전된 모양으로 변경
        if (rotate)
        {
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 5; j++)
                {
                    blockShape[i][j] = temp_arr[i][j];
                }
            }
        }
        for (int i = 0; i < 5; i++)
        {
            RightArray[i] = Block.LimitRight(temp_arr, i, RightArray, &RightRow, &RightCol); // 그 행의 제일 오른쪽에 있는 블럭의 열
            LeftArray[i] = Block.LimitLeft(temp_arr, i, LeftArray, &LeftRow, &LeftCol);      // 그 행의 제일 왼쪽에 있는 블럭의 열
        }

        Block.LimitBottom(temp_arr, BottomArray, &BottomRow);
    }
    void RemoveShape(char Map[MAPHEIGHT][MAPWIDTH], int blockShape[5][5], Position *BlockPos)
    {
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (blockShape[i][j] == 1)
                    Map[BlockPos->Y + i][BlockPos->X + j] = '0';
            }
        }
    }

    bool GameOverCheck(char map[MAPHEIGHT][MAPWIDTH])
    {
        for (int i = 0; i < MAPWIDTH; i++)
            if (map[0][i] == '1')
                return true;
        return false;
    }
};
int Play(char Map[MAPHEIGHT][MAPWIDTH])
{

    int Score = 0, BestScore = 0;
    int BlockShape[5][5] = {}; // 블럭 5x5
    int NextBlock[5][5] = {};  // 다음 블럭
    bool Bottom = false;
    bool noRotate = false;
    CMap map;
    CBlock block;
    Position BlockPos = {2, 2};
    map.InitMap(Map);
    map.DrawMap(Map);
    PositionInit(&BlockPos); // 위치 초기화

    block.SetBlock(BlockShape); // 블럭 모양 설정, 회전 가능 도형인지 반환
    block.SetBlock(NextBlock);  // 다음블럭 모양 설정
    block.DrawNextBlock(NextBlock);

    while (true)
    {
        if (Bottom)
        {
            if (map.GameOverCheck(Map))
                return 0;

            PositionInit(&BlockPos); // 커서 초기화

            for (int i = 0; i < 5; i++) // block을 다음 블럭 모양으로 가져오기
                for (int j = 0; j < 5; j++)
                    BlockShape[i][j] = NextBlock[i][j];

            block.SetBlock(NextBlock);      // 다음 블럭 새로 만들기
            block.DrawNextBlock(NextBlock); // 다음 블럭 모양 Map에 출력
            Bottom = false;                 // 새로운 블록의 움직임을 위해 false로 변환
            continue;
        }
        map.RemoveShape(Map, BlockShape, &BlockPos);
        map.OutputBlock(Map, BlockShape, BlockPos);
        map.DrawMap(Map);

        Bottom = map.GoDown(Map, BlockShape, &BlockPos);
        if (Bottom)
            continue;

        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        { // Press 'Space' : 블록 회전
            if (noRotate == false)
            {
                map.RemoveShape(Map, BlockShape, &BlockPos);
                map.Rotate(Map, BlockShape, &BlockPos);
            }
        }
        if (GetAsyncKeyState('A') & 0x8000)
        { // Press 'A' : 블록 좌측 이동
            map.RemoveShape(Map, BlockShape, &BlockPos);
            map.GoLeft(Map, BlockShape, &BlockPos);
        }
        if (GetAsyncKeyState('D') & 0x8000)
        { // Press 'D' : 블록 우측 이동
            map.RemoveShape(Map, BlockShape, &BlockPos);
            map.GoRight(Map, BlockShape, &BlockPos);
        }
        Sleep(50);
    }
    return 0;
}

int main()
{
    SetConsoleEncoding();               // 콘솔 인코딩 설정
    char Map[MAPHEIGHT][MAPWIDTH] = {}; // 맵 좌표 제어를 위한 2차원 배열 선언
    ConsoleInit();                      // 커서 숨기기 및 콘솔창 크기 초기화 함수
    CMenu Menu;
    int iInput = Menu.OutputMenu(); // 구조체, Switch-Case문을 통한 메뉴 선택지 입력

    switch (iInput)
    {
    case MENU_START:
        system("cls");
        Play(Map); // 게임 시작
        break;
    case MENU_EXIT:
        system("cls");
        break;
    }
}