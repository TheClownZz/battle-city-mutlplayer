#pragma once
#include <stdint.h>

namespace PNet
{
	enum PacketType : uint8_t
	{
		PT_Invalid,
		PT_Time,
		PT_ConnectUdp,
		PT_SetID,
		PT_Ready,
		PT_Start,
		PT_Input,
		PT_TankProperties,
		PT_Reset,
		PT_UpdateMap,
		PT_Shoot,
		PT_Item,
	};
}