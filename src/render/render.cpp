#include "../includes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND, UINT, WPARAM, LPARAM );

Render g_render{ };

static bool is_input_msg( UINT msg ) {
	switch ( msg ) {
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN: case WM_LBUTTONUP: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONUP: case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:  case WM_MOUSEHWHEEL:
	case WM_KEYDOWN:     case WM_KEYUP:
	case WM_SYSKEYDOWN:  case WM_SYSKEYUP:
	case WM_CHAR:        case WM_SETCURSOR:
	case WM_INPUT:
		return true;
	}
	return false;
}

BOOL CALLBACK Render::enum_proc( HWND hwnd, LPARAM lp ) {
	DWORD pid = 0;
	GetWindowThreadProcessId( hwnd, &pid );
	if ( pid != GetCurrentProcessId( ) )
		return TRUE;
	*reinterpret_cast<HWND*>( lp ) = hwnd;
	return FALSE;
}

HWND Render::find_game_window( ) {
	HWND hwnd = nullptr;
	EnumWindows( enum_proc, reinterpret_cast<LPARAM>( &hwnd ) );
	return hwnd;
}

BOOL WINAPI Render::hk_set_cursor_pos( int x, int y ) {
	if ( g_render.m_menu_open )
		return TRUE;
	return g_render.m_orig_set_cursor_pos( x, y );
}

static POINT  s_frozen_cursor{ };
static bool   s_cursor_frozen = false;

BOOL WINAPI Render::hk_get_cursor_pos( LPPOINT p ) {
	if ( g_render.m_menu_open && p ) {
		if ( !s_cursor_frozen ) {
			g_render.m_orig_get_cursor_pos( &s_frozen_cursor );
			s_cursor_frozen = true;
		}
		*p = s_frozen_cursor;
		return TRUE;
	}
	s_cursor_frozen = false;
	return g_render.m_orig_get_cursor_pos( p );
}

LRESULT __stdcall Render::hk_wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam ) {
	ImGui_ImplWin32_WndProcHandler( hwnd, msg, wparam, lparam );

	if ( g_render.m_menu_open && is_input_msg( msg ) )
		return 0;

	return CallWindowProc( g_render.m_orig_wndproc, hwnd, msg, wparam, lparam );
}

HRESULT __stdcall Render::hk_endscene( IDirect3DDevice9* device ) {
	if ( g_render.m_shutting_down )
		return g_render.m_orig_endscene( device );

	if ( !g_render.m_imgui_initialized ) {
		D3DDEVICE_CREATION_PARAMETERS params{ };
		device->GetCreationParameters( &params );
		g_render.m_game_hwnd = params.hFocusWindow;

		ImGui::CreateContext( );
		ImGui::StyleColorsDark( );
		ImGui_ImplWin32_Init( g_render.m_game_hwnd );
		ImGui_ImplDX9_Init( device );

		g_render.m_orig_wndproc = (WNDPROC)SetWindowLongPtr(
			g_render.m_game_hwnd, GWLP_WNDPROC, (LONG_PTR)hk_wndproc );
		g_render.m_imgui_initialized = true;

		std::printf( "[+] render initialized\n" );
	}

	if ( GetAsyncKeyState( VK_INSERT ) & 1 )
		g_render.m_menu_open = !g_render.m_menu_open;

	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	ImVec2 display = ImGui::GetIO( ).DisplaySize;
	g_render.m_screen_w = static_cast<int>( display.x );
	g_render.m_screen_h = static_cast<int>( display.y );
	ImGui::GetIO( ).MouseDrawCursor = g_render.m_menu_open;

	if ( g_render.m_menu_open ) {
		ImGui::Begin( "menu", &g_render.m_menu_open );
		ImGui::Text( "hello" );
		ImGui::End( );
	}

	ImGui::EndFrame( );
	ImGui::Render( );
	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

	return g_render.m_orig_endscene( device );
}

