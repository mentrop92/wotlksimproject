# LivingWorld

LivingWorld is the higher-level simulation module for the private WotLK AI server project. It sits beside `mod-playerbots` and adds persistent identity, intelligence differences, preferences, schedules, relationships, memories, and long-term goals without replacing Playerbots combat or gameplay logic.

## Current status

The `agent/livingworld-phase-0-1` branch contains the Phase 0 foundation and the first Phase 1 identity components:

- module configuration and startup bootstrap
- deterministic profile generation
- separate personality, intelligence, lifestyle, and preference dimensions
- playtime archetypes ranging from dormant players to rare 24/7 grinders
- character-database tables for profiles, schedules, memories, and relationships
- architecture, schema, and roadmap documentation
- GitHub Actions compilation with the `mentrop92/mod-playerbots` fork

Runtime Playerbot detection and profile persistence are intentionally the next step after the first green combined build.

## Architecture

```text
AzerothCore Playerbot fork
├── mod-playerbots
│   └── movement, combat, questing, grouping, dungeons, raids, battlegrounds
└── mod-livingworld
    └── identity, schedules, goals, memories, relationships, economy, organizations
```

LivingWorld should use AzerothCore script hooks and stable Playerbots interfaces. Direct edits to Playerbots or core code require a demonstrated integration need.

## Configuration

Copy `conf/mod_livingworld.conf.dist` through the normal AzerothCore module configuration process.

Important defaults:

- `LivingWorld.Enable = 1`
- `LivingWorld.ProfileHumans = 0`
- `LivingWorld.ProfileCheckIntervalMs = 5000`
- `LivingWorld.MemoryLimit = 50`

Human-controlled characters are not profiled by default.

## Documentation

- `docs/livingworld/AI_ARCHITECTURE.md`
- `docs/livingworld/BOT_PROFILE_SCHEMA.md`
- `docs/livingworld/ROADMAP.md`

## Next implementation slice

1. Make the combined GitHub Actions build green.
2. Add deterministic generator fixture tests.
3. Add profile serialization and an in-memory cache.
4. Detect eligible Playerbots after AI registration.
5. Create and persist a profile exactly once.
6. Add `.livingworld profile <name>` for inspection.
