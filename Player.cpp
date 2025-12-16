#include "Player.h"
#include "DxLib.h"
#include "Input.h"
#include <cmath>
#include <array>
#include "globals.h"
#include <DirectXMath.h>

namespace
{
	// 安全な正規化（dir_を単位ベクトル運用にする）
	Vector2D NormalizeSafe(const Vector2D& v)
	{
		const float len2 = v.x * v.x + v.y * v.y;
		if (len2 <= 0.000001f)
		{
			return Vector2D(1.0f, 0.0f);
		}
		const float invLen = 1.0f / std::sqrt(len2);
		return Vector2D(v.x * invLen, v.y * invLen);
	}

	// ベクトルを角度radだけ回す（行列×ベクトル）
	Vector2D RotateVec(const Vector2D& v, float rad)
	{
		const Matrix2D R = Matrix2D::Rotation(rad);
		// Rotation は tx,ty=0 なので TransformPoint でOK
		return TransformPoint(v, R);
	}
}

Player::Player()
	: Base(Vector2D(320.0f, 240.0f), Vector2D(0.0f, 0.0f), 0xffffffff)
	, dir_(1.0f, 0.0f)
	, radius_(120.0f)
	, tri_{ Vector2D(), Vector2D(), Vector2D() }
	, omega_(0.0f)
{
	SetTrianglePoints();
}

Player::Player(const Vector2D& pos, float radius, unsigned int color)
	: Base(pos, Vector2D(0.0f, 0.0f), color)
	, dir_(1.0f, 0.0f)
	, radius_(radius)
	, tri_{ Vector2D(), Vector2D(), Vector2D() }
	, omega_(0.0f)
{
	SetTrianglePoints();
}

void Player::Update()
{

	constexpr float TWO_PI = DirectX::XM_2PI;
	// 入力からomega_を決める
	omega_ = 0.0f;

	if (Input::IsKeepKeyDown(KEY_INPUT_LEFT))
	{
		omega_ = -1.0f;
	}
	if (Input::IsKeepKeyDown(KEY_INPUT_RIGHT))
	{
		omega_ = +1.0;
	}

	// rps → rad/frame にして、行列×ベクトルで向きを回す
	if (omega_ != 0.0f)
	{
		const float dtheta = omega_ * TWO_PI * GetDeltaTime();
		dir_ = RotateVec(dir_, dtheta);   // RotateVec は「Matrix2D::Rotation(dtheta)」×dir_ のやつ
	}

	dir_ = NormalizeSafe(dir_);
	SetTrianglePoints();
}



void Player::SetTrianglePoints()
{
	// dir_ から角度へ（atan2）
	const float theta = std::atan2(dir_.y, dir_.x);

	const float R = radius_;

	// ローカル三角形（中心=原点、半径Rの円周上に3点）
	const std::array<Vector2D, 3> local =
	{
		Vector2D(0.0f, -R),               // 先端
		Vector2D(R * 0.7f,  R * 0.7f),    // 右下
		Vector2D(-R * 0.7f,  R * 0.7f)     // 左下
	};

	// 回転→平行移動（アフィン変換）
	const Matrix2D Rot = Matrix2D::Rotation(theta);
	const Matrix2D Trs = Matrix2D::Translation(pos_);

	for (int i = 0; i < 3; ++i)
	{
		const Vector2D v = TransformPoint(local[i], Rot);
		tri_[i] = TransformPoint(v, Trs);
	}
}

void Player::Draw()
{
	DrawTriangleAA(
		tri_[0].x, tri_[0].y,
		tri_[1].x , tri_[1].y,
		tri_[2].x, tri_[2].y ,
		Color_,
		TRUE
	);
}
