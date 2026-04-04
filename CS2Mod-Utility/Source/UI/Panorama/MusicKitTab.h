#pragma once

#include <Features/SkinManager.h>
#include <Features/MusicKits.h>
#include <Features/SkinChangerConfigVariables.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct MusicKitSelectionDropdownSelectionChangeHandler {
    explicit MusicKitSelectionDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(skin_changer::MUSIC_KITS.size()))
            return;

        const auto& musicKit = skin_changer::MUSIC_KITS[selectedIndex];
        SET_CONFIG_VAR(music_kits_vars::SelectedMusicKit, musicKit.id);
    }

private:
    HookContext& hookContext;
};

template <typename HookContext>
class MusicKitTab {
public:
    explicit MusicKitTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& guiPanel) const noexcept
    {
        initMusicKitsTab(guiPanel);
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        updateMusicKitsTab(mainMenu);
    }

private:
    void initMusicKitsTab(auto&& guiPanel) const noexcept
    {
        initDropDown<MusicKitSelectionDropdownSelectionChangeHandler<HookContext>>(guiPanel, "selected_kit");
    }

    void updateMusicKitsTab(auto&& mainMenu) const noexcept
    {
        const auto selectedKitId = GET_CONFIG_VAR(music_kits_vars::SelectedMusicKit);
        
        int selectedIndex = 0;
        for (int i = 0; i < static_cast<int>(skin_changer::MUSIC_KITS.size()); ++i) {
            if (skin_changer::MUSIC_KITS[i].id == selectedKitId) {
                selectedIndex = i;
                break;
            }
        }
        
        setDropDownSelectedIndex(mainMenu, "selected_kit", selectedIndex);
    }

    template <typename Handler>
    void initDropDown(auto&& guiPanel, const char* panelId) const
    {
        auto&& dropDown = guiPanel.findChildInLayoutFile(panelId).clientPanel().template as<PanoramaDropDown>();
        dropDown.registerSelectionChangedHandler(&GuiEntryPoints<HookContext>::template dropDownSelectionChanged<Handler>);
    }

    void setDropDownSelectedIndex(auto&& mainMenu, const char* dropDownId, int selectedIndex) const noexcept
    {
        mainMenu.findChildInLayoutFile(dropDownId).clientPanel().template as<PanoramaDropDown>().setSelectedIndex(selectedIndex);
    }

    HookContext& hookContext;
};
