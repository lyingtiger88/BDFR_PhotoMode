# Architecture

## Runtime ownership

`UBDFRPhotoModeSubsystem` is a `ULocalPlayerSubsystem`. It owns the local Photo Mode session and keeps the implementation independent of a specific GameMode, Character class, or PlayerController subclass.

## Main runtime pieces

### UBDFRPhotoModeSubsystem
Session orchestrator.

Responsibilities:
- Enter/exit Photo Mode
- Preserve gameplay pawn/HUD/pause state
- Own the photo subject reference
- Switch camera modes
- Execute pose commands
- Resolve data-driven pose presets
- Restore subject state on exit

### ABDFRPhotoModePawn
Transient photo camera pawn.

Current modes:
- `Free`
- `Selfie`
- `Orbit` reserved for the next camera layer

Selfie mode tracks an anchor on the subject (normally the `head` socket/bone) and computes a camera transform in front of the character.

### UBDFRPhotoPosePreset
Primary Data Asset used to expose project-authored pose content to the plugin.

A preset may point to:
- an authored `UAnimMontage`, or
- a raw `UAnimSequenceBase` played as a dynamic montage through an AnimBP slot.

This keeps animation content out of C++ and lets designers extend the pose library without rebuilding the plugin.

### UBDFRPhotoPoseComponent
Small runtime executor responsible for animation playback and cleanup. The subsystem can use a component already present on the subject or create a transient component automatically.

## Intended UI architecture

The runtime module intentionally does not hard-code a UMG layout. A project UI can query the subsystem and build:
- pose categories
- pose cards/thumbnails
- selfie button
- camera controls
- filters and post process controls

A dedicated optional UI module can be added later without coupling the runtime core to a specific visual design.

## Networking policy

Photo Mode is local-player first. Camera state and local UI should remain local by default. Pose replication should be an explicit project choice because multiplayer games differ in authority and animation replication policy.
