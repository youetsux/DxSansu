#pragma once

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

private:
    Player* player_;
};

