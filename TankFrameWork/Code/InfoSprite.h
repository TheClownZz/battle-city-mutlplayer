#pragma once
#define TIXML_USE_TICPP
#include <d3dx9.h>
#include "tinyxml.h"
#pragma warning(disable: 4244)
class InfoSprite
{
protected:
	//Chiệu rộng và dài Texture và Frame
	float Width, Height, frameW, frameH;
	//Số lượng frame và số frame trên mỗi dòng
	int FrameCount, FramePerRow;
	struct Infoframe
	{
		float x;//Vị trí góc trái trên X
		float y;//Vị trí góc trái trên Y
		float w;//Chiều rộng frame
		float h;//Chiều dài frame
		float pX;//Orgin.X là vị trí đặc ảnh
		float pY;//Orgin.Y
		float sX;//Scale ảnh
		float sY;
	};
	Infoframe InfoFrame[500];
public:
	InfoSprite(const char* path);
	InfoSprite(float frameW, float frameH, int frameCount, int framePerRow);
	InfoSprite() {}
	~InfoSprite();

	float GetWidth();
	float GetHeight();
	float GetFrameWidth();
	float GetFrameHeight();

	void ReadXML(const char* path);
	RECT GetRect(int Index);
	D3DXVECTOR2 GetOrgin(int Index);
};