HRESULT __stdcall Render::hk_reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp ) {
	if ( g_render.m_shutting_down )
		return g_render.m_orig_reset( device, pp );

	if ( g_render.m_imgui_initialized )
		ImGui_ImplDX9_InvalidateDeviceObjects( );

	HRESULT hr = g_render.m_orig_reset( device, pp );

	if ( g_render.m_imgui_initialized )
		ImGui_ImplDX9_CreateDeviceObjects( );

	return hr;
}

bool Render::init( ) {
	HWND target = find_game_window( );
	if ( !target ) {
		std::printf( "[!] no game window found\n" );
		return false;
	}

	IDirect3D9* d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if ( !d3d ) return false;

	D3DPRESENT_PARAMETERS pp{ };
	pp.Windowed   = ( GetWindowLongPtr( target, GWL_STYLE ) & WS_POPUP ) == 0;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.hDeviceWindow = target;

	IDirect3DDevice9* dummy = nullptr;
	HRESULT hr = d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, target,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dummy );

	if ( FAILED( hr ) ) {
		pp.Windowed = !pp.Windowed;
		hr = d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, target,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &dummy );
	}

	if ( FAILED( hr ) ) {
		d3d->Release( );
		std::printf( "[!] d3d9 CreateDevice failed\n" );
		return false;
	}

	void* vtable[119];
	memcpy( vtable, *reinterpret_cast<void***>( dummy ), sizeof( vtable ) );
	dummy->Release( );
	d3d->Release( );

	m_endscene_addr = vtable[42];
	m_reset_addr    = vtable[16];

	if ( MH_CreateHook( m_endscene_addr, hk_endscene, reinterpret_cast<void**>( &m_orig_endscene ) ) != MH_OK ) return false;
	if ( MH_CreateHook( m_reset_addr,    hk_reset,    reinterpret_cast<void**>( &m_orig_reset ) )    != MH_OK ) return false;
	if ( MH_EnableHook( m_endscene_addr ) != MH_OK ) return false;
	if ( MH_EnableHook( m_reset_addr )    != MH_OK ) return false;

	HMODULE user32 = GetModuleHandleA( "user32.dll" );
	if ( user32 ) {
		m_set_cursor_addr = GetProcAddress( user32, "SetCursorPos" );
		m_get_cursor_addr = GetProcAddress( user32, "GetCursorPos" );

		if ( m_set_cursor_addr )
			if ( MH_CreateHook( m_set_cursor_addr, hk_set_cursor_pos, reinterpret_cast<void**>( &m_orig_set_cursor_pos ) ) == MH_OK )
				MH_EnableHook( m_set_cursor_addr );

		if ( m_get_cursor_addr )
			if ( MH_CreateHook( m_get_cursor_addr, hk_get_cursor_pos, reinterpret_cast<void**>( &m_orig_get_cursor_pos ) ) == MH_OK )
				MH_EnableHook( m_get_cursor_addr );
	}

	std::printf( "[+] d3d9 hooks installed\n" );
	return true;
}

void Render::shutdown( ) {
	m_shutting_down = true;

	if ( m_endscene_addr )   MH_DisableHook( m_endscene_addr );
	if ( m_reset_addr )      MH_DisableHook( m_reset_addr );
	if ( m_set_cursor_addr ) MH_DisableHook( m_set_cursor_addr );
	if ( m_get_cursor_addr ) MH_DisableHook( m_get_cursor_addr );

	Sleep( 100 );

	if ( m_orig_wndproc && m_game_hwnd ) {
		SetWindowLongPtr( m_game_hwnd, GWLP_WNDPROC, (LONG_PTR)m_orig_wndproc );
		m_orig_wndproc = nullptr;
	}

	if ( m_imgui_initialized ) {
		ImGui_ImplDX9_Shutdown( );
		ImGui_ImplWin32_Shutdown( );
		ImGui::DestroyContext( );
		m_imgui_initialized = false;
	}
}
