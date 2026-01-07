#include "DxLib.h"

const int MARGIN = 44;

class Image
{
public:
	void Load();
	void Draw();
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

void Image::Draw()
{
	if (gh_back != -1)
	{
		DrawGraph(0, 0, gh_back, TRUE);
	}

	if (gh_turf != -1)
	{
		for (int i = 0; i < 12; i++)
		{
			for (int j = 0; j < 15; j++)
			{
				DrawGraph((j * 32), MARGIN + (i * 32), gh_turf, TRUE);
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

	Image Box;

	Box.Load();

	while (1) 
	{
		//裏画面のデータを全て削除
		ClearDrawScreen();

		//処理----------------------------------------------------------------
		Box.Draw();

		//マウスカーソルの位置を取得
		GetMousePoint(&mouseX, &mouseY);

		//--------------------------------------------------------------------

		ScreenFlip(); //裏画面データを表画面へ反映

		//毎ループ呼び出す。エラーになった場合breakする
		if (ProcessMessage() == -1)break;
		//エスケープキーを押したり、エラーになった場合、breakする
		if (CheckHitKey(KEY_INPUT_ESCAPE))break;
	}

	//画像データ削除
	

	WaitKey();	 //キー入力待ち
	DxLib_End(); //DXライブラリ使用の終了処理
	return 0;
}