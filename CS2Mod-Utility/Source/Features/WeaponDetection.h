#pragma once

#include <Features/SkinManager.h>
#include <optional>
#include <vector>

namespace skin_changer {

// Stub - not currently used since SkinApplier uses getActiveWeapon() directly
template <typename HookContext>
class WeaponDetection {
public:
    explicit WeaponDetection(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] std::optional<WeaponsEnum> getEquippedWeapon() const noexcept
    {
        return std::nullopt;
    }

    [[nodiscard]] std::vector<std::uintptr_t> getAllWeapons() const noexcept
    {
        return std::vector<std::uintptr_t>();
    }

private:
    HookContext& hookContext;
};

}
