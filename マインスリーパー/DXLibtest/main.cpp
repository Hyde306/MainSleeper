#include "DxLib.h"
#include <cstdlib>
#include <ctime>

const int BOARD_W = 15;
const int BOARD_H = 12;
const int CELL_SIZE = 32;
const int OFFSET_Y = 48;
const int MINE_COUNT = 30;

// セル構造体
struct Cell
{
    bool mine;   // 地雷
    bool open;   // 開かれているか
    int around;  // 周囲の地雷数
};

Cell mineMap[BOARD_H][BOARD_W];
bool drawMap[BOARD_H][BOARD_W];

class Renderer
{
public:
    bool Load();
    void DrawBack();
    void DrawCell(int x, int y);

private:
    int gh_back;
    int gh_turf;
    int gh_mine;
    int gh_empty;
    int gh_number[8]; // 0～7
};

bool Renderer::Load()
{
    gh_back = LoadGraph("image\\back.png");
    gh_turf = LoadGraph("image\\turf.png");
    gh_mine = LoadGraph("image\\mine.png");
    gh_empty = LoadGraph("image\\empty.png");

    gh_number[1] = LoadGraph("image\\one.png");
    gh_number[2] = LoadGraph("image\\two.png");
    gh_number[3] = LoadGraph("image\\three.png");
    gh_number[4] = LoadGraph("image\\four.png");
    gh_number[5] = LoadGraph("image\\five.png");
    gh_number[6] = LoadGraph("image\\six.png");
    gh_number[7] = LoadGraph("image\\seven.png");

    if (gh_turf == -1 || gh_mine == -1 || gh_empty == -1) return false;
    for (int i = 1; i <= 7; i++) if (gh_number[i] == -1) return false;
    return true;
}
void Renderer::DrawBack() { DrawGraph(0, 0, gh_back, true); }

void Renderer::DrawCell(int x, int y)
{
    int drawX = x * CELL_SIZE;
    int drawY = y * CELL_SIZE + OFFSET_Y;

    if (!drawMap[y][x])
    {
        DrawGraph(drawX, drawY, gh_turf, true);
        return;
    }

    if (mineMap[y][x].mine)
        DrawGraph(drawX, drawY, gh_mine, true);
    else if (mineMap[y][x].around == 0)
        DrawGraph(drawX, drawY, gh_empty, true);
    else
        DrawGraph(drawX, drawY, gh_number[mineMap[y][x].around], true);
}

// 周囲の地雷数を計算
void CalcAround()
{
    for (int y = 0; y < BOARD_H; y++)
    {
        for (int x = 0; x < BOARD_W; x++)
        {
            if (mineMap[y][x].mine) continue;
            int count = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < BOARD_W && ny >= 0 && ny < BOARD_H)
                        if (mineMap[ny][nx].mine) count++;
                }
            mineMap[y][x].around = count;
        }
    }
}

void PlaceMinesSafe(int safeX, int safeY)
{
    srand((unsigned int)time(nullptr));
    int placed = 0;
    while (placed < MINE_COUNT)
    {
        int x = rand() % BOARD_W;
        int y = rand() % BOARD_H;

        if ((x >= safeX - 1 && x <= safeX + 1) && (y >= safeY - 1 && y <= safeY + 1))
            continue;

        if (!mineMap[y][x].mine)
        {
            mineMap[y][x].mine = true;
            placed++;
        }
    }
}

// セルを開く
void OpenCell(int x, int y)
{
    if (x < 0 || x >= BOARD_W || y < 0 || y >= BOARD_H) return;
    if (drawMap[y][x]) return;

    drawMap[y][x] = true;

    if (mineMap[y][x].around == 0 && !mineMap[y][x].mine)
    {
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
                if (dx != 0 || dy != 0)
                    OpenCell(x + dx, y + dy);
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    SetOutApplicationLogValidFlag(false);
    ChangeWindowMode(TRUE);
    SetGraphMode(480, 434, 32);

    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);
    SetWindowText("Minesweeper");

    Renderer renderer;
    if (!renderer.Load()) { DxLib_End(); return -1; }

    // 初期化
    for (int y = 0; y < BOARD_H; y++)
        for (int x = 0; x < BOARD_W; x++)
        {
            mineMap[y][x] = { false, false, 0 };
            drawMap[y][x] = false;
        }

    int mouseX, mouseY;
    bool firstClick = true;

    while (1)
    {
        ClearDrawScreen();
        renderer.DrawBack();

        for (int y = 0; y < BOARD_H; y++)
            for (int x = 0; x < BOARD_W; x++)
                renderer.DrawCell(x, y);

        GetMousePoint(&mouseX, &mouseY);

        // 左クリック
        if (GetMouseInput() & MOUSE_INPUT_LEFT)
        {
            int x = mouseX / CELL_SIZE;
            int y = (mouseY - OFFSET_Y) / CELL_SIZE;
            if (x >= 0 && x < BOARD_W && y >= 0 && y < BOARD_H)
            {
                if (firstClick)
                {
                    PlaceMinesSafe(x, y);
                    CalcAround();
                    firstClick = false;
                }
                OpenCell(x, y);
            }
        }

        ScreenFlip();
        if (ProcessMessage() == -1) break;
        if (CheckHitKey(KEY_INPUT_ESCAPE)) break;
    }

    DxLib_End();
    return 0;
}
