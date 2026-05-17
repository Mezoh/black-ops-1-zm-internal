#pragma once

class Render {
public:
	using endscene_t       = HRESULT( __stdcall* )( IDirect3DDevice9* );
	using reset_t          = HRESULT( __stdcall* )( IDirect3DDevice9*, D3DPRESENT_PARAMETERS* );
	using set_cursor_pos_t = BOOL( WINAPI* )( int, int );
	using get_cursor_pos_t = BOOL( WINAPI* )( LPPOINT );

	endscene_t       m_orig_endscene       = nullptr;
	reset_t          m_orig_reset          = nullptr;
	set_cursor_pos_t m_orig_set_cursor_pos = nullptr;
	get_cursor_pos_t m_orig_get_cursor_pos = nullptr;

	void*    m_endscene_addr     = nullptr;
	void*    m_reset_addr        = nullptr;
	void*    m_set_cursor_addr   = nullptr;
	void*    m_get_cursor_addr   = nullptr;

	HWND     m_game_hwnd         = nullptr;
	WNDPROC  m_orig_wndproc      = nullptr;
	int      m_screen_w          = 0;
	int      m_screen_h          = 0;
	bool     m_imgui_initialized = false;
	bool     m_shutting_down     = false;
	bool     m_menu_open         = false;

	bool init( );
	void shutdown( );

	static HRESULT __stdcall hk_endscene( IDirect3DDevice9* device );
	static HRESULT __stdcall hk_reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp );
	static LRESULT __stdcall hk_wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
	static BOOL    WINAPI    hk_set_cursor_pos( int x, int y );
	static BOOL    WINAPI    hk_get_cursor_pos( LPPOINT p );

private:
	HWND find_game_window( );
	static BOOL CALLBACK enum_proc( HWND hwnd, LPARAM lp );

}; extern Render g_render;
