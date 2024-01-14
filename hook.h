#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <Windows.h>
#include <polyhook2/ZydisDisassembler.hpp>
#include <polyhook2/Detour/x86Detour.hpp>
#include <polyhook2/Enums.hpp>
#include <polyhook2/ErrorLog.hpp>

#include <map>
#include <string>

/* DirectX9 Function Types */
typedef HRESULT(__stdcall *tEndScene)(IDirect3DDevice9 *pDevice);

struct Hook
{
    uint64_t trampoline;
    PLH::x86Detour *detour;
};

namespace Hooks
{
    extern PLH::ZydisDisassembler disassembler;
    extern std::map<std::string, Hook> hooks;


    void createHook(std::string name, uintptr_t original, uintptr_t hook);
    void removeAllHooks();
}

