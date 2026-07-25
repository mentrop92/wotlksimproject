#ifndef MOD_LIVINGWORLD_PERMISSIONS_H
#define MOD_LIVINGWORLD_PERMISSIONS_H

#include <cstdint>

namespace LivingWorld::Permissions
{
    // High module-owned RBAC range. Keep these values synchronized with
    // data/sql/db-auth/base/livingworld_rbac.sql.
    inline constexpr std::uint32_t View = 19000;
    inline constexpr std::uint32_t ProfileInspect = 19001;
    inline constexpr std::uint32_t RuntimeControl = 19002;
    inline constexpr std::uint32_t PopulationControl = 19003;
    inline constexpr std::uint32_t DestructiveSeed = 19004;
}

#endif
