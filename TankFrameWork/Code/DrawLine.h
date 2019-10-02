#pragma once
#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include "Graphic.h"
#include "Viewport.h"

class DrawLine
{
protected:
	
	LPD3DXLINE LineDraw;
public:
	DrawLine(Graphic* graphic);
	~DrawLine();

	void Draw(D3DXVECTOR2 lines[], int count);
	void DrawRect(RECT rect, Viewport *view);
};

