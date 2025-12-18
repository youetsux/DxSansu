#pragma once
#include <vector>

class Bullet;
class Player;
class Enemy;
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
	void SpawnEnemy();    // “G¶¬ˆ—
private:
    Player* player_;
    std::vector<Bullet *> bullets_;
    std::vector<Enemy*> enemies_;
};

