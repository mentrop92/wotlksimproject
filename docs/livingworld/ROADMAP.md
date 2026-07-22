# LivingWorld Roadmap

## Phase 0 — Foundation

**Goal:** establish an isolated, buildable module with repeatable CI.

- [x] Create `mod-livingworld` module boundary.
- [x] Add module configuration.
- [x] Add startup bootstrap and logging.
- [x] Document architecture and repository ownership.
- [x] Add the first character-database schema.
- [ ] Compile AzerothCore, `mod-playerbots`, and `mod-livingworld` together in GitHub Actions.
- [ ] Resolve all compile and SQL validation failures.
- [ ] Merge the green foundation PR.

### Phase 0 exit criteria

- The module is discovered and linked by AzerothCore.
- Ubuntu 24.04 CI completes successfully.
- The startup log reports that LivingWorld is enabled and the deterministic generator is ready.
- No Playerbots source file is modified.

## Phase 1 — Persistent Identity

**Goal:** every eligible Playerbot has one stable, inspectable identity.

### 1A — Model and generation

- [x] Define personality dimensions.
- [x] Define intelligence dimensions.
- [x] Define lifestyle and playtime archetypes.
- [x] Define activity preferences and primary goals.
- [x] Implement deterministic generation.
- [ ] Add deterministic fixture tests and population-distribution tests.

### 1B — Persistence

- [x] Define `lw_profiles`, `lw_schedules`, `lw_memories`, and `lw_relationships` tables.
- [ ] Implement profile serialization and deserialization.
- [ ] Add an in-memory profile cache keyed by character GUID.
- [ ] Persist a generated profile exactly once.
- [ ] Load the same stored profile after relogging or server restart.
- [ ] Update `last_seen_at` on logout.

### 1C — Playerbot integration

- [ ] Detect Playerbots without profiling human players by default.
- [ ] Handle delayed Playerbots AI registration after login.
- [ ] Remove cached profiles cleanly when bots log out.
- [ ] Add `.livingworld profile <name>` for GM inspection.
- [ ] Add `.livingworld population` for profile-distribution summaries.

### Phase 1 exit criteria

- A Playerbot receives a profile on its first eligible login.
- The profile is not regenerated on subsequent logins.
- Intelligence, lifestyle, personality, and preferences are visible through an admin command.
- A population includes rare 24/7 grinders, casual players, weekend players, and varied intelligence specialists.
- Human characters remain unprofiled unless explicitly enabled.

## Phase 2 — Scheduling and Dynamic State

- Translate lifestyle traits into weekly login windows.
- Schedule realistic session lengths and skipped sessions.
- Add motivation, fatigue, frustration, and burnout.
- Add accelerated simulation commands for days and months.
- Make online population vary by hour and weekday.

## Phase 3 — Memory and Social Graph

- Record meaningful events rather than every action.
- Add memory importance, valence, decay, and protected memories.
- Update affinity, trust, familiarity, and rivalry from shared events.
- Allow relationships to affect invitations, assistance, trade, and guild choices.

## Phase 4 — Goals and Organizations

- Add persistent short-, medium-, and long-term goals.
- Form autonomous parties and recurring groups.
- Create, join, leave, and lead guilds.
- Schedule dungeon, raid, battleground, and arena teams.
- Add guild loyalty, conflict, recruitment, and succession.

## Phase 5 — Economy and Professions

- Give bots economic strategies and budgets.
- Model profession specialization and supply chains.
- Connect economic intelligence to auction and crafting decisions.
- Add personal and guild financial goals.

## Phase 6 — Living World Simulation

- Run population progression across months and expansion phases.
- Produce server-history summaries and notable-character reports.
- Balance population distributions without erasing individuality.
- Add reproducible simulation seeds and scenario test suites.
