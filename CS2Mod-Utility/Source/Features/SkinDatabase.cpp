#include "SkinDatabase.h"
#include "MusicKits.h"

namespace skin_changer {

static const std::vector<SkinInfo_t> EMPTY_SKINS;

void SkinDatabase::initialize() noexcept {
    if (initialized)
        return;

    loadDefaultSkins();
    initialized = true;
}

const std::vector<SkinInfo_t>& SkinDatabase::getSkinsForWeapon(WeaponsEnum weaponType) const noexcept {
    auto it = weaponSkinsMap.find(weaponType);
    if (it != weaponSkinsMap.end())
        return it->second;
    return EMPTY_SKINS;
}

const std::array<MusicKitInfo, 71>& SkinDatabase::getMusicKits() const noexcept {
    return MUSIC_KITS;
}

void SkinDatabase::loadDefaultSkins() noexcept {
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

}
