# LivingWorld Goal Model Foundation

## Purpose

This milestone introduces a bounded, deterministic, transport-independent goal model for future LivingWorld short-, medium-, and long-term planning.

## Current guarantees

- Each goal belongs to a nonzero actor and has a nonzero actor-local goal identifier.
- Goal tags use the same conservative `[A-Za-z0-9_.-]` style contract and are limited to 48 bytes.
- Priority is expressed as an integer basis-point value bounded to 0–10,000.
- Goal horizons are explicitly limited to short-, medium-, and long-term values.
- Goal states are explicitly limited to planned, active, completed, and abandoned.
- State timestamps cannot move backward.
- Terminal goals cannot be restarted.
- The in-memory goal set is capped at 32 records; at capacity, only an existing terminal goal may be evicted to admit another goal.
- Read-only goal snapshots use a versioned deterministic serialization format capped at 4,096 bytes and 32 goals.
- Deserialization validates identifiers, timestamps, priority, horizon, state, tag safety, and actor-local goal uniqueness before returning a detached snapshot.

## Deliberate non-goals

This foundation does **not** claim persistent goals yet. Snapshot decoding does not restore or mutate the live goal model. It performs no database writes, live Playerbot action, organization mutation, seeding, or external voice work. Persistence and gameplay integration require separate milestones with their own validation and rollback boundaries.
