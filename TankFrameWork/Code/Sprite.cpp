#include "Sprite.h"


Sprite::Sprite()
{
}

//Contructor
Sprite::Sprite(Graphic* graphic, const char* pathpng, const char* pathxml)
{
	this->SpriteGraphic = graphic;
	//Graphic->LoadTexture(đường dẫn, Màu không nhận D3DCOLOR_XRGB);
	this->Texture = SpriteGraphic->LoadTexture(pathpng, D3DCOLOR_XRGB(0, 0, 1));
	//Tạo InfoSprite(InfoTexture)
	this->InfoTexture = new InfoSprite(pathxml);

	this->position = D3DXVECTOR2(0, 0);
	this->center = D3DXVECTOR2(0, 0);
	this->scale = D3DXVECTOR2(1, 1);
	this->transform = D3DXVECTOR2(0, 0);
	this->angle = 0;
	this->FlipFlag = false;
}

Sprite::Sprite(Graphic* graphic, const char* pathpng, float frameW, float frameH, int frameCount, int framePerRow)
{
	this->SpriteGraphic = graphic;
	//Graphic->LoadTexture(đường dẫn, Màu không nhận D3DCOLOR_XRGB);
	this->Texture = SpriteGraphic->LoadTexture(pathpng, D3DCOLOR_XRGB(0, 0, 1));
	//Tạo InfoSprite(InfoTexture)
	this->InfoTexture = new InfoSprite(frameW, frameH, frameCount, framePerRow);

	this->position = D3DXVECTOR2(0, 0);
	this->center = D3DXVECTOR2(0, 0);
	this->scale = D3DXVECTOR2(1, 1);
	this->transform = D3DXVECTOR2(0, 0);
	this->angle = 0;
	this->FlipFlag = false;
}

Sprite::~Sprite()
{
}

//Load lại Texture theo đường dẫn khác
void Sprite::SetTexture(const char* pathpng)
{
	//Graphic->LoadTexture(đường dẫn, Màu không nhận D3DCOLOR_XRGB);
	Texture = SpriteGraphic->LoadTexture(pathpng, D3DCOLOR_XRGB(0, 0, 0));
}

//Đổi thông tin ảnh
void Sprite::SetInfoSprite(const char* pathxml)
{
	this->InfoTexture = new InfoSprite(pathxml);
}
InfoSprite* Sprite::GetInfoSprite()
{
	return this->InfoTexture;
}

//Get set các giá trị
//Chiều dài rộng frame;
float Sprite::GetWidthFrame()
{
	return rect.right - rect.left;
}
float Sprite::GetHeightFrame()
{
	return rect.bottom - rect.top;
}

//Rect
RECT Sprite::GetRect()
{
	return this->rect;
}
void Sprite::SetRect(RECT Rect)
{
	this->rect = Rect;
}
void Sprite::SetRect(int index)
{
	this->rect = this->InfoTexture->GetRect(index);
}

//Vị trí trong world
D3DXVECTOR2 Sprite::GetPosition()
{
	return this->position;
}
void Sprite::SetPosition(D3DXVECTOR2 Position)
{
	this->position = Position;
}

//Vị trí vẽ
D3DXVECTOR2 Sprite::GetCenter()
{
	return this->center;
}
void Sprite::SetCenter(D3DXVECTOR2 Center)
{
	this->center = Center;
}

//Phóng ảnh
D3DXVECTOR2 Sprite::GetScale()
{
	return this->scale;
}
void Sprite::SetScale(D3DXVECTOR2 Scale)
{
	this->scale = Scale;
}

//Dịch chuyển
D3DXVECTOR2 Sprite::GetTransform()
{
	return this->transform;
}
void Sprite::SetTransform(float x, float y)
{
	this->transform.x = x;
	this->transform.y = y;
}

//Góc xoay
float Sprite::GetAngle()
{
	return this->angle;
}
void Sprite::SetAngle(float Angle)
{
	this->angle = Angle;
}

//Cờ lật ảnh theo trục y
void Sprite::SetFlipFlag(bool flag)
{
	this->FlipFlag = flag;
}

//Lật hình theo trục y
void Sprite::Flip(bool flag)
{
	if (flag)
	{
		this->scale = D3DXVECTOR2(-1, 1);
	}
	else this->scale = D3DXVECTOR2(1, 1);
}

//Update Sprite
void Sprite::Update(float gameTime)
{
}

//Vẽ Sprite lên màn hình theo tọa độ world
void Sprite::Render(Viewport* viewport)
{
	//SpriteGraphic->DrawTexture(
	//Texture cần vẽ TEXTURE,
	//kích thước khung vẽ RECT, 
	//Vị trí CENTER, 
	//Vị trí vẽ POSITION, 
	//Thông số Zoom SCALE, 
	//Vector transform, 
	//Góc theo độ ANGLE, 
	//Màu hiển thị. D3DCOLOR_XRGB(225, 225, 225) là màu mặc định của texture);
	SpriteGraphic->DrawTexture(Texture, rect, center, viewport->GetPositionViewport(position), scale, transform, angle, D3DCOLOR_XRGB(225, 225, 225));
}

void Sprite::Render(Viewport* viewport, RECT Rect, D3DXVECTOR2 Center, D3DXVECTOR2 Position, D3DXVECTOR2 Scale, D3DXVECTOR2 Transform, float Angle)
{
	SpriteGraphic->DrawTexture(Texture, Rect, Center, viewport->GetPositionViewport(Position), Scale, Transform, Angle, D3DCOLOR_XRGB(225, 225, 225));
}

//Vẽ Sprite lên màn hình View
void Sprite::Render()
{
	SpriteGraphic->DrawTexture(Texture, rect, center, position, scale, transform, angle, D3DCOLOR_XRGB(225, 225, 225));
}

void Sprite::Render(RECT Rect, D3DXVECTOR2 Center, D3DXVECTOR2 Position, D3DXVECTOR2 Scale, D3DXVECTOR2 Transform, float Angle)
{
	SpriteGraphic->DrawTexture(Texture, Rect, Center, Position, Scale, Transform, Angle, D3DCOLOR_XRGB(225, 225, 225));
}