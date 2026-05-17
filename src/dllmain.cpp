#include "includes.h"
#include <cstring>

HMODULE h_mod;
volatile bool g_running = true;

static DWORD WINAPI main_thread( LPVOID args );

namespace
{
    constexpr BYTE kUnloadStubTemplate[] = {
        0x6A, 0xFF,
        0x68, 0x00, 0x00, 0x00, 0x00,
        0xB8, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xD0,
        0x68, 0x00, 0x00, 0x00, 0x00,
        0xB8, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xD0,
        0x68, 0x00, 0x80, 0x00, 0x00,
        0x6A, 0x00,
        0x68, 0x00, 0x00, 0x00, 0x00,
        0xB8, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xD0,
        0x6A, 0x00,
        0xB8, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xD0,
        0xC3,
    };

    bool is_loader_registered( HMODULE image_base )
    {
        HMODULE listed_module = nullptr;
        if ( !GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                  reinterpret_cast<LPCSTR>( &main_thread ), &listed_module ) )
            return false;

        return listed_module == image_base;
    }

    void start_self_unload( HMODULE image_base )
    {
        HMODULE k32 = GetModuleHandleA( "kernel32.dll" );
        if ( !k32 )
            return;

        FARPROC p_wait = GetProcAddress( k32, "WaitForSingleObject" );
        FARPROC p_close = GetProcAddress( k32, "CloseHandle" );
        FARPROC p_free = GetProcAddress( k32, "VirtualFree" );
        FARPROC p_exit = GetProcAddress( k32, "ExitThread" );
        if ( !p_wait || !p_close || !p_free || !p_exit )
            return;

        HANDLE dup_thread = nullptr;
        if ( !DuplicateHandle( GetCurrentProcess( ), GetCurrentThread( ),
                               GetCurrentProcess( ), &dup_thread, SYNCHRONIZE, FALSE, 0 ) )
            return;

        void* page = VirtualAlloc( nullptr, sizeof( kUnloadStubTemplate ),
                                   MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE );
        if ( !page )
        {
            CloseHandle( dup_thread );
            return;
        }

        BYTE stub[ sizeof( kUnloadStubTemplate ) ];
        std::memcpy( stub, kUnloadStubTemplate, sizeof( stub ) );

        const DWORD handle_val = reinterpret_cast<DWORD>( dup_thread );
        const DWORD wait_val = reinterpret_cast<DWORD>( p_wait );
        const DWORD close_val = reinterpret_cast<DWORD>( p_close );
        const DWORD image_val = reinterpret_cast<DWORD>( image_base );
        const DWORD free_val = reinterpret_cast<DWORD>( p_free );
        const DWORD exit_val = reinterpret_cast<DWORD>( p_exit );

        std::memcpy( &stub[ 3 ], &handle_val, sizeof( DWORD ) );
        std::memcpy( &stub[ 8 ], &wait_val, sizeof( DWORD ) );
        std::memcpy( &stub[ 15 ], &handle_val, sizeof( DWORD ) );
        std::memcpy( &stub[ 20 ], &close_val, sizeof( DWORD ) );
        std::memcpy( &stub[ 34 ], &image_val, sizeof( DWORD ) );
        std::memcpy( &stub[ 39 ], &free_val, sizeof( DWORD ) );
        std::memcpy( &stub[ 48 ], &exit_val, sizeof( DWORD ) );

        std::memcpy( page, stub, sizeof( stub ) );

        HANDLE freer = CreateThread( nullptr, 0,
                                     reinterpret_cast<LPTHREAD_START_ROUTINE>( page ),
                                     nullptr, 0, nullptr );
        if ( !freer )
        {
            VirtualFree( page, 0, MEM_RELEASE );
            CloseHandle( dup_thread );
            return;
        }

        CloseHandle( freer );
    }
}

static DWORD WINAPI main_thread( LPVOID args )
{
    h_mod = reinterpret_cast<HMODULE>( args );
    const bool loader_registered = is_loader_registered( h_mod );

    AllocConsole( );
    FILE* f;
    freopen_s( &f, "CONOUT$", "w", stdout );

    MH_Initialize( );

    g_game.init( );
    g_render.init( );

    MH_CreateHook( (void*)offsets::cg_DrawFPS, Game::hk_cg_DrawFPS, reinterpret_cast<void**>( &g_game.cg_DrawFPS_o ) );
    MH_EnableHook( (void*)offsets::cg_DrawFPS );

    std::printf( "[+] initialized\n" );

    while ( g_running )
    {
        if ( GetAsyncKeyState( VK_END ) & 1 )
            g_running = false;

        Sleep( 50 );
    }

    std::printf( "[-] shutting down\n" );

    g_render.shutdown( );
    g_game.shutdown( );

    MH_DisableHook( MH_ALL_HOOKS );
    MH_Uninitialize( );

    fclose( f );
    FreeConsole( );
    if ( loader_registered )
        FreeLibraryAndExitThread( h_mod, 0 );

    start_self_unload( h_mod );
    ExitThread( 0 );
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
    if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
    {
        h_mod = hModule;
        DisableThreadLibraryCalls( hModule );
        CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)main_thread, hModule, 0, nullptr );
    }
    return TRUE;
}
