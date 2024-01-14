// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include <thread>
#include <math.h>

#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "hook.h"
#include "drawing.h"
#include "gameLoop.h"

//todo
//add a timer (possibly with tick count like serverframe, since sv_fps is always 20 / consistent)
//rpg lookdown (with user adjustable angles)
//segmenter (maybe start without packets?)

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

WNDPROC oWndproc;

bool isMenuOpen = false;
bool* p_open;

//mouseLock address
uint32_t mouseLock = 0x013E06F5;

//get cod4 for hook
HWND getCOD4WindowHandle()
{
    static const char windowName[] = "Call of Duty 4";
    static const char windowNameFallback[] = "Call of Duty 4 X";

    HWND cod4xHandle = FindWindowA(NULL, windowName);
    return cod4xHandle ? cod4xHandle : FindWindowA(NULL, windowNameFallback);
}

LRESULT Wndproc(HWND window, UINT message, WPARAM parameter, LPARAM parameter2)
{
    ImGui_ImplWin32_WndProcHandler(window, message, parameter, parameter2);
    if (isMenuOpen)
    {
        return true;
    }    
    return CallWindowProcA(oWndproc, window, message, parameter, parameter2);
}

void render(IDirect3DDevice9 *device)
{
    
    static bool didInit = false;
    if (!didInit)
    {
        HWND cod4Window = getCOD4WindowHandle();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(cod4Window); 
        ImGui_ImplDX9_Init(device);
        ImGui::SetNextWindowSize(ImVec2(100, 100));
        oWndproc = (WNDPROC)SetWindowLongPtr(cod4Window, GWL_WNDPROC, (LONG_PTR)Wndproc);
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig cfg = ImFontConfig();
        cfg.FontDataOwnedByAtlas = false;
        ImGuiStyle& style = ImGui::GetStyle();
        
        didInit = true;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = isMenuOpen; // set this to true if want mouse visible
    
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    if (isMenuOpen)
    {
       
        *(char*)mouseLock = 0;
        //menu code here
        Overlay.Menu();
    }
    else
        *(char*)mouseLock = 1; //if menu is closed lock mouse
        
    //Draw on top here (background draw list)
    Overlay.StaticOverlay();

    //End imgui frames and render
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

bool turnOff;

HRESULT __stdcall EndSceneHook(IDirect3DDevice9 *device)
{

    HRESULT result = ((tEndScene)Hooks::hooks["EndScene"].trampoline)(device);

    //here is for functions that should run regardless if imgui menu is open. 
    Game.Run(); // main loop for all game funcs

    render(device);
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        isMenuOpen = !isMenuOpen;
    }
    return result;
}

DWORD_PTR* FindDevice(VOID)
{
    DWORD Base = (DWORD)LoadLibraryW(L"d3d9.dll");

    for (DWORD i = 0; i < 0x128000; i++)
    {
        if ((*(BYTE*)(Base + i + 0x00)) == 0xC7
            && (*(BYTE*)(Base + i + 0x01)) == 0x06
            && (*(BYTE*)(Base + i + 0x06)) == 0x89
            && (*(BYTE*)(Base + i + 0x07)) == 0x86
            && (*(BYTE*)(Base + i + 0x0C)) == 0x89
            && (*(BYTE*)(Base + i + 0x0D)) == 0x86)
            return (DWORD_PTR*)(Base + i + 2);
    }
    return NULL;
}

DWORD WINAPI MainThread(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    printf("Injected!\nPress END to un-inject\n");

    DWORD_PTR* VtablePtr = FindDevice();

    if (VtablePtr == NULL)
    {
        ExitProcess(0); //or it will crash anyway
    }

    DWORD_PTR* VTable;
    *(DWORD_PTR*)&VTable = *(DWORD_PTR*)VtablePtr;

    uintptr_t endSceneAddress = (uintptr_t) VTable[42];
    printf("endscene @ 0x%p\n", endSceneAddress); //print endscene address to screen

    Hooks::createHook("EndScene", endSceneAddress, (uintptr_t)EndSceneHook);

    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            *(char*)mouseLock = 1;
            Hooks::removeAllHooks();
            SetWindowLongPtr(getCOD4WindowHandle(), GWL_WNDPROC, (LONG_PTR)oWndproc);
            ImGui_ImplDX9_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            fclose(f);
            FreeConsole();
            FreeLibraryAndExitThread(hModule, NULL);

        }       
    }
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)MainThread, hModule, NULL, NULL);
        break;
    }
    return TRUE;
}
