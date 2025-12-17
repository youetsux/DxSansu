#pragma once
#include <vector>

class Bullet;
class Player;
class Stage
{
public:
    Stage();
	~Stage();
	void Initialize();
    void Update();
    void Draw();
	void Release();
private:
    void SpawnBullet();   // ”­Ëˆ—iZ‰Ÿ‰ºj
private:
    Player* player_;
    std::vector<Bullet *> bullets_;
};

