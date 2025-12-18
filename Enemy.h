#pragma once
#include "Base.h"
#include <vector>

class Enemy : public Base
{
public:
    // いびつな岩（Asteroids風）を生成
    // segments : 円周分割数（12～24推奨）
    // jitter   : 半径の乱れ（0.25～0.45推奨）
    Enemy(const Vector2D& pos,
        const Vector2D& vel,
        unsigned int color,
        float baseRadius,
        int segments,
        float jitter,
        float omegaRadPerSec);

    void Update() override;
    void Draw() override;

    // 便利（当たり判定用に後で使う）
    float GetRadius() const { return radius_; }

    // 今は常に生存（弾で消すならフラグ追加）
    bool IsDead() const { return dead_; }
    void Kill() { dead_ = true; }

private:
    void GenerateShape(int segments, float baseRadius, float jitter);
    void SmoothShape(float w);
    void Warp();

private:
    std::vector<Vector2D> local_; // 原点中心のローカル形状（数学座標 +y上）
    float radius_;                // 代表半径（当たり判定用）
    float angle_;                 // 回転角（rad）
    float omega_;                 // 角速度（rad/sec）
    bool  dead_;
};
