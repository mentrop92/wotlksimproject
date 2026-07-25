#ifndef MOD_LIVINGWORLD_PROFILE_MANAGER_H
#define MOD_LIVINGWORLD_PROFILE_MANAGER_H

#include "LivingWorldProfile.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>

class Player;

namespace LivingWorld
{
    class ProfileManager
    {
    public:
        static ProfileManager& Instance();

        BotProfile const* EnsureProfile(Player* player);
        BotProfile const* GetProfile(std::uint32_t characterGuid) const;
        bool IsLoaded(std::uint32_t characterGuid) const;
        void UnloadProfile(std::uint32_t characterGuid, bool updateLastSeen = true);
        std::size_t LoadedCount() const;

    private:
        ProfileManager() = default;

        std::optional<BotProfile> LoadFromDatabase(std::uint32_t characterGuid) const;
        bool InsertProfile(BotProfile const& profile) const;

        std::unordered_map<std::uint32_t, BotProfile> _profiles;
    };
}

#define sLivingWorldProfiles LivingWorld::ProfileManager::Instance()

#endif
