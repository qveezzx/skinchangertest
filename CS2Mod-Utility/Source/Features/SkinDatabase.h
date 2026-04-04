#pragma once

#include <Features/SkinManager.h>
#include <Features/MusicKits.h>
#include <map>
#include <vector>

namespace skin_changer {

class SkinDatabase {
public:
    SkinDatabase() noexcept = default;

    void initialize() noexcept;

    [[nodiscard]] const std::vector<SkinInfo_t>& getSkinsForWeapon(WeaponsEnum weaponType) const noexcept;
    [[nodiscard]] const std::array<MusicKitInfo, 71>& getMusicKits() const noexcept;

    static SkinDatabase& instance() noexcept {
        static SkinDatabase db;
        return db;
    }

private:
    std::map<WeaponsEnum, std::vector<SkinInfo_t>> weaponSkinsMap;
    bool initialized = false;

    void loadDefaultSkins() noexcept;
};

}
