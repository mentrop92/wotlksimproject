# LivingWorld

LivingWorld is the higher-level simulation module for the WotLK AI server project.
It sits beside `mod-playerbots` and adds persistent identity, preferences, schedules,
relationships, and memories without replacing Playerbots combat or gameplay logic.

## Phase 0

- Keep custom simulation code isolated from upstream AzerothCore and Playerbots.
- Compile the custom module and Playerbots together in GitHub Actions.
- Document ownership boundaries and the development