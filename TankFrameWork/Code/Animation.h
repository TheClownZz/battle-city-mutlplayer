#pragma once
#include "Sprite.h"

class Animation : public Sprite
{
protected:
	//tạo sprite
	Sprite* sprite;
	//Biến đếm thời gian
	float TimeCurrent;
	int delay;
	//Frame bắt đầu, kết thúc, đang dừng
	int start, end, AnimationCount, IndexPause;
	//
	bool Pause;

public:
	Animation(Sprite* _sprite);
	Animation();
	~Animation();

	//Set thông số để vẽ 
	void SetFrame(D3DXVECTOR2 Position, bool Flip, int Delay, int Start, int End);

	//Thông tin STT frame hiện tại
	int GetIndex();
	void SetIndex(int index);

	//Lấy Cờ đảo
	bool GetFlipFlag();

	//Lấy thời gian lập frame đó
	int GetDelay();

	//Frame bắt đầu
	int GetStart();
	//Frame kết thúc
	int GetEnd();
	//Kiểm tra vong lặp
	int LoopAnimation();
	//Lệnh dừng tại Frame nào đó
	void SetPause(bool pause);
	void SetPause(bool, int);

	//Update Animation
	void Update(float gameTime);

	//Vẽ ảnh trong world
	void Render(Viewport* viewport);
	//Vẽ ảnh trong Viewport
	void Render();
};
