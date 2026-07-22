# Bot Profile Schema

## Design rule

A bot profile describes stable initial identity. Dynamic state such as current fatigue, recent frustration, active guild obligations, and temporary interests will be stored separately in later migrations.

All generated scores use an inclusive 1–100 scale unless stated otherwise.

## Identity

| Field | Purpose |
|---|---|
| `character_guid` | Permanent link to the AzerothCore character. |
| `seed` | Stable 64-bit deterministic generation seed. |
| `schema_version` | Identifies the profile-generator and storage contract. |
| `primary_goal` | Initial broad long-term goal category. |
| `activity_archetype` | Broad playtime pattern used to generate lifestyle values. |

## Personality

| Field | Low score | High score |
|---|---|---|
| `ambition` | Accepts slow progress | Pursues advancement aggressively |
| `sociability` | Prefers solo activity | Seeks groups and conversation |
| `competitiveness` | Ignores status | Chases rank, gear, and wins |
| `curiosity` | Repeats familiar content | Explores and experiments |
| `loyalty` | Changes allies readily | Maintains long-term commitments |
| `patience` | Abandons difficult goals | Repeats and persists |
| `risk_tolerance` | Avoids uncertainty | Attempts dangerous content |

## Intelligence

Intelligence is multidimensional. A bot may be tactically strong and economically weak, or socially gifted but inattentive.

| Field | Affects |
|---|---|
| `tactical_intelligence` | Combat reactions, positioning, target choice, and cooldown use |
| `strategic_intelligence` | Route selection, progression planning, gear priorities, and preparation |
| `social_intelligence` | Group composition, recruiting, cooperation, and conflict management |
| `economic_intelligence` | Pricing, budgeting, crafting choices, and purchase quality |
| `game_knowledge` | Mechanics, locations, quest chains, travel, and content familiarity |
| `adaptability` | Learning from deaths, wipes, and failed plans |
| `attention` | Missed mechanics, tunnel vision, and execution consistency |

## Lifestyle

| Field | Meaning |
|---|---|
| `target_weekly_hours` | Intended online time, from near-dormant to 168 hours |
| `minimum_session_minutes` | Typical lower session bound |
| `maximum_session_minutes` | Typical upper session bound |
| `schedule_consistency` | Reliability of normal login windows |
| `skip_session_chance` | Probability of missing a planned session |
| `marathon_session_chance` | Probability of extending a session unusually long |
| `burnout_sensitivity` | Susceptibility to later motivation loss and extended breaks |

### Initial activity distribution

The generator intentionally creates a broad population rather than equal archetype counts:

- approximately 2% no-life grinders at 140–168 target hours per week
- approximately 7% hardcore players at 65–110 hours
- approximately 13% dedicated players at 38–65 hours
- approximately 36% regular players at 16–38 hours
- approximately 18% weekend-oriented players at 8–20 hours
- approximately 19% casual players at 3–12 hours
- approximately 5% nearly dormant players at 1–5 hours

These are generation targets, not guaranteed realized playtime. Fatigue, burnout, social activity, PvP, professions, travel, deaths, and inefficiency will affect actual leveling speed.

## Activity preferences

The profile independently scores:

- leveling
- questing
- dungeons
- raiding
- PvP
- professions
- collecting
- socializing

High weekly playtime does not automatically mean fast leveling. A highly active bot may spend most of its time in battlegrounds, trading, crafting, collecting, helping friends, or idling socially.

## Related tables

### `lw_schedules`

Stores generated play windows and the next planned login/logout times.

### `lw_memories`

Stores meaningful events with importance, emotional valence, subjects, payload, decay, and protection status.

### `lw_relationships`

Stores directional affinity, trust, familiarity, rivalry, interaction counts, and recency.

## Versioning

Profiles are persisted after generation. Changing generation rules does not silently rewrite existing characters. Any incompatible generator change requires:

1. incrementing `PROFILE_SCHEMA_VERSION`;
2. defining whether old profiles remain valid;
3. writing an explicit migration when needed;
4. preserving reproducibility for recorded simulation runs.
