#include "Stage.h"
#include "Player.h"
#include <DxLib.h>
#include "Input.h"
#include "Bullet.h"
#include "Enemy.h"

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
	namespace BulletParams
	{
		constexpr float SPEED = 600.0f;          // 速度
		const unsigned int COLOR = GetColor(255, 255, 255); // 白
		constexpr float RADIUS = 2.0f;           // 半径
		constexpr float LIFE = 3.0f;             // 寿命（秒）
	}

    namespace EnemyParams
    {
        // 見た目
        constexpr int   SEGMENTS_MIN = 12;
        constexpr int   SEGMENTS_MAX = 20;
        constexpr float JITTER_MIN = 0.25f;
        constexpr float JITTER_MAX = 0.45f;

        // サイズ
        constexpr float R_MIN = 25.0f;
        constexpr float R_MAX = 60.0f;

        // 移動
        constexpr float SPEED_MIN = 80.0f;
        constexpr float SPEED_MAX = 220.0f;

        // 回転
        constexpr float OMEGA_MIN = -1.2f; // rad/sec
        constexpr float OMEGA_MAX = 1.2f;

        inline unsigned int COLOR()
        {
            return GetColor(180, 180, 180);
        }

        inline float Rand01()
        {
            return (float)GetRand(10000) / 10000.0f;
        }

        inline float RandRange(float a, float b)
        {
            return a + (b - a) * Rand01();
        }

        inline int RandRangeInt(int a, int b)
        {
            // a..b
            return a + GetRand(b - a);
        }
    }
}


Stage::Stage()
    : player_(nullptr)
{

}

void Stage::Initialize()
{
    Release();
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
    // とりあえず敵を数体出す
    for (int i = 0; i < 5; ++i)
    {
        SpawnEnemy();
    }
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

    // 敵追加スポーン（デバッグ用：Eキー）
    if (Input::IsKeyDown(KEY_INPUT_E))
    {
        SpawnEnemy();
    }

    for (auto b : bullets_) b->Update();

    // Update enemies
    for (Enemy* e : enemies_)
    {
        if (e) e->Update();
    }


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

    Vector2D dir = player_->GetDirVec();
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
        new Bullet(pos, vel, BulletParams::COLOR, BulletParams::RADIUS, BulletParams::LIFE)
    );
}

void Stage::SpawnEnemy()
{
    // 画面端付近から湧かせる（中心湧きよりゲームっぽい）
    const float W = (float)WIN_WIDTH;
    const float H = (float)WIN_HEIGHT;

    Vector2D pos;
    int side = GetRand(3); // 0..3
    if (side == 0) { pos.x = 0.0f; pos.y = EnemyParams::RandRange(0.0f, H); }
    if (side == 1) { pos.x = W;    pos.y = EnemyParams::RandRange(0.0f, H); }
    if (side == 2) { pos.x = EnemyParams::RandRange(0.0f, W); pos.y = 0.0f; }
    if (side == 3) { pos.x = EnemyParams::RandRange(0.0f, W); pos.y = H; }

    // 速度：ランダム方向
    float ang = EnemyParams::RandRange(0.0f, 6.2831853072f);
    float spd = EnemyParams::RandRange(EnemyParams::SPEED_MIN, EnemyParams::SPEED_MAX);
    Vector2D vel(std::cos(ang) * spd, std::sin(ang) * spd);

    // 形状
    float baseR = EnemyParams::RandRange(EnemyParams::R_MIN, EnemyParams::R_MAX);
    int segments = EnemyParams::RandRangeInt(EnemyParams::SEGMENTS_MIN, EnemyParams::SEGMENTS_MAX);
    float jitter = EnemyParams::RandRange(EnemyParams::JITTER_MIN, EnemyParams::JITTER_MAX);

    // 回転
    float omega = EnemyParams::RandRange(EnemyParams::OMEGA_MIN, EnemyParams::OMEGA_MAX);

    enemies_.push_back(new Enemy(
        pos,
        vel,
        EnemyParams::COLOR(),
        baseR,
        segments,
        jitter,
        omega
    ));
}

void Stage::Draw()
{
    if (player_)
    {
        player_->Draw();
    }
    // 敵→弾→プレイヤー（好み。弾を上に出すなら敵→プレイヤー→弾でもOK）
    for (Enemy* e : enemies_)
    {
        if (e) e->Draw();
    }

    for (Bullet* b : bullets_)
    {
        if (b) b->Draw();
    }
}

void Stage::Release()
{

	// 終了処理が必要ならここに書く
    for (Bullet* b : bullets_)
    {
        delete b;
    }
    bullets_.clear();
    // Enemies
    for (Enemy* e : enemies_)
    {
        delete e;
    }
    enemies_.clear();
	if (player_)
	{
		delete player_;
		player_ = nullptr;
	}
}