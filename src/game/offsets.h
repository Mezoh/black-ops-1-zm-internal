#pragma once

#define ADDRESS constexpr std::uintptr_t

namespace offsets
{
	// fn calls
	ADDRESS IsInGame			= 0x526450;
	ADDRESS DLC_Init			= 0x4E8350;

	// hooks
	ADDRESS cg_DrawFPS		= 0x5BF0F0;

	// vtable addr's
	ADDRESS SteamApps		= 0x9A3544;


}