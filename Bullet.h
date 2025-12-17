#pragma once
#include "Base.h"

class Bullet :
    public Base
{
public:
    Bullet(const Vector2D& pos, const Vector2D& vel, unsigned int color,
        float radius, float lifeSec);

    void Update() override;
    void Draw() override;

    bool IsDead() const { return life_ <= 0.0f; }
    float Radius() const { return radius_; }

private:
    float radius_;
    float life_; // Žc‚èŽõ–½i•bj
};

