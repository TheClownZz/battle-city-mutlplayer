#pragma once
#include <stdint.h>

namespace PNet
{
	enum Statetank :uint8_t
	{
		Appearing,//Xuất hiện
		Standing,//Đứng yên
		RunningUp, //Di chuyển lên
		RunningLeft, //Di chuyển trái 
		RunningDown, //Di chuyển xuống 
		RunningRight, //Di chuyển phải
		Bleeing,//Bị bắn trúng
		Dying,	//Đang chết
		Dead	//Chết
	};
	struct InputState
	{
		bool isShoot;
		bool isLeft;
		bool isRight;
		bool isUp;
		bool isDown;
	};
	struct TankBulletProperties {
		float x;
		float y;
		uint8_t direct;
		uint8_t state;
	};

	struct TankProperties
	{
		float x;
		float y;
		bool isBlock;
		int direct;
		Statetank state;
		TankBulletProperties bullet;
	};
	struct BulletProperties {
		long timeSend;
		float x;
		float y;
		uint8_t direct;
	};
	struct TileInfo
	{
		int x;
		int y;
		int value;
		long timeSave;
	};

	#define UNDEFINE_ID 99
	#define SERVER_ID 10
	#define MY_PORT 2019
	#define MAX_PlAYER 4
	#define NUM_ENEMY 4
	const float TIME_UPDATE_MAP = 3;
	const float TIME_UPDATE_ITEM = 20;
	const float TIME_LIVE_ITEM = 10;
	const int SERVER_BUFFER_SIZE = 38;
	const int CLIENT_BUFFER_SIZE = 8;
	const int g_MaxPacketSize = 8192;
	const int SEND_RATE = 30;
	const int Min_Time_Ping = 30;
	const int Max_Ping = 99;
	inline long My_ABS(long x)
	{
		return (x >= 0) ? x : -x;
	}

}
