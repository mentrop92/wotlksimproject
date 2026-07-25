# LivingWorld AI Voice Architecture

## Goal

Allow a human player to speak naturally with AI guildmates, parties, raids, battleground groups, and nearby characters while preserving each bot's LivingWorld identity, memories, relationships, current goal, and gameplay context.

Voice is a presentation and interaction layer. It must not become a second source of truth for bot identity or game state.

## Recommended architecture

```text
WoW 3.3.5a client
  + lightweight LivingWorld companion client
        |
        | microphone audio + push-to-talk state
        v
Voice gateway
  - authentication and session mapping
  - voice activity detection
  - speech-to-speech or speech-to-text/LLM/text-to-speech
  - interruption and turn management
  - per-channel routing and rate limits
        |
        | structured intent and dialogue events
        v
LivingWorld bridge service
  - maps voice user to authenticated game character
  - reads party, raid, guild, proximity, encounter, and bot context
  - requests dialogue policy from LivingWorld
  - sends only approved game actions to worldserver
        |
        v
AzerothCore + mod-playerbots + mod-livingworld
```

## Preferred transport: Mumble-compatible voice first

The first practical version should use a separate open voice stack rather than modifying the closed WoW 3.3.5a client protocol.

Recommended initial components:

- Mumble client/server for low-latency human voice transport
- a LivingWorld voice gateway that joins channels as AI participants
- a lightweight companion app or Mumble plugin that reports the authenticated character, group, and optional position context
- one logical AI voice participant per actively speaking bot, with pooling behind the gateway

Reasons:

- mature low-latency voice transport already exists
- channel and ACL concepts map naturally to guild, party, raid, officer, and encounter channels
- positional audio can later support nearby-world conversations
- we avoid building codecs, jitter buffers, NAT traversal, echo cancellation, and encryption before validating the game experience

Ventrilo may be supported later as an adapter, but it should not be the primary architecture because the project would depend on a proprietary legacy protocol and server ecosystem.

## In-game voice integration

Direct integration with the original WoW in-game voice interface is a research path, not the initial plan. It may require client patching, protocol reverse engineering, or unsupported hooks. The project must not depend on it until legal and technical feasibility are established.

A companion overlay can provide an in-game-feeling experience without modifying the WoW executable:

- push-to-talk indicator
- current AI speaker and portrait
- subtitles/transcript
- channel selection
- mute and per-bot volume
- consent and recording indicators

## Dialogue context

The gateway should assemble a bounded context package for each response:

```text
speaker identity and voice
human character identity
channel type and membership
nearby or grouped AI characters
current map, zone, instance, encounter, and objective
recent relevant dialogue
LivingWorld profile, memories, relationships, and current goals
allowed game actions for this conversation
```

The LLM must never receive database credentials, raw server configuration, administrator tokens, or unrestricted SQL access.

## Action boundary

Spoken dialogue may produce structured requests such as:

```json
{
  "intent": "invite_to_group",
  "actor_guid": 123,
  "target_guid": 456,
  "confidence": 0.93
}
```

The worldserver independently validates every request against game rules, permissions, cooldowns, group state, and LivingWorld policy. The LLM cannot execute arbitrary commands or directly mutate the database.

## Raid voice behavior

Raid voice should be hierarchical to prevent noise and cost explosions:

- raid leader and officers have priority
- role leaders may speak for tank, healer, melee, and ranged groups
- ordinary bots speak only for meaningful callouts, questions, social moments, or explicit conversation
- encounter callouts use deterministic game-state logic first, with the LLM adding natural phrasing only
- simultaneous speech is limited and interruptible

## Security and privacy requirements

- explicit push-to-talk or clearly visible voice activation
- no background recording when voice is disabled
- short retention by default; transcripts and audio retention are separate settings
- per-user mute, block, and opt-out controls
- authenticated mapping between voice identity and game account/character
- signed, short-lived gateway credentials
- strict rate limits and budget limits per user, channel, and server
- prompt-injection-resistant separation between player speech, system policy, and trusted game state
- allowlisted structured game actions only
- no voice cloning of a real person without documented authorization
- audit logs for AI-issued structured requests without logging unnecessary raw audio

## Development phases

### Voice Phase 0 — simulation and text bridge

- define dialogue event schemas
- expose read-only LivingWorld context through an authenticated local service
- generate text responses for guild, party, raid, and proximity scenarios
- validate personality consistency and action boundaries

### Voice Phase 1 — companion push-to-talk

- standalone desktop companion
- microphone input and streamed AI output
- one guild or party channel
- subtitles and speaker identity
- no direct game actions

### Voice Phase 2 — Mumble integration

- AI gateway connects to Mumble channels
- authenticated channel mapping
- multiple AI voices with speaker arbitration
- optional positional context

### Voice Phase 3 — controlled game actions

- allowlisted structured intents
- worldserver validation and audit logging
- party invites, ready checks, role assignments, simple strategy questions, and social actions

### Voice Phase 4 — raids and living social world

- raid hierarchy and encounter-aware callouts
- guild meetings and officer channels
- persistent conversational memories
- proximity conversations and social events

### Voice Phase 5 — investigate native-client integration

- assess technical and legal feasibility
- prototype only if it can be done without distributing proprietary client assets or unsafe patches

## Decision

Build the dialogue and authorization layer independently of any one voice transport. Use Mumble-compatible transport for the first real voice prototype, while keeping adapters possible for Ventrilo, WebRTC, Discord-like clients, or future native integration.
