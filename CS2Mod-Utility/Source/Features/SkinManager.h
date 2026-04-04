#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

namespace skin_changer {

enum class WeaponsEnum : std::uint16_t {
    none = 0,
    Deagle = 1,
    Elite = 2,
    FiveSeven = 3,
    Glock = 4,
    Ak47 = 7,
    Aug = 8,
    Awp = 9,
    Famas = 10,
    G3Sg1 = 11,
    M249 = 14,
    Mac10 = 17,
    P90 = 19,
    Ump45 = 24,
    Xm1014 = 25,
    Bizon = 26,
    Mag7 = 27,
    Negev = 28,
    Sawedoof = 29,
    Tec9 = 30,
    Zeus = 31,
    P200 = 32,
    Mp7 = 33,
    Mp9 = 34,
    Nova = 35,
    p250 = 36,
    Scar20 = 38,
    Sg556 = 39,
    Ssg08 = 40,
    CtKnife = 42,
    FlashBang = 43,
    HeGrenade = 44,
    SmokeGrenade = 45,
    Molotov = 46,
    Decoy = 47,
    IncGrenade = 48,
    C4 = 49,
    M4A4 = 16,
    UspS = 61,
    M4A1Silencer = 60,
    Cz65A = 63,
    Revolver = 64,
    Tknife = 59,
    Galil = 13,
    Mp5SD = 23
};

struct SkinInfo_t {
    int Paint = 0;
    bool bUsesOldModel = false;
    std::string name;
    WeaponsEnum weaponType = WeaponsEnum::none;
    int rarity = 0;
    std::string image_url;
};

struct MusicKit_t {
    std::uint16_t id = 0;
    std::string name;

    MusicKit_t() = default;
    MusicKit_t(std::uint16_t id, const std::string& name) : id(id), name(name) {}
};

struct Knife_t {
    std::uint16_t defIndex = 0;
    std::string name;
    std::string model;

    Knife_t() = default;
    explicit Knife_t(std::uint16_t def);
};

class SkinManager {
public:
    SkinManager() noexcept = default;

    void AddSkin(const SkinInfo_t& AddedSkin) noexcept;
    SkinInfo_t GetSkin(WeaponsEnum def) const noexcept;

    std::vector<SkinInfo_t> Skins;
    MusicKit_t MusicKit{1, "Counter-Strike 2"};
    Knife_t Knife;
};

}
