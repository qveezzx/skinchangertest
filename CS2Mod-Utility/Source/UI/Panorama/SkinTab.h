#pragma once

#include <Features/SkinManager.h>
#include <Features/SkinChangerConfigVariables.h>
#include <Features/SkinDatabase.h>
#include <GameClient/Panorama/PanoramaDropDown.h>
#include <EntryPoints/GuiEntryPoints.h>
#include <HookContext/HookContextMacros.h>

template <typename HookContext>
struct SkinChangerEnableDropdownSelectionChangeHandler {
    explicit SkinChangerEnableDropdownSelectionChangeHandler(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void onSelectionChanged(int selectedIndex)
    {
        bool isEnabled;
        switch (selectedIndex) {
        case 0: isEnabled = true; break;
        case 1: isEnabled = false; break;
        default: return;
        }
        SET_CONFIG_VAR(skins_vars::Enabled, isEnabled);
    }

private:
    HookContext& hookContext;
};

template <typename HookContext>
class SkinTab {
public:
    explicit SkinTab(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    void init(auto&& guiPanel) const noexcept
    {
        initSkinsTab(guiPanel);
    }

    void updateFromConfig(auto&& mainMenu) const noexcept
    {
        updateSkinsTab(mainMenu);
    }

private:
    void initSkinsTab(auto&& guiPanel) const noexcept
    {
        initDropDown<SkinChangerEnableDropdownSelectionChangeHandler<HookContext>>(guiPanel, "enabled");
    }

    void updateSkinsTab(auto&& mainMenu) const noexcept
    {
        setDropDownSelectedIndex(mainMenu, "enabled", !GET_CONFIG_VAR(skins_vars::Enabled));
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
