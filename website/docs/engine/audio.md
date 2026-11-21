# Audio playback

<p class="lead">Choose an audio player, assign a stream, and route it through the appropriate bus.</p>

## Choose a player

| Player | Behavior |
| --- | --- |
| [AudioStreamPlayer](../api/engine/AudioStreamPlayer.md) | Non-positional audio, such as UI sounds and music. |
| [AudioStreamPlayer2D](../api/engine/AudioStreamPlayer2D.md) | Positional audio in a 2D scene. |
| [AudioStreamPlayer3D](../api/engine/AudioStreamPlayer3D.md) | Positional audio with 3D distance and direction. |

Import an audio file, assign it to the player's `Stream`, and select a bus. A player without a stream has no audio to play.

## Start playback

```luau
class Music extends Node = {
	onready private Player = self:getNode("Player") :: AudioStreamPlayer

	function _ready()
		self.Player:play()
	end
}
```

Add an `AudioStreamPlayer` child named `Player` and assign a music stream in the Inspector. Calling `play` starts playback at the requested position, or at the beginning if no argument is supplied. `stop` stops it. Configure looping through the stream's import or resource settings where the format supports it.

## Buses and volume

The audio bus layout routes streams through effects and gain stages. Route music and effects separately if users need independent volume controls. Bus names are strings and must match the bus layout.

Decibels are logarithmic. A change in `VolumeDb` is not the same as multiplying a linear amplitude by that number. Use the property's documented units when saving and restoring volume settings.

## Playback lifetime

A positional player's location follows its node transform. Freeing the node stops its playback. A short sound attached to an object that is immediately freed may be cut off; let an audio-owning node live until playback ends when the sound should finish independently.

Use completion signals for one-shot cleanup. A looping stream does not reach an ordinary end each time through its loop.

## Troubleshooting

Check that the stream is assigned, the bus is not muted, the selected device is valid, and the player is actually playing. For positional audio, also check listener position, distance attenuation, and the scene's active camera or listener.
