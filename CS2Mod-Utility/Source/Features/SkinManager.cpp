#include "SkinManager.h"

namespace skin_changer {

static const std::unordered_map<std::uint16_t, std::string> KnifeModels = {
    {500, "phase2/weapons/models/knife/knife_bayonet/weapon_knife_bayonet_ag2.vmdl"},
    {503, "phase2/weapons/models/knife/knife_css/weapon_knife_css_ag2.vmdl"},
    {505, "phase2/weapons/models/knife/knife_flip/weapon_knife_flip_ag2.vmdl"},
    {506, "phase2/weapons/models/knife/knife_gut/weapon_knife_gut_ag2.vmdl"},
    {507, "phase2/weapons/models/knife/knife_karambit/weapon_knife_karambit_ag2.vmdl"},
    {508, "phase2/weapons/models/knife/knife_m9/weapon_knife_m9_ag2.vmdl"},
    {509, "phase2/weapons/models/knife/knife_tactical/weapon_knife_tactical_ag2.vmdl"},
    {512, "phase2/weapons/models/knife/knife_falchion/weapon_knife_falchion_ag2.vmdl"},
    {514, "phase2/weapons/models/knife/knife_bowie/weapon_knife_bowie_ag2.vmdl"},
    {515, "phase2/weapons/models/knife/knife_butterfly/weapon_knife_butterfly_ag2.vmdl"},
    {516, "phase2/weapons/models/knife/knife_push/weapon_knife_push_ag2.vmdl"},
    {517, "phase2/weapons/models/knife/knife_cord/weapon_knife_cord_ag2.vmdl"},
    {518, "phase2/weapons/models/knife/knife_canis/weapon_knife_canis_ag2.vmdl"},
    {519, "phase2/weapons/models/knife/knife_ursus/weapon_knife_ursus_ag2.vmdl"},
    {520, "phase2/weapons/models/knife/knife_navaja/weapon_knife_navaja_ag2.vmdl"},
    {521, "phase2/weapons/models/knife/knife_outdoor/weapon_knife_outdoor_ag2.vmdl"},
    {522, "phase2/weapons/models/knife/knife_stiletto/weapon_knife_stiletto_ag2.vmdl"},
    {523, "phase2/weapons/models/knife/knife_talon/weapon_knife_talon_ag2.vmdl"},
    {525, "phase2/weapons/models/knife/knife_skeleton/weapon_knife_skeleton_ag2.vmdl"},
    {526, "phase2/weapons/models/knife/knife_kukri/weapon_knife_kukri_ag2.vmdl"}
};

static const std::map<std::uint16_t, std::string> KnifeNames = {
    {500, "Bayonet"},
    {503, "Classic"},
    {505, "Flip"},
    {506, "Gut"},
    {507, "Karambit"},
    {508, "M9 Bayonet"},
    {509, "Huntsman"},
    {512, "Falchion"},
    {514, "Bowie"},
    {515, "Butterfly"},
    {516, "Daggers"},
    {517, "Paracord"},
    {518, "Survival"},
    {519, "Ursus"},
    {520, "Navaja"},
    {521, "Nomad"},
    {522, "Stiletto"},
    {523, "Talon"},
    {525, "Skeleton"},
    {526, "Kukri"}
};

Knife_t::Knife_t(std::uint16_t def) : defIndex(def) {
    if (def) {
        auto nameIt = KnifeNames.find(def);
        if (nameIt != KnifeNames.end())
            name = nameIt->second;

        auto modelIt = KnifeModels.find(def);
        if (modelIt != KnifeModels.end())
            model = modelIt->second;
    }
}

void SkinManager::AddSkin(const SkinInfo_t& AddedSkin) noexcept {
    for (auto& skin : Skins) {
        if (skin.weaponType == AddedSkin.weaponType) {
            if (skin.Paint == AddedSkin.Paint)
                return;
            skin = AddedSkin;
            return;
        }
    }
    Skins.push_back(AddedSkin);
}

SkinInfo_t SkinManager::GetSkin(WeaponsEnum def) const noexcept {
    for (const auto& skin : Skins) {
        if (skin.weaponType == def)
            return skin;
    }
    return SkinInfo_t{0, false, std::string(), WeaponsEnum::none};
}

}
