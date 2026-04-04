#pragma once

#include <Config/ConfigVariable.h>
#include <cstdint>

namespace skins_vars {
    CONFIG_VARIABLE(Enabled, bool, false);
    CONFIG_VARIABLE(ShowSkinSelector, bool, true);
}

namespace music_kits_vars {
    CONFIG_VARIABLE(Enabled, bool, false);
    CONFIG_VARIABLE(SelectedMusicKit, std::uint16_t, 1);
}
