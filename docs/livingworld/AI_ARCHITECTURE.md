# LivingWorld AI Architecture

## Purpose

LivingWorld turns AzerothCore Playerbots into persistent simulated MMO citizens. It does not replace the existing movement, combat, questing, dungeon, raid, or battleground AI. Instead, it supplies the identity and long-term decision layer that determines when a bot plays, what it values, who it associates with, and which goals it pursues.

## Repository boundaries

### AzerothCore Playerbot fork (`wotlksimproject`)

Owns the server engine, scripting hooks, database layer, maps, networking, and the embedded `mod-livingworld` module while the architecture is being established.

### `mod-playerbots`

Owns moment-to-moment gameplay execution:

- navigation and travel
- combat rotations and reactions
- quest interaction
- group mechanics
- dungeon and raid execution
- battleground behavior
- equipment and consumable use

LivingWorld should consume stable Playerbots entry points instead of copying or rewriting those systems.

### `mod-livingworld`

Owns the persistent simulation layer:

- initial identity generation
- personality and intelligence dimensions
- play schedules and session duration
- activity preferences
- memories and relationships
- long-term goals
- guild and organization behavior
- economy and profession strategy
- population-level simulation and reporting

## Decision flow

```text
Character becomes eligible as a Playerbot
        |
        v
Load an existing LivingWorld profile
        |
        +-- no profile --> deterministically generate and persist one
        |
        v
Update dynamic state from memories, relationships, fatigue, and goals
        |
        v
Select a high-level activity and time budget
        |
        v
Translate the decision into Playerbots-compatible actions or strategies
        |
        v
Observe results and record memories, relationships, and progress
```

## Profile layers

A profile is deliberately multidimensional. One generic intelligence number would produce shallow, repetitive bots.

1. **Personality** — ambition, sociability, competitiveness, curiosity, loyalty, patience, and risk tolerance.
2. **Intelligence** — tactical, strategic, social, economic, game knowledge, adaptability, and attention.
3. **Lifestyle** — weekly hours, session lengths, consistency, skipped sessions, marathon sessions, and burnout sensitivity.
4. **Preferences** — leveling, questing, dungeons, raids, PvP, professions, collecting, and socializing.
5. **Dynamic state** — motivation, fatigue, frustration, current goals, recent success, guild obligations, and available companions.

## Deterministic generation

Each character receives a stable 64-bit seed derived from its character GUID, race, class, and a fixed generator-version salt. The seed is used only to create the initial profile. The generated profile is then persisted.

Determinism provides:

- a stable identity across restarts
- reproducible bug reports and simulations
- controlled schema migrations
- population generation without uniform archetypes

Deterministic does not mean identical. Different seeds generate different combinations, including rare 140–168-hour-per-week grinders, casual players, specialists, social leaders, poor tacticians, strong economists, and inconsistent weekend players.

## Integration policy

- Prefer AzerothCore script hooks over core patches.
- Prefer public Playerbots managers and strategy interfaces over internal modifications.
- Keep the first implementation synchronous and simple; optimize only after profiling.
- Never allow profile generation to change silently. Generator changes require a schema version and migration plan.
- Keep human-character profiling disabled by default.

## Testing layers

1. **Compile tests** in GitHub Actions.
2. **Deterministic generator tests** using known GUID/race/class fixtures.
3. **Database persistence tests** against temporary character tables.
4. **Headless simulation tests** for schedules, progression, and population distributions.
5. **Occasional client tests** for visible gameplay and social behavior.

A graphical client is useful for validation, but it must not be required for most development or testing.
