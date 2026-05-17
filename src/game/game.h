#pragma once
class Game {
public:
	using IsInGame_t		= BOOL( * )( );
	using cg_DrawFPS_t		= void( __cdecl* )( int );
	using BIsDlcInstalled_t = bool( __thiscall* )( void*, uint32_t );
	using DLC_Init_t		= void( __cdecl* )( );

	// game fns
	IsInGame_t			IsInGame;
	DLC_Init_t			DLC_Init;

	// hook orig's
	BIsDlcInstalled_t	BIsDlcInstalled_o;
	cg_DrawFPS_t		cg_DrawFPS_o;

	// class methods
	uintptr_t*			m_dlc_vtable_slot;
	bool				m_dlc_init_done;

	bool init( );
	void shutdown( );

	void unlock_dlc( );
	void restore_dlc( );

	static void __cdecl		hk_cg_DrawFPS( int a1 );
	static bool __fastcall	hk_BIsDlcInstalled( void* thisPtr, void* edx, uint32_t appID );

}; extern Game g_game;

