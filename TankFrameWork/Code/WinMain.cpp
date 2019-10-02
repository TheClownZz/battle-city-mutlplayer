#include <Windows.h>
#include "Game.h"
#include "GameDefine.h"
using namespace Define;
#pragma warning(disable: 4996)
int WINAPI WinMain(HINSTANCE Hins, HINSTANCE HIns, LPTSTR a, int c)
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	//Khai báo Object game
	Game game(Hins, Define::WinWidth, Define::WinHeight, "Tank");
	if (!game.InitWD())
	{
		MessageBox(NULL, "Can't load Init Game", "Error", MB_OK);
		return 0;
	}
	//Load Data 
	game.InitDT();

	//Biến thời gian
	MSG Msg;
	ZeroMemory(&Msg, sizeof(MSG));
	//Tần số count
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	//Thời gian bắt đầu 1 frame
	LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);
	//Thời gian kết thúc
	LARGE_INTEGER endTime;
	//Thời gian chạy 1 frame thường 60fps
	float frame_rate = 1000.0f / Define::FPSGame;
	float gameTime = 0;
	while (Msg.message != WM_QUIT)
	{
		if (PeekMessage(&Msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			QueryPerformanceCounter(&endTime);
			gameTime = (endTime.QuadPart - startTime.QuadPart) * (1000.0f / Frequency.QuadPart);
			if (gameTime >= frame_rate)
			{
				startTime = endTime;
				game.Update(gameTime);
				game.Render();
			}
			else
			{
				Sleep(frame_rate - gameTime);
			}
		}
	}
	return 0;
}