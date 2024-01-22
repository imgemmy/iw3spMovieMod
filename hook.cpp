#include "hook.h"

namespace Hooks
{
    /* Globals */
    PLH::ZydisDisassembler disassembler = PLH::ZydisDisassembler(PLH::Mode::x86);

    // Functionname -> Hook
    std::map<std::string, Hook> hooks;

    void createHook(std::string name, uintptr_t original, uintptr_t hook)
    {
        hooks[name] = Hook();
        hooks[name].detour = new PLH::x86Detour(original, hook, &hooks[name].trampoline);
        hooks[name].detour->hook();
    }

    void removeAllHooks()
    {
        for (auto &[functionName, hook] : hooks)
        {
            hook.detour->unHook();
            delete hook.detour;
        }
    }
}