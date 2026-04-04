#pragma once

#include <Features/SkinManager.h>
#include <Features/SkinChangerConfigVariables.h>
#include <HookContext/HookContextMacros.h>
#include <CS2/Classes/Entities/C_BaseEntity.h>
#include <cstdint>

namespace skin_changer {

template <typename HookContext>
class SkinApplier {
public:
    explicit SkinApplier(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void applySkinsAndMusicKit() noexcept
    {
        if (!GET_CONFIG_VAR(skins_vars::Enabled))
            return;

        applySkins();
    }

private:
    HookContext& hookContext;

    void applySkins() noexcept
    {
        auto playerPawn = hookContext.activeLocalPlayerPawn();
        if (!playerPawn)
            return;

        applyPaintKit(playerPawn.getActiveWeapon());
    }

    void applyPaintKit(auto&& weapon) noexcept
    {
        auto weaponPtr = static_cast<cs2::C_BaseEntity*>(weapon.baseEntity());
        if (!weaponPtr)
            return;

        auto weaponAddr = reinterpret_cast<std::uintptr_t>(weaponPtr);
        auto paintKitPtr = reinterpret_cast<std::uint32_t*>(weaponAddr + 0x1540);
        if (paintKitPtr)
            *paintKitPtr = 0u;
    }
};

}
