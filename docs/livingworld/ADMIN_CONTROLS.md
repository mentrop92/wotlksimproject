# LivingWorld Administrator Controls

## Purpose

LivingWorld must be operable by a server administrator without editing source code or manually changing database rows. Runtime settings are stored in the singleton `lw_world_settings` row and survive worldserver restarts.

The static `LivingWorld.Enable` configuration option controls whether the module starts at all. Runtime commands control the simulated world after startup.

## Authorization model

LivingWorld uses dedicated RBAC permissions rather than the generic GM command permission:

| Permission | ID | Default assignment |
|---|---:|---|
| View status | 19000 | GameMaster and Administrator |
| Inspect profiles | 19001 | GameMaster and Administrator |
| Runtime control | 19002 | Administrator only |
| Population control | 19003 | Administrator only |
| Destructive world seeding | 19004 | No default assignment; explicit grant required |

The IDs are installed by `data/sql/db-auth/base/livingworld_rbac.sql`. The installer is idempotent and does not delete unrelated permissions or links.

## Current commands

### Status

```text
.livingworld status
```

Reports simulation state, login state, population policy, compiled safety ceilings, auto-scale state, and loaded profile count.

### Enable and resume

```text
.livingworld enable
.livingworld resume
```

Both enable high-level simulation and allow new LivingWorld AI logins. These require the runtime-control permission.

### Disable

```text
.livingworld disable
```

Disables high-level simulation and new LivingWorld AI logins. It does not yet forcibly disconnect existing Playerbots.

### Soft pause

```text
.livingworld pause soft
```

Keeps simulation state available but prevents new LivingWorld AI profile provisioning and future scheduled logins.

### Hard pause

```text
.livingworld pause hard
```

Freezes high-level LivingWorld activity and blocks new AI logins. Forced, rate-limited logout will be implemented with population execution.

### Population policy

```text
.livingworld population
.livingworld population min 75
.livingworld population target 250
.livingworld population max 500
.livingworld population tolerance 15
.livingworld population auto on
.livingworld population auto off
```

The following invariant is always enforced:

```text
minimum <= target <= maximum
```

Population mutations require the population-control permission.

## Compiled safety ceilings

Configuration, persisted database values, and commands are all bounded by compiled limits:

```text
maximum AI population: 10,000
maximum AI login rate: 100 per minute
maximum AI logout rate: 200 per minute
```

Unsafe persisted or configuration values are clamped before the controller can use them. Commands reject values outside the limits. These ceilings remain active even if a configuration file or database row is edited directly.

## Persisted settings

`lw_world_settings` stores:

- `simulation_enabled`
- `login_enabled`
- `minimum_online`
- `maximum_online`
- `target_online`
- `population_tolerance`
- `auto_scale`
- `login_rate_per_minute`
- `logout_rate_per_minute`
- `population_mode`
- `seed_state`

Configuration values provide initial defaults only when no persisted settings row exists.

## Population-controller rules

The runtime controller must:

1. Move gradually toward the target instead of logging hundreds of bots in simultaneously.
2. Respect persisted and compiled bounds.
3. Use hysteresis so small fluctuations do not cause login/logout churn.
4. Reserve capacity for human players.
5. Reduce AI population when server load crosses safeguards.
6. Prefer natural session endings during soft reductions.
7. Rate-limit forced logouts during hard reductions.

## Safety rules

- Runtime pause and population-limit changes are reversible and do not delete profiles.
- World seeding, replacement, and wiping are separate destructive operations.
- No destructive command may execute without the dedicated permission and a short-lived confirmation token.
- Administrative commands must report limitations honestly when behavior belongs to a later phase.
- The LLM or future voice gateway will never receive administrative RBAC authority or unrestricted database access.
