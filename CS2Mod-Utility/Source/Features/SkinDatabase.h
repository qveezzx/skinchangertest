#pragma once

namespace skin_changer {

// Stub database class - not currently used
class SkinDatabase {
public:
    SkinDatabase() noexcept = default;

    void initialize() noexcept {
        // No initialization needed
    }

    static SkinDatabase& instance() noexcept {
        static SkinDatabase db;
        return db;
    }
};

}
