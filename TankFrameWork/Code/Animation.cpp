#include "Animation.h"


Animation::Animation(Sprite* _sprite)
{
	this->sprite = _sprite;
	//Gán InfoSprite
	this->InfoTexture = _sprite->GetInfoSprite();

	//Thiết lập thông số ban đầu
	this->position = D3DXVECTOR2(0, 0);
	this->center = D3DXVECTOR2(0, 0);
	this->scale = D3DXVECTOR2(1, 1);
	this->transform = D3DXVECTOR2(0, 0);
	this->angle = 0;
	this->FlipFlag = false;

	this->delay = 0;
	this->start = 0;
	this->end = 0;
	this->AnimationCount = 0;
	this->Pause = false;
	this->TimeCurrent = 0;
}

Animation::Animation()
{
}

Animation::~Animation()
{
}

//Thông tin STT frame hiện tại
int Animation::GetIndex()
{
	return this->Index;
}
void Animation::SetIndex(int index)
{
	this->Index = index;
}

//Lấy Cờ đảo
bool Animation::GetFlipFlag()
{
	return this->FlipFlag;
}

//Lấy thời gian lập frame đó
int Animation::GetDelay()
{
	return this->delay;
}

//Frame bắt đầu
int Animation::GetStart()
{
	return this->start;
}

//Frame kết thúc
int Animation::GetEnd()
{
	return this->end;
}

int Animation::LoopAnimation()
{
	return this->AnimationCount;
}

//Lệnh dừng tại Frame nào đó
void Animation::SetPause(bool pause)
{
	this->Pause = pause;
}
void Animation::SetPause(bool pause, int index)
{
	this->IndexPause = index;
	this->Pause = pause;
}

//Chỉnh thống só để chuyển qua vẽ
void Animation::SetFrame(D3DXVECTOR2 Position, bool Flip, int Delay, int Start, int End)
{
	this->position = Position;
	//start end khác thì set lại
	if (this->start != Start || this->end != End)
	{
		this->FlipFlag = Flip;
		this->delay = Delay;
		this->start = Start;
		this->end = End;

		this->AnimationCount = 0;
		this->Index = Start;
		this->TimeCurrent = 0;
	}
}

//Update Animation
void Animation::Update(float gameTime)
{
	//Chuyển frame tiếp theo nếu thời gian lập lớn hơn thời gian delay
	if (this->TimeCurrent > this->delay )
	{
		this->TimeCurrent = 0;
		this->Index++;
	}
	else this->TimeCurrent += gameTime;

	//kết thúc 1 lần Animation
	if (Index > end)
	{
		this->AnimationCount ++;
		this->Index = this->start;
	}

	//Kiểm tra có Pause Frame không
	if (Pause)
	{
		this->Index = this->IndexPause;
	}

	//Kiểm tra Flip
	Flip(this->FlipFlag);

	//Set rect mới
	SetRect(Index);

	//Lấy center ở giữa
	this->center.x = (rect.right - rect.left)/2;
	this->center.y = (rect.bottom - rect.top) / 2;

	//Kiểm tra Orgin flip chỉnh transform 0.5 là không dịch chuyển
	if (InfoTexture->GetOrgin(Index).x != 0.5)
		//Sprite đang lật thì chuyển hướng khác
		if (FlipFlag)
		{
			this->transform.x = InfoTexture->GetOrgin(Index).x;
		}
		else
		{
			this->transform.x = -InfoTexture->GetOrgin(Index).x;
		}
	else this->transform.x = 0;

	if (InfoTexture->GetOrgin(Index).y != 0.5)
	{
		this->transform.y = -center.y;
	}
	else this->transform.y = 0;
}

//Vẽ Sprite lên màn hình theo tọa độ world
void Animation::Render(Viewport* viewport)
{
	this->sprite->Render(viewport, this->rect, this->center, this->position, this->scale, this->transform, this->angle);
}

//Vẽ Sprite lên màn hình View
void Animation::Render()
{
	this->sprite->Render(this->rect, this->center, this->position, this->scale, this->transform, this->angle);
}