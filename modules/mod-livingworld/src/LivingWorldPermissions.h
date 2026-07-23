#ifndef MOD_LIVINGWORLD_PERMISSIONS_H
#define MOD_LIVINGWORLD_PERMISSIONS_H

#include <cstdint>

namespace LivingWorld::Permissions
{
    // Module-owned RBAC range. Keep these values synchronized with
    // data/sql/db-auth/base/livingworld_rbac.sql.
    inline constexpr std::uint32_t View = 1900;
    inline constexpr std::uint32_t ProfileInspect = 1901;
    inline constexpr std::uint32_t RuntimeControl = 1902;
    inline constexpr std::uint32_t PopulationControl = 1903;
    inline constexpr std::uint32_t DestructiveSeed = 1904;
}

#endif
