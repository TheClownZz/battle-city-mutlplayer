#pragma once
#include <d3dx9.h>
#include "Graphic.h"
#include "Keyboard.h"
#include "Viewport.h"
#include "InfoSprite.h"

class Sprite
{
protected:
	Graphic* SpriteGraphic;//Tạo graphic
	LPDIRECT3DTEXTURE9 Texture;//Tạo texture hiển thị ảnh
	InfoSprite* InfoTexture; //biến lưu thông tin ảnh

	float Width;//Sprite width
	float Height;
	float WidthFrame;//width mỗi khung ảnh tương đối
	float HeightFrame;

	int Index = 0;
	int count = 0;	//bộ đếm
	bool FlipFlag;  //Cờ lật ảnh

	RECT rect;// Kích thước hình cần vẽ
	D3DXVECTOR2 position; //vị trí vẽ trong world
	D3DXVECTOR2 center; //vị trí vẽ trong rect
	D3DXVECTOR2 scale; // Vector tùy chỉnh kích thước
	D3DXVECTOR2 transform; //dịch chuyển ảnh
	float angle;	//góc quay (đơn vị độ)

public:
	Sprite();
	Sprite(Graphic* graphic, const char* pathpng, const char* pathxml);
	Sprite(Graphic* graphic, const char* pathpng, float frameW, float frameH, int frameCount, int framePerRow);
	~Sprite();

	//Đổi hình ảnh khác của đối tượng
	virtual void SetTexture(const char* pathpng);
	//Đổi thông tin ảnh
	virtual void SetInfoSprite(const char* pathxml);
	virtual InfoSprite* GetInfoSprite();

	//Lấy thông số kích thước ảnh
	virtual float GetWidthFrame();
	virtual float GetHeightFrame();

	//Chỉnh thông số kích thước vẽ
	virtual RECT GetRect();
	virtual void SetRect(RECT Rect);
	virtual void SetRect(int index);

	//Chỉnh thông số vị trí trong world
	virtual D3DXVECTOR2 GetPosition();
	virtual void SetPosition(D3DXVECTOR2 Position);

	//Chỉnh thông số vị trí vẽ
	virtual D3DXVECTOR2 GetCenter();
	virtual void SetCenter(D3DXVECTOR2 Center);

	//Chỉnh thông số Zoom
	virtual D3DXVECTOR2 GetScale();
	virtual void SetScale(D3DXVECTOR2 Scale);

	//Chỉnh thông số dịch chuyển
	virtual D3DXVECTOR2 GetTransform();
	virtual void SetTransform(float x, float y);

	//Chỉnh thông số góc
	virtual float GetAngle();
	virtual void SetAngle(float Angle);

	//Chỉnh thông số cờ ảnh
	virtual void SetFlipFlag(bool flag);
	//Xoay ảnh theo trục Y
	virtual void Flip(bool flag);

	//Update Sprite
	virtual void Update(float gameTime);

	//Vẽ ảnh trong world
	virtual void Render(Viewport* viewport);
	virtual void Render(Viewport* viewport, RECT Rect, D3DXVECTOR2 Center, D3DXVECTOR2 Position, D3DXVECTOR2 Scale, D3DXVECTOR2 Transform, float Angle);
	//Vẽ ảnh trong Viewport
	virtual void Render();
	virtual void Render(RECT Rect, D3DXVECTOR2 Center, D3DXVECTOR2 Position, D3DXVECTOR2 Scale, D3DXVECTOR2 Transform, float Angle);
};