#include "Viewport.h"



Viewport::Viewport()
{
}

//x, y là vị trí View trong world (gốc trái-trên)
Viewport::Viewport(float x, float y)
{
	//kiểu nguyên để không lệch pixel khi di chuyển viewport
	this->positionWorld.x = (int)x;
	this->positionWorld.y = (int)y;

	//Mặc định là kích thước game
	this->Width = GameWidth;
	this->Height = GameHeight;
}

//View kích thước tùy chỉnh
Viewport::Viewport(float x, float y, float width, float height)
{
	//kiểu nguyên để không lệch pixel khi di chuyển viewport
	this->positionWorld.x = (int)x;
	this->positionWorld.y = (int)y;
	this->Width = width;//kích thước Viewport
	this->Height = height;
}

Viewport::~Viewport()
{
}

//Get set vị trí Viewport
D3DXVECTOR2 Viewport::GetPosition()
{
	return this->positionWorld;
}
void Viewport::SetPosition(D3DXVECTOR2 Position)
{
	//kiểu nguyên để không lệch pixel khi di chuyển viewport
	this->positionWorld.x = (int)Position.x;
	this->positionWorld.y = (int)Position.y;
}
//Chỉnh vị trí View x
void Viewport::SetPositionX(float x)
{
	this->positionWorld.x = (int)x;
}
//Chỉnh vị trí View y
void Viewport::SetPositionY(float y)
{
	this->positionWorld.y = (int)y;
}

//Lấy chiều dài rộng của Viewport
float Viewport::GetWidth()
{
	return this->Width;
}
float Viewport::GetHeight()
{
	return this->Height;
}

//Lấy vị trí của ViewPort từ vị trí object cần vẽ theo hệ trục left-top
D3DXVECTOR2 Viewport::GetPositionViewport_Left_Top(D3DXVECTOR2 Position)
{
	return Position - this->positionWorld;
}

//Lấy vị trí của ViewPort từ vị trí object cần vẽ theo hệ Đề-các
D3DXVECTOR2 Viewport::GetPositionViewport(D3DXVECTOR2 Position)
{
	//Chuyển tọa độ Object về Vector3 kiểu int cho không vỡ hình thiếu hình
	D3DXVECTOR3* positionV3 = &D3DXVECTOR3((int)Position.x, (int)Position.y, 0);

	//Tạo matrix trống
	D3DXMATRIX mt;
	//Tạo Vector4 trống
	D3DXVECTOR4 posObjectView;

	//Lấy thông số Matrix hiện thời chưa đổi
	D3DXMatrixIdentity(&mt);

	//Đổi thông số ma trận chuẩn bị nhân lấy tọa độ mới
	mt._22 = -1.0f;
	mt._41 = -this->positionWorld.x;
	mt._42 = this->positionWorld.y;

	//Chuyển tọa độ Object world về View
	D3DXVec3Transform(&posObjectView, positionV3, &mt);

	//Trả về Vector2
	return D3DXVECTOR2(posObjectView.x, posObjectView.y);
}

//Kiểm tra 1 rect có ở trong màn hình không
bool Viewport::isContains(RECT rect)
{
	if (rect.right < this->positionWorld.x)
		return false;
	if (rect.bottom > this->positionWorld.y)
		return false;
	if (rect.left > this->positionWorld.x + Width)
		return false;
	if (rect.top < this->positionWorld.y - Height)
		return false;

	return true;
}

//Kích thước RECT màn hình tính trong World
RECT Viewport::GetBoundViewport()
{
	RECT rect;

	rect.left = (int)this->positionWorld.x;
	rect.top = (int)this->positionWorld.y;
	rect.bottom = rect.top - (int)Height;
	rect.right = rect.left + (int)Width;

	return rect;
}

//Update View theo 1 điểm trong world
void Viewport::Update(float gameTime, Keyboard* key, D3DXVECTOR2 &posobject)
{
	//Di chuyển View
		//trục X
		if (posobject.x - this->positionWorld.x <= this->Width / 3)
			this->positionWorld.x = posobject.x - this->Width / 3;
		else if (posobject.x - this->positionWorld.x >= 2 * this->Width / 3)
			this->positionWorld.x = posobject.x - 2 * this->Width / 3;

		//trục Y
		if (this->positionWorld.y - posobject.y <= this->Height / 3)
			this->positionWorld.y = posobject.y + this->Height / 3;
		else if (this->positionWorld.y - posobject.y >= 2 * this->Height / 3)
			this->positionWorld.y = posobject.y + 2 * this->Height / 3;
}