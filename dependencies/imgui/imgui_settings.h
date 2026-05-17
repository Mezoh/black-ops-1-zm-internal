#pragma once
#include <imgui/imgui.h>

namespace f
{
	namespace font
	{
		struct family_desc
		{
			const char* name;
			const char* regular_path;
			const char* bold_path;
		};

		inline constexpr family_desc menu_families[] = {
			{ "Segoe UI",   "C:\\Windows\\Fonts\\segoeui.ttf",  "C:\\Windows\\Fonts\\segoeuib.ttf" },
			{ "Corbel",     "C:\\Windows\\Fonts\\corbel.ttf",   "C:\\Windows\\Fonts\\corbelb.ttf" },
			{ "Candara",    "C:\\Windows\\Fonts\\Candara.ttf",  "C:\\Windows\\Fonts\\Candarab.ttf" },
			{ "Calibri",    "C:\\Windows\\Fonts\\calibri.ttf",  "C:\\Windows\\Fonts\\calibrib.ttf" },
			{ "Verdana",    "C:\\Windows\\Fonts\\verdana.ttf",  "C:\\Windows\\Fonts\\verdanab.ttf" },
			{ "Tahoma",     "C:\\Windows\\Fonts\\tahoma.ttf",   "C:\\Windows\\Fonts\\tahomabd.ttf" },
			{ "Trebuchet",  "C:\\Windows\\Fonts\\trebuc.ttf",   "C:\\Windows\\Fonts\\trebucbd.ttf" },
			{ "Arial",      "C:\\Windows\\Fonts\\arial.ttf",    "C:\\Windows\\Fonts\\arialbd.ttf" },
			{ "Century Gothic", "C:\\Windows\\Fonts\\GOTHIC.TTF", "C:\\Windows\\Fonts\\GOTHICB.TTF" },
			{ "Gadugi",     "C:\\Windows\\Fonts\\gadugi.ttf",   "C:\\Windows\\Fonts\\gadugib.ttf" },
			{ "Leelawadee UI", "C:\\Windows\\Fonts\\LeelawUI.ttf", "C:\\Windows\\Fonts\\LeelaUIb.ttf" },
			{ "Lucida Sans", "C:\\Windows\\Fonts\\LSANS.TTF",   "C:\\Windows\\Fonts\\LSANSD.TTF" },
			{ "Bahnschrift", "C:\\Windows\\Fonts\\bahnschrift.ttf", "C:\\Windows\\Fonts\\bahnschrift.ttf" }
		};

		inline constexpr int menu_family_count = static_cast< int >( sizeof( menu_families ) / sizeof( menu_families[ 0 ] ) );

		inline int sanitize_menu_family_index( int index )
		{
			return ( index >= 0 && index < menu_family_count ) ? index : 0;
		}

		inline const family_desc& get_menu_family( int index )
		{
			return menu_families[ sanitize_menu_family_index( index ) ];
		}

		inline ImFont* menu_regular = nullptr;
		inline ImFont* menu_bold = nullptr;
		inline ImFont* esp_small = nullptr;
		inline ImFont* icon = nullptr;
	}

	namespace bg
	{
		inline ImU32 fill			= IM_COL32( 20, 20, 20, 255 );
		inline ImU32 stroke			= IM_COL32( 49, 49, 49, 255 );
		inline ImU32 outline		= IM_COL32( 8, 8, 8, 255 );

		inline ImVec2 main_size		= ImVec2( 500, 380 );

		inline float rounding = 0.f;
	}

	namespace child
	{
		inline ImU32 fill			= IM_COL32( 21, 21, 21, 255 );
		inline ImU32 outline		= IM_COL32( 58, 58, 58, 255 );
	}

	namespace tab_bar
	{
		inline ImU32 fill			= IM_COL32( 23, 23, 23, 255 );
		inline ImU32 stroke			= IM_COL32( 49, 49, 49, 255 );

		inline float rounding = 0.f;
	}

	inline ImVec4 rgba( int r, int g, int b, int a = 255 )
	{
		return ImVec4(
			static_cast< float >( r ) / 255.0f,
			static_cast< float >( g ) / 255.0f,
			static_cast< float >( b ) / 255.0f,
			static_cast< float >( a ) / 255.0f );
	}

	namespace palette
	{
		inline ImVec4 text					= rgba( 240, 240, 240 );
		inline ImVec4 text_dim				= rgba( 158, 158, 158 );
		inline ImVec4 title					= rgba( 235, 235, 235 );
		inline ImVec4 row_hover				= rgba( 46, 46, 46, 180 );

		inline ImVec4 window_bg				= rgba( 26, 26, 26 );
		inline ImVec4 child_bg				= rgba( 32, 32, 32 );
		inline ImVec4 popup_bg				= rgba( 28, 28, 28, 250 );
		inline ImVec4 title_bg				= rgba( 23, 23, 23 );
		inline ImVec4 menu_bar_bg			= rgba( 26, 26, 26 );

		inline ImVec4 frame_bg				= rgba( 36, 36, 36 );
		inline ImVec4 frame_bg_hover		= rgba( 46, 46, 46 );
		inline ImVec4 frame_bg_active		= rgba( 51, 51, 51 );

		inline ImVec4 button				= rgba( 38, 38, 38 );
		inline ImVec4 button_hover			= rgba( 51, 51, 51 );
		inline ImVec4 header				= rgba( 36, 36, 36 );
		inline ImVec4 header_hover			= rgba( 46, 46, 46 );

		inline ImVec4 tab					= rgba( 31, 31, 31 );
		inline ImVec4 tab_hover				= rgba( 46, 46, 46 );
		inline ImVec4 tab_active			= rgba( 46, 46, 46 );
		inline ImVec4 tab_unfocused			= rgba( 26, 26, 26 );
		inline ImVec4 tab_unfocused_active	= rgba( 36, 36, 36 );

		inline ImVec4 scrollbar_bg			= rgba( 20, 20, 20 );
		inline ImVec4 scrollbar_grab		= rgba( 51, 51, 51 );
		inline ImVec4 scrollbar_grab_hover	= rgba( 66, 66, 66 );

		inline ImVec4 border				= rgba( 59, 59, 59 );
		inline ImVec4 border_soft			= rgba( 82, 82, 82 );
		inline ImVec4 check_bg				= rgba( 26, 26, 26 );
		inline ImVec4 check_border			= rgba( 77, 77, 77 );
		inline ImVec4 separator				= rgba( 59, 59, 59 );
		inline ImVec4 accent				= rgba( 232, 138, 42 );
	}
}
