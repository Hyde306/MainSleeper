#include "DxLib.h"

const int MARGIN     = 44;
const int VAR        = 12;//縦
const int HOR        = 15;//横
const int MINE_COUNT = 30;

// 地雷マップ
class Land
{
public:
	void Mine(int SafeCol, int SafeRow);
	void Num();

	int Mine_map[HOR][VAR];
};

// 描画マップ
class Map
{
public:
	void Open_Cell(int row, int col, int data);//最初は芝生マス→地雷マップの情報を得て反映
	void Open_Area(int col, int row, Land& land);

	int Cell[HOR][VAR];
};

class Image
{
public:
	void Load();
	void Draw(const Map& map);

private:
	int gh_back; 
	int gh_turf;
	int frag;
	int mine;
	int empty;
	int n_one;
	int n_two;
	int n_three;
	int n_four;
	int n_five;
	int n_six;
	int n_seven;
};

void Land::Mine(int SafeCol, int SafeRow)
{
	for (int i = 0; i < VAR; i++)
	{
		for (int j = 0; j < HOR; j++)
		{
			Mine_map[j][i] = 0;
		}
	}

	int count = 0;
	while (count < MINE_COUNT)
	{
		int x = rand() % HOR;
		int y = rand() % VAR;

		if (abs(x - SafeCol) <= 1 && abs(y - SafeRow) <= 1)
			continue;

		if (Mine_map[x][y] == 0)
		{	
			Mine_map[x][y]= -1;
			count++;
		}
	}
}

void Land::Num()
{
	for (int i = 0; i < VAR; i++)
	{
		for (int j = 0; j < HOR; j++)
		{
			if (Mine_map[j][i] == -1)// 地雷判定
			{
				for (int hor = -1; hor <= 1; hor++)// 8方向の走査
				{
					for (int var = -1; var <= 1; var++)
					{
						if ((hor != 0) || (var != 0))// 地雷位置の除外
						{
							if (((0 <= j + var) && (j + var < HOR)) && ((0 <= i + hor) && (i + hor < VAR))) // 外に出ないか
							{
								if (Mine_map[j + var][i + hor] != -1) // 周囲が地雷ではないとき
								{
									Mine_map[j + var][i + hor]++;
								}
							}
						}
					}
				}
			}
		}
	}
}

void Map::Open_Cell(int row, int col, int data)
{
	Cell[row][col] = data;
}

void Map::Open_Area(int col, int row, Land& land)
{
	// 範囲外
	if ((col < 0 || col >= HOR || row < 0 || row >= VAR) || (Cell[col][row] != -2))
	{
		return;
	}

	Cell[col][row] = land.Mine_map[col][row];

	// 0 でなければ終了
	if (Cell[col][row] != 0)
		return;

	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}
			Open_Area(col + x, row + y, land);
		}
	}
}

void Image::Load()
{
	gh_back = LoadGraph("image/back.png");
	gh_turf = LoadGraph("image/turf.png");
	frag    = LoadGraph("image/frag.png");
	mine	= LoadGraph("image/mine.png");
	empty   = LoadGraph("image/empty.png");
	n_one   = LoadGraph("image/one.png");
	n_two	= LoadGraph("image/two.png");
	n_three = LoadGraph("image/three.png");
	n_four  = LoadGraph("image/four.png");
	n_five  = LoadGraph("image/five.png");
	n_six	= LoadGraph("image/six.png");
	n_seven = LoadGraph("image/seven.png");
}

