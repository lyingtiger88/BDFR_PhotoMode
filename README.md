# BDFR_PhotoMode

Runtime Photo Mode plugin foundation for Unreal Engine 5.

**Current version: 0.2.0 (development)**

The plugin is deliberately gameplay-framework agnostic: the core is a `ULocalPlayerSubsystem`, the photo camera is a transient pawn, pose behavior is data-driven, and the active photo subject can be replaced at runtime.

## Implemented

### Core photo mode
- Enter / Exit / Toggle Photo Mode
- Saves and restores the gameplay pawn
- Collision-free free camera spawned at the current player view
- Optional world pause
- Optional HUD hide / restore
- Runtime FOV control
- Screenshot request API
- Blueprint helper library
- Project Settings integration

### Free camera controls
| Control | Action |
|---|---|
| RMB + Mouse | Free look |
| W/A/S/D | Move |
| Q / E | Down / Up |
| Shift | Fast movement |
| Mouse Wheel | Change free-camera movement speed |

### Pose system
- `UBDFRPhotoPosePreset` data assets
- Pose selection by preset or by `PoseId`
- Supports authored `UAnimMontage`
- Supports `UAnimSequenceBase` through an AnimBP slot / dynamic montage
- Loopable photo poses
- Optional real-time pose animation while the game world remains paused
- Per-preset start time for freezing on a specific authored animation frame
- Blend in / blend out
- Optional automatic look-at-camera behavior
- Runtime pose component is automatically attached when the subject does not already own one
- Pose playback is configured to keep the skeletal mesh ticking while the world is paused

### Selfie mode
- Dedicated `Selfie` camera mode
- Defaults to the original player pawn as the photo subject
- Configurable head/socket anchor
- Configurable distance, horizontal offset, vertical offset and FOV
- Smooth subject tracking
- RMB + Mouse changes selfie orbit
- Mouse Wheel changes selfie distance
- Optional automatic subject look-at-camera
- Subject rotation can be restored when leaving Photo Mode

## Installation
1. Copy `BDFR_PhotoMode` into `<Project>/Plugins/`.
2. Regenerate project files for a C++ project.
3. Build the project.
4. Enable **BDFR Photo Mode** if needed.
5. Open `Project Settings > Plugins > BDFR Photo Mode`.

## Minimal Blueprint hookup
Bind any project input (for example `P`) to:

`Toggle Photo Mode`

Then build a UI from the subsystem API:
- `Enter Selfie Mode`
- `Exit Selfie Mode`
- `Get Available Pose Ids`
- `Apply Pose By Id`
- `Clear Pose`
- `Set Photo Subject`

## Creating pose presets
1. In the Content Browser create a Data Asset of class **BDFRPhotoPosePreset**.
2. Give it a unique `PoseId`, e.g. `Selfie_Peace`.
3. Assign either a Montage or an Animation Sequence.
4. For raw Animation Sequences, ensure the character AnimBP contains the configured slot (default: `DefaultSlot`).
5. Add the pose asset to `Project Settings > Plugins > BDFR Photo Mode > Pose Presets`.
6. Call `ApplyPoseById("Selfie_Peace")` from UI or Blueprint.

> For Montage assets, looping is expected to be authored in the Montage itself when continuous playback is required.

## Architecture
See [`Docs/ARCHITECTURE.md`](Docs/ARCHITECTURE.md).

## Roadmap
See [`Docs/ROADMAP.md`](Docs/ROADMAP.md).

## Compatibility
The source intentionally uses standard Unreal Engine 5 runtime APIs and avoids project-specific character classes. Compile and validate against the exact Unreal Engine branch used by the shipping project before release.
