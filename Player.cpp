#include "Player.h"
#include "Math2D.h"
#include <DxLib.h>
#include <cmath>
#include "globals.h"
#include "Input.h"

namespace {

	// 加速度（world座標/秒^2）
	const float ACC = 400.0f;

	// 抵抗（0～1、1に近いほど減速しない）
	// 宇宙っぽくするなら 1.0f に近く、ゲーム的に止めやすくするなら 0.98～0.995
	const float DAMP = 0.995f;

	// 速度上限（world座標/秒）
	const float MAX_SPEED = 500.0f;

}


//引数なしコンストラクタ
//適当な値を入れるだけ、して初期化してないメンバ変数をなくす
//=値に意味はない。
Player::Player()
	:Base(Vector2D(0,0), Vector2D(0, 0), GetColor(0,0,0)),
	dir_({ 0,-1 }), radius_(1.0f), omega_{1.0f}, angle_(0.0f)
{
	vertex_[0] = { 0,0 };
	vertex_[1] = { 0,1 };
	vertex_[2] = { 0,2 };
}

Player::Player(const Vector2D& pos, const Vector2D& vel, unsigned int color,
	           const Vector2D& dir, float r, float omega)
	:Base(pos,vel,color), dir_(dir), radius_(r), omega_(omega)
{
	vertex_[0] = { 0, 0 }; //０で初期化
	vertex_[1] = { 0, 0 }; //０で初期化
	vertex_[2] = { 0, 0 }; //０で初期化
	angle_ = 0.0f; //０で初期化
}

Player::~Player()
{
}

void Player::Update()
{
	const float dt = GetDeltaTime();
	//原点にある半径1の内接する３角形を考える
	Vector2D p[3];
	p[0] = { 0.0f, 1.0f };
	p[1] = { cos(Math2D::ToRadian( - 60.0f)), sin(Math2D::ToRadian(-60.0f))};
	p[2] = { cos(Math2D::ToRadian(240.0f)), sin(Math2D::ToRadian(240.0f)) };
	//半径を１→radius_に拡大
	for (int i = 0;i < 3;i++)
	{
		p[i] = Math2D::Mul(p[i], radius_);
	}
	//p[0].x = radius_ * p[0].x; p[0].y = radius_ * p[0].y;
	//p[1].x = radius_ * p[1].x; p[1].y = radius_ * p[1].y;
	//p[2].x = radius_ * p[2].x; p[2].y = radius_ * p[2].y;
	
	//3角形の3頂点をdir_とradius_とpos_から求めよう
	vertex_[0] = { pos_.x + p[0].x, pos_.y + p[0].y };
	vertex_[1] = { pos_.x + p[1].x, pos_.y + p[1].y };
	vertex_[2] = { pos_.x + p[2].x, pos_.y + p[2].y };

	if (Input::IsKeepKeyDown(KEY_INPUT_LEFT))
	{
		angle_ = angle_ + omega_ * GetDeltaTime();
	}
	if (Input::IsKeepKeyDown(KEY_INPUT_RIGHT))
	{
		angle_ = angle_ - omega_ * GetDeltaTime();
	}
	angle_ = fmodf(angle_, Math2D::PI2);
	if (angle_ < 0.0f) angle_ += Math2D::PI2;

	//angle_から回転角度を求めて、3頂点を回転させる
	//angle_はもともとラジアンなので、そのまま使う
	for (int i = 0; i < 3; i++)
	{
		//回転行列を使って回転させる
		Mat2 rMat = Math2D::Rotation(angle_);
		Mat2 tMat = Math2D::Translation({ -pos_.x, -pos_.y }); //原点に移動させる行列
		Mat2 tmMat = Math2D::Translation({ pos_.x, pos_.y }); //原点に移動させる行列
		Mat2 M = Math2D::Multiply(tmMat, Math2D::Multiply(rMat, tMat));
		vertex_[i] = Math2D::TransformPoint(vertex_[i], M);
		//vertex_[i] = Math2D::TransformPoint(vertex_[i], tMat); //原点に移動
		//vertex_[i] = Math2D::TransformPoint(vertex_[i], rMat); //回転
		//tMat = Math2D::Translation({ pos_.x, pos_.y }); //元の位置に戻す行列
		//vertex_[i] = Math2D::TransformPoint(vertex_[i], tMat); //元の位置に戻す
	}
	// angle_ から前方向（数学座標）を作る
	dir_ = Math2D::FromAngle(angle_ + Math2D::PI / 2);

	// 上キーで推進（加速度）
	if (Input::IsKeepKeyDown(KEY_INPUT_SPACE))
	{
		vel_.x += dir_.x * ACC * dt;
		vel_.y += dir_.y * ACC * dt;
	}

	// 抵抗（任意）
	vel_.x *= DAMP;
	vel_.y *= DAMP;

	// 速度制限（任意）
	float sp2 = vel_.x * vel_.x + vel_.y * vel_.y;
	float max2 = MAX_SPEED * MAX_SPEED;
	if (sp2 > max2)
	{
		float inv = MAX_SPEED / sqrtf(sp2);
		vel_.x *= inv;
		vel_.y *= inv;
	}

	// 位置更新（慣性移動）
	pos_.x += vel_.x * dt;
	pos_.y += vel_.y * dt;

	// ----------------------------
// 画面端ワープ（数学座標）
// ----------------------------
	const float W = (float)WIN_WIDTH;
	const float H = (float)WIN_HEIGHT;

	// 半径分だけ外に出たら反対側へ（見た目が自然）
	if (pos_.x < -radius_)      pos_.x += (W + radius_ * 2.0f);
	else if (pos_.x > W + radius_) pos_.x -= (W + radius_ * 2.0f);

	if (pos_.y < -radius_)      pos_.y += (H + radius_ * 2.0f);
	else if (pos_.y > H + radius_) pos_.y -= (H + radius_ * 2.0f);
}

void Player::Draw()
{

	Vector2D scrPos[3];
	scrPos[0] = Math2D::World2Screen(vertex_[0]);
	scrPos[1] = Math2D::World2Screen(vertex_[1]);
	scrPos[2] = Math2D::World2Screen(vertex_[2]);

	DrawTriangleAA(
		scrPos[0].x, scrPos[0].y,
		scrPos[1].x, scrPos[1].y,
		scrPos[2].x, scrPos[2].y,
		GetColor(255, 0, 0), FALSE, 2.0
	);

	DrawFormatString(50, 50, GetColor(255, 255, 255), "RotAngle:%lf", angle_);
	DrawFormatString(50, 80, GetColor(255, 255, 255), "Velocity:(%lf, %lf)", vel_.x, vel_.y);

}
