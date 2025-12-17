#include "Stage.h"
#include "Player.h"
#include <DxLib.h>

namespace
{
    // ---- 初期パラメータ ----

    // 初期位置（画面中央）
	constexpr float START_X = (float)(WIN_WIDTH / 2);
	constexpr float START_Y = (float)(WIN_HEIGHT / 2);
	const Vector2D START_POS = Vector2D(START_X, START_Y);

    // 初期速度
    constexpr float START_VX = 0.0f;
    constexpr float START_VY = 0.0f;
	const Vector2D START_VEL = Vector2D(START_VX, START_VY);


    // 初期向き（上向き）
    constexpr float DIR_X = 0.0f;
    constexpr float DIR_Y = -1.0f;
	const Vector2D START_DIR = Vector2D(DIR_X, DIR_Y);

    // 形状
    constexpr float RADIUS = 20.0f;

    // 回転速度（rad/sec）
    constexpr float OMEGA = 2.0f;
	const unsigned int COLOR = GetColor(255, 0, 0); // 赤

}


Stage::Stage()
    : player_(nullptr)
{

}

void Stage::Initialize()
{
	// 初期化処理が必要ならここに書く
    // Player は Stage が所有（生成して保持）
    player_ = new Player(
		START_POS,
		START_VEL,
		COLOR,
		START_DIR,
		RADIUS,
		OMEGA
	);
}

// Stage.h にデストラクタ宣言が無くても、暗黙デストラクタをここで定義できる
Stage::~Stage()
{
    delete player_;
    player_ = nullptr;
}

void Stage::Update()
{
    if (player_)
    {
        player_->Update();
    }
}

void Stage::Draw()
{
    if (player_)
    {
        player_->Draw();
    }
}

void Stage::Release()
{
	// 終了処理が必要ならここに書く
	if (player_)
	{
		delete player_;
		player_ = nullptr;
	}
}