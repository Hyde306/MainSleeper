#include "DxLib.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	SetOutApplicationLogValidFlag(false);//ログ出力オフ
	ChangeWindowMode(TRUE); //ウィンドウモード切り替え
	SetGraphMode(720,640, 32); //ウィンドウサイズ

	if (DxLib_Init() == -1) { //DXライブラリ初期化処理
		return -1;			  //エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); //描画先を裏画面に変更
	SetWindowText("speed"); //ウィンドウの名前

	//画像---------------------
	int gh_back;//背景
	//画像読み込み
	gh_back = LoadGraph("image\\back.png");

	//画像の分割読み込み
	//LoadDivGraph(画像ファイルポインタ、分割総数、横分割数、縦分割数、横サイズ、縦サイズ、保存配列ポインタ)
	//トランプの横サイズ：64、縦サイズ：92

	int mouseX, mouseY;//カーソル位置保存用

	while (1) 
	{
		//裏画面のデータを全て削除
		ClearDrawScreen();

		//処理----------------------------------------------------------------

		//マウスカーソルの位置を取得
		GetMousePoint(&mouseX, &mouseY);




		//画像の描画(位置X、位置Y、グラフィックハンドル、透明度の有効無効)
		//背景
		DrawGraph(0, 0, gh_back, true);

		//--------------------------------------------------------------------

		ScreenFlip(); //裏画面データを表画面へ反映

		//毎ループ呼び出す。エラーになった場合breakする
		if (ProcessMessage() == -1)break;
		//エスケープキーを押したり、エラーになった場合、breakする
		if (CheckHitKey(KEY_INPUT_ESCAPE))break;
	}

	//画像データ削除
	DeleteGraph(gh_back);

	WaitKey();	 //キー入力待ち
	DxLib_End(); //DXライブラリ使用の終了処理
	return 0;
}