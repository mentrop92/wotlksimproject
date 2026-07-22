# LivingWorld Roadmap

## Phase 0 — Foundation

**Goal:** establish an isolated, buildable module with repeatable CI.

- [x] Create `mod-livingworld` module boundary.
- [x] Add module configuration.
- [x] Add startup bootstrap and logging.
- [x] Document architecture and repository ownership.
- [x] Add the first character-database schema.
- [x] Compile AzerothCore, `mod-playerbots`, and `mod-livingworld` together in GitHub Actions.
- [x] Resolve compile and SQL-layout failures.
- [x] Merge the green foundation PR.

## Phase 1 — Persistent Identity and Administration

**Goal:** every eligible Playerbot has one stable, inspectable identity and administrators can safely control runtime policy.

### 1A — Model and generation

- [x] Define personality dimensions.
- [x] Define intelligence dimensions.
- [x] Define lifestyle and playtime archetypes.
- [x] Define activity preferences and primary goals.
- [x] Implement deterministic generation.
- [x] Add deterministic fixture and population-distribution tests.

### 1B — Persistence

- [x] Define `lw_profiles`, `lw_schedules`, `lw_memories`, and `lw_relationships` tables.
- [x] Implement profile serialization and deserialization.
- [x] Add an in-memory profile cache keyed by character GUID.
- [x] Persist a generated profile exactly once.
- [x] Load the same stored profile after relogging or server restart.
- [x] Update `last_seen_at` on logout.
- [ ] Validate persistence behavior in a running worldserver and database.

### 1C — Playerbot integration

- [x] Detect Playerbots without profiling human players by default.
- [x] Handle delayed Playerbots AI registration after login.
- [x] Remove cached profiles cleanly when bots log out.
- [x] Add `.livingworld profile` for GM inspection.
- [x] Add `.livingworld population` for current policy and loaded-profile summaries.

### 1D — Administrator foundation

- [x] Add persisted `lw_world_settings`.
- [x] Add status, enable, disable, soft-pause, hard-pause, and resume commands.
- [x] Add persisted minimum, target, and maximum online population settings.
- [x] Add persisted auto-scale and login/logout rate settings.
- [x] Make new profile provisioning obey runtime pause state.
- [ ] Validate commands in a running server environment.

### Phase 1 exit criteria

- A Playerbot receives a profile on its first eligible login.
- The profile is not regenerated on subsequent logins.
- Intelligence, lifestyle, personality, and preferences are visible through an admin command.
- A population includes rare 24/7 grinders, casual players, weekend players, and varied intelligence specialists.
- Human characters remain unprofiled unless explicitly enabled.
- Runtime settings survive a worldserver restart.

## Phase 2 — Scheduling and Population Controller

**Goal:** translate identity and administrator policy into safe, realistic online population behavior.

- Translate lifestyle traits into weekly login windows.
- Schedule realistic session lengths, skipped sessions, and marathon sessions.
- Add motivation, fatigue, frustration, and burnout.
- Gradually move the online AI count toward the configured target.
- Respect minimum, target, and maximum population bounds.
- Enforce login/logout rate limits and hysteresis.
- Reserve online capacity for human players.
- Reduce AI population when server-load safeguards are crossed.
- Allow soft pause to drain naturally.
- Allow hard pause to perform rate-limited forced Playerbot logout.
- Add time-of-day and weekday population curves.
- Add accelerated simulation commands for days and months.

## Phase 3 — World Seeding and Synthetic History

**Goal:** initialize either a fresh realm or a coherent developed world.

- Add fresh, young, established, mature, and custom presets.
- Implement short-lived confirmation tokens for destructive operations.
- Support add, replace, and LivingWorld-only wipe operations.
- Create account-like main/alt identity groups.
- Generate coherent levels, talents, skills, reputations, equipment, professions, and gold.
- Generate guilds, raid teams, arena teams, friendships, and rivalries.
- Generate summarized histories and accomplishments.
- Validate distributions and referential integrity before committing.
- Commit seeded worlds atomically or roll back on failure.

## Phase 4 — Memory and Social Graph

- Record meaningful events rather than every action.
- Add memory importance, valence, decay, and protected memories.
- Update affinity, trust, familiarity, and rivalry from shared events.
- Allow relationships to affect invitations, assistance, trade, and guild choices.

## Phase 5 — Goals and Organizations

- Add persistent short-, medium-, and long-term goals.
- Form autonomous parties and recurring groups.
- Create, join, leave, and lead guilds.
- Schedule dungeon, raid, battleground, and arena teams.
- Add guild loyalty, conflict, recruitment, and succession.

## Phase 6 — Economy and Professions

- Give bots economic strategies and budgets.
- Model profession specialization and supply chains.
- Connect economic intelligence to auction and crafting decisions.
- Add personal and guild financial goals.

## Phase 7 — Living World Simulation

- Run population progression across months and expansion phases.
- Produce server-history summaries and notable-character reports.
- Balance population distributions without erasing individuality.
- Add reproducible simulation seeds and scenario test suites.
