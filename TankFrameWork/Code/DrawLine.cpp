#include "DrawLine.h"



DrawLine::DrawLine(Graphic* graphic)
{
	D3DXCreateLine(graphic->GetDevice(), &LineDraw);
	LineDraw->SetWidth(2);
	
}


DrawLine::~DrawLine()
{
}

void DrawLine::Draw(D3DXVECTOR2 lines[], int count)
{
	LineDraw->Begin();
	LineDraw->Draw(lines, count, D3DCOLOR_XRGB(255, 0, 0));//Màu đỏ
	LineDraw->End();
}

void DrawLine::DrawRect(RECT rect, Viewport *view)
{
	
	D3DXVECTOR2 lines[] = { view->GetPositionViewport(D3DXVECTOR2(rect.left, rect.top)),
		view->GetPositionViewport(D3DXVECTOR2(rect.right, rect.top)),
			view->GetPositionViewport(D3DXVECTOR2(rect.right, rect.bottom)),
				view->GetPositionViewport(D3DXVECTOR2(rect.left, rect.bottom)),
					view->GetPositionViewport(D3DXVECTOR2(rect.left, rect.top))
						};

	Draw(lines, 5);
}