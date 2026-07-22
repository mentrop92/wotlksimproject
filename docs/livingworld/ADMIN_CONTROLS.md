# LivingWorld Administrator Controls

## Purpose

LivingWorld must be operable by a server administrator without editing source code or manually changing database rows. Runtime settings are stored in the singleton `lw_world_settings` row and survive worldserver restarts.

The static `LivingWorld.Enable` configuration option controls whether the module starts at all. Runtime commands control the simulated world after startup.

## Current commands

### Status

```text
.livingworld status
```

Reports:

- whether high-level LivingWorld simulation is enabled
- whether new LivingWorld AI logins/profile provisioning are enabled
- minimum, target, and maximum online AI population
- login and logout rate limits
- auto-scale state
- number of profiles currently loaded in memory

### Enable and resume

```text
.livingworld enable
.livingworld resume
```

Both enable high-level simulation and allow new LivingWorld AI logins. `resume` is the normal counterpart to a pause; `enable` is the explicit master runtime action.

### Disable

```text
.livingworld disable
```

Disables high-level simulation and new LivingWorld AI logins. In the current foundation, it does not forcibly disconnect existing Playerbots.

### Soft pause

```text
.livingworld pause soft
```

Keeps the simulation state available but prevents new LivingWorld AI profile provisioning and future scheduled logins. Existing bots are allowed to remain online.

### Hard pause

```text
.livingworld pause hard
```

Freezes high-level LivingWorld activity and blocks new AI logins. Forced, rate-limited logout of existing Playerbots will be implemented with the population controller rather than hidden inside the foundation command.

### Population policy

```text
.livingworld population
.livingworld population min 75
.livingworld population target 250
.livingworld population max 500
.livingworld population auto on
.livingworld population auto off
```

The following invariant is always enforced:

```text
minimum <= target <= maximum
```

These commands currently persist the policy contract. The scheduling phase will consume it to gradually log bots in and out.

## Persisted settings

`lw_world_settings` stores:

- `simulation_enabled`
- `login_enabled`
- `minimum_online`
- `maximum_online`
- `target_online`
- `auto_scale`
- `login_rate_per_minute`
- `logout_rate_per_minute`
- `population_mode`
- `seed_state`

Configuration values provide initial defaults only when no persisted settings row exists.

## Population-controller rules

The runtime controller must:

1. Move gradually toward the target instead of logging hundreds of bots in simultaneously.
2. Respect minimum and maximum bounds.
3. Use hysteresis so small fluctuations do not cause constant login/logout churn.
4. Reserve capacity for human players.
5. Reduce AI population when server load crosses configured safeguards.
6. Prefer natural session endings during soft reductions.
7. Rate-limit forced logouts during hard reductions.

## Safety rules

- Runtime pause and population-limit changes are reversible and do not delete profiles.
- World seeding, replacement, and wiping are separate destructive operations.
- No destructive command may execute without a short-lived confirmation token.
- Administrative commands must report limitations honestly when the requested behavior belongs to a later phase.
