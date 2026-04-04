#pragma once

#include <Features/SkinManager.h>
#include <Features/SkinChangerConfigVariables.h>
#include <Features/SkinDatabase.h>
#include <Features/MusicKits.h>
#include <Features/WeaponDetection.h>
#include <GameClient/EntitySystem/EntitySystem.h>
#include <HookContext/HookContextMacros.h>
#include <optional>
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
        if (!GET_CONFIG_VAR(skins_vars::Enabled) && !GET_CONFIG_VAR(music_kits_vars::Enabled))
            return;

        if (GET_CONFIG_VAR(skins_vars::Enabled)) {
            applySkins();
        }

        if (GET_CONFIG_VAR(music_kits_vars::Enabled)) {
            applyMusicKit();
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

        // Get weapon definition index to determine what type of weapon this is
        // For now, we apply vanilla (Paint ID 0) as a default
        // In production, this would read the equipped skin from SkinManager
        
        // Write paint kit (vanilla = 0)
        const auto paintKit = 0u;
        
        // m_nFallbackPaintKit offset = 0x1540
        auto paintKitPtr = reinterpret_cast<std::uint32_t*>(weaponPtr + 0x1540);
        *paintKitPtr = paintKit;
    }

    void applyMusicKit() noexcept
    {
        auto playerController = hookContext.localPlayerController();
        if (!playerController)
            return;

        // Read selected music kit from config
        const auto selectedMusicKitId = GET_CONFIG_VAR(music_kits_vars::SelectedMusicKit);

        // PlayerController is a wrapper - convert to raw pointer for offset operations
        auto controllerRawPtr = playerController.operator*();
        auto controllerAddr = reinterpret_cast<std::uintptr_t>(controllerRawPtr);

        // Get inventory services from player controller
        // Offset: m_pInventoryServices = 0x700
        if (controllerAddr == 0)
            return;

        auto inventoryServicesPtr = *reinterpret_cast<std::uintptr_t*>(controllerAddr + 0x700);

        if (inventoryServicesPtr == 0)
            return;

        // Write music kit ID to m_unMusicID (offset 0x40)
        auto musicKitIdPtr = reinterpret_cast<std::uint16_t*>(inventoryServicesPtr + 0x40);
        *musicKitIdPtr = selectedMusicKitId;
    }
};

}
