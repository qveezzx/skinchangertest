#pragma once

#include <Features/SkinManager.h>
#include <optional>
#include <cstdint>

namespace skin_changer {

template <typename HookContext>
class WeaponDetection {
public:
    explicit WeaponDetection(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] std::optional<WeaponsEnum> getEquippedWeapon() const noexcept
    {
        try {
            // Get local player pawn from HookContext
            auto playerPawn = hookContext.activeLocalPlayerPawn();
            if (!playerPawn)
                return std::nullopt;

            // Get weapon services from player pawn
            auto weaponServices = playerPawn.weaponServices();
            if (!weaponServices)
                return std::nullopt;

            // Get active weapon handle
            auto activeWeaponHandle = weaponServices.activeWeapon();
            if (!activeWeaponHandle)
                return std::nullopt;

            // Resolve handle to weapon entity pointer
            auto weapon = resolveHandleToEntity(*activeWeaponHandle);
            if (!weapon)
                return std::nullopt;

            // Get weapon definition index from item
            auto weaponDefIndex = getWeaponDefinitionIndex(weapon);
            if (!weaponDefIndex)
                return std::nullopt;

            return static_cast<WeaponsEnum>(*weaponDefIndex);
        } catch (...) {
            return std::nullopt;
        }
    }

    [[nodiscard]] std::vector<std::uintptr_t> getAllWeapons() const noexcept
    {
        std::vector<std::uintptr_t> weapons;

        try {
            auto playerPawn = hookContext.activeLocalPlayerPawn();
            if (!playerPawn)
                return weapons;

            auto weaponServices = playerPawn.weaponServices();
            if (!weaponServices)
                return weapons;

            // Get all weapon handles
            auto weaponHandles = weaponServices.myWeapons();
            if (!weaponHandles)
                return weapons;

            // Resolve each handle to entity pointer
            for (auto handle : weaponHandles) {
                if (auto weapon = resolveHandleToEntity(handle)) {
                    weapons.push_back(weapon);
                }
            }
        } catch (...) {
            // Silent failure
        }

        return weapons;
    }

private:
    HookContext& hookContext;

    [[nodiscard]] std::uintptr_t resolveHandleToEntity(std::uint32_t handle) const noexcept
    {
        try {
            auto entitySystem = hookContext.template make<EntitySystem>();
            if (auto entity = entitySystem.getEntityFromHandle(handle)) {
                return reinterpret_cast<std::uintptr_t>(entity);
            }
        } catch (...) {
            // Silent failure
        }
        return 0;
    }

    [[nodiscard]] std::optional<std::uint16_t> getWeaponDefinitionIndex(std::uintptr_t weapon) const noexcept
    {
        if (!weapon)
            return std::nullopt;

        try {
            // C_EconItemView::m_iItemDefinitionIndex offset = 0x1BA (from offsets-latest)
            // This is the item definition index that determines weapon type
            const auto defIndex = *reinterpret_cast<std::uint16_t*>(weapon + 0x1BA);
            if (defIndex > 0)
                return defIndex;
            return std::nullopt;
        } catch (...) {
            return std::nullopt;
        }
    }
};

}
