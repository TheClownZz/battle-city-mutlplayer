#pragma once
#include "Graphic.h"
#include <d3dx9.h>

class Collision
{
public:
	Collision();
	~Collision();

	static RECT GetBoard(RECT bound, D3DXVECTOR2 distance);

	//Kiểm tra có chạm 1 điểm và RECT
	static bool isCollision(float x, float y, RECT other);

	//Kiểm tra có chạm 2 Rect
	static bool isCollision(RECT object, RECT other);

	//Đã lồng vào nhau
	static bool isCollision1(RECT object, RECT other);

	static float CollisionAABB(RECT object, RECT other, D3DXVECTOR2 distance, D3DXVECTOR2 &side);

};

