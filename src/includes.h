#pragma once
#define WIN32_LEAN_AND_MEAN

// globals
extern volatile bool g_running;

// system
#include <Windows.h>
#include <iostream>
#include <cstdint>

// minhook
#include "../dependencies/minhook/MinHook.h"

// game
#include "game/game.h"
#include "game/offsets.h"

// render
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui/imgui.h>
#include <imgui/imgui_freetype.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_settings.h>

#include "render/render.h"
