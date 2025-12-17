#include "Stage.h"
#include "Player.h"
#include <DxLib.h>
#include "Input.h"
#include "Bullet.h"

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
    // 弾発射（押した瞬間）
    if (Input::IsKeyDown(KEY_INPUT_Z))
    {
        SpawnBullet();
    }

    for (auto b : bullets_) b->Update();

    // 寿命で削除
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const Bullet* b) { return b->IsDead(); }),
        bullets_.end()
    );
}

void Stage::SpawnBullet()
{
    if (!player_) return;

    // ※ Player 側に以下のアクセサがある前提：
    //   - Vector2D GetPos() const;
    //   - Vector2D GetVel() const;   （無いなら 0 でOK）
    //   - Vector2D GetDir() const;
    //   - float    GetRadius() const;

    Vector2D dir = player_->GetDir();
    Vector2D pos = player_->GetPos();

    // 発射位置を少し前へ（自分の半径＋少し）
    const float offset = player_->GetRadius() + 8.0f;
    pos.x += dir.x * offset;
    pos.y += dir.y * offset;

    Vector2D vel;
    vel.x = dir.x * BulletParams::SPEED;
    vel.y = dir.y * BulletParams::SPEED;

    // プレイヤーの慣性を弾に乗せたいなら（GetVel() がある場合のみ）
    // Vector2D pv = player_->GetVel();
    // vel.x += pv.x;
    // vel.y += pv.y;

    bullets_.push_back(
        new Bullet(pos, vel, BulletParams::COLOR(), BulletParams::RADIUS, BulletParams::LIFE)
    );
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