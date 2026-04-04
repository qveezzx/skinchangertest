#pragma once

#include <Features/SkinManager.h>
#include <Features/SkinChangerConfigVariables.h>
#include <Features/SkinDatabase.h>
#include <Features/MusicKits.h>
#include <Features/WeaponDetection.h>
#include <HookContext/HookContextMacros.h>
#include <cstdint>

namespace skin_changer {

template <typename HookContext>
class SkinApplier {
public:
    explicit SkinApplier(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
        SkinDatabase::instance().initialize();
    }

    void applySkinsAndMusicKit() noexcept
    {
        if (GET_CONFIG_VAR(skins_vars::Enabled)) {
            applySkins();
        }
    }

private:
    HookContext& hookContext;

    void applySkins() noexcept
    {
        auto playerPawn = hookContext.activeLocalPlayerPawn();
        if (!playerPawn)
            return;

        WeaponDetection<HookContext> weaponDetection{hookContext};
        auto weapons = weaponDetection.getAllWeapons();

        for (auto weaponPtr : weapons) {
            if (weaponPtr == 0)
                continue;

            applyPaintKitToWeapon(weaponPtr);
        }
    }

    void applyPaintKitToWeapon(std::uintptr_t weaponPtr) noexcept
    {
        if (weaponPtr == 0)
            return;

        // Write paint kit (vanilla = 0)
        // m_nFallbackPaintKit offset = 0x1540
        auto paintKitPtr = reinterpret_cast<std::uint32_t*>(weaponPtr + 0x1540);
        if (paintKitPtr)
            *paintKitPtr = 0u;
    }
};

}
