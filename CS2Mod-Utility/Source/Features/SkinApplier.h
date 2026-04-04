#pragma once

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

        // Get weapons container from player
        auto weaponServices = playerPawn.weaponServices();
        if (!weaponServices)
            return;

        // Get all weapons and iterate
        auto weapons = weaponServices.weapons();
        
        // Use forEach to apply skins to each weapon
        weapons.forEach([this](auto&& weaponEntity) {
            this->applyPaintKitToWeapon(weaponEntity);
        });
    }

    void applyPaintKitToWeapon(auto&& weaponEntity) noexcept
    {
        // Get raw weapon pointer from base entity
        auto weaponPtr = static_cast<cs2::C_BaseEntity*>(weaponEntity);
        if (!weaponPtr)
            return;

        auto weaponAddr = reinterpret_cast<std::uintptr_t>(weaponPtr);

        // Step 1: Set m_iItemIDHigh to -1 to trigger item resync
        // m_AttributeManager is at 0x1090 within the weapon
        // m_iItemIDHigh is at 0x1D0 within the item/attribute manager
        auto itemAddr = weaponAddr + 0x1090;  // m_AttributeManager offset
        auto itemIDHighPtr = reinterpret_cast<std::uint32_t*>(itemAddr + 0x1D0);
        if (itemIDHighPtr)
            *itemIDHighPtr = 0xFFFFFFFFu;  // Set to -1

        // Step 2: Write paint kit to m_nFallbackPaintKit (offset 0x1540)
        auto paintKitPtr = reinterpret_cast<std::uint32_t*>(weaponAddr + 0x1540);
        if (paintKitPtr)
            *paintKitPtr = 1u;  // Apply skin with paint kit ID 1
    }
};

}
