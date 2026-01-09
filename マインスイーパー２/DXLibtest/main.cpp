#include "DxLib.h"

const int MARGIN     = 44;
const int VAR        = 12;//縦
const int HOR        = 15;//横
const int MINE_COUNT = 30;

// 描画マップ
class Map
{
public:
	void Open_Cell(int row, int col);//最初は芝生マス→地雷マップの情報を得て反映
	int Cell[HOR][VAR];
private:
	
};

class Image
{
public:
	void Load();
	void Draw(const Map& map);
	~Image();

private:
	int gh_back; //背景
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

// 地雷マップ
class Land
{
public:
	void Mine();
	void Num();
	void Space();
	int Mine_map[HOR][VAR];
private:

};

void Land::Mine()
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

void Map::Open_Cell(int row, int col)
{
	Cell[row][col] = 0;//Empty
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
	if (gh_back != -1)
	{
		DrawGraph(0, 0, gh_back, TRUE);
	}

	if ((gh_turf != -1) && (empty != -1))
	{
		for (int i = 0; i < VAR; i++)
		{
			for (int j = 0; j < HOR; j++)
			{
				if (map.Cell[j][i] == 0)
				{
					DrawGraph((j * 32), MARGIN + (i * 32), empty, TRUE);
				}
				else 
				{
					DrawGraph((j * 32), MARGIN + (i * 32), gh_turf, TRUE);
				}
			}
		}
	}
	
}
Image::~Image()
{
	if (gh_back != -1)
	{
		DeleteGraph(gh_back);
	}
	if (gh_turf != -1)
	{
		DeleteGraph(gh_turf);
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

	Image Box;
	Map map;
	Land land;

	Box.Load();
	land.Mine();

	while (1) 
	{
		//裏画面のデータを全て削除
		ClearDrawScreen();

		//マウスカーソルの位置を取得
		GetMousePoint(&mouseX, &mouseY);

		int mouseInput = GetMouseInput();

		if ((mouseInput & MOUSE_INPUT_LEFT) && !(prevMouseInput & MOUSE_INPUT_LEFT))
		{
			int row = (mouseY - MARGIN) / 32;
			int col = mouseX / 32;

			if ((0 <= row && row < 12) && (0 <= col && col < 15))
			{
				map.Open_Cell(col, row);
			}
		}

		prevMouseInput = mouseInput;

		Box.Draw(map);

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