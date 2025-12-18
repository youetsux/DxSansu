#include "Bullet.h"
#include "Math2D.h"
#include "globals.h"
#include <DxLib.h>
#include <cmath>


Bullet::Bullet(const Vector2D& pos, const Vector2D& vel, unsigned int color,
    float radius, float lifeSec)
    : Base(pos, vel, color)
    , radius_(radius)
    , life_(lifeSec)
{
}

void Bullet::Update()
{
    float dt = GetDeltaTime();
    if (dt > 0.05f) dt = 0.05f;

    // 寿命
    life_ -= dt;

    // 移動（数学座標）
    pos_.x += vel_.x * dt;
    pos_.y += vel_.y * dt;

    // 画面端ワープ（プレイヤーと同じ方式）
    const float W = (float)WIN_WIDTH;
    const float H = (float)WIN_HEIGHT;

    if (pos_.x < -radius_)           pos_.x += (W + radius_ * 2.0f);
    else if (pos_.x > W + radius_)   pos_.x -= (W + radius_ * 2.0f);

    if (pos_.y < -radius_)           pos_.y += (H + radius_ * 2.0f);
    else if (pos_.y > H + radius_)   pos_.y -= (H + radius_ * 2.0f);
}

void Bullet::Draw()
{
    // ワールド→スクリーン
    Vector2D s = Math2D::World2Screen(pos_);

    DrawCircleAA(
        s.x, s.y,
        radius_,
        16,
        Color_,
        TRUE
    );
}
