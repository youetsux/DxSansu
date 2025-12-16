#pragma once
#include "Base.h"
#include <array>

class Player :
    public Base
{
public:
	//コンストラクタ
	//OverRide類
	Player();
	Player(const Vector2D& pos, float radius, unsigned int color);

	void Update() override;
	void Draw() override;

public://ゲタセタ
	void SetDir(const Vector2D& dir) { dir_ = dir; }
	Vector2D GetDir() const { return dir_; }

	void SetRadius(float r) { radius_ = r; }
	float GetRadius() const { return radius_; }
	void SetTrianglePoints();
private:
	//独自のメンバ
	//自分の向き（ベクトル）
	//3角形の3点の座標
	Vector2D dir_;   // 向きベクトル（例：右=(1,0)）
	float    radius_; // 内接円の半径（=円の半径、中心は pos_）
	std::array<Vector2D, 3> tri_;
	float omega_; // 回転角速度
};