void Image::Draw(const Map & map)
{
	DrawGraph(0, 0, gh_back, TRUE);
	DrawGraph(0, 13, frag, TRUE);

	for (int i = 0; i < VAR; i++)
	{
		for (int j = 0; j < HOR; j++)
		{
			int x = j * 32;
			int y = MARGIN + (i * 32);
			int value = map.Cell[j][i];

			if (value == -2)
			{
				DrawGraph(x, y, gh_turf, TRUE);
			}
			else if (value == -1)
			{
				DrawGraph(x, y, mine, TRUE);
			}
			else if (value == 0)
			{
				DrawGraph(x, y, empty, TRUE);
			}
			else if (value == -3)
			{
				DrawGraph(x, y, frag, TRUE);
			}
			else if (value == 1)
			{
				DrawGraph(x, y, n_one, TRUE);
			}
			else if (value == 2)
			{
				DrawGraph(x, y, n_two, TRUE);
			}
			else if (value == 3)
			{
				DrawGraph(x, y, n_three, TRUE);
			}
			else if (value == 4)
			{
				DrawGraph(x, y, n_four, TRUE);
			}
			else if (value == 5)
			{
				DrawGraph(x, y, n_five, TRUE);
			}
			else if (value == 6)
			{
				DrawGraph(x, y, n_six, TRUE);
			}
			else if (value == 7)
			{
				DrawGraph(x, y, n_seven, TRUE);
			}
		}
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//ログ出力オフ
	ChangeWindowMode(TRUE); //ウィンドウモード切り替え
	SetGraphMode(480,430, 32); //ウィンドウサイズ

	if (DxLib_Init() == -1) { //DXライブラリ初期化処理
		return -1;			  //エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); //描画先を裏画面に変更
	SetWindowText("speed"); //ウィンドウの名前

	int mouseX, mouseY;//カーソル位置保存用
	int prevMouseInput = 0;// 前のフレームのときのマウス状態
	int Remaining = MINE_COUNT;
	int StartTimer = 0;
	int Time = 0;

	bool GameOver = false;
	bool GameClear = false;
	bool FirstClick = true;

	Image Box;
	Map map;
	Land land;

	Box.Load();

	for (int i = 0; i < VAR; i++)
	{
		for (int j = 0; j < HOR; j++)
		{
			map.Cell[j][i] = -2; // 開いていない
		}
	}

	while (1) 
	{
		//裏画面のデータを全て削除
		ClearDrawScreen();

		//マウスカーソルの位置を取得
		GetMousePoint(&mouseX, &mouseY);

		int mouseInput = GetMouseInput();
		int row = (mouseY - MARGIN) / 32;
		int col = mouseX / 32;
		int data;

		if ((mouseInput & MOUSE_INPUT_LEFT) && !(prevMouseInput & MOUSE_INPUT_LEFT) && (GameOver == false))
		{
			if ((0 <= row && row < VAR) && (0 <= col && col < HOR)) 
			{
				if (FirstClick)
				{
					land.Mine(col, row);
					land.Num();

					// タイマースタート
					StartTimer = GetNowCount();

					FirstClick = false;
				}
				if (map.Cell[col][row] == -2)
				{
					data = land.Mine_map[col][row];

					if (data == -1)
					{
						map.Open_Cell(col, row, data);
						GameOver = true;
					}
					else if (data == 0)
					{
						map.Open_Area(col, row, land);
					}
					else
					{
						map.Open_Cell(col, row, data);
					}
				}
			}
		}

		if ((mouseInput & MOUSE_INPUT_RIGHT) && !(prevMouseInput & MOUSE_INPUT_RIGHT) && (GameOver == false))
		{
			if ((0 <= row && row < VAR) && (0 <= col && col < HOR))
			{
				if (map.Cell[col][row] == -2)
				{
					map.Cell[col][row] = -3; // 旗を置く
					Remaining--;
				}
				else if (map.Cell[col][row] == -3)
				{
					map.Cell[col][row] = -2; // 旗を外す
					Remaining++;
				}
			}
		}
		prevMouseInput = mouseInput;
		
		if ((FirstClick == false) && (GameOver == false))
		{
			Time = (GetNowCount() - StartTimer) / 1000;
		}

		Box.Draw(map);

		SetFontSize(30);
		DrawFormatString(40, 15, GetColor(255, 255, 255), "%d", Remaining);
		DrawFormatString(120, 15, GetColor(255, 255, 255), "%d秒", Time);

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