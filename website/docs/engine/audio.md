# Audio playback

<p class="lead">Choose an audio player, assign a stream, and route it through the appropriate bus.</p>

## Choose a player

| Player | Behavior |
| --- | --- |
| [AudioStreamPlayer](../api/engine/AudioStreamPlayer.md) | Non-positional audio, such as UI sounds and music. |
| [AudioStreamPlayer2D](../api/engine/AudioStreamPlayer2D.md) | Positional audio in a 2D scene. |
| [AudioStreamPlayer3D](../api/engine/AudioStreamPlayer3D.md) | Positional audio with 3D distance and direction. |

Import an audio file, assign it to the player's `Stream` property, and pick a bus.

## Start playback

```luau
class Music extends Node = {
	onready private Player = self:getNode("Player") :: AudioStreamPlayer

	function _ready()
		self.Player:play()
	end
}
```

Add an `AudioStreamPlayer` child named `Player` and assign a stream to it in the Inspector.

- `play()` starts from the beginning. `play(seconds)` starts from that position.
- `stop()` stops playback.
- Looping is set in the audio file's import settings, for formats that support it.

## Buses and volume

The audio bus layout sends sound through effects and volume controls. Put music and sound effects on separate buses so players can adjust them separately. Bus names are strings and must match the layout exactly.

`VolumeDb` is in decibels, which are logarithmic: −6 dB is roughly half the amplitude, not 6 less. For a 0–1 volume slider, convert with `20 * math.log10(value)`.

## Playback lifetime

A positional player moves with its node, and freeing the node stops the sound. That cuts off sounds like an explosion on an enemy that's freed right away. Play those sounds from a separate node that frees itself when the sound ends.

The `Finished` signal fires when playback ends. Looping streams never fire it.

## Troubleshooting

If you hear nothing, check that:

- the player has a stream
- the bus isn't muted
- the output device is correct
- the player is actually playing

For positional audio, also check the listener or active camera position and the player's distance settings.
