#include "../includes.h"

Game g_game{ };

bool Game::init( )
{
    IsInGame = reinterpret_cast< IsInGame_t >( offsets::IsInGame );
    DLC_Init = reinterpret_cast< DLC_Init_t >( offsets::DLC_Init );

    cg_DrawFPS_o = nullptr;
    BIsDlcInstalled_o = nullptr;

    m_dlc_vtable_slot = nullptr;
    m_dlc_init_done = false;

	return true;
}

void Game::shutdown( )
{
    restore_dlc( );
}

bool __fastcall Game::hk_BIsDlcInstalled( void* thisPtr, void* edx, uint32_t appID )
{
    return true;
}

void __cdecl Game::hk_cg_DrawFPS( int a1 )
{
    if ( !g_running )
    {
        g_game.cg_DrawFPS_o( a1 );
        return;
    }

    if ( !g_game.m_dlc_init_done )
    {
        g_game.unlock_dlc( );
        g_game.DLC_Init( );
        g_game.m_dlc_init_done = true;
    }

    g_game.cg_DrawFPS_o( a1 );
}

void Game::unlock_dlc( )
{
    uintptr_t steam_apps_fn = *(uintptr_t*)offsets::SteamApps;
    void* iSteamApps = ( ( void* ( __cdecl* )( ) )steam_apps_fn )( );
    if ( !iSteamApps ) return;

    uintptr_t vtable = *(uintptr_t*)iSteamApps;
    m_dlc_vtable_slot = (uintptr_t*)( vtable + 0x1C );

    DWORD old_protect;
    VirtualProtect( m_dlc_vtable_slot, 4, PAGE_EXECUTE_READWRITE, &old_protect );
    BIsDlcInstalled_o = (BIsDlcInstalled_t)*m_dlc_vtable_slot;
    *m_dlc_vtable_slot = (uintptr_t)hk_BIsDlcInstalled;
    VirtualProtect( m_dlc_vtable_slot, 4, old_protect, &old_protect );
}

void Game::restore_dlc( )
{
    if ( m_dlc_vtable_slot && BIsDlcInstalled_o )
    {
        DWORD old_protect;
        VirtualProtect( m_dlc_vtable_slot, 4, PAGE_EXECUTE_READWRITE, &old_protect );
        *m_dlc_vtable_slot = (uintptr_t)BIsDlcInstalled_o;
        VirtualProtect( m_dlc_vtable_slot, 4, old_protect, &old_protect );
    }
}