# LivingWorld World Seeding

## Goal

A server administrator should be able to start with a brand-new realm or a believable world that already has months of simulated history. Seeding must create internally consistent characters and organizations rather than randomly assigning levels and equipment.

World-seeding commands are not active yet. This document defines the contract that later implementation must follow.

## Presets

### Fresh realm

```text
.livingworld seed fresh 3000
```

- all AI characters begin at level 1
- no established guilds or raid teams
- minimal money and professions
- no raid progression
- only small optional friend or family clusters
- progression begins naturally after initialization

### Young realm

```text
.livingworld seed young 3000
```

Suggested level distribution:

| Level range | Share |
|---|---:|
| 1–19 | 35% |
| 20–39 | 30% |
| 40–59 | 20% |
| 60–69 | 10% |
| 70–80 | 5% |

Includes small guilds, early profession specialization, and a limited number of max-level characters.

### Established realm

```text
.livingworld seed established 5000
```

Suggested population state:

| State | Share |
|---|---:|
| Low-level characters | 25% |
| Mid-level characters | 25% |
| Northrend leveling | 20% |
| Fresh level 80 | 15% |
| Geared level 80 | 10% |
| High-end raiders or PvPers | 5% |

Includes established guilds, profession specialists, raid teams, arena teams, wealth inequality, friendships, rivalries, inactive veterans, and alts.

### Mature realm

```text
.livingworld seed mature 5000
```

Creates a developed ecosystem with:

- progression, social, PvP, and economic guilds
- geared and poorly geared level-80 characters
- long-term officers and guild leaders
- raid and arena histories
- established profession supply chains
- burned-out veterans and returning players
- characters who remain permanently below max level
- account-like identity groups with mains and alts

### Custom scenario

```text
.livingworld seed custom <scenario-name>
```

Scenario files may define:

- total character count
- faction, race, class, and level distributions
- simulated realm age
- guild counts and types
- max-level and raid-ready shares
- economy maturity
- PvP intensity
- profession supply chains
- inactive and returning-player shares

## Consistency requirements

A developed character must have a believable state derived from an estimated history. A max-level raider should not be created with unrelated random equipment and no supporting progression.

Seed generation should derive:

```text
present-day role and identity
        -> estimated play history
        -> level and progression state
        -> talents, skills, reputations, and travel access
        -> equipment and professions
        -> gold and economic history
        -> guild, party, friend, and rival relationships
        -> summarized memories and accomplishments
```

The system does not need to replay every historical quest. It must generate a coherent present state and enough summarized history to support future decisions.

## Operations

### Add population

```text
.livingworld seed add 500
```

Adds characters to an existing world without replacing current profiles, guilds, or history.

### Replace population

```text
.livingworld seed replace established 5000
```

Replaces the simulated population while preserving unrelated server data only when explicitly confirmed.

### Wipe LivingWorld data

```text
.livingworld seed wipe
```

Deletes LivingWorld-owned simulation data. This operation must never delete unrelated AzerothCore or Playerbots records.

## Confirmation protocol

Every destructive operation must return a short-lived confirmation token instead of executing immediately.

Example:

```text
.livingworld seed replace established 5000
```

Response:

```text
This will replace the LivingWorld population.
Run: .livingworld seed confirm 7F29A1
```

Requirements:

- tokens expire quickly
- tokens are tied to the requesting account and exact operation
- a changed command requires a new token
- confirmation attempts are logged
- wipe and replace operations create a database backup checkpoint when supported

## Implementation stages

1. Store scenario metadata and seeding state.
2. Generate identities and account-like main/alt groups.
3. Generate coherent character progression.
4. Generate guilds, teams, relationships, and summarized histories.
5. Validate distributions and referential integrity.
6. Commit the generated world atomically or roll it back.
7. Allow the runtime scheduler to begin logging the population in gradually.
