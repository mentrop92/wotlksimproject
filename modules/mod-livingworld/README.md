# LivingWorld

LivingWorld is the higher-level simulation module for the private WotLK AI server project. It sits beside `mod-playerbots` and adds persistent identity, intelligence differences, preferences, schedules, relationships, memories, long-term goals, population administration, and world-history generation without replacing Playerbots combat or gameplay logic.

## Current status

Phase 0 is merged and the current Phase 1 branch adds:

- deterministic multidimensional profiles
- character-database persistence and in-memory caching
- actual Playerbot detection after delayed AI registration
- human-character exclusion by default
- deterministic generator and population-distribution tests
- administrator runtime settings that survive restarts
- pause/resume and online-population policy commands

The dedicated Ubuntu 24.04 combined build compiles AzerothCore, `mod-playerbots`, and `mod-livingworld` together through GitHub Actions.

## Architecture

```text
AzerothCore Playerbot fork
├── mod-playerbots
│   └── movement, combat, questing, grouping, dungeons, raids, battlegrounds
└── mod-livingworld
    └── identity, schedules, goals, memories, relationships, economy, organizations
```

LivingWorld uses AzerothCore script hooks and stable Playerbots interfaces. Direct edits to Playerbots or core code require a demonstrated integration need.

## Configuration

Copy `conf/mod_livingworld.conf.dist` through the normal AzerothCore module configuration process.

Important defaults:

- `LivingWorld.Enable = 1`
- `LivingWorld.RuntimeEnabled = 1`
- `LivingWorld.LoginEnabled = 1`
- `LivingWorld.ProfileHumans = 0`
- population minimum/target/maximum: `50/250/500`
- population login/logout rates: `15/20` per minute

Persisted values in `lw_world_settings` override runtime defaults after the initial settings row is created. Human-controlled characters are not profiled by default.

## Administrator commands

```text
.livingworld status
.livingworld enable
.livingworld disable
.livingworld pause soft
.livingworld pause hard
.livingworld resume
.livingworld population
.livingworld population min <number>
.livingworld population target <number>
.livingworld population max <number>
.livingworld population auto on|off
.livingworld profile
```

The current commands persist and enforce the control policy. Automatic rate-limited bot login/logout begins in the scheduling and population-controller phase.

## Documentation

- `docs/livingworld/AI_ARCHITECTURE.md`
- `docs/livingworld/BOT_PROFILE_SCHEMA.md`
- `docs/livingworld/ADMIN_CONTROLS.md`
- `docs/livingworld/WORLD_SEEDING.md`
- `docs/livingworld/ROADMAP.md`

## Next implementation slice

1. Finish the green Phase 1 persistence PR.
2. Validate database load/save behavior in a running server environment.
3. Build the online population controller using the persisted minimum, target, maximum, and rate limits.
4. Translate lifestyle profiles into weekly login schedules.
5. Implement the fresh-realm seeding preset before developed-world presets.
