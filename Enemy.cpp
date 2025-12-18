#include "Enemy.h"
#include "Math2D.h"
#include "globals.h"
#include <DxLib.h>
#include <cmath>
#include <algorithm>

namespace
{
    inline float Rand01()
    {
        return (float)GetRand(10000) / 10000.0f; // 0..1
    }

    inline float ClampDt(float dt)
    {
        if (dt > 0.05f) dt = 0.05f;
        return dt;
    }

    inline float WrapAngle(float a)
    {
        a = std::fmod(a, Math2D::PI2);
        if (a < 0.0f) a += Math2D::PI2;
        return a;
    }
}

Enemy::Enemy(const Vector2D& pos,
    const Vector2D& vel,
    unsigned int color,
    float baseRadius,
    int segments,
    float jitter,
    float omegaRadPerSec)
    : Base(pos, vel, color)
    , radius_(baseRadius)
    , angle_(0.0f)
    , omega_(omegaRadPerSec)
    , dead_(false)
{
    if (segments < 6) segments = 6;
    if (jitter < 0.0f) jitter = 0.0f;
    if (jitter > 0.8f) jitter = 0.8f;

    GenerateShape(segments, baseRadius, jitter);

    // ちょい平滑化すると “岩” っぽくなる
    //SmoothShape(0.5f);
    //SmoothShape(0.5f);

    // 代表半径を少し安全側に（当たり判定用）
    // 最大頂点距離を採用
    float maxR2 = 0.0f;
    for (const auto& p : local_)
    {
        float r2 = p.x * p.x + p.y * p.y;
        if (r2 > maxR2) maxR2 = r2;
    }
    radius_ = std::sqrt(maxR2);
}

void Enemy::GenerateShape(int segments, float baseRadius, float jitter)
{
    local_.clear();
    local_.reserve(segments);

    for (int i = 0; i < segments; ++i)
    {
        float t = (float)i / (float)segments; // 0..1
        float ang = t * Math2D::PI2;

        float k = 1.0f + (Rand01() * 2.0f - 1.0f) * jitter; // 1±jitter
        float r = baseRadius * k;

        local_.push_back(Vector2D(std::cos(ang) * r, std::sin(ang) * r));
    }
}

void Enemy::SmoothShape(float w)
{
    const int n = (int)local_.size();
    if (n < 3) return;

    std::vector<Vector2D> tmp = local_;
    for (int i = 0; i < n; ++i)
    {
        int prev = (i - 1 + n) % n;
        int next = (i + 1) % n;

        local_[i].x = tmp[i].x * (1.0f - w) + (tmp[prev].x + tmp[next].x) * 0.5f * w;
        local_[i].y = tmp[i].y * (1.0f - w) + (tmp[prev].y + tmp[next].y) * 0.5f * w;
    }
}

void Enemy::Warp()
{
    const float W = (float)WIN_WIDTH;
    const float H = (float)WIN_HEIGHT;

    if (pos_.x < -radius_)            pos_.x += (W + radius_ * 2.0f);
    else if (pos_.x > W + radius_)    pos_.x -= (W + radius_ * 2.0f);

    if (pos_.y < -radius_)            pos_.y += (H + radius_ * 2.0f);
    else if (pos_.y > H + radius_)    pos_.y -= (H + radius_ * 2.0f);
}

void Enemy::Update()
{
    float dt = ClampDt(GetDeltaTime());

    // 回転
    angle_ += omega_ * dt;
    angle_ = WrapAngle(angle_);

    // 移動（慣性）
    pos_.x += vel_.x * dt;
    pos_.y += vel_.y * dt;

    // ワープ
    Warp();
}

void Enemy::Draw()
{
    const int n = (int)local_.size();
    if (n < 3) return;

    // ローカル→ワールド変換行列（回転→平行移動）
    Mat2 R = Math2D::Rotation(angle_);
    Mat2 T = Math2D::Translation(pos_);
    Mat2 M = Math2D::Multiply(T, R);

    // 変換して線で輪郭を描く（Asteroids風）
    Vector2D prevW = Math2D::TransformPoint(local_[n - 1], M);
    Vector2D prevS = Math2D::World2Screen(prevW);

    for (int i = 0; i < n; ++i)
    {
        Vector2D curW = Math2D::TransformPoint(local_[i], M);
        Vector2D curS = Math2D::World2Screen(curW);

        DrawLineAA(prevS.x, prevS.y, curS.x, curS.y, Color_, 2.0f);

        prevS = curS;
    }
}
