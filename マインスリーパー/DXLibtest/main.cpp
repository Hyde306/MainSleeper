#include "DxLib.h"

struct Cell
{
	bool mine;   // 地雷があるか
	bool open;   // 開かれているか
	int around;  // 周囲の地雷数
};

const int BOARD_W = 15;
const int BOARD_H = 12;

Cell board[BOARD_H][BOARD_W];

// 描画クラス
class Renderer
{
public:
	bool Load();
	void DrawCell(int x, int y, const Cell& cell);
	void DrawBack();

private:
	int gh_back;
	int gh_turf;
	int gh_mine;
	int gh_empty;
	int gh_number[8]; // 0～7
};

// 画像読み込み
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

	if (gh_turf == -1 || gh_mine == -1 || gh_empty == -1)
		return false;

	for (int i = 1; i <= 7; i++)
	{
		if (gh_number[i] == -1)
			return false;
	}
	return true;
}

void Renderer::DrawBack()
{
	DrawGraph(0, 0, gh_back, true);
}

void Renderer::DrawCell(int x, int y, const Cell& cell)
{
	const int CELL_SIZE = 32;
	const int OFFSET_Y = 48;

	int drawX = x * CELL_SIZE;
	int drawY = y * CELL_SIZE + OFFSET_Y ;

	if (!cell.open)
	{
		DrawGraph(drawX, drawY, gh_turf, true);
		return;
	}

	if (cell.mine)
	{
		DrawGraph(drawX, drawY, gh_mine, true);
		return;
	}

	if (cell.around == 0)
	{
		DrawGraph(drawX, drawY, gh_empty, true);
		return;
	}

	DrawGraph(drawX, drawY, gh_number[cell.around], true);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//ログ出力オフ
	ChangeWindowMode(TRUE); //ウィンドウモード切り替え
	SetGraphMode(480,434, 32); //ウィンドウサイズ

	if (DxLib_Init() == -1) { //DXライブラリ初期化処理
		return -1;			  //エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); //描画先を裏画面に変更
	SetWindowText("speed"); //ウィンドウの名前

	Renderer renderer;
	if (!renderer.Load())
	{
		DxLib_End();
		return -1;
	}

	for (int y = 0; y < BOARD_H; y++)
	{
		for (int x = 0; x < BOARD_W; x++)
		{
			board[y][x].mine = false;
			board[y][x].open = false;
			board[y][x].around = 0;
		}
	}

	int mouseX, mouseY;//カーソル位置保存用

	while (1) 
	{
		//裏画面のデータを全て削除
		ClearDrawScreen();

		renderer.DrawBack();

		for (int y = 0; y < BOARD_H; y++)
		{
			for (int x = 0; x < BOARD_W; x++)
			{
				renderer.DrawCell(x, y, board[y][x]);
			}
		}

		//マウスカーソルの位置を取得
		GetMousePoint(&mouseX, &mouseY);

		//--------------------------------------------------------------------

		ScreenFlip(); //裏画面データを表画面へ反映

		//毎ループ呼び出す。エラーになった場合breakする
		if (ProcessMessage() == -1)break;
		//エスケープキーを押したり、エラーになった場合、breakする
		if (CheckHitKey(KEY_INPUT_ESCAPE))break;
	}
	WaitKey();	 //キー入力待ち
	DxLib_End(); //DXライブラリ使用の終了処理
	return 0;
}