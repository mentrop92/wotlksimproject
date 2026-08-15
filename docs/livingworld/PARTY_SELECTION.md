# LivingWorld Party Selection Foundation

## Purpose

This milestone adds a bounded, deterministic, read-only policy for proposing compatible party members from caller-supplied candidate availability and LivingWorld social relationships.

## Behavior

- The initiator must be a nonzero character identifier.
- Candidate input is capped at 64 records.
- Proposed party size is limited to the normal 2-5 member party range.
- Candidates must have unique nonzero identifiers, cannot be the initiator, and must use a known party-role value.
- Unavailable candidates are ignored.
- A candidate must have a directed social edge from the initiator and satisfy configured affinity, trust, and familiarity thresholds.
- Ranking is deterministic: combined relationship score first, then trust, familiarity, lower rivalry, and finally lower character identifier.
- The result contains only detached identifiers. It does not create or modify an in-game party.

## Deliberate non-goals

This foundation does not call Playerbot APIs, create WoW groups, invite characters, force logins or logouts, write to a database, alter goals, seed the world, change RBAC, or implement voice infrastructure. Recurring-group identity, role composition rules, scheduling, invitations, acceptance behavior, and actual group execution remain later milestones with separate validation boundaries.
