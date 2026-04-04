#pragma once

#include <Features/SkinManager.h>
#include <Features/MusicKits.h>
#include <map>
#include <vector>

namespace skin_changer {

class SkinDatabase {
public:
    SkinDatabase() noexcept = default;

    void initialize() noexcept {
        if (initialized)
            return;

        loadDefaultSkins();
        initialized = true;
    }

    [[nodiscard]] const std::vector<SkinInfo_t>& getSkinsForWeapon(WeaponsEnum weaponType) const noexcept {
        auto it = weaponSkinsMap.find(weaponType);
        if (it != weaponSkinsMap.end())
            return it->second;
        static const std::vector<SkinInfo_t> EMPTY_SKINS;
        return EMPTY_SKINS;
    }

    [[nodiscard]] const std::array<MusicKitInfo, 71>& getMusicKits() const noexcept {
        return MUSIC_KITS;
    }

    static SkinDatabase& instance() noexcept {
        static SkinDatabase db;
        return db;
    }

private:
    std::map<WeaponsEnum, std::vector<SkinInfo_t>> weaponSkinsMap;
    bool initialized = false;

    void loadDefaultSkins() noexcept {
        // Add vanilla/default option for all weapons
        for (int i = 0; i <= 64; ++i) {
            auto weaponEnum = static_cast<WeaponsEnum>(i);
            std::vector<SkinInfo_t> skins;
            
            // Always add vanilla option first (Paint = 0)
            SkinInfo_t vanillaSkin{
                0,              // Paint ID (0 = vanilla)
                false,          // Uses old model
                "Vanilla",      // Name
                weaponEnum,     // Weapon type
                1,              // Rarity (common)
                ""              // No image for vanilla
            };
            skins.push_back(vanillaSkin);
            
            // Store even with just vanilla for now
            // In production, this would load from JSON API or cache file
            if (!skins.empty())
                weaponSkinsMap[weaponEnum] = skins;
        }
    }
};

}
