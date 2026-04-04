#pragma once

#include <Features/SkinManager.h>
#include <map>
#include <vector>

namespace skin_changer {

class SkinDatabase {
public:
    SkinDatabase() noexcept = default;

    void initialize() noexcept;

    [[nodiscard]] const std::vector<SkinInfo_t>& getSkinsForWeapon(WeaponsEnum weaponType) const noexcept;
    [[nodiscard]] const std::vector<MusicKit_t>& getMusicKits() const noexcept;

    static SkinDatabase& instance() noexcept {
        static SkinDatabase db;
        return db;
    }

private:
    std::map<WeaponsEnum, std::vector<SkinInfo_t>> weaponSkinsMap;
    std::vector<MusicKit_t> musicKitsList;
    bool initialized = false;

    void loadDefaultSkins() noexcept;
    void loadDefaultMusicKits() noexcept;
};

}
