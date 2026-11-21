---
source: types/engine.api
---

# Native enum groups

This is the complete set of enum groups recorded by the engine manifest. Names here are native reflection names, including class ownership. The [engine enum guide](../luau/enums.md#engine-enums) explains global `Enum` names, collision resolution, and bitfields.

## Side

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SIDE_LEFT` | `0` |
| `SIDE_TOP` | `1` |
| `SIDE_RIGHT` | `2` |
| `SIDE_BOTTOM` | `3` |

## Corner

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CORNER_TOP_LEFT` | `0` |
| `CORNER_TOP_RIGHT` | `1` |
| `CORNER_BOTTOM_RIGHT` | `2` |
| `CORNER_BOTTOM_LEFT` | `3` |

## Orientation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VERTICAL` | `1` |
| `HORIZONTAL` | `0` |

## ClockDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CLOCKWISE` | `0` |
| `COUNTERCLOCKWISE` | `1` |

## HorizontalAlignment

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HORIZONTAL_ALIGNMENT_LEFT` | `0` |
| `HORIZONTAL_ALIGNMENT_CENTER` | `1` |
| `HORIZONTAL_ALIGNMENT_RIGHT` | `2` |
| `HORIZONTAL_ALIGNMENT_FILL` | `3` |

## VerticalAlignment

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VERTICAL_ALIGNMENT_TOP` | `0` |
| `VERTICAL_ALIGNMENT_CENTER` | `1` |
| `VERTICAL_ALIGNMENT_BOTTOM` | `2` |
| `VERTICAL_ALIGNMENT_FILL` | `3` |

## InlineAlignment

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INLINE_ALIGNMENT_TOP_TO` | `0` |
| `INLINE_ALIGNMENT_CENTER_TO` | `1` |
| `INLINE_ALIGNMENT_BASELINE_TO` | `3` |
| `INLINE_ALIGNMENT_BOTTOM_TO` | `2` |
| `INLINE_ALIGNMENT_TO_TOP` | `0` |
| `INLINE_ALIGNMENT_TO_CENTER` | `4` |
| `INLINE_ALIGNMENT_TO_BASELINE` | `8` |
| `INLINE_ALIGNMENT_TO_BOTTOM` | `12` |
| `INLINE_ALIGNMENT_TOP` | `0` |
| `INLINE_ALIGNMENT_CENTER` | `5` |
| `INLINE_ALIGNMENT_BOTTOM` | `14` |
| `INLINE_ALIGNMENT_IMAGE_MASK` | `3` |
| `INLINE_ALIGNMENT_TEXT_MASK` | `12` |

## EulerOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EULER_ORDER_XYZ` | `0` |
| `EULER_ORDER_XZY` | `1` |
| `EULER_ORDER_YXZ` | `2` |
| `EULER_ORDER_YZX` | `3` |
| `EULER_ORDER_ZXY` | `4` |
| `EULER_ORDER_ZYX` | `5` |

## Key

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `KEY_NONE` | `0` |
| `KEY_SPECIAL` | `4194304` |
| `KEY_ESCAPE` | `4194305` |
| `KEY_TAB` | `4194306` |
| `KEY_BACKTAB` | `4194307` |
| `KEY_BACKSPACE` | `4194308` |
| `KEY_ENTER` | `4194309` |
| `KEY_KP_ENTER` | `4194310` |
| `KEY_INSERT` | `4194311` |
| `KEY_DELETE` | `4194312` |
| `KEY_PAUSE` | `4194313` |
| `KEY_PRINT` | `4194314` |
| `KEY_SYSREQ` | `4194315` |
| `KEY_CLEAR` | `4194316` |
| `KEY_HOME` | `4194317` |
| `KEY_END` | `4194318` |
| `KEY_LEFT` | `4194319` |
| `KEY_UP` | `4194320` |
| `KEY_RIGHT` | `4194321` |
| `KEY_DOWN` | `4194322` |
| `KEY_PAGEUP` | `4194323` |
| `KEY_PAGEDOWN` | `4194324` |
| `KEY_SHIFT` | `4194325` |
| `KEY_CTRL` | `4194326` |
| `KEY_META` | `4194327` |
| `KEY_ALT` | `4194328` |
| `KEY_CAPSLOCK` | `4194329` |
| `KEY_NUMLOCK` | `4194330` |
| `KEY_SCROLLLOCK` | `4194331` |
| `KEY_F1` | `4194332` |
| `KEY_F2` | `4194333` |
| `KEY_F3` | `4194334` |
| `KEY_F4` | `4194335` |
| `KEY_F5` | `4194336` |
| `KEY_F6` | `4194337` |
| `KEY_F7` | `4194338` |
| `KEY_F8` | `4194339` |
| `KEY_F9` | `4194340` |
| `KEY_F10` | `4194341` |
| `KEY_F11` | `4194342` |
| `KEY_F12` | `4194343` |
| `KEY_F13` | `4194344` |
| `KEY_F14` | `4194345` |
| `KEY_F15` | `4194346` |
| `KEY_F16` | `4194347` |
| `KEY_F17` | `4194348` |
| `KEY_F18` | `4194349` |
| `KEY_F19` | `4194350` |
| `KEY_F20` | `4194351` |
| `KEY_F21` | `4194352` |
| `KEY_F22` | `4194353` |
| `KEY_F23` | `4194354` |
| `KEY_F24` | `4194355` |
| `KEY_F25` | `4194356` |
| `KEY_F26` | `4194357` |
| `KEY_F27` | `4194358` |
| `KEY_F28` | `4194359` |
| `KEY_F29` | `4194360` |
| `KEY_F30` | `4194361` |
| `KEY_F31` | `4194362` |
| `KEY_F32` | `4194363` |
| `KEY_F33` | `4194364` |
| `KEY_F34` | `4194365` |
| `KEY_F35` | `4194366` |
| `KEY_KP_MULTIPLY` | `4194433` |
| `KEY_KP_DIVIDE` | `4194434` |
| `KEY_KP_SUBTRACT` | `4194435` |
| `KEY_KP_PERIOD` | `4194436` |
| `KEY_KP_ADD` | `4194437` |
| `KEY_KP_0` | `4194438` |
| `KEY_KP_1` | `4194439` |
| `KEY_KP_2` | `4194440` |
| `KEY_KP_3` | `4194441` |
| `KEY_KP_4` | `4194442` |
| `KEY_KP_5` | `4194443` |
| `KEY_KP_6` | `4194444` |
| `KEY_KP_7` | `4194445` |
| `KEY_KP_8` | `4194446` |
| `KEY_KP_9` | `4194447` |
| `KEY_MENU` | `4194370` |
| `KEY_HYPER` | `4194371` |
| `KEY_HELP` | `4194373` |
| `KEY_BACK` | `4194376` |
| `KEY_FORWARD` | `4194377` |
| `KEY_STOP` | `4194378` |
| `KEY_REFRESH` | `4194379` |
| `KEY_VOLUMEDOWN` | `4194380` |
| `KEY_VOLUMEMUTE` | `4194381` |
| `KEY_VOLUMEUP` | `4194382` |
| `KEY_MEDIAPLAY` | `4194388` |
| `KEY_MEDIASTOP` | `4194389` |
| `KEY_MEDIAPREVIOUS` | `4194390` |
| `KEY_MEDIANEXT` | `4194391` |
| `KEY_MEDIARECORD` | `4194392` |
| `KEY_HOMEPAGE` | `4194393` |
| `KEY_FAVORITES` | `4194394` |
| `KEY_SEARCH` | `4194395` |
| `KEY_STANDBY` | `4194396` |
| `KEY_OPENURL` | `4194397` |
| `KEY_LAUNCHMAIL` | `4194398` |
| `KEY_LAUNCHMEDIA` | `4194399` |
| `KEY_LAUNCH0` | `4194400` |
| `KEY_LAUNCH1` | `4194401` |
| `KEY_LAUNCH2` | `4194402` |
| `KEY_LAUNCH3` | `4194403` |
| `KEY_LAUNCH4` | `4194404` |
| `KEY_LAUNCH5` | `4194405` |
| `KEY_LAUNCH6` | `4194406` |
| `KEY_LAUNCH7` | `4194407` |
| `KEY_LAUNCH8` | `4194408` |
| `KEY_LAUNCH9` | `4194409` |
| `KEY_LAUNCHA` | `4194410` |
| `KEY_LAUNCHB` | `4194411` |
| `KEY_LAUNCHC` | `4194412` |
| `KEY_LAUNCHD` | `4194413` |
| `KEY_LAUNCHE` | `4194414` |
| `KEY_LAUNCHF` | `4194415` |
| `KEY_GLOBE` | `4194416` |
| `KEY_KEYBOARD` | `4194417` |
| `KEY_JIS_EISU` | `4194418` |
| `KEY_JIS_KANA` | `4194419` |
| `KEY_UNKNOWN` | `8388607` |
| `KEY_SPACE` | `32` |
| `KEY_EXCLAM` | `33` |
| `KEY_QUOTEDBL` | `34` |
| `KEY_NUMBERSIGN` | `35` |
| `KEY_DOLLAR` | `36` |
| `KEY_PERCENT` | `37` |
| `KEY_AMPERSAND` | `38` |
| `KEY_APOSTROPHE` | `39` |
| `KEY_PARENLEFT` | `40` |
| `KEY_PARENRIGHT` | `41` |
| `KEY_ASTERISK` | `42` |
| `KEY_PLUS` | `43` |
| `KEY_COMMA` | `44` |
| `KEY_MINUS` | `45` |
| `KEY_PERIOD` | `46` |
| `KEY_SLASH` | `47` |
| `KEY_0` | `48` |
| `KEY_1` | `49` |
| `KEY_2` | `50` |
| `KEY_3` | `51` |
| `KEY_4` | `52` |
| `KEY_5` | `53` |
| `KEY_6` | `54` |
| `KEY_7` | `55` |
| `KEY_8` | `56` |
| `KEY_9` | `57` |
| `KEY_COLON` | `58` |
| `KEY_SEMICOLON` | `59` |
| `KEY_LESS` | `60` |
| `KEY_EQUAL` | `61` |
| `KEY_GREATER` | `62` |
| `KEY_QUESTION` | `63` |
| `KEY_AT` | `64` |
| `KEY_A` | `65` |
| `KEY_B` | `66` |
| `KEY_C` | `67` |
| `KEY_D` | `68` |
| `KEY_E` | `69` |
| `KEY_F` | `70` |
| `KEY_G` | `71` |
| `KEY_H` | `72` |
| `KEY_I` | `73` |
| `KEY_J` | `74` |
| `KEY_K` | `75` |
| `KEY_L` | `76` |
| `KEY_M` | `77` |
| `KEY_N` | `78` |
| `KEY_O` | `79` |
| `KEY_P` | `80` |
| `KEY_Q` | `81` |
| `KEY_R` | `82` |
| `KEY_S` | `83` |
| `KEY_T` | `84` |
| `KEY_U` | `85` |
| `KEY_V` | `86` |
| `KEY_W` | `87` |
| `KEY_X` | `88` |
| `KEY_Y` | `89` |
| `KEY_Z` | `90` |
| `KEY_BRACKETLEFT` | `91` |
| `KEY_BACKSLASH` | `92` |
| `KEY_BRACKETRIGHT` | `93` |
| `KEY_ASCIICIRCUM` | `94` |
| `KEY_UNDERSCORE` | `95` |
| `KEY_QUOTELEFT` | `96` |
| `KEY_BRACELEFT` | `123` |
| `KEY_BAR` | `124` |
| `KEY_BRACERIGHT` | `125` |
| `KEY_ASCIITILDE` | `126` |
| `KEY_YEN` | `165` |
| `KEY_SECTION` | `167` |

## KeyModifierMask

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `KEY_CODE_MASK` | `8388607` |
| `KEY_MODIFIER_MASK` | `2130706432` |
| `KEY_MASK_CMD_OR_CTRL` | `16777216` |
| `KEY_MASK_SHIFT` | `33554432` |
| `KEY_MASK_ALT` | `67108864` |
| `KEY_MASK_META` | `134217728` |
| `KEY_MASK_CTRL` | `268435456` |
| `KEY_MASK_KPAD` | `536870912` |
| `KEY_MASK_GROUP_SWITCH` | `1073741824` |

## KeyLocation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `KEY_LOCATION_UNSPECIFIED` | `0` |
| `KEY_LOCATION_LEFT` | `1` |
| `KEY_LOCATION_RIGHT` | `2` |

## MouseButton

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOUSE_BUTTON_NONE` | `0` |
| `MOUSE_BUTTON_LEFT` | `1` |
| `MOUSE_BUTTON_RIGHT` | `2` |
| `MOUSE_BUTTON_MIDDLE` | `3` |
| `MOUSE_BUTTON_WHEEL_UP` | `4` |
| `MOUSE_BUTTON_WHEEL_DOWN` | `5` |
| `MOUSE_BUTTON_WHEEL_LEFT` | `6` |
| `MOUSE_BUTTON_WHEEL_RIGHT` | `7` |
| `MOUSE_BUTTON_XBUTTON1` | `8` |
| `MOUSE_BUTTON_XBUTTON2` | `9` |

## MouseButtonMask

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `MOUSE_BUTTON_MASK_LEFT` | `1` |
| `MOUSE_BUTTON_MASK_RIGHT` | `2` |
| `MOUSE_BUTTON_MASK_MIDDLE` | `4` |
| `MOUSE_BUTTON_MASK_MB_XBUTTON1` | `128` |
| `MOUSE_BUTTON_MASK_MB_XBUTTON2` | `256` |

## JoyButton

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOY_BUTTON_INVALID` | `-1` |
| `JOY_BUTTON_A` | `0` |
| `JOY_BUTTON_B` | `1` |
| `JOY_BUTTON_X` | `2` |
| `JOY_BUTTON_Y` | `3` |
| `JOY_BUTTON_BACK` | `4` |
| `JOY_BUTTON_GUIDE` | `5` |
| `JOY_BUTTON_START` | `6` |
| `JOY_BUTTON_LEFT_STICK` | `7` |
| `JOY_BUTTON_RIGHT_STICK` | `8` |
| `JOY_BUTTON_LEFT_SHOULDER` | `9` |
| `JOY_BUTTON_RIGHT_SHOULDER` | `10` |
| `JOY_BUTTON_DPAD_UP` | `11` |
| `JOY_BUTTON_DPAD_DOWN` | `12` |
| `JOY_BUTTON_DPAD_LEFT` | `13` |
| `JOY_BUTTON_DPAD_RIGHT` | `14` |
| `JOY_BUTTON_MISC1` | `15` |
| `JOY_BUTTON_PADDLE1` | `16` |
| `JOY_BUTTON_PADDLE2` | `17` |
| `JOY_BUTTON_PADDLE3` | `18` |
| `JOY_BUTTON_PADDLE4` | `19` |
| `JOY_BUTTON_TOUCHPAD` | `20` |
| `JOY_BUTTON_MISC2` | `21` |
| `JOY_BUTTON_MISC3` | `22` |
| `JOY_BUTTON_MISC4` | `23` |
| `JOY_BUTTON_MISC5` | `24` |
| `JOY_BUTTON_MISC6` | `25` |
| `JOY_BUTTON_SDL_MAX` | `26` |
| `JOY_BUTTON_MAX` | `128` |

## JoyAxis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOY_AXIS_INVALID` | `-1` |
| `JOY_AXIS_LEFT_X` | `0` |
| `JOY_AXIS_LEFT_Y` | `1` |
| `JOY_AXIS_RIGHT_X` | `2` |
| `JOY_AXIS_RIGHT_Y` | `3` |
| `JOY_AXIS_TRIGGER_LEFT` | `4` |
| `JOY_AXIS_TRIGGER_RIGHT` | `5` |
| `JOY_AXIS_SDL_MAX` | `6` |
| `JOY_AXIS_MAX` | `10` |

## MIDIMessage

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MIDI_MESSAGE_NONE` | `0` |
| `MIDI_MESSAGE_NOTE_OFF` | `8` |
| `MIDI_MESSAGE_NOTE_ON` | `9` |
| `MIDI_MESSAGE_AFTERTOUCH` | `10` |
| `MIDI_MESSAGE_CONTROL_CHANGE` | `11` |
| `MIDI_MESSAGE_PROGRAM_CHANGE` | `12` |
| `MIDI_MESSAGE_CHANNEL_PRESSURE` | `13` |
| `MIDI_MESSAGE_PITCH_BEND` | `14` |
| `MIDI_MESSAGE_SYSTEM_EXCLUSIVE` | `240` |
| `MIDI_MESSAGE_QUARTER_FRAME` | `241` |
| `MIDI_MESSAGE_SONG_POSITION_POINTER` | `242` |
| `MIDI_MESSAGE_SONG_SELECT` | `243` |
| `MIDI_MESSAGE_TUNE_REQUEST` | `246` |
| `MIDI_MESSAGE_TIMING_CLOCK` | `248` |
| `MIDI_MESSAGE_START` | `250` |
| `MIDI_MESSAGE_CONTINUE` | `251` |
| `MIDI_MESSAGE_STOP` | `252` |
| `MIDI_MESSAGE_ACTIVE_SENSING` | `254` |
| `MIDI_MESSAGE_SYSTEM_RESET` | `255` |

## Error

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OK` | `0` |
| `FAILED` | `1` |
| `ERR_UNAVAILABLE` | `2` |
| `ERR_UNCONFIGURED` | `3` |
| `ERR_UNAUTHORIZED` | `4` |
| `ERR_PARAMETER_RANGE_ERROR` | `5` |
| `ERR_OUT_OF_MEMORY` | `6` |
| `ERR_FILE_NOT_FOUND` | `7` |
| `ERR_FILE_BAD_DRIVE` | `8` |
| `ERR_FILE_BAD_PATH` | `9` |
| `ERR_FILE_NO_PERMISSION` | `10` |
| `ERR_FILE_ALREADY_IN_USE` | `11` |
| `ERR_FILE_CANT_OPEN` | `12` |
| `ERR_FILE_CANT_WRITE` | `13` |
| `ERR_FILE_CANT_READ` | `14` |
| `ERR_FILE_UNRECOGNIZED` | `15` |
| `ERR_FILE_CORRUPT` | `16` |
| `ERR_FILE_MISSING_DEPENDENCIES` | `17` |
| `ERR_FILE_EOF` | `18` |
| `ERR_CANT_OPEN` | `19` |
| `ERR_CANT_CREATE` | `20` |
| `ERR_QUERY_FAILED` | `21` |
| `ERR_ALREADY_IN_USE` | `22` |
| `ERR_LOCKED` | `23` |
| `ERR_TIMEOUT` | `24` |
| `ERR_CANT_CONNECT` | `25` |
| `ERR_CANT_RESOLVE` | `26` |
| `ERR_CONNECTION_ERROR` | `27` |
| `ERR_CANT_ACQUIRE_RESOURCE` | `28` |
| `ERR_CANT_FORK` | `29` |
| `ERR_INVALID_DATA` | `30` |
| `ERR_INVALID_PARAMETER` | `31` |
| `ERR_ALREADY_EXISTS` | `32` |
| `ERR_DOES_NOT_EXIST` | `33` |
| `ERR_DATABASE_CANT_READ` | `34` |
| `ERR_DATABASE_CANT_WRITE` | `35` |
| `ERR_COMPILATION_FAILED` | `36` |
| `ERR_METHOD_NOT_FOUND` | `37` |
| `ERR_LINK_FAILED` | `38` |
| `ERR_SCRIPT_FAILED` | `39` |
| `ERR_CYCLIC_LINK` | `40` |
| `ERR_INVALID_DECLARATION` | `41` |
| `ERR_DUPLICATE_SYMBOL` | `42` |
| `ERR_PARSE_ERROR` | `43` |
| `ERR_BUSY` | `44` |
| `ERR_SKIP` | `45` |
| `ERR_HELP` | `46` |
| `ERR_BUG` | `47` |
| `ERR_PRINTER_ON_FIRE` | `48` |

## PropertyHint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PROPERTY_HINT_NONE` | `0` |
| `PROPERTY_HINT_RANGE` | `1` |
| `PROPERTY_HINT_ENUM` | `2` |
| `PROPERTY_HINT_ENUM_SUGGESTION` | `3` |
| `PROPERTY_HINT_EXP_EASING` | `4` |
| `PROPERTY_HINT_LINK` | `5` |
| `PROPERTY_HINT_FLAGS` | `6` |
| `PROPERTY_HINT_LAYERS_2D_RENDER` | `7` |
| `PROPERTY_HINT_LAYERS_2D_PHYSICS` | `8` |
| `PROPERTY_HINT_LAYERS_2D_NAVIGATION` | `9` |
| `PROPERTY_HINT_LAYERS_3D_RENDER` | `10` |
| `PROPERTY_HINT_LAYERS_3D_PHYSICS` | `11` |
| `PROPERTY_HINT_LAYERS_3D_NAVIGATION` | `12` |
| `PROPERTY_HINT_LAYERS_AVOIDANCE` | `37` |
| `PROPERTY_HINT_FILE` | `13` |
| `PROPERTY_HINT_DIR` | `14` |
| `PROPERTY_HINT_GLOBAL_FILE` | `15` |
| `PROPERTY_HINT_GLOBAL_DIR` | `16` |
| `PROPERTY_HINT_RESOURCE_TYPE` | `17` |
| `PROPERTY_HINT_MULTILINE_TEXT` | `18` |
| `PROPERTY_HINT_EXPRESSION` | `19` |
| `PROPERTY_HINT_PLACEHOLDER_TEXT` | `20` |
| `PROPERTY_HINT_COLOR_NO_ALPHA` | `21` |
| `PROPERTY_HINT_OBJECT_ID` | `22` |
| `PROPERTY_HINT_TYPE_STRING` | `23` |
| `PROPERTY_HINT_NODE_PATH_TO_EDITED_NODE` | `24` |
| `PROPERTY_HINT_OBJECT_TOO_BIG` | `25` |
| `PROPERTY_HINT_NODE_PATH_VALID_TYPES` | `26` |
| `PROPERTY_HINT_SAVE_FILE` | `27` |
| `PROPERTY_HINT_GLOBAL_SAVE_FILE` | `28` |
| `PROPERTY_HINT_INT_IS_OBJECTID` | `29` |
| `PROPERTY_HINT_INT_IS_POINTER` | `30` |
| `PROPERTY_HINT_ARRAY_TYPE` | `31` |
| `PROPERTY_HINT_DICTIONARY_TYPE` | `38` |
| `PROPERTY_HINT_LOCALE_ID` | `32` |
| `PROPERTY_HINT_LOCALIZABLE_STRING` | `33` |
| `PROPERTY_HINT_NODE_TYPE` | `34` |
| `PROPERTY_HINT_HIDE_QUATERNION_EDIT` | `35` |
| `PROPERTY_HINT_PASSWORD` | `36` |
| `PROPERTY_HINT_TOOL_BUTTON` | `39` |
| `PROPERTY_HINT_ONESHOT` | `40` |
| `PROPERTY_HINT_GROUP_ENABLE` | `42` |
| `PROPERTY_HINT_INPUT_NAME` | `43` |
| `PROPERTY_HINT_FILE_PATH` | `44` |
| `PROPERTY_HINT_MAX` | `45` |

## PropertyUsageFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `PROPERTY_USAGE_NONE` | `0` |
| `PROPERTY_USAGE_STORAGE` | `2` |
| `PROPERTY_USAGE_EDITOR` | `4` |
| `PROPERTY_USAGE_INTERNAL` | `8` |
| `PROPERTY_USAGE_CHECKABLE` | `16` |
| `PROPERTY_USAGE_CHECKED` | `32` |
| `PROPERTY_USAGE_GROUP` | `64` |
| `PROPERTY_USAGE_CATEGORY` | `128` |
| `PROPERTY_USAGE_SUBGROUP` | `256` |
| `PROPERTY_USAGE_CLASS_IS_BITFIELD` | `512` |
| `PROPERTY_USAGE_NO_INSTANCE_STATE` | `1024` |
| `PROPERTY_USAGE_RESTART_IF_CHANGED` | `2048` |
| `PROPERTY_USAGE_SCRIPT_VARIABLE` | `4096` |
| `PROPERTY_USAGE_STORE_IF_NULL` | `8192` |
| `PROPERTY_USAGE_UPDATE_ALL_IF_MODIFIED` | `16384` |
| `PROPERTY_USAGE_SCRIPT_DEFAULT_VALUE` | `32768` |
| `PROPERTY_USAGE_CLASS_IS_ENUM` | `65536` |
| `PROPERTY_USAGE_NIL_IS_VARIANT` | `131072` |
| `PROPERTY_USAGE_ARRAY` | `262144` |
| `PROPERTY_USAGE_ALWAYS_DUPLICATE` | `524288` |
| `PROPERTY_USAGE_NEVER_DUPLICATE` | `1048576` |
| `PROPERTY_USAGE_HIGH_END_GFX` | `2097152` |
| `PROPERTY_USAGE_NODE_PATH_FROM_SCENE_ROOT` | `4194304` |
| `PROPERTY_USAGE_RESOURCE_NOT_PERSISTENT` | `8388608` |
| `PROPERTY_USAGE_KEYING_INCREMENTS` | `16777216` |
| `PROPERTY_USAGE_DEFERRED_SET_RESOURCE` | `33554432` |
| `PROPERTY_USAGE_EDITOR_INSTANTIATE_OBJECT` | `67108864` |
| `PROPERTY_USAGE_EDITOR_BASIC_SETTING` | `134217728` |
| `PROPERTY_USAGE_READ_ONLY` | `268435456` |
| `PROPERTY_USAGE_SECRET` | `536870912` |
| `PROPERTY_USAGE_DEFAULT` | `6` |
| `PROPERTY_USAGE_NO_EDITOR` | `2` |

## MethodFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `METHOD_FLAG_NORMAL` | `1` |
| `METHOD_FLAG_EDITOR` | `2` |
| `METHOD_FLAG_CONST` | `4` |
| `METHOD_FLAG_VIRTUAL` | `8` |
| `METHOD_FLAG_VARARG` | `16` |
| `METHOD_FLAG_STATIC` | `32` |
| `METHOD_FLAG_OBJECT_CORE` | `64` |
| `METHOD_FLAG_VIRTUAL_REQUIRED` | `128` |
| `METHOD_FLAGS_DEFAULT` | `1` |

## Variant.Type

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_NIL` | `0` |
| `TYPE_BOOL` | `1` |
| `TYPE_INT` | `2` |
| `TYPE_FLOAT` | `3` |
| `TYPE_STRING` | `4` |
| `TYPE_VECTOR2` | `5` |
| `TYPE_VECTOR2I` | `6` |
| `TYPE_RECT2` | `7` |
| `TYPE_RECT2I` | `8` |
| `TYPE_VECTOR3` | `9` |
| `TYPE_VECTOR3I` | `10` |
| `TYPE_TRANSFORM2D` | `11` |
| `TYPE_VECTOR4` | `12` |
| `TYPE_VECTOR4I` | `13` |
| `TYPE_PLANE` | `14` |
| `TYPE_QUATERNION` | `15` |
| `TYPE_AABB` | `16` |
| `TYPE_BASIS` | `17` |
| `TYPE_TRANSFORM3D` | `18` |
| `TYPE_PROJECTION` | `19` |
| `TYPE_COLOR` | `20` |
| `TYPE_STRING_NAME` | `21` |
| `TYPE_NODE_PATH` | `22` |
| `TYPE_RID` | `23` |
| `TYPE_OBJECT` | `24` |
| `TYPE_CALLABLE` | `25` |
| `TYPE_SIGNAL` | `26` |
| `TYPE_DICTIONARY` | `27` |
| `TYPE_ARRAY` | `28` |
| `TYPE_PACKED_BYTE_ARRAY` | `29` |
| `TYPE_PACKED_INT32_ARRAY` | `30` |
| `TYPE_PACKED_INT64_ARRAY` | `31` |
| `TYPE_PACKED_FLOAT32_ARRAY` | `32` |
| `TYPE_PACKED_FLOAT64_ARRAY` | `33` |
| `TYPE_PACKED_STRING_ARRAY` | `34` |
| `TYPE_PACKED_VECTOR2_ARRAY` | `35` |
| `TYPE_PACKED_VECTOR3_ARRAY` | `36` |
| `TYPE_PACKED_COLOR_ARRAY` | `37` |
| `TYPE_PACKED_VECTOR4_ARRAY` | `38` |
| `TYPE_MAX` | `39` |

## Variant.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_EQUAL` | `0` |
| `OP_NOT_EQUAL` | `1` |
| `OP_LESS` | `2` |
| `OP_LESS_EQUAL` | `3` |
| `OP_GREATER` | `4` |
| `OP_GREATER_EQUAL` | `5` |
| `OP_ADD` | `6` |
| `OP_SUBTRACT` | `7` |
| `OP_MULTIPLY` | `8` |
| `OP_DIVIDE` | `9` |
| `OP_NEGATE` | `10` |
| `OP_POSITIVE` | `11` |
| `OP_MODULE` | `12` |
| `OP_POWER` | `13` |
| `OP_SHIFT_LEFT` | `14` |
| `OP_SHIFT_RIGHT` | `15` |
| `OP_BIT_AND` | `16` |
| `OP_BIT_OR` | `17` |
| `OP_BIT_XOR` | `18` |
| `OP_BIT_NEGATE` | `19` |
| `OP_AND` | `20` |
| `OP_OR` | `21` |
| `OP_XOR` | `22` |
| `OP_NOT` | `23` |
| `OP_IN` | `24` |
| `OP_MAX` | `25` |

## Vector2.Axis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_X` | `0` |
| `AXIS_Y` | `1` |

## Vector2i.Axis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_X` | `0` |
| `AXIS_Y` | `1` |

## Vector3.Axis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_X` | `0` |
| `AXIS_Y` | `1` |
| `AXIS_Z` | `2` |

## Vector3i.Axis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_X` | `0` |
| `AXIS_Y` | `1` |
| `AXIS_Z` | `2` |

## Vector4.Axis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_X` | `0` |
| `AXIS_Y` | `1` |
| `AXIS_Z` | `2` |
| `AXIS_W` | `3` |

## Vector4i.Axis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_X` | `0` |
| `AXIS_Y` | `1` |
| `AXIS_Z` | `2` |
| `AXIS_W` | `3` |

## Projection.Planes

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLANE_NEAR` | `0` |
| `PLANE_FAR` | `1` |
| `PLANE_LEFT` | `2` |
| `PLANE_TOP` | `3` |
| `PLANE_RIGHT` | `4` |
| `PLANE_BOTTOM` | `5` |

## AESContext.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_ECB_ENCRYPT` | `0` |
| `MODE_ECB_DECRYPT` | `1` |
| `MODE_CBC_ENCRYPT` | `2` |
| `MODE_CBC_DECRYPT` | `3` |
| `MODE_MAX` | `4` |

## AStarGrid2D.Heuristic

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HEURISTIC_EUCLIDEAN` | `0` |
| `HEURISTIC_MANHATTAN` | `1` |
| `HEURISTIC_OCTILE` | `2` |
| `HEURISTIC_CHEBYSHEV` | `3` |
| `HEURISTIC_MAX` | `4` |

## AStarGrid2D.DiagonalMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DIAGONAL_MODE_ALWAYS` | `0` |
| `DIAGONAL_MODE_NEVER` | `1` |
| `DIAGONAL_MODE_AT_LEAST_ONE_WALKABLE` | `2` |
| `DIAGONAL_MODE_ONLY_IF_NO_OBSTACLES` | `3` |
| `DIAGONAL_MODE_MAX` | `4` |

## AStarGrid2D.CellShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CELL_SHAPE_SQUARE` | `0` |
| `CELL_SHAPE_ISOMETRIC_RIGHT` | `1` |
| `CELL_SHAPE_ISOMETRIC_DOWN` | `2` |
| `CELL_SHAPE_MAX` | `3` |

## AccessibilityServer.AccessibilityRole

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ROLE_UNKNOWN` | `0` |
| `ROLE_DEFAULT_BUTTON` | `1` |
| `ROLE_AUDIO` | `2` |
| `ROLE_VIDEO` | `3` |
| `ROLE_STATIC_TEXT` | `4` |
| `ROLE_CONTAINER` | `5` |
| `ROLE_PANEL` | `6` |
| `ROLE_BUTTON` | `7` |
| `ROLE_LINK` | `8` |
| `ROLE_CHECK_BOX` | `9` |
| `ROLE_RADIO_BUTTON` | `10` |
| `ROLE_CHECK_BUTTON` | `11` |
| `ROLE_SCROLL_BAR` | `12` |
| `ROLE_SCROLL_VIEW` | `13` |
| `ROLE_SPLITTER` | `14` |
| `ROLE_SLIDER` | `15` |
| `ROLE_SPIN_BUTTON` | `16` |
| `ROLE_PROGRESS_INDICATOR` | `17` |
| `ROLE_TEXT_FIELD` | `18` |
| `ROLE_MULTILINE_TEXT_FIELD` | `19` |
| `ROLE_COLOR_PICKER` | `20` |
| `ROLE_TABLE` | `21` |
| `ROLE_CELL` | `22` |
| `ROLE_ROW` | `23` |
| `ROLE_ROW_GROUP` | `24` |
| `ROLE_ROW_HEADER` | `25` |
| `ROLE_COLUMN_HEADER` | `26` |
| `ROLE_TREE` | `27` |
| `ROLE_TREE_ITEM` | `28` |
| `ROLE_LIST` | `29` |
| `ROLE_LIST_ITEM` | `30` |
| `ROLE_LIST_BOX` | `31` |
| `ROLE_LIST_BOX_OPTION` | `32` |
| `ROLE_TAB_BAR` | `33` |
| `ROLE_TAB` | `34` |
| `ROLE_TAB_PANEL` | `35` |
| `ROLE_MENU_BAR` | `36` |
| `ROLE_MENU` | `37` |
| `ROLE_MENU_ITEM` | `38` |
| `ROLE_MENU_ITEM_CHECK_BOX` | `39` |
| `ROLE_MENU_ITEM_RADIO` | `40` |
| `ROLE_IMAGE` | `41` |
| `ROLE_WINDOW` | `42` |
| `ROLE_TITLE_BAR` | `43` |
| `ROLE_DIALOG` | `44` |
| `ROLE_TOOLTIP` | `45` |
| `ROLE_REGION` | `46` |
| `ROLE_TEXT_RUN` | `47` |

## AccessibilityServer.AccessibilityPopupType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `POPUP_MENU` | `0` |
| `POPUP_LIST` | `1` |
| `POPUP_TREE` | `2` |
| `POPUP_DIALOG` | `3` |

## AccessibilityServer.AccessibilityFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_HIDDEN` | `0` |
| `FLAG_MULTISELECTABLE` | `1` |
| `FLAG_REQUIRED` | `2` |
| `FLAG_VISITED` | `3` |
| `FLAG_BUSY` | `4` |
| `FLAG_MODAL` | `5` |
| `FLAG_TOUCH_PASSTHROUGH` | `6` |
| `FLAG_READONLY` | `7` |
| `FLAG_DISABLED` | `8` |
| `FLAG_CLIPS_CHILDREN` | `9` |

## AccessibilityServer.AccessibilityAction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ACTION_CLICK` | `0` |
| `ACTION_FOCUS` | `1` |
| `ACTION_BLUR` | `2` |
| `ACTION_COLLAPSE` | `3` |
| `ACTION_EXPAND` | `4` |
| `ACTION_DECREMENT` | `5` |
| `ACTION_INCREMENT` | `6` |
| `ACTION_HIDE_TOOLTIP` | `7` |
| `ACTION_SHOW_TOOLTIP` | `8` |
| `ACTION_SET_TEXT_SELECTION` | `9` |
| `ACTION_REPLACE_SELECTED_TEXT` | `10` |
| `ACTION_SCROLL_BACKWARD` | `11` |
| `ACTION_SCROLL_DOWN` | `12` |
| `ACTION_SCROLL_FORWARD` | `13` |
| `ACTION_SCROLL_LEFT` | `14` |
| `ACTION_SCROLL_RIGHT` | `15` |
| `ACTION_SCROLL_UP` | `16` |
| `ACTION_SCROLL_INTO_VIEW` | `17` |
| `ACTION_SCROLL_TO_POINT` | `18` |
| `ACTION_SET_SCROLL_OFFSET` | `19` |
| `ACTION_SET_VALUE` | `20` |
| `ACTION_SHOW_CONTEXT_MENU` | `21` |
| `ACTION_CUSTOM` | `22` |

## AccessibilityServer.AccessibilityLiveMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIVE_OFF` | `0` |
| `LIVE_POLITE` | `1` |
| `LIVE_ASSERTIVE` | `2` |

## AccessibilityServer.AccessibilityScrollUnit

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_UNIT_ITEM` | `0` |
| `SCROLL_UNIT_PAGE` | `1` |

## AccessibilityServer.AccessibilityScrollHint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_HINT_TOP_LEFT` | `0` |
| `SCROLL_HINT_BOTTOM_RIGHT` | `1` |
| `SCROLL_HINT_TOP_EDGE` | `2` |
| `SCROLL_HINT_BOTTOM_EDGE` | `3` |
| `SCROLL_HINT_LEFT_EDGE` | `4` |
| `SCROLL_HINT_RIGHT_EDGE` | `5` |

## Animation.TrackType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_VALUE` | `0` |
| `TYPE_POSITION_3D` | `1` |
| `TYPE_ROTATION_3D` | `2` |
| `TYPE_SCALE_3D` | `3` |
| `TYPE_BLEND_SHAPE` | `4` |
| `TYPE_METHOD` | `5` |
| `TYPE_BEZIER` | `6` |
| `TYPE_AUDIO` | `7` |
| `TYPE_ANIMATION` | `8` |

## Animation.InterpolationType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INTERPOLATION_NEAREST` | `0` |
| `INTERPOLATION_LINEAR` | `1` |
| `INTERPOLATION_CUBIC` | `2` |
| `INTERPOLATION_LINEAR_ANGLE` | `3` |
| `INTERPOLATION_CUBIC_ANGLE` | `4` |

## Animation.UpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UPDATE_CONTINUOUS` | `0` |
| `UPDATE_DISCRETE` | `1` |
| `UPDATE_CAPTURE` | `2` |

## Animation.LoopMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOOP_NONE` | `0` |
| `LOOP_LINEAR` | `1` |
| `LOOP_PINGPONG` | `2` |

## Animation.LoopedFlag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOOPED_FLAG_NONE` | `0` |
| `LOOPED_FLAG_END` | `1` |
| `LOOPED_FLAG_START` | `2` |

## Animation.FindMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FIND_MODE_NEAREST` | `0` |
| `FIND_MODE_APPROX` | `1` |
| `FIND_MODE_EXACT` | `2` |

## AnimationMixer.AnimationCallbackModeProcess

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANIMATION_CALLBACK_MODE_PROCESS_PHYSICS` | `0` |
| `ANIMATION_CALLBACK_MODE_PROCESS_IDLE` | `1` |
| `ANIMATION_CALLBACK_MODE_PROCESS_MANUAL` | `2` |

## AnimationMixer.AnimationCallbackModeMethod

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANIMATION_CALLBACK_MODE_METHOD_DEFERRED` | `0` |
| `ANIMATION_CALLBACK_MODE_METHOD_IMMEDIATE` | `1` |

## AnimationMixer.AnimationCallbackModeDiscrete

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANIMATION_CALLBACK_MODE_DISCRETE_DOMINANT` | `0` |
| `ANIMATION_CALLBACK_MODE_DISCRETE_RECESSIVE` | `1` |
| `ANIMATION_CALLBACK_MODE_DISCRETE_FORCE_CONTINUOUS` | `2` |

## AnimationNode.FilterAction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILTER_IGNORE` | `0` |
| `FILTER_PASS` | `1` |
| `FILTER_STOP` | `2` |
| `FILTER_BLEND` | `3` |

## AnimationNodeAnimation.PlayMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLAY_MODE_FORWARD` | `0` |
| `PLAY_MODE_BACKWARD` | `1` |

## AnimationNodeBlendSpace1D.BlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_MODE_INTERPOLATED` | `0` |
| `BLEND_MODE_DISCRETE` | `1` |
| `BLEND_MODE_DISCRETE_CARRY` | `2` |

## AnimationNodeBlendSpace1D.SyncMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SYNC_MODE_NONE` | `0` |
| `SYNC_MODE_INDEPENDENT` | `1` |
| `SYNC_MODE_CYCLIC_MUTABLE` | `2` |
| `SYNC_MODE_CYCLIC_CONSTANT` | `3` |

## AnimationNodeBlendSpace2D.BlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_MODE_INTERPOLATED` | `0` |
| `BLEND_MODE_DISCRETE` | `1` |
| `BLEND_MODE_DISCRETE_CARRY` | `2` |

## AnimationNodeBlendSpace2D.SyncMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SYNC_MODE_NONE` | `0` |
| `SYNC_MODE_INDEPENDENT` | `1` |
| `SYNC_MODE_CYCLIC_MUTABLE` | `2` |
| `SYNC_MODE_CYCLIC_CONSTANT` | `3` |

## AnimationNodeOneShot.OneShotRequest

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ONE_SHOT_REQUEST_NONE` | `0` |
| `ONE_SHOT_REQUEST_FIRE` | `1` |
| `ONE_SHOT_REQUEST_ABORT` | `2` |
| `ONE_SHOT_REQUEST_FADE_OUT` | `3` |

## AnimationNodeOneShot.MixMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MIX_MODE_BLEND` | `0` |
| `MIX_MODE_ADD` | `1` |

## AnimationNodeStateMachine.StateMachineType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATE_MACHINE_TYPE_ROOT` | `0` |
| `STATE_MACHINE_TYPE_NESTED` | `1` |
| `STATE_MACHINE_TYPE_GROUPED` | `2` |

## AnimationNodeStateMachineTransition.SwitchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SWITCH_MODE_IMMEDIATE` | `0` |
| `SWITCH_MODE_SYNC` | `1` |
| `SWITCH_MODE_AT_END` | `2` |

## AnimationNodeStateMachineTransition.AdvanceMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ADVANCE_MODE_DISABLED` | `0` |
| `ADVANCE_MODE_ENABLED` | `1` |
| `ADVANCE_MODE_AUTO` | `2` |

## AnimationPlayer.AnimationProcessCallback

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANIMATION_PROCESS_PHYSICS` | `0` |
| `ANIMATION_PROCESS_IDLE` | `1` |
| `ANIMATION_PROCESS_MANUAL` | `2` |

## AnimationPlayer.AnimationMethodCallMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANIMATION_METHOD_CALL_DEFERRED` | `0` |
| `ANIMATION_METHOD_CALL_IMMEDIATE` | `1` |

## AnimationTree.AnimationProcessCallback

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANIMATION_PROCESS_PHYSICS` | `0` |
| `ANIMATION_PROCESS_IDLE` | `1` |
| `ANIMATION_PROCESS_MANUAL` | `2` |

## Area2D.SpaceOverride

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPACE_OVERRIDE_DISABLED` | `0` |
| `SPACE_OVERRIDE_COMBINE` | `1` |
| `SPACE_OVERRIDE_COMBINE_REPLACE` | `2` |
| `SPACE_OVERRIDE_REPLACE` | `3` |
| `SPACE_OVERRIDE_REPLACE_COMBINE` | `4` |

## Area3D.SpaceOverride

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPACE_OVERRIDE_DISABLED` | `0` |
| `SPACE_OVERRIDE_COMBINE` | `1` |
| `SPACE_OVERRIDE_COMBINE_REPLACE` | `2` |
| `SPACE_OVERRIDE_REPLACE` | `3` |
| `SPACE_OVERRIDE_REPLACE_COMBINE` | `4` |

## AspectRatioContainer.StretchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STRETCH_WIDTH_CONTROLS_HEIGHT` | `0` |
| `STRETCH_HEIGHT_CONTROLS_WIDTH` | `1` |
| `STRETCH_FIT` | `2` |
| `STRETCH_COVER` | `3` |

## AspectRatioContainer.AlignmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALIGNMENT_BEGIN` | `0` |
| `ALIGNMENT_CENTER` | `1` |
| `ALIGNMENT_END` | `2` |

## AudioEffectDistortion.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_CLIP` | `0` |
| `MODE_ATAN` | `1` |
| `MODE_LOFI` | `2` |
| `MODE_OVERDRIVE` | `3` |
| `MODE_WAVESHAPE` | `4` |

## AudioEffectFilter.FilterDB

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILTER_6DB` | `0` |
| `FILTER_12DB` | `1` |
| `FILTER_18DB` | `2` |
| `FILTER_24DB` | `3` |

## AudioEffectPitchShift.FFTSize

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FFT_SIZE_256` | `0` |
| `FFT_SIZE_512` | `1` |
| `FFT_SIZE_1024` | `2` |
| `FFT_SIZE_2048` | `3` |
| `FFT_SIZE_4096` | `4` |
| `FFT_SIZE_MAX` | `5` |

## AudioEffectSpectrumAnalyzer.FFTSize

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FFT_SIZE_256` | `0` |
| `FFT_SIZE_512` | `1` |
| `FFT_SIZE_1024` | `2` |
| `FFT_SIZE_2048` | `3` |
| `FFT_SIZE_4096` | `4` |
| `FFT_SIZE_MAX` | `5` |

## AudioEffectSpectrumAnalyzerInstance.MagnitudeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MAGNITUDE_AVERAGE` | `0` |
| `MAGNITUDE_MAX` | `1` |

## AudioListener3D.DopplerTracking

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOPPLER_TRACKING_DISABLED` | `0` |
| `DOPPLER_TRACKING_IDLE_STEP` | `1` |
| `DOPPLER_TRACKING_PHYSICS_STEP` | `2` |

## AudioServer.SpeakerMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPEAKER_MODE_STEREO` | `0` |
| `SPEAKER_SURROUND_31` | `1` |
| `SPEAKER_SURROUND_51` | `2` |
| `SPEAKER_SURROUND_71` | `3` |

## AudioServer.PlaybackType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLAYBACK_TYPE_DEFAULT` | `0` |
| `PLAYBACK_TYPE_STREAM` | `1` |
| `PLAYBACK_TYPE_SAMPLE` | `2` |
| `PLAYBACK_TYPE_MAX` | `3` |

## AudioStreamGenerator.AudioStreamGeneratorMixRate

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MIX_RATE_OUTPUT` | `0` |
| `MIX_RATE_INPUT` | `1` |
| `MIX_RATE_CUSTOM` | `2` |
| `MIX_RATE_MAX` | `3` |

## AudioStreamInteractive.TransitionFromTime

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSITION_FROM_TIME_IMMEDIATE` | `0` |
| `TRANSITION_FROM_TIME_NEXT_BEAT` | `1` |
| `TRANSITION_FROM_TIME_NEXT_BAR` | `2` |
| `TRANSITION_FROM_TIME_END` | `3` |

## AudioStreamInteractive.TransitionToTime

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSITION_TO_TIME_SAME_POSITION` | `0` |
| `TRANSITION_TO_TIME_START` | `1` |
| `TRANSITION_TO_TIME_PREVIOUS_POSITION` | `2` |

## AudioStreamInteractive.FadeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FADE_DISABLED` | `0` |
| `FADE_IN` | `1` |
| `FADE_OUT` | `2` |
| `FADE_CROSS` | `3` |
| `FADE_AUTOMATIC` | `4` |

## AudioStreamInteractive.AutoAdvanceMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AUTO_ADVANCE_DISABLED` | `0` |
| `AUTO_ADVANCE_ENABLED` | `1` |
| `AUTO_ADVANCE_RETURN_TO_HOLD` | `2` |

## AudioStreamPlayer.MixTarget

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MIX_TARGET_STEREO` | `0` |
| `MIX_TARGET_SURROUND` | `1` |
| `MIX_TARGET_CENTER` | `2` |

## AudioStreamPlayer3D.AttenuationModel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ATTENUATION_INVERSE_DISTANCE` | `0` |
| `ATTENUATION_INVERSE_SQUARE_DISTANCE` | `1` |
| `ATTENUATION_LOGARITHMIC` | `2` |
| `ATTENUATION_DISABLED` | `3` |

## AudioStreamPlayer3D.DopplerTracking

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOPPLER_TRACKING_DISABLED` | `0` |
| `DOPPLER_TRACKING_IDLE_STEP` | `1` |
| `DOPPLER_TRACKING_PHYSICS_STEP` | `2` |

## AudioStreamRandomizer.PlaybackMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLAYBACK_RANDOM_NO_REPEATS` | `0` |
| `PLAYBACK_RANDOM` | `1` |
| `PLAYBACK_SEQUENTIAL` | `2` |

## AudioStreamWAV.Format

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FORMAT_8_BITS` | `0` |
| `FORMAT_16_BITS` | `1` |
| `FORMAT_IMA_ADPCM` | `2` |
| `FORMAT_QOA` | `3` |

## AudioStreamWAV.LoopMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOOP_DISABLED` | `0` |
| `LOOP_FORWARD` | `1` |
| `LOOP_PINGPONG` | `2` |
| `LOOP_BACKWARD` | `3` |

## BackBufferCopy.CopyMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COPY_MODE_DISABLED` | `0` |
| `COPY_MODE_RECT` | `1` |
| `COPY_MODE_VIEWPORT` | `2` |

## BaseButton.DrawMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAW_NORMAL` | `0` |
| `DRAW_PRESSED` | `1` |
| `DRAW_HOVER` | `2` |
| `DRAW_DISABLED` | `3` |
| `DRAW_HOVER_PRESSED` | `4` |

## BaseButton.ActionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ACTION_MODE_BUTTON_PRESS` | `0` |
| `ACTION_MODE_BUTTON_RELEASE` | `1` |

## BaseMaterial3D.TextureParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_ALBEDO` | `0` |
| `TEXTURE_METALLIC` | `1` |
| `TEXTURE_ROUGHNESS` | `2` |
| `TEXTURE_EMISSION` | `3` |
| `TEXTURE_NORMAL` | `4` |
| `TEXTURE_BENT_NORMAL` | `18` |
| `TEXTURE_RIM` | `5` |
| `TEXTURE_CLEARCOAT` | `6` |
| `TEXTURE_FLOWMAP` | `7` |
| `TEXTURE_AMBIENT_OCCLUSION` | `8` |
| `TEXTURE_HEIGHTMAP` | `9` |
| `TEXTURE_SUBSURFACE_SCATTERING` | `10` |
| `TEXTURE_SUBSURFACE_TRANSMITTANCE` | `11` |
| `TEXTURE_BACKLIGHT` | `12` |
| `TEXTURE_REFRACTION` | `13` |
| `TEXTURE_DETAIL_MASK` | `14` |
| `TEXTURE_DETAIL_ALBEDO` | `15` |
| `TEXTURE_DETAIL_NORMAL` | `16` |
| `TEXTURE_ORM` | `17` |
| `TEXTURE_MAX` | `19` |

## BaseMaterial3D.TextureFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_FILTER_NEAREST` | `0` |
| `TEXTURE_FILTER_LINEAR` | `1` |
| `TEXTURE_FILTER_NEAREST_WITH_MIPMAPS` | `2` |
| `TEXTURE_FILTER_LINEAR_WITH_MIPMAPS` | `3` |
| `TEXTURE_FILTER_NEAREST_WITH_MIPMAPS_ANISOTROPIC` | `4` |
| `TEXTURE_FILTER_LINEAR_WITH_MIPMAPS_ANISOTROPIC` | `5` |
| `TEXTURE_FILTER_MAX` | `6` |

## BaseMaterial3D.DetailUV

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DETAIL_UV_1` | `0` |
| `DETAIL_UV_2` | `1` |

## BaseMaterial3D.Transparency

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSPARENCY_DISABLED` | `0` |
| `TRANSPARENCY_ALPHA` | `1` |
| `TRANSPARENCY_ALPHA_SCISSOR` | `2` |
| `TRANSPARENCY_ALPHA_HASH` | `3` |
| `TRANSPARENCY_ALPHA_DEPTH_PRE_PASS` | `4` |
| `TRANSPARENCY_MAX` | `5` |

## BaseMaterial3D.ShadingMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADING_MODE_UNSHADED` | `0` |
| `SHADING_MODE_PER_PIXEL` | `1` |
| `SHADING_MODE_PER_VERTEX` | `2` |
| `SHADING_MODE_MAX` | `3` |

## BaseMaterial3D.Feature

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEATURE_EMISSION` | `0` |
| `FEATURE_NORMAL_MAPPING` | `1` |
| `FEATURE_RIM` | `2` |
| `FEATURE_CLEARCOAT` | `3` |
| `FEATURE_ANISOTROPY` | `4` |
| `FEATURE_AMBIENT_OCCLUSION` | `5` |
| `FEATURE_HEIGHT_MAPPING` | `6` |
| `FEATURE_SUBSURFACE_SCATTERING` | `7` |
| `FEATURE_SUBSURFACE_TRANSMITTANCE` | `8` |
| `FEATURE_BACKLIGHT` | `9` |
| `FEATURE_REFRACTION` | `10` |
| `FEATURE_DETAIL` | `11` |
| `FEATURE_BENT_NORMAL_MAPPING` | `12` |
| `FEATURE_MAX` | `13` |

## BaseMaterial3D.BlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_MODE_MIX` | `0` |
| `BLEND_MODE_ADD` | `1` |
| `BLEND_MODE_SUB` | `2` |
| `BLEND_MODE_MUL` | `3` |
| `BLEND_MODE_PREMULT_ALPHA` | `4` |

## BaseMaterial3D.AlphaAntiAliasing

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALPHA_ANTIALIASING_OFF` | `0` |
| `ALPHA_ANTIALIASING_ALPHA_TO_COVERAGE` | `1` |
| `ALPHA_ANTIALIASING_ALPHA_TO_COVERAGE_AND_TO_ONE` | `2` |

## BaseMaterial3D.DepthDrawMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEPTH_DRAW_OPAQUE_ONLY` | `0` |
| `DEPTH_DRAW_ALWAYS` | `1` |
| `DEPTH_DRAW_DISABLED` | `2` |

## BaseMaterial3D.DepthTest

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEPTH_TEST_DEFAULT` | `0` |
| `DEPTH_TEST_INVERTED` | `1` |

## BaseMaterial3D.CullMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CULL_BACK` | `0` |
| `CULL_FRONT` | `1` |
| `CULL_DISABLED` | `2` |

## BaseMaterial3D.Flags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_DISABLE_DEPTH_TEST` | `0` |
| `FLAG_ALBEDO_FROM_VERTEX_COLOR` | `1` |
| `FLAG_SRGB_VERTEX_COLOR` | `2` |
| `FLAG_USE_POINT_SIZE` | `3` |
| `FLAG_FIXED_SIZE` | `4` |
| `FLAG_BILLBOARD_KEEP_SCALE` | `5` |
| `FLAG_UV1_USE_TRIPLANAR` | `6` |
| `FLAG_UV2_USE_TRIPLANAR` | `7` |
| `FLAG_UV1_USE_WORLD_TRIPLANAR` | `8` |
| `FLAG_UV2_USE_WORLD_TRIPLANAR` | `9` |
| `FLAG_AO_ON_UV2` | `10` |
| `FLAG_EMISSION_ON_UV2` | `11` |
| `FLAG_ALBEDO_TEXTURE_FORCE_SRGB` | `12` |
| `FLAG_DONT_RECEIVE_SHADOWS` | `13` |
| `FLAG_DISABLE_AMBIENT_LIGHT` | `14` |
| `FLAG_USE_SHADOW_TO_OPACITY` | `15` |
| `FLAG_USE_TEXTURE_REPEAT` | `16` |
| `FLAG_INVERT_HEIGHTMAP` | `17` |
| `FLAG_SUBSURFACE_MODE_SKIN` | `18` |
| `FLAG_PARTICLE_TRAILS_MODE` | `19` |
| `FLAG_ALBEDO_TEXTURE_MSDF` | `20` |
| `FLAG_DISABLE_FOG` | `21` |
| `FLAG_DISABLE_SPECULAR_OCCLUSION` | `22` |
| `FLAG_USE_Z_CLIP_SCALE` | `23` |
| `FLAG_USE_FOV_OVERRIDE` | `24` |
| `FLAG_MAX` | `25` |

## BaseMaterial3D.DiffuseMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DIFFUSE_BURLEY` | `0` |
| `DIFFUSE_LAMBERT` | `1` |
| `DIFFUSE_LAMBERT_WRAP` | `2` |
| `DIFFUSE_TOON` | `3` |

## BaseMaterial3D.SpecularMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPECULAR_SCHLICK_GGX` | `0` |
| `SPECULAR_TOON` | `1` |
| `SPECULAR_DISABLED` | `2` |

## BaseMaterial3D.BillboardMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BILLBOARD_DISABLED` | `0` |
| `BILLBOARD_ENABLED` | `1` |
| `BILLBOARD_FIXED_Y` | `2` |
| `BILLBOARD_PARTICLES` | `3` |

## BaseMaterial3D.TextureChannel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_CHANNEL_RED` | `0` |
| `TEXTURE_CHANNEL_GREEN` | `1` |
| `TEXTURE_CHANNEL_BLUE` | `2` |
| `TEXTURE_CHANNEL_ALPHA` | `3` |
| `TEXTURE_CHANNEL_GRAYSCALE` | `4` |

## BaseMaterial3D.EmissionOperator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMISSION_OP_ADD` | `0` |
| `EMISSION_OP_MULTIPLY` | `1` |

## BaseMaterial3D.DistanceFadeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISTANCE_FADE_DISABLED` | `0` |
| `DISTANCE_FADE_PIXEL_ALPHA` | `1` |
| `DISTANCE_FADE_PIXEL_DITHER` | `2` |
| `DISTANCE_FADE_OBJECT_DITHER` | `3` |

## BaseMaterial3D.StencilMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STENCIL_MODE_DISABLED` | `0` |
| `STENCIL_MODE_OUTLINE` | `1` |
| `STENCIL_MODE_XRAY` | `2` |
| `STENCIL_MODE_CUSTOM` | `3` |

## BaseMaterial3D.StencilFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STENCIL_FLAG_READ` | `1` |
| `STENCIL_FLAG_WRITE` | `2` |
| `STENCIL_FLAG_WRITE_DEPTH_FAIL` | `4` |

## BaseMaterial3D.StencilCompare

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STENCIL_COMPARE_ALWAYS` | `0` |
| `STENCIL_COMPARE_LESS` | `1` |
| `STENCIL_COMPARE_EQUAL` | `2` |
| `STENCIL_COMPARE_LESS_OR_EQUAL` | `3` |
| `STENCIL_COMPARE_GREATER` | `4` |
| `STENCIL_COMPARE_NOT_EQUAL` | `5` |
| `STENCIL_COMPARE_GREATER_OR_EQUAL` | `6` |

## BlitMaterial.BlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_MODE_MIX` | `0` |
| `BLEND_MODE_ADD` | `1` |
| `BLEND_MODE_SUB` | `2` |
| `BLEND_MODE_MUL` | `3` |
| `BLEND_MODE_DISABLED` | `4` |

## BoneConstraint3D.ReferenceType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REFERENCE_TYPE_BONE` | `0` |
| `REFERENCE_TYPE_NODE` | `1` |

## BoneTwistDisperser3D.DisperseMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISPERSE_MODE_EVEN` | `0` |
| `DISPERSE_MODE_WEIGHTED` | `1` |
| `DISPERSE_MODE_CUSTOM` | `2` |

## BoxContainer.AlignmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALIGNMENT_BEGIN` | `0` |
| `ALIGNMENT_CENTER` | `1` |
| `ALIGNMENT_END` | `2` |

## CPUParticles2D.DrawOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAW_ORDER_INDEX` | `0` |
| `DRAW_ORDER_LIFETIME` | `1` |

## CPUParticles2D.Parameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_INITIAL_LINEAR_VELOCITY` | `0` |
| `PARAM_ANGULAR_VELOCITY` | `1` |
| `PARAM_ORBIT_VELOCITY` | `2` |
| `PARAM_LINEAR_ACCEL` | `3` |
| `PARAM_RADIAL_ACCEL` | `4` |
| `PARAM_TANGENTIAL_ACCEL` | `5` |
| `PARAM_DAMPING` | `6` |
| `PARAM_ANGLE` | `7` |
| `PARAM_SCALE` | `8` |
| `PARAM_HUE_VARIATION` | `9` |
| `PARAM_ANIM_SPEED` | `10` |
| `PARAM_ANIM_OFFSET` | `11` |
| `PARAM_MAX` | `12` |

## CPUParticles2D.ParticleFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLE_FLAG_ALIGN_Y_TO_VELOCITY` | `0` |
| `PARTICLE_FLAG_ROTATE_Y` | `1` |
| `PARTICLE_FLAG_DISABLE_Z` | `2` |
| `PARTICLE_FLAG_MAX` | `3` |

## CPUParticles2D.EmissionShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMISSION_SHAPE_POINT` | `0` |
| `EMISSION_SHAPE_SPHERE` | `1` |
| `EMISSION_SHAPE_SPHERE_SURFACE` | `2` |
| `EMISSION_SHAPE_RECTANGLE` | `3` |
| `EMISSION_SHAPE_POINTS` | `4` |
| `EMISSION_SHAPE_DIRECTED_POINTS` | `5` |
| `EMISSION_SHAPE_RING` | `6` |
| `EMISSION_SHAPE_MAX` | `7` |

## CPUParticles3D.DrawOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAW_ORDER_INDEX` | `0` |
| `DRAW_ORDER_LIFETIME` | `1` |
| `DRAW_ORDER_VIEW_DEPTH` | `2` |

## CPUParticles3D.Parameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_INITIAL_LINEAR_VELOCITY` | `0` |
| `PARAM_ANGULAR_VELOCITY` | `1` |
| `PARAM_ORBIT_VELOCITY` | `2` |
| `PARAM_LINEAR_ACCEL` | `3` |
| `PARAM_RADIAL_ACCEL` | `4` |
| `PARAM_TANGENTIAL_ACCEL` | `5` |
| `PARAM_DAMPING` | `6` |
| `PARAM_ANGLE` | `7` |
| `PARAM_SCALE` | `8` |
| `PARAM_HUE_VARIATION` | `9` |
| `PARAM_ANIM_SPEED` | `10` |
| `PARAM_ANIM_OFFSET` | `11` |
| `PARAM_MAX` | `12` |

## CPUParticles3D.ParticleFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLE_FLAG_ALIGN_Y_TO_VELOCITY` | `0` |
| `PARTICLE_FLAG_ROTATE_Y` | `1` |
| `PARTICLE_FLAG_DISABLE_Z` | `2` |
| `PARTICLE_FLAG_MAX` | `3` |

## CPUParticles3D.EmissionShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMISSION_SHAPE_POINT` | `0` |
| `EMISSION_SHAPE_SPHERE` | `1` |
| `EMISSION_SHAPE_SPHERE_SURFACE` | `2` |
| `EMISSION_SHAPE_BOX` | `3` |
| `EMISSION_SHAPE_POINTS` | `4` |
| `EMISSION_SHAPE_DIRECTED_POINTS` | `5` |
| `EMISSION_SHAPE_RING` | `6` |
| `EMISSION_SHAPE_MAX` | `7` |

## CSGPolygon3D.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_DEPTH` | `0` |
| `MODE_SPIN` | `1` |
| `MODE_PATH` | `2` |

## CSGPolygon3D.PathRotation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATH_ROTATION_POLYGON` | `0` |
| `PATH_ROTATION_PATH` | `1` |
| `PATH_ROTATION_PATH_FOLLOW` | `2` |

## CSGPolygon3D.PathIntervalType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATH_INTERVAL_DISTANCE` | `0` |
| `PATH_INTERVAL_SUBDIVIDE` | `1` |

## CSGShape3D.Operation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OPERATION_UNION` | `0` |
| `OPERATION_INTERSECTION` | `1` |
| `OPERATION_SUBTRACTION` | `2` |

## Camera2D.AnchorMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANCHOR_MODE_FIXED_TOP_LEFT` | `0` |
| `ANCHOR_MODE_DRAG_CENTER` | `1` |

## Camera2D.Camera2DProcessCallback

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CAMERA2D_PROCESS_PHYSICS` | `0` |
| `CAMERA2D_PROCESS_IDLE` | `1` |

## Camera3D.ProjectionType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PROJECTION_PERSPECTIVE` | `0` |
| `PROJECTION_ORTHOGONAL` | `1` |
| `PROJECTION_FRUSTUM` | `2` |

## Camera3D.KeepAspect

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `KEEP_WIDTH` | `0` |
| `KEEP_HEIGHT` | `1` |

## Camera3D.DopplerTracking

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOPPLER_TRACKING_DISABLED` | `0` |
| `DOPPLER_TRACKING_IDLE_STEP` | `1` |
| `DOPPLER_TRACKING_PHYSICS_STEP` | `2` |

## CameraFeed.FeedDataType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEED_NOIMAGE` | `0` |
| `FEED_RGB` | `1` |
| `FEED_YCBCR` | `2` |
| `FEED_YCBCR_SEP` | `3` |
| `FEED_EXTERNAL` | `4` |

## CameraFeed.FeedPosition

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEED_UNSPECIFIED` | `0` |
| `FEED_FRONT` | `1` |
| `FEED_BACK` | `2` |

## CameraServer.FeedImage

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEED_RGBA_IMAGE` | `0` |
| `FEED_YCBCR_IMAGE` | `0` |
| `FEED_Y_IMAGE` | `0` |
| `FEED_CBCR_IMAGE` | `1` |

## CanvasItem.TextureFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_FILTER_PARENT_NODE` | `0` |
| `TEXTURE_FILTER_NEAREST` | `1` |
| `TEXTURE_FILTER_LINEAR` | `2` |
| `TEXTURE_FILTER_NEAREST_WITH_MIPMAPS` | `3` |
| `TEXTURE_FILTER_LINEAR_WITH_MIPMAPS` | `4` |
| `TEXTURE_FILTER_NEAREST_WITH_MIPMAPS_ANISOTROPIC` | `5` |
| `TEXTURE_FILTER_LINEAR_WITH_MIPMAPS_ANISOTROPIC` | `6` |
| `TEXTURE_FILTER_MAX` | `7` |

## CanvasItem.TextureRepeat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_REPEAT_PARENT_NODE` | `0` |
| `TEXTURE_REPEAT_DISABLED` | `1` |
| `TEXTURE_REPEAT_ENABLED` | `2` |
| `TEXTURE_REPEAT_MIRROR` | `3` |
| `TEXTURE_REPEAT_MAX` | `4` |

## CanvasItem.ClipChildrenMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CLIP_CHILDREN_DISABLED` | `0` |
| `CLIP_CHILDREN_ONLY` | `1` |
| `CLIP_CHILDREN_AND_DRAW` | `2` |
| `CLIP_CHILDREN_MAX` | `3` |

## CanvasItem.OversamplingWithScale

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OVERSAMPLING_WITH_SCALE_PARENT_NODE` | `0` |
| `OVERSAMPLING_WITH_SCALE_DISABLED` | `1` |
| `OVERSAMPLING_WITH_SCALE_ENABLED` | `2` |
| `OVERSAMPLING_WITH_SCALE_MAX` | `3` |

## CanvasItemMaterial.BlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_MODE_MIX` | `0` |
| `BLEND_MODE_ADD` | `1` |
| `BLEND_MODE_SUB` | `2` |
| `BLEND_MODE_MUL` | `3` |
| `BLEND_MODE_PREMULT_ALPHA` | `4` |

## CanvasItemMaterial.LightMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_MODE_NORMAL` | `0` |
| `LIGHT_MODE_UNSHADED` | `1` |
| `LIGHT_MODE_LIGHT_ONLY` | `2` |

## CharacterBody2D.MotionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOTION_MODE_GROUNDED` | `0` |
| `MOTION_MODE_FLOATING` | `1` |

## CharacterBody2D.PlatformOnLeave

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLATFORM_ON_LEAVE_ADD_VELOCITY` | `0` |
| `PLATFORM_ON_LEAVE_ADD_UPWARD_VELOCITY` | `1` |
| `PLATFORM_ON_LEAVE_DO_NOTHING` | `2` |

## CharacterBody3D.MotionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOTION_MODE_GROUNDED` | `0` |
| `MOTION_MODE_FLOATING` | `1` |

## CharacterBody3D.PlatformOnLeave

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLATFORM_ON_LEAVE_ADD_VELOCITY` | `0` |
| `PLATFORM_ON_LEAVE_ADD_UPWARD_VELOCITY` | `1` |
| `PLATFORM_ON_LEAVE_DO_NOTHING` | `2` |

## ClassDB.APIType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `API_CORE` | `0` |
| `API_EDITOR` | `1` |
| `API_EXTENSION` | `2` |
| `API_EDITOR_EXTENSION` | `3` |
| `API_NONE` | `4` |

## CodeEdit.CodeCompletionKind

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `KIND_CLASS` | `0` |
| `KIND_FUNCTION` | `1` |
| `KIND_SIGNAL` | `2` |
| `KIND_VARIABLE` | `3` |
| `KIND_MEMBER` | `4` |
| `KIND_ENUM` | `5` |
| `KIND_CONSTANT` | `6` |
| `KIND_NODE_PATH` | `7` |
| `KIND_FILE_PATH` | `8` |
| `KIND_PLAIN_TEXT` | `9` |
| `KIND_KEYWORD` | `10` |

## CodeEdit.CodeCompletionLocation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOCATION_LOCAL` | `0` |
| `LOCATION_PARENT_MASK` | `256` |
| `LOCATION_OTHER_USER_CODE` | `512` |
| `LOCATION_OTHER` | `1024` |

## CollisionObject2D.DisableMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISABLE_MODE_REMOVE` | `0` |
| `DISABLE_MODE_MAKE_STATIC` | `1` |
| `DISABLE_MODE_KEEP_ACTIVE` | `2` |

## CollisionObject3D.DisableMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISABLE_MODE_REMOVE` | `0` |
| `DISABLE_MODE_MAKE_STATIC` | `1` |
| `DISABLE_MODE_KEEP_ACTIVE` | `2` |

## CollisionPolygon2D.BuildMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BUILD_SOLIDS` | `0` |
| `BUILD_SEGMENTS` | `1` |

## ColorPicker.ColorModeType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_RGB` | `0` |
| `MODE_HSV` | `1` |
| `MODE_RAW` | `2` |
| `MODE_LINEAR` | `2` |
| `MODE_OKHSL` | `3` |

## ColorPicker.PickerShapeType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHAPE_HSV_RECTANGLE` | `0` |
| `SHAPE_HSV_WHEEL` | `1` |
| `SHAPE_VHS_CIRCLE` | `2` |
| `SHAPE_OKHSL_CIRCLE` | `3` |
| `SHAPE_NONE` | `4` |
| `SHAPE_OK_HS_RECTANGLE` | `5` |
| `SHAPE_OK_HL_RECTANGLE` | `6` |

## CompositorEffect.EffectCallbackType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EFFECT_CALLBACK_TYPE_PRE_OPAQUE` | `0` |
| `EFFECT_CALLBACK_TYPE_POST_OPAQUE` | `1` |
| `EFFECT_CALLBACK_TYPE_POST_SKY` | `2` |
| `EFFECT_CALLBACK_TYPE_PRE_TRANSPARENT` | `3` |
| `EFFECT_CALLBACK_TYPE_POST_TRANSPARENT` | `4` |
| `EFFECT_CALLBACK_TYPE_MAX` | `5` |

## ConeTwistJoint3D.Param

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_SWING_SPAN` | `0` |
| `PARAM_TWIST_SPAN` | `1` |
| `PARAM_BIAS` | `2` |
| `PARAM_SOFTNESS` | `3` |
| `PARAM_RELAXATION` | `4` |
| `PARAM_MAX` | `5` |

## Control.FocusMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FOCUS_NONE` | `0` |
| `FOCUS_CLICK` | `1` |
| `FOCUS_ALL` | `2` |
| `FOCUS_ACCESSIBILITY` | `3` |

## Control.FocusBehaviorRecursive

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FOCUS_BEHAVIOR_INHERITED` | `0` |
| `FOCUS_BEHAVIOR_DISABLED` | `1` |
| `FOCUS_BEHAVIOR_ENABLED` | `2` |

## Control.MouseBehaviorRecursive

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOUSE_BEHAVIOR_INHERITED` | `0` |
| `MOUSE_BEHAVIOR_DISABLED` | `1` |
| `MOUSE_BEHAVIOR_ENABLED` | `2` |

## Control.CursorShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CURSOR_ARROW` | `0` |
| `CURSOR_IBEAM` | `1` |
| `CURSOR_POINTING_HAND` | `2` |
| `CURSOR_CROSS` | `3` |
| `CURSOR_WAIT` | `4` |
| `CURSOR_BUSY` | `5` |
| `CURSOR_DRAG` | `6` |
| `CURSOR_CAN_DROP` | `7` |
| `CURSOR_FORBIDDEN` | `8` |
| `CURSOR_VSIZE` | `9` |
| `CURSOR_HSIZE` | `10` |
| `CURSOR_BDIAGSIZE` | `11` |
| `CURSOR_FDIAGSIZE` | `12` |
| `CURSOR_MOVE` | `13` |
| `CURSOR_VSPLIT` | `14` |
| `CURSOR_HSPLIT` | `15` |
| `CURSOR_HELP` | `16` |

## Control.LayoutPreset

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PRESET_TOP_LEFT` | `0` |
| `PRESET_TOP_RIGHT` | `1` |
| `PRESET_BOTTOM_LEFT` | `2` |
| `PRESET_BOTTOM_RIGHT` | `3` |
| `PRESET_CENTER_LEFT` | `4` |
| `PRESET_CENTER_TOP` | `5` |
| `PRESET_CENTER_RIGHT` | `6` |
| `PRESET_CENTER_BOTTOM` | `7` |
| `PRESET_CENTER` | `8` |
| `PRESET_LEFT_WIDE` | `9` |
| `PRESET_TOP_WIDE` | `10` |
| `PRESET_RIGHT_WIDE` | `11` |
| `PRESET_BOTTOM_WIDE` | `12` |
| `PRESET_VCENTER_WIDE` | `13` |
| `PRESET_HCENTER_WIDE` | `14` |
| `PRESET_FULL_RECT` | `15` |

## Control.LayoutPresetMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PRESET_MODE_MINSIZE` | `0` |
| `PRESET_MODE_KEEP_WIDTH` | `1` |
| `PRESET_MODE_KEEP_HEIGHT` | `2` |
| `PRESET_MODE_KEEP_SIZE` | `3` |

## Control.SizeFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `SIZE_SHRINK_BEGIN` | `0` |
| `SIZE_FILL` | `1` |
| `SIZE_EXPAND` | `2` |
| `SIZE_EXPAND_FILL` | `3` |
| `SIZE_SHRINK_CENTER` | `4` |
| `SIZE_SHRINK_END` | `8` |

## Control.MouseFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOUSE_FILTER_STOP` | `0` |
| `MOUSE_FILTER_PASS` | `1` |
| `MOUSE_FILTER_IGNORE` | `2` |

## Control.GrowDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GROW_DIRECTION_BEGIN` | `0` |
| `GROW_DIRECTION_END` | `1` |
| `GROW_DIRECTION_BOTH` | `2` |

## Control.Anchor

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANCHOR_BEGIN` | `0` |
| `ANCHOR_END` | `1` |

## Control.LayoutDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LAYOUT_DIRECTION_INHERITED` | `0` |
| `LAYOUT_DIRECTION_APPLICATION_LOCALE` | `1` |
| `LAYOUT_DIRECTION_LTR` | `2` |
| `LAYOUT_DIRECTION_RTL` | `3` |
| `LAYOUT_DIRECTION_SYSTEM_LOCALE` | `4` |
| `LAYOUT_DIRECTION_MAX` | `5` |
| `LAYOUT_DIRECTION_LOCALE` | `1` |

## Control.TextDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXT_DIRECTION_INHERITED` | `3` |
| `TEXT_DIRECTION_AUTO` | `0` |
| `TEXT_DIRECTION_LTR` | `1` |
| `TEXT_DIRECTION_RTL` | `2` |

## ConvertTransformModifier3D.TransformMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSFORM_MODE_POSITION` | `0` |
| `TRANSFORM_MODE_ROTATION` | `1` |
| `TRANSFORM_MODE_SCALE` | `2` |

## CopyTransformModifier3D.TransformFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `TRANSFORM_FLAG_POSITION` | `1` |
| `TRANSFORM_FLAG_ROTATION` | `2` |
| `TRANSFORM_FLAG_SCALE` | `4` |
| `TRANSFORM_FLAG_ALL` | `7` |

## CopyTransformModifier3D.AxisFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `AXIS_FLAG_X` | `1` |
| `AXIS_FLAG_Y` | `2` |
| `AXIS_FLAG_Z` | `4` |
| `AXIS_FLAG_ALL` | `7` |

## Curve.TangentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TANGENT_FREE` | `0` |
| `TANGENT_LINEAR` | `1` |
| `TANGENT_MODE_COUNT` | `2` |

## CurveTexture.TextureMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_MODE_RGB` | `0` |
| `TEXTURE_MODE_RED` | `1` |

## Decal.DecalTexture

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_ALBEDO` | `0` |
| `TEXTURE_NORMAL` | `1` |
| `TEXTURE_ORM` | `2` |
| `TEXTURE_EMISSION` | `3` |
| `TEXTURE_MAX` | `4` |

## DirectionalLight3D.ShadowMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_ORTHOGONAL` | `0` |
| `SHADOW_PARALLEL_2_SPLITS` | `1` |
| `SHADOW_PARALLEL_4_SPLITS` | `2` |

## DirectionalLight3D.SkyMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SKY_MODE_LIGHT_AND_SKY` | `0` |
| `SKY_MODE_LIGHT_ONLY` | `1` |
| `SKY_MODE_SKY_ONLY` | `2` |

## DisplayServer.Feature

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEATURE_GLOBAL_MENU` | `0` |
| `FEATURE_SUBWINDOWS` | `1` |
| `FEATURE_TOUCHSCREEN` | `2` |
| `FEATURE_MOUSE` | `3` |
| `FEATURE_MOUSE_WARP` | `4` |
| `FEATURE_CLIPBOARD` | `5` |
| `FEATURE_VIRTUAL_KEYBOARD` | `6` |
| `FEATURE_CURSOR_SHAPE` | `7` |
| `FEATURE_CUSTOM_CURSOR_SHAPE` | `8` |
| `FEATURE_NATIVE_DIALOG` | `9` |
| `FEATURE_IME` | `10` |
| `FEATURE_WINDOW_TRANSPARENCY` | `11` |
| `FEATURE_HIDPI` | `12` |
| `FEATURE_ICON` | `13` |
| `FEATURE_NATIVE_ICON` | `14` |
| `FEATURE_ORIENTATION` | `15` |
| `FEATURE_SWAP_BUFFERS` | `16` |
| `FEATURE_CLIPBOARD_PRIMARY` | `18` |
| `FEATURE_TEXT_TO_SPEECH` | `19` |
| `FEATURE_EXTEND_TO_TITLE` | `20` |
| `FEATURE_SCREEN_CAPTURE` | `21` |
| `FEATURE_STATUS_INDICATOR` | `22` |
| `FEATURE_NATIVE_HELP` | `23` |
| `FEATURE_NATIVE_DIALOG_INPUT` | `24` |
| `FEATURE_NATIVE_DIALOG_FILE` | `25` |
| `FEATURE_NATIVE_DIALOG_FILE_EXTRA` | `26` |
| `FEATURE_WINDOW_DRAG` | `27` |
| `FEATURE_SCREEN_EXCLUDE_FROM_CAPTURE` | `28` |
| `FEATURE_WINDOW_EMBEDDING` | `29` |
| `FEATURE_NATIVE_DIALOG_FILE_MIME` | `30` |
| `FEATURE_EMOJI_AND_SYMBOL_PICKER` | `31` |
| `FEATURE_NATIVE_COLOR_PICKER` | `32` |
| `FEATURE_SELF_FITTING_WINDOWS` | `33` |
| `FEATURE_ACCESSIBILITY_SCREEN_READER` | `34` |
| `FEATURE_HDR_OUTPUT` | `35` |
| `FEATURE_PIP_MODE` | `36` |

## DisplayServer.AccessibilityRole

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ROLE_UNKNOWN` | `0` |
| `ROLE_DEFAULT_BUTTON` | `1` |
| `ROLE_AUDIO` | `2` |
| `ROLE_VIDEO` | `3` |
| `ROLE_STATIC_TEXT` | `4` |
| `ROLE_CONTAINER` | `5` |
| `ROLE_PANEL` | `6` |
| `ROLE_BUTTON` | `7` |
| `ROLE_LINK` | `8` |
| `ROLE_CHECK_BOX` | `9` |
| `ROLE_RADIO_BUTTON` | `10` |
| `ROLE_CHECK_BUTTON` | `11` |
| `ROLE_SCROLL_BAR` | `12` |
| `ROLE_SCROLL_VIEW` | `13` |
| `ROLE_SPLITTER` | `14` |
| `ROLE_SLIDER` | `15` |
| `ROLE_SPIN_BUTTON` | `16` |
| `ROLE_PROGRESS_INDICATOR` | `17` |
| `ROLE_TEXT_FIELD` | `18` |
| `ROLE_MULTILINE_TEXT_FIELD` | `19` |
| `ROLE_COLOR_PICKER` | `20` |
| `ROLE_TABLE` | `21` |
| `ROLE_CELL` | `22` |
| `ROLE_ROW` | `23` |
| `ROLE_ROW_GROUP` | `24` |
| `ROLE_ROW_HEADER` | `25` |
| `ROLE_COLUMN_HEADER` | `26` |
| `ROLE_TREE` | `27` |
| `ROLE_TREE_ITEM` | `28` |
| `ROLE_LIST` | `29` |
| `ROLE_LIST_ITEM` | `30` |
| `ROLE_LIST_BOX` | `31` |
| `ROLE_LIST_BOX_OPTION` | `32` |
| `ROLE_TAB_BAR` | `33` |
| `ROLE_TAB` | `34` |
| `ROLE_TAB_PANEL` | `35` |
| `ROLE_MENU_BAR` | `36` |
| `ROLE_MENU` | `37` |
| `ROLE_MENU_ITEM` | `38` |
| `ROLE_MENU_ITEM_CHECK_BOX` | `39` |
| `ROLE_MENU_ITEM_RADIO` | `40` |
| `ROLE_IMAGE` | `41` |
| `ROLE_WINDOW` | `42` |
| `ROLE_TITLE_BAR` | `43` |
| `ROLE_DIALOG` | `44` |
| `ROLE_TOOLTIP` | `45` |
| `ROLE_REGION` | `46` |
| `ROLE_TEXT_RUN` | `47` |

## DisplayServer.AccessibilityPopupType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `POPUP_MENU` | `0` |
| `POPUP_LIST` | `1` |
| `POPUP_TREE` | `2` |
| `POPUP_DIALOG` | `3` |

## DisplayServer.AccessibilityFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_HIDDEN` | `0` |
| `FLAG_MULTISELECTABLE` | `1` |
| `FLAG_REQUIRED` | `2` |
| `FLAG_VISITED` | `3` |
| `FLAG_BUSY` | `4` |
| `FLAG_MODAL` | `5` |
| `FLAG_TOUCH_PASSTHROUGH` | `6` |
| `FLAG_READONLY` | `7` |
| `FLAG_DISABLED` | `8` |
| `FLAG_CLIPS_CHILDREN` | `9` |

## DisplayServer.AccessibilityAction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ACTION_CLICK` | `0` |
| `ACTION_FOCUS` | `1` |
| `ACTION_BLUR` | `2` |
| `ACTION_COLLAPSE` | `3` |
| `ACTION_EXPAND` | `4` |
| `ACTION_DECREMENT` | `5` |
| `ACTION_INCREMENT` | `6` |
| `ACTION_HIDE_TOOLTIP` | `7` |
| `ACTION_SHOW_TOOLTIP` | `8` |
| `ACTION_SET_TEXT_SELECTION` | `9` |
| `ACTION_REPLACE_SELECTED_TEXT` | `10` |
| `ACTION_SCROLL_BACKWARD` | `11` |
| `ACTION_SCROLL_DOWN` | `12` |
| `ACTION_SCROLL_FORWARD` | `13` |
| `ACTION_SCROLL_LEFT` | `14` |
| `ACTION_SCROLL_RIGHT` | `15` |
| `ACTION_SCROLL_UP` | `16` |
| `ACTION_SCROLL_INTO_VIEW` | `17` |
| `ACTION_SCROLL_TO_POINT` | `18` |
| `ACTION_SET_SCROLL_OFFSET` | `19` |
| `ACTION_SET_VALUE` | `20` |
| `ACTION_SHOW_CONTEXT_MENU` | `21` |
| `ACTION_CUSTOM` | `22` |

## DisplayServer.AccessibilityLiveMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIVE_OFF` | `0` |
| `LIVE_POLITE` | `1` |
| `LIVE_ASSERTIVE` | `2` |

## DisplayServer.AccessibilityScrollUnit

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_UNIT_ITEM` | `0` |
| `SCROLL_UNIT_PAGE` | `1` |

## DisplayServer.AccessibilityScrollHint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_HINT_TOP_LEFT` | `0` |
| `SCROLL_HINT_BOTTOM_RIGHT` | `1` |
| `SCROLL_HINT_TOP_EDGE` | `2` |
| `SCROLL_HINT_BOTTOM_EDGE` | `3` |
| `SCROLL_HINT_LEFT_EDGE` | `4` |
| `SCROLL_HINT_RIGHT_EDGE` | `5` |

## DisplayServer.MouseMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOUSE_MODE_VISIBLE` | `0` |
| `MOUSE_MODE_HIDDEN` | `1` |
| `MOUSE_MODE_CAPTURED` | `2` |
| `MOUSE_MODE_CONFINED` | `3` |
| `MOUSE_MODE_CONFINED_HIDDEN` | `4` |
| `MOUSE_MODE_MAX` | `5` |

## DisplayServer.ScreenOrientation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCREEN_LANDSCAPE` | `0` |
| `SCREEN_PORTRAIT` | `1` |
| `SCREEN_REVERSE_LANDSCAPE` | `2` |
| `SCREEN_REVERSE_PORTRAIT` | `3` |
| `SCREEN_SENSOR_LANDSCAPE` | `4` |
| `SCREEN_SENSOR_PORTRAIT` | `5` |
| `SCREEN_SENSOR` | `6` |

## DisplayServer.VirtualKeyboardType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `KEYBOARD_TYPE_DEFAULT` | `0` |
| `KEYBOARD_TYPE_MULTILINE` | `1` |
| `KEYBOARD_TYPE_NUMBER` | `2` |
| `KEYBOARD_TYPE_NUMBER_DECIMAL` | `3` |
| `KEYBOARD_TYPE_PHONE` | `4` |
| `KEYBOARD_TYPE_EMAIL_ADDRESS` | `5` |
| `KEYBOARD_TYPE_PASSWORD` | `6` |
| `KEYBOARD_TYPE_URL` | `7` |

## DisplayServer.CursorShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CURSOR_ARROW` | `0` |
| `CURSOR_IBEAM` | `1` |
| `CURSOR_POINTING_HAND` | `2` |
| `CURSOR_CROSS` | `3` |
| `CURSOR_WAIT` | `4` |
| `CURSOR_BUSY` | `5` |
| `CURSOR_DRAG` | `6` |
| `CURSOR_CAN_DROP` | `7` |
| `CURSOR_FORBIDDEN` | `8` |
| `CURSOR_VSIZE` | `9` |
| `CURSOR_HSIZE` | `10` |
| `CURSOR_BDIAGSIZE` | `11` |
| `CURSOR_FDIAGSIZE` | `12` |
| `CURSOR_MOVE` | `13` |
| `CURSOR_VSPLIT` | `14` |
| `CURSOR_HSPLIT` | `15` |
| `CURSOR_HELP` | `16` |
| `CURSOR_MAX` | `17` |

## DisplayServer.FileDialogMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILE_DIALOG_MODE_OPEN_FILE` | `0` |
| `FILE_DIALOG_MODE_OPEN_FILES` | `1` |
| `FILE_DIALOG_MODE_OPEN_DIR` | `2` |
| `FILE_DIALOG_MODE_OPEN_ANY` | `3` |
| `FILE_DIALOG_MODE_SAVE_FILE` | `4` |

## DisplayServer.WindowMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WINDOW_MODE_WINDOWED` | `0` |
| `WINDOW_MODE_MINIMIZED` | `1` |
| `WINDOW_MODE_MAXIMIZED` | `2` |
| `WINDOW_MODE_FULLSCREEN` | `3` |
| `WINDOW_MODE_EXCLUSIVE_FULLSCREEN` | `4` |

## DisplayServer.ProgressState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PROGRESS_STATE_NOPROGRESS` | `0` |
| `PROGRESS_STATE_INDETERMINATE` | `1` |
| `PROGRESS_STATE_NORMAL` | `2` |
| `PROGRESS_STATE_ERROR` | `3` |
| `PROGRESS_STATE_PAUSED` | `4` |

## DisplayServer.WindowFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WINDOW_FLAG_RESIZE_DISABLED` | `0` |
| `WINDOW_FLAG_BORDERLESS` | `1` |
| `WINDOW_FLAG_ALWAYS_ON_TOP` | `2` |
| `WINDOW_FLAG_TRANSPARENT` | `3` |
| `WINDOW_FLAG_NO_FOCUS` | `4` |
| `WINDOW_FLAG_POPUP` | `5` |
| `WINDOW_FLAG_EXTEND_TO_TITLE` | `6` |
| `WINDOW_FLAG_MOUSE_PASSTHROUGH` | `7` |
| `WINDOW_FLAG_SHARP_CORNERS` | `8` |
| `WINDOW_FLAG_EXCLUDE_FROM_CAPTURE` | `9` |
| `WINDOW_FLAG_POPUP_WM_HINT` | `10` |
| `WINDOW_FLAG_MINIMIZE_DISABLED` | `11` |
| `WINDOW_FLAG_MAXIMIZE_DISABLED` | `12` |
| `WINDOW_FLAG_MAX` | `13` |

## DisplayServer.WindowEvent

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WINDOW_EVENT_MOUSE_ENTER` | `0` |
| `WINDOW_EVENT_MOUSE_EXIT` | `1` |
| `WINDOW_EVENT_FOCUS_IN` | `2` |
| `WINDOW_EVENT_FOCUS_OUT` | `3` |
| `WINDOW_EVENT_CLOSE_REQUEST` | `4` |
| `WINDOW_EVENT_GO_BACK_REQUEST` | `5` |
| `WINDOW_EVENT_DPI_CHANGE` | `6` |
| `WINDOW_EVENT_TITLEBAR_CHANGE` | `7` |
| `WINDOW_EVENT_FORCE_CLOSE` | `8` |
| `WINDOW_EVENT_OUTPUT_MAX_LINEAR_VALUE_CHANGED` | `9` |

## DisplayServer.WindowResizeEdge

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WINDOW_EDGE_TOP_LEFT` | `0` |
| `WINDOW_EDGE_TOP` | `1` |
| `WINDOW_EDGE_TOP_RIGHT` | `2` |
| `WINDOW_EDGE_LEFT` | `3` |
| `WINDOW_EDGE_RIGHT` | `4` |
| `WINDOW_EDGE_BOTTOM_LEFT` | `5` |
| `WINDOW_EDGE_BOTTOM` | `6` |
| `WINDOW_EDGE_BOTTOM_RIGHT` | `7` |
| `WINDOW_EDGE_MAX` | `8` |

## DisplayServer.VSyncMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VSYNC_DISABLED` | `0` |
| `VSYNC_ENABLED` | `1` |
| `VSYNC_ADAPTIVE` | `2` |
| `VSYNC_MAILBOX` | `3` |

## DisplayServer.HandleType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISPLAY_HANDLE` | `0` |
| `WINDOW_HANDLE` | `1` |
| `WINDOW_VIEW` | `2` |
| `OPENGL_CONTEXT` | `3` |
| `EGL_DISPLAY` | `4` |
| `EGL_CONFIG` | `5` |
| `GLX_VISUALID` | `6` |
| `GLX_FBCONFIG` | `7` |

## DisplayServer.TTSUtteranceEvent

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TTS_UTTERANCE_STARTED` | `0` |
| `TTS_UTTERANCE_ENDED` | `1` |
| `TTS_UTTERANCE_CANCELED` | `2` |
| `TTS_UTTERANCE_BOUNDARY` | `3` |

## DrawableTexture2D.DrawableFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAWABLE_FORMAT_RGBA8` | `0` |
| `DRAWABLE_FORMAT_RGBA8_SRGB` | `1` |
| `DRAWABLE_FORMAT_RGBAH` | `2` |
| `DRAWABLE_FORMAT_RGBAF` | `3` |

## ENetConnection.CompressionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPRESS_NONE` | `0` |
| `COMPRESS_RANGE_CODER` | `1` |
| `COMPRESS_FASTLZ` | `2` |
| `COMPRESS_ZLIB` | `3` |
| `COMPRESS_ZSTD` | `4` |

## ENetConnection.EventType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EVENT_ERROR` | `-1` |
| `EVENT_NONE` | `0` |
| `EVENT_CONNECT` | `1` |
| `EVENT_DISCONNECT` | `2` |
| `EVENT_RECEIVE` | `3` |

## ENetConnection.HostStatistic

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HOST_TOTAL_SENT_DATA` | `0` |
| `HOST_TOTAL_SENT_PACKETS` | `1` |
| `HOST_TOTAL_RECEIVED_DATA` | `2` |
| `HOST_TOTAL_RECEIVED_PACKETS` | `3` |

## ENetPacketPeer.PeerState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATE_DISCONNECTED` | `0` |
| `STATE_CONNECTING` | `1` |
| `STATE_ACKNOWLEDGING_CONNECT` | `2` |
| `STATE_CONNECTION_PENDING` | `3` |
| `STATE_CONNECTION_SUCCEEDED` | `4` |
| `STATE_CONNECTED` | `5` |
| `STATE_DISCONNECT_LATER` | `6` |
| `STATE_DISCONNECTING` | `7` |
| `STATE_ACKNOWLEDGING_DISCONNECT` | `8` |
| `STATE_ZOMBIE` | `9` |

## ENetPacketPeer.PeerStatistic

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PEER_PACKET_LOSS` | `0` |
| `PEER_PACKET_LOSS_VARIANCE` | `1` |
| `PEER_PACKET_LOSS_EPOCH` | `2` |
| `PEER_ROUND_TRIP_TIME` | `3` |
| `PEER_ROUND_TRIP_TIME_VARIANCE` | `4` |
| `PEER_LAST_ROUND_TRIP_TIME` | `5` |
| `PEER_LAST_ROUND_TRIP_TIME_VARIANCE` | `6` |
| `PEER_PACKET_THROTTLE` | `7` |
| `PEER_PACKET_THROTTLE_LIMIT` | `8` |
| `PEER_PACKET_THROTTLE_COUNTER` | `9` |
| `PEER_PACKET_THROTTLE_EPOCH` | `10` |
| `PEER_PACKET_THROTTLE_ACCELERATION` | `11` |
| `PEER_PACKET_THROTTLE_DECELERATION` | `12` |
| `PEER_PACKET_THROTTLE_INTERVAL` | `13` |

## Environment.BGMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BG_CLEAR_COLOR` | `0` |
| `BG_COLOR` | `1` |
| `BG_SKY` | `2` |
| `BG_CANVAS` | `3` |
| `BG_KEEP` | `4` |
| `BG_CAMERA_FEED` | `5` |
| `BG_MAX` | `6` |

## Environment.AmbientSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AMBIENT_SOURCE_BG` | `0` |
| `AMBIENT_SOURCE_DISABLED` | `1` |
| `AMBIENT_SOURCE_COLOR` | `2` |
| `AMBIENT_SOURCE_SKY` | `3` |

## Environment.ReflectionSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REFLECTION_SOURCE_BG` | `0` |
| `REFLECTION_SOURCE_DISABLED` | `1` |
| `REFLECTION_SOURCE_SKY` | `2` |

## Environment.ToneMapper

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TONE_MAPPER_LINEAR` | `0` |
| `TONE_MAPPER_REINHARDT` | `1` |
| `TONE_MAPPER_FILMIC` | `2` |
| `TONE_MAPPER_ACES` | `3` |
| `TONE_MAPPER_AGX` | `4` |

## Environment.GlowBlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GLOW_BLEND_MODE_ADDITIVE` | `0` |
| `GLOW_BLEND_MODE_SCREEN` | `1` |
| `GLOW_BLEND_MODE_SOFTLIGHT` | `2` |
| `GLOW_BLEND_MODE_REPLACE` | `3` |
| `GLOW_BLEND_MODE_MIX` | `4` |

## Environment.FogMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FOG_MODE_EXPONENTIAL` | `0` |
| `FOG_MODE_DEPTH` | `1` |

## Environment.SDFGIYScale

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SDFGI_Y_SCALE_50_PERCENT` | `0` |
| `SDFGI_Y_SCALE_75_PERCENT` | `1` |
| `SDFGI_Y_SCALE_100_PERCENT` | `2` |

## FastNoiseLite.NoiseType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_VALUE` | `5` |
| `TYPE_VALUE_CUBIC` | `4` |
| `TYPE_PERLIN` | `3` |
| `TYPE_CELLULAR` | `2` |
| `TYPE_SIMPLEX` | `0` |
| `TYPE_SIMPLEX_SMOOTH` | `1` |

## FastNoiseLite.FractalType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FRACTAL_NONE` | `0` |
| `FRACTAL_FBM` | `1` |
| `FRACTAL_RIDGED` | `2` |
| `FRACTAL_PING_PONG` | `3` |

## FastNoiseLite.CellularDistanceFunction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISTANCE_EUCLIDEAN` | `0` |
| `DISTANCE_EUCLIDEAN_SQUARED` | `1` |
| `DISTANCE_MANHATTAN` | `2` |
| `DISTANCE_HYBRID` | `3` |

## FastNoiseLite.CellularReturnType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RETURN_CELL_VALUE` | `0` |
| `RETURN_DISTANCE` | `1` |
| `RETURN_DISTANCE2` | `2` |
| `RETURN_DISTANCE2_ADD` | `3` |
| `RETURN_DISTANCE2_SUB` | `4` |
| `RETURN_DISTANCE2_MUL` | `5` |
| `RETURN_DISTANCE2_DIV` | `6` |

## FastNoiseLite.DomainWarpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOMAIN_WARP_SIMPLEX` | `0` |
| `DOMAIN_WARP_SIMPLEX_REDUCED` | `1` |
| `DOMAIN_WARP_BASIC_GRID` | `2` |

## FastNoiseLite.DomainWarpFractalType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOMAIN_WARP_FRACTAL_NONE` | `0` |
| `DOMAIN_WARP_FRACTAL_PROGRESSIVE` | `1` |
| `DOMAIN_WARP_FRACTAL_INDEPENDENT` | `2` |

## FileAccess.ModeFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `READ` | `1` |
| `WRITE` | `2` |
| `READ_WRITE` | `3` |
| `WRITE_READ` | `7` |

## FileAccess.CompressionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPRESSION_FASTLZ` | `0` |
| `COMPRESSION_DEFLATE` | `1` |
| `COMPRESSION_ZSTD` | `2` |
| `COMPRESSION_GZIP` | `3` |
| `COMPRESSION_BROTLI` | `4` |

## FileAccess.UnixPermissionFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `UNIX_READ_OWNER` | `256` |
| `UNIX_WRITE_OWNER` | `128` |
| `UNIX_EXECUTE_OWNER` | `64` |
| `UNIX_READ_GROUP` | `32` |
| `UNIX_WRITE_GROUP` | `16` |
| `UNIX_EXECUTE_GROUP` | `8` |
| `UNIX_READ_OTHER` | `4` |
| `UNIX_WRITE_OTHER` | `2` |
| `UNIX_EXECUTE_OTHER` | `1` |
| `UNIX_SET_USER_ID` | `2048` |
| `UNIX_SET_GROUP_ID` | `1024` |
| `UNIX_RESTRICTED_DELETE` | `512` |

## FileDialog.FileMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILE_MODE_OPEN_FILE` | `0` |
| `FILE_MODE_OPEN_FILES` | `1` |
| `FILE_MODE_OPEN_DIR` | `2` |
| `FILE_MODE_OPEN_ANY` | `3` |
| `FILE_MODE_SAVE_FILE` | `4` |

## FileDialog.Access

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ACCESS_RESOURCES` | `0` |
| `ACCESS_USERDATA` | `1` |
| `ACCESS_FILESYSTEM` | `2` |

## FileDialog.DisplayMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISPLAY_THUMBNAILS` | `0` |
| `DISPLAY_LIST` | `1` |

## FileDialog.Customization

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CUSTOMIZATION_HIDDEN_FILES` | `0` |
| `CUSTOMIZATION_CREATE_FOLDER` | `1` |
| `CUSTOMIZATION_FILE_FILTER` | `2` |
| `CUSTOMIZATION_FILE_SORT` | `3` |
| `CUSTOMIZATION_FAVORITES` | `4` |
| `CUSTOMIZATION_RECENT` | `5` |
| `CUSTOMIZATION_LAYOUT` | `6` |
| `CUSTOMIZATION_OVERWRITE_WARNING` | `7` |
| `CUSTOMIZATION_DELETE` | `8` |

## FlowContainer.AlignmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALIGNMENT_BEGIN` | `0` |
| `ALIGNMENT_CENTER` | `1` |
| `ALIGNMENT_END` | `2` |

## FlowContainer.LastWrapAlignmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LAST_WRAP_ALIGNMENT_INHERIT` | `0` |
| `LAST_WRAP_ALIGNMENT_BEGIN` | `1` |
| `LAST_WRAP_ALIGNMENT_CENTER` | `2` |
| `LAST_WRAP_ALIGNMENT_END` | `3` |

## FoldableContainer.TitlePosition

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `POSITION_TOP` | `0` |
| `POSITION_BOTTOM` | `1` |

## GDExtension.InitializationLevel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INITIALIZATION_LEVEL_CORE` | `0` |
| `INITIALIZATION_LEVEL_SERVERS` | `1` |
| `INITIALIZATION_LEVEL_SCENE` | `2` |
| `INITIALIZATION_LEVEL_EDITOR` | `3` |

## GDExtensionManager.LoadStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOAD_STATUS_OK` | `0` |
| `LOAD_STATUS_FAILED` | `1` |
| `LOAD_STATUS_ALREADY_LOADED` | `2` |
| `LOAD_STATUS_NOT_LOADED` | `3` |
| `LOAD_STATUS_NEEDS_RESTART` | `4` |

## GLTFAccessor.GLTFAccessorType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_SCALAR` | `0` |
| `TYPE_VEC2` | `1` |
| `TYPE_VEC3` | `2` |
| `TYPE_VEC4` | `3` |
| `TYPE_MAT2` | `4` |
| `TYPE_MAT3` | `5` |
| `TYPE_MAT4` | `6` |

## GLTFAccessor.GLTFComponentType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPONENT_TYPE_NONE` | `0` |
| `COMPONENT_TYPE_SIGNED_BYTE` | `5120` |
| `COMPONENT_TYPE_UNSIGNED_BYTE` | `5121` |
| `COMPONENT_TYPE_SIGNED_SHORT` | `5122` |
| `COMPONENT_TYPE_UNSIGNED_SHORT` | `5123` |
| `COMPONENT_TYPE_SIGNED_INT` | `5124` |
| `COMPONENT_TYPE_UNSIGNED_INT` | `5125` |
| `COMPONENT_TYPE_SINGLE_FLOAT` | `5126` |
| `COMPONENT_TYPE_DOUBLE_FLOAT` | `5130` |
| `COMPONENT_TYPE_HALF_FLOAT` | `5131` |
| `COMPONENT_TYPE_SIGNED_LONG` | `5134` |
| `COMPONENT_TYPE_UNSIGNED_LONG` | `5135` |

## GLTFDocument.RootNodeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ROOT_NODE_MODE_SINGLE_ROOT` | `0` |
| `ROOT_NODE_MODE_KEEP_ROOT` | `1` |
| `ROOT_NODE_MODE_MULTI_ROOT` | `2` |

## GLTFDocument.TextureMapMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_MAP_MODE_DO_NOT_REMAP` | `0` |
| `TEXTURE_MAP_MODE_REMAP_TO_STANDARD_MATERIAL` | `1` |

## GLTFDocument.VisibilityMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_MODE_INCLUDE_REQUIRED` | `0` |
| `VISIBILITY_MODE_INCLUDE_OPTIONAL` | `1` |
| `VISIBILITY_MODE_EXCLUDE` | `2` |

## GLTFDocument.ImportFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `IMPORT_FLAG_GENERATE_TANGENT_ARRAYS` | `8` |
| `IMPORT_FLAG_USE_NAMED_SKIN_BINDS` | `16` |
| `IMPORT_FLAG_DISCARD_MESHES_AND_MATERIALS` | `32` |
| `IMPORT_FLAG_FORCE_DISABLE_MESH_COMPRESSION` | `64` |

## GLTFObjectModelProperty.GLTFObjectModelType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GLTF_OBJECT_MODEL_TYPE_UNKNOWN` | `0` |
| `GLTF_OBJECT_MODEL_TYPE_BOOL` | `1` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT` | `2` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT_ARRAY` | `3` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT2` | `4` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT3` | `5` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT4` | `6` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT2X2` | `7` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT3X3` | `8` |
| `GLTF_OBJECT_MODEL_TYPE_FLOAT4X4` | `9` |
| `GLTF_OBJECT_MODEL_TYPE_INT` | `10` |

## GLTFState.HandleBinaryImageMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HANDLE_BINARY_IMAGE_MODE_DISCARD_TEXTURES` | `0` |
| `HANDLE_BINARY_IMAGE_MODE_EXTRACT_TEXTURES` | `1` |
| `HANDLE_BINARY_IMAGE_MODE_EMBED_AS_BASISU` | `2` |
| `HANDLE_BINARY_IMAGE_MODE_EMBED_AS_UNCOMPRESSED` | `3` |

## GPUParticles2D.DrawOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAW_ORDER_INDEX` | `0` |
| `DRAW_ORDER_LIFETIME` | `1` |
| `DRAW_ORDER_REVERSE_LIFETIME` | `2` |

## GPUParticles2D.EmitFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMIT_FLAG_POSITION` | `1` |
| `EMIT_FLAG_ROTATION_SCALE` | `2` |
| `EMIT_FLAG_VELOCITY` | `4` |
| `EMIT_FLAG_COLOR` | `8` |
| `EMIT_FLAG_CUSTOM` | `16` |

## GPUParticles3D.DrawOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAW_ORDER_INDEX` | `0` |
| `DRAW_ORDER_LIFETIME` | `1` |
| `DRAW_ORDER_REVERSE_LIFETIME` | `2` |
| `DRAW_ORDER_VIEW_DEPTH` | `3` |

## GPUParticles3D.EmitFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMIT_FLAG_POSITION` | `1` |
| `EMIT_FLAG_ROTATION_SCALE` | `2` |
| `EMIT_FLAG_VELOCITY` | `4` |
| `EMIT_FLAG_COLOR` | `8` |
| `EMIT_FLAG_CUSTOM` | `16` |

## GPUParticles3D.TransformAlign

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSFORM_ALIGN_DISABLED` | `0` |
| `TRANSFORM_ALIGN_Z_BILLBOARD` | `1` |
| `TRANSFORM_ALIGN_Y_TO_VELOCITY` | `2` |
| `TRANSFORM_ALIGN_Z_BILLBOARD_Y_TO_VELOCITY` | `3` |
| `TRANSFORM_ALIGN_LOCAL_BILLBOARD` | `4` |

## GPUParticlesCollisionHeightField3D.Resolution

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESOLUTION_256` | `0` |
| `RESOLUTION_512` | `1` |
| `RESOLUTION_1024` | `2` |
| `RESOLUTION_2048` | `3` |
| `RESOLUTION_4096` | `4` |
| `RESOLUTION_8192` | `5` |
| `RESOLUTION_MAX` | `6` |

## GPUParticlesCollisionHeightField3D.UpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UPDATE_MODE_WHEN_MOVED` | `0` |
| `UPDATE_MODE_ALWAYS` | `1` |

## GPUParticlesCollisionSDF3D.Resolution

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESOLUTION_16` | `0` |
| `RESOLUTION_32` | `1` |
| `RESOLUTION_64` | `2` |
| `RESOLUTION_128` | `3` |
| `RESOLUTION_256` | `4` |
| `RESOLUTION_512` | `5` |
| `RESOLUTION_MAX` | `6` |

## Generic6DOFJoint3D.Param

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_LINEAR_LOWER_LIMIT` | `0` |
| `PARAM_LINEAR_UPPER_LIMIT` | `1` |
| `PARAM_LINEAR_LIMIT_SOFTNESS` | `2` |
| `PARAM_LINEAR_RESTITUTION` | `3` |
| `PARAM_LINEAR_DAMPING` | `4` |
| `PARAM_LINEAR_MOTOR_TARGET_VELOCITY` | `5` |
| `PARAM_LINEAR_MOTOR_FORCE_LIMIT` | `6` |
| `PARAM_LINEAR_SPRING_STIFFNESS` | `7` |
| `PARAM_LINEAR_SPRING_DAMPING` | `8` |
| `PARAM_LINEAR_SPRING_EQUILIBRIUM_POINT` | `9` |
| `PARAM_ANGULAR_LOWER_LIMIT` | `10` |
| `PARAM_ANGULAR_UPPER_LIMIT` | `11` |
| `PARAM_ANGULAR_LIMIT_SOFTNESS` | `12` |
| `PARAM_ANGULAR_DAMPING` | `13` |
| `PARAM_ANGULAR_RESTITUTION` | `14` |
| `PARAM_ANGULAR_FORCE_LIMIT` | `15` |
| `PARAM_ANGULAR_ERP` | `16` |
| `PARAM_ANGULAR_MOTOR_TARGET_VELOCITY` | `17` |
| `PARAM_ANGULAR_MOTOR_FORCE_LIMIT` | `18` |
| `PARAM_ANGULAR_SPRING_STIFFNESS` | `19` |
| `PARAM_ANGULAR_SPRING_DAMPING` | `20` |
| `PARAM_ANGULAR_SPRING_EQUILIBRIUM_POINT` | `21` |
| `PARAM_MAX` | `22` |

## Generic6DOFJoint3D.Flag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_ENABLE_LINEAR_LIMIT` | `0` |
| `FLAG_ENABLE_ANGULAR_LIMIT` | `1` |
| `FLAG_ENABLE_LINEAR_SPRING` | `3` |
| `FLAG_ENABLE_ANGULAR_SPRING` | `2` |
| `FLAG_ENABLE_MOTOR` | `4` |
| `FLAG_ENABLE_LINEAR_MOTOR` | `5` |
| `FLAG_MAX` | `6` |

## Geometry2D.PolyBooleanOperation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OPERATION_UNION` | `0` |
| `OPERATION_DIFFERENCE` | `1` |
| `OPERATION_INTERSECTION` | `2` |
| `OPERATION_XOR` | `3` |

## Geometry2D.PolyJoinType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOIN_SQUARE` | `0` |
| `JOIN_ROUND` | `1` |
| `JOIN_MITER` | `2` |

## Geometry2D.PolyEndType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `END_POLYGON` | `0` |
| `END_JOINED` | `1` |
| `END_BUTT` | `2` |
| `END_SQUARE` | `3` |
| `END_ROUND` | `4` |

## GeometryInstance3D.ShadowCastingSetting

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_CASTING_SETTING_OFF` | `0` |
| `SHADOW_CASTING_SETTING_ON` | `1` |
| `SHADOW_CASTING_SETTING_DOUBLE_SIDED` | `2` |
| `SHADOW_CASTING_SETTING_SHADOWS_ONLY` | `3` |

## GeometryInstance3D.GIMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GI_MODE_DISABLED` | `0` |
| `GI_MODE_STATIC` | `1` |
| `GI_MODE_DYNAMIC` | `2` |

## GeometryInstance3D.LightmapScale

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHTMAP_SCALE_1X` | `0` |
| `LIGHTMAP_SCALE_2X` | `1` |
| `LIGHTMAP_SCALE_4X` | `2` |
| `LIGHTMAP_SCALE_8X` | `3` |
| `LIGHTMAP_SCALE_MAX` | `4` |

## GeometryInstance3D.VisibilityRangeFadeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_RANGE_FADE_DISABLED` | `0` |
| `VISIBILITY_RANGE_FADE_SELF` | `1` |
| `VISIBILITY_RANGE_FADE_DEPENDENCIES` | `2` |

## Gradient.InterpolationMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GRADIENT_INTERPOLATE_LINEAR` | `0` |
| `GRADIENT_INTERPOLATE_CONSTANT` | `1` |
| `GRADIENT_INTERPOLATE_CUBIC` | `2` |

## Gradient.ColorSpace

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GRADIENT_COLOR_SPACE_SRGB` | `0` |
| `GRADIENT_COLOR_SPACE_LINEAR_SRGB` | `1` |
| `GRADIENT_COLOR_SPACE_OKLAB` | `2` |

## GradientTexture2D.Fill

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILL_LINEAR` | `0` |
| `FILL_RADIAL` | `1` |
| `FILL_SQUARE` | `2` |
| `FILL_CONIC` | `3` |

## GradientTexture2D.Repeat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REPEAT_NONE` | `0` |
| `REPEAT` | `1` |
| `REPEAT_MIRROR` | `2` |

## GraphEdit.PanningScheme

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_ZOOMS` | `0` |
| `SCROLL_PANS` | `1` |

## GraphEdit.GridPattern

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GRID_PATTERN_LINES` | `0` |
| `GRID_PATTERN_DOTS` | `1` |

## GridMap.DebugVisibilityMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEBUG_VISIBILITY_MODE_DEFAULT` | `0` |
| `DEBUG_VISIBILITY_MODE_FORCE_SHOW` | `1` |
| `DEBUG_VISIBILITY_MODE_FORCE_HIDE` | `2` |

## HTTPClient.Method

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `METHOD_GET` | `0` |
| `METHOD_HEAD` | `1` |
| `METHOD_POST` | `2` |
| `METHOD_PUT` | `3` |
| `METHOD_DELETE` | `4` |
| `METHOD_OPTIONS` | `5` |
| `METHOD_TRACE` | `6` |
| `METHOD_CONNECT` | `7` |
| `METHOD_PATCH` | `8` |
| `METHOD_MAX` | `9` |

## HTTPClient.Status

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATUS_DISCONNECTED` | `0` |
| `STATUS_RESOLVING` | `1` |
| `STATUS_CANT_RESOLVE` | `2` |
| `STATUS_CONNECTING` | `3` |
| `STATUS_CANT_CONNECT` | `4` |
| `STATUS_CONNECTED` | `5` |
| `STATUS_REQUESTING` | `6` |
| `STATUS_BODY` | `7` |
| `STATUS_CONNECTION_ERROR` | `8` |
| `STATUS_TLS_HANDSHAKE_ERROR` | `9` |

## HTTPClient.ResponseCode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESPONSE_CONTINUE` | `100` |
| `RESPONSE_SWITCHING_PROTOCOLS` | `101` |
| `RESPONSE_PROCESSING` | `102` |
| `RESPONSE_OK` | `200` |
| `RESPONSE_CREATED` | `201` |
| `RESPONSE_ACCEPTED` | `202` |
| `RESPONSE_NON_AUTHORITATIVE_INFORMATION` | `203` |
| `RESPONSE_NO_CONTENT` | `204` |
| `RESPONSE_RESET_CONTENT` | `205` |
| `RESPONSE_PARTIAL_CONTENT` | `206` |
| `RESPONSE_MULTI_STATUS` | `207` |
| `RESPONSE_ALREADY_REPORTED` | `208` |
| `RESPONSE_IM_USED` | `226` |
| `RESPONSE_MULTIPLE_CHOICES` | `300` |
| `RESPONSE_MOVED_PERMANENTLY` | `301` |
| `RESPONSE_FOUND` | `302` |
| `RESPONSE_SEE_OTHER` | `303` |
| `RESPONSE_NOT_MODIFIED` | `304` |
| `RESPONSE_USE_PROXY` | `305` |
| `RESPONSE_SWITCH_PROXY` | `306` |
| `RESPONSE_TEMPORARY_REDIRECT` | `307` |
| `RESPONSE_PERMANENT_REDIRECT` | `308` |
| `RESPONSE_BAD_REQUEST` | `400` |
| `RESPONSE_UNAUTHORIZED` | `401` |
| `RESPONSE_PAYMENT_REQUIRED` | `402` |
| `RESPONSE_FORBIDDEN` | `403` |
| `RESPONSE_NOT_FOUND` | `404` |
| `RESPONSE_METHOD_NOT_ALLOWED` | `405` |
| `RESPONSE_NOT_ACCEPTABLE` | `406` |
| `RESPONSE_PROXY_AUTHENTICATION_REQUIRED` | `407` |
| `RESPONSE_REQUEST_TIMEOUT` | `408` |
| `RESPONSE_CONFLICT` | `409` |
| `RESPONSE_GONE` | `410` |
| `RESPONSE_LENGTH_REQUIRED` | `411` |
| `RESPONSE_PRECONDITION_FAILED` | `412` |
| `RESPONSE_REQUEST_ENTITY_TOO_LARGE` | `413` |
| `RESPONSE_REQUEST_URI_TOO_LONG` | `414` |
| `RESPONSE_UNSUPPORTED_MEDIA_TYPE` | `415` |
| `RESPONSE_REQUESTED_RANGE_NOT_SATISFIABLE` | `416` |
| `RESPONSE_EXPECTATION_FAILED` | `417` |
| `RESPONSE_IM_A_TEAPOT` | `418` |
| `RESPONSE_MISDIRECTED_REQUEST` | `421` |
| `RESPONSE_UNPROCESSABLE_ENTITY` | `422` |
| `RESPONSE_LOCKED` | `423` |
| `RESPONSE_FAILED_DEPENDENCY` | `424` |
| `RESPONSE_UPGRADE_REQUIRED` | `426` |
| `RESPONSE_PRECONDITION_REQUIRED` | `428` |
| `RESPONSE_TOO_MANY_REQUESTS` | `429` |
| `RESPONSE_REQUEST_HEADER_FIELDS_TOO_LARGE` | `431` |
| `RESPONSE_UNAVAILABLE_FOR_LEGAL_REASONS` | `451` |
| `RESPONSE_INTERNAL_SERVER_ERROR` | `500` |
| `RESPONSE_NOT_IMPLEMENTED` | `501` |
| `RESPONSE_BAD_GATEWAY` | `502` |
| `RESPONSE_SERVICE_UNAVAILABLE` | `503` |
| `RESPONSE_GATEWAY_TIMEOUT` | `504` |
| `RESPONSE_HTTP_VERSION_NOT_SUPPORTED` | `505` |
| `RESPONSE_VARIANT_ALSO_NEGOTIATES` | `506` |
| `RESPONSE_INSUFFICIENT_STORAGE` | `507` |
| `RESPONSE_LOOP_DETECTED` | `508` |
| `RESPONSE_NOT_EXTENDED` | `510` |
| `RESPONSE_NETWORK_AUTH_REQUIRED` | `511` |

## HTTPRequest.Result

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESULT_SUCCESS` | `0` |
| `RESULT_CHUNKED_BODY_SIZE_MISMATCH` | `1` |
| `RESULT_CANT_CONNECT` | `2` |
| `RESULT_CANT_RESOLVE` | `3` |
| `RESULT_CONNECTION_ERROR` | `4` |
| `RESULT_TLS_HANDSHAKE_ERROR` | `5` |
| `RESULT_NO_RESPONSE` | `6` |
| `RESULT_BODY_SIZE_LIMIT_EXCEEDED` | `7` |
| `RESULT_BODY_DECOMPRESS_FAILED` | `8` |
| `RESULT_REQUEST_FAILED` | `9` |
| `RESULT_DOWNLOAD_FILE_CANT_OPEN` | `10` |
| `RESULT_DOWNLOAD_FILE_WRITE_ERROR` | `11` |
| `RESULT_REDIRECT_LIMIT_REACHED` | `12` |
| `RESULT_TIMEOUT` | `13` |

## HashingContext.HashType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HASH_MD5` | `0` |
| `HASH_SHA1` | `1` |
| `HASH_SHA256` | `2` |

## HingeJoint3D.Param

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_BIAS` | `0` |
| `PARAM_LIMIT_UPPER` | `1` |
| `PARAM_LIMIT_LOWER` | `2` |
| `PARAM_LIMIT_BIAS` | `3` |
| `PARAM_LIMIT_SOFTNESS` | `4` |
| `PARAM_LIMIT_RELAXATION` | `5` |
| `PARAM_MOTOR_TARGET_VELOCITY` | `6` |
| `PARAM_MOTOR_MAX_IMPULSE` | `7` |
| `PARAM_MAX` | `8` |

## HingeJoint3D.Flag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_USE_LIMIT` | `0` |
| `FLAG_ENABLE_MOTOR` | `1` |
| `FLAG_MAX` | `2` |

## IP.ResolverStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESOLVER_STATUS_NONE` | `0` |
| `RESOLVER_STATUS_WAITING` | `1` |
| `RESOLVER_STATUS_DONE` | `2` |
| `RESOLVER_STATUS_ERROR` | `3` |

## IP.Type

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_NONE` | `0` |
| `TYPE_IPV4` | `1` |
| `TYPE_IPV6` | `2` |
| `TYPE_ANY` | `3` |

## Image.Format

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FORMAT_L8` | `0` |
| `FORMAT_LA8` | `1` |
| `FORMAT_R8` | `2` |
| `FORMAT_RG8` | `3` |
| `FORMAT_RGB8` | `4` |
| `FORMAT_RGBA8` | `5` |
| `FORMAT_RGBA4444` | `6` |
| `FORMAT_RGB565` | `7` |
| `FORMAT_RF` | `8` |
| `FORMAT_RGF` | `9` |
| `FORMAT_RGBF` | `10` |
| `FORMAT_RGBAF` | `11` |
| `FORMAT_RH` | `12` |
| `FORMAT_RGH` | `13` |
| `FORMAT_RGBH` | `14` |
| `FORMAT_RGBAH` | `15` |
| `FORMAT_RGBE9995` | `16` |
| `FORMAT_DXT1` | `17` |
| `FORMAT_DXT3` | `18` |
| `FORMAT_DXT5` | `19` |
| `FORMAT_RGTC_R` | `20` |
| `FORMAT_RGTC_RG` | `21` |
| `FORMAT_BPTC_RGBA` | `22` |
| `FORMAT_BPTC_RGBF` | `23` |
| `FORMAT_BPTC_RGBFU` | `24` |
| `FORMAT_ETC` | `25` |
| `FORMAT_ETC2_R11` | `26` |
| `FORMAT_ETC2_R11S` | `27` |
| `FORMAT_ETC2_RG11` | `28` |
| `FORMAT_ETC2_RG11S` | `29` |
| `FORMAT_ETC2_RGB8` | `30` |
| `FORMAT_ETC2_RGBA8` | `31` |
| `FORMAT_ETC2_RGB8A1` | `32` |
| `FORMAT_ETC2_RA_AS_RG` | `33` |
| `FORMAT_DXT5_RA_AS_RG` | `34` |
| `FORMAT_ASTC_4x4` | `35` |
| `FORMAT_ASTC_4x4_HDR` | `36` |
| `FORMAT_ASTC_8x8` | `37` |
| `FORMAT_ASTC_8x8_HDR` | `38` |
| `FORMAT_R16` | `39` |
| `FORMAT_RG16` | `40` |
| `FORMAT_RGB16` | `41` |
| `FORMAT_RGBA16` | `42` |
| `FORMAT_R16I` | `43` |
| `FORMAT_RG16I` | `44` |
| `FORMAT_RGB16I` | `45` |
| `FORMAT_RGBA16I` | `46` |
| `FORMAT_MAX` | `47` |

## Image.Interpolation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INTERPOLATE_NEAREST` | `0` |
| `INTERPOLATE_BILINEAR` | `1` |
| `INTERPOLATE_CUBIC` | `2` |
| `INTERPOLATE_TRILINEAR` | `3` |
| `INTERPOLATE_LANCZOS` | `4` |

## Image.AlphaMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALPHA_NONE` | `0` |
| `ALPHA_BIT` | `1` |
| `ALPHA_BLEND` | `2` |

## Image.CompressMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPRESS_S3TC` | `0` |
| `COMPRESS_ETC` | `1` |
| `COMPRESS_ETC2` | `2` |
| `COMPRESS_BPTC` | `3` |
| `COMPRESS_ASTC` | `4` |
| `COMPRESS_MAX` | `5` |

## Image.UsedChannels

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `USED_CHANNELS_L` | `0` |
| `USED_CHANNELS_LA` | `1` |
| `USED_CHANNELS_R` | `2` |
| `USED_CHANNELS_RG` | `3` |
| `USED_CHANNELS_RGB` | `4` |
| `USED_CHANNELS_RGBA` | `5` |

## Image.CompressSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPRESS_SOURCE_GENERIC` | `0` |
| `COMPRESS_SOURCE_SRGB` | `1` |
| `COMPRESS_SOURCE_NORMAL` | `2` |

## Image.ASTCFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ASTC_FORMAT_4x4` | `0` |
| `ASTC_FORMAT_8x8` | `1` |

## ImageFormatLoader.LoaderFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `FLAG_NONE` | `0` |
| `FLAG_FORCE_LINEAR` | `1` |
| `FLAG_CONVERT_COLORS` | `2` |

## Input.MouseMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOUSE_MODE_VISIBLE` | `0` |
| `MOUSE_MODE_HIDDEN` | `1` |
| `MOUSE_MODE_CAPTURED` | `2` |
| `MOUSE_MODE_CONFINED` | `3` |
| `MOUSE_MODE_CONFINED_HIDDEN` | `4` |
| `MOUSE_MODE_MAX` | `5` |

## Input.CursorShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CURSOR_ARROW` | `0` |
| `CURSOR_IBEAM` | `1` |
| `CURSOR_POINTING_HAND` | `2` |
| `CURSOR_CROSS` | `3` |
| `CURSOR_WAIT` | `4` |
| `CURSOR_BUSY` | `5` |
| `CURSOR_DRAG` | `6` |
| `CURSOR_CAN_DROP` | `7` |
| `CURSOR_FORBIDDEN` | `8` |
| `CURSOR_VSIZE` | `9` |
| `CURSOR_HSIZE` | `10` |
| `CURSOR_BDIAGSIZE` | `11` |
| `CURSOR_FDIAGSIZE` | `12` |
| `CURSOR_MOVE` | `13` |
| `CURSOR_VSPLIT` | `14` |
| `CURSOR_HSPLIT` | `15` |
| `CURSOR_HELP` | `16` |

## ItemList.IconMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ICON_MODE_TOP` | `0` |
| `ICON_MODE_LEFT` | `1` |

## ItemList.SelectMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SELECT_SINGLE` | `0` |
| `SELECT_MULTI` | `1` |
| `SELECT_TOGGLE` | `2` |

## ItemList.ScrollHintMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_HINT_MODE_DISABLED` | `0` |
| `SCROLL_HINT_MODE_BOTH` | `1` |
| `SCROLL_HINT_MODE_TOP` | `2` |
| `SCROLL_HINT_MODE_BOTTOM` | `3` |

## JSONRPC.ErrorCode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARSE_ERROR` | `-32700` |
| `INVALID_REQUEST` | `-32600` |
| `METHOD_NOT_FOUND` | `-32601` |
| `INVALID_PARAMS` | `-32602` |
| `INTERNAL_ERROR` | `-32603` |

## Label3D.DrawFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_SHADED` | `0` |
| `FLAG_DOUBLE_SIDED` | `1` |
| `FLAG_DISABLE_DEPTH_TEST` | `2` |
| `FLAG_FIXED_SIZE` | `3` |
| `FLAG_MAX` | `4` |

## Label3D.AlphaCutMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALPHA_CUT_DISABLED` | `0` |
| `ALPHA_CUT_DISCARD` | `1` |
| `ALPHA_CUT_OPAQUE_PREPASS` | `2` |
| `ALPHA_CUT_HASH` | `3` |

## Light2D.ShadowFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_FILTER_NONE` | `0` |
| `SHADOW_FILTER_PCF5` | `1` |
| `SHADOW_FILTER_PCF13` | `2` |

## Light2D.BlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_MODE_ADD` | `0` |
| `BLEND_MODE_SUB` | `1` |
| `BLEND_MODE_MIX` | `2` |

## Light3D.Param

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_ENERGY` | `0` |
| `PARAM_INDIRECT_ENERGY` | `1` |
| `PARAM_VOLUMETRIC_FOG_ENERGY` | `2` |
| `PARAM_SPECULAR` | `3` |
| `PARAM_RANGE` | `4` |
| `PARAM_SIZE` | `5` |
| `PARAM_ATTENUATION` | `6` |
| `PARAM_SPOT_ANGLE` | `7` |
| `PARAM_SPOT_ATTENUATION` | `8` |
| `PARAM_SHADOW_MAX_DISTANCE` | `9` |
| `PARAM_SHADOW_SPLIT_1_OFFSET` | `10` |
| `PARAM_SHADOW_SPLIT_2_OFFSET` | `11` |
| `PARAM_SHADOW_SPLIT_3_OFFSET` | `12` |
| `PARAM_SHADOW_FADE_START` | `13` |
| `PARAM_SHADOW_NORMAL_BIAS` | `14` |
| `PARAM_SHADOW_BIAS` | `15` |
| `PARAM_SHADOW_PANCAKE_SIZE` | `16` |
| `PARAM_SHADOW_OPACITY` | `17` |
| `PARAM_SHADOW_BLUR` | `18` |
| `PARAM_TRANSMITTANCE_BIAS` | `19` |
| `PARAM_INTENSITY` | `20` |
| `PARAM_MAX` | `21` |

## Light3D.BakeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BAKE_DISABLED` | `0` |
| `BAKE_STATIC` | `1` |
| `BAKE_DYNAMIC` | `2` |

## LightmapGI.BakeQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BAKE_QUALITY_LOW` | `0` |
| `BAKE_QUALITY_MEDIUM` | `1` |
| `BAKE_QUALITY_HIGH` | `2` |
| `BAKE_QUALITY_ULTRA` | `3` |

## LightmapGI.GenerateProbes

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GENERATE_PROBES_DISABLED` | `0` |
| `GENERATE_PROBES_SUBDIV_4` | `1` |
| `GENERATE_PROBES_SUBDIV_8` | `2` |
| `GENERATE_PROBES_SUBDIV_16` | `3` |
| `GENERATE_PROBES_SUBDIV_32` | `4` |

## LightmapGI.BakeError

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BAKE_ERROR_OK` | `0` |
| `BAKE_ERROR_NO_SCENE_ROOT` | `1` |
| `BAKE_ERROR_FOREIGN_DATA` | `2` |
| `BAKE_ERROR_NO_LIGHTMAPPER` | `3` |
| `BAKE_ERROR_NO_SAVE_PATH` | `4` |
| `BAKE_ERROR_NO_MESHES` | `5` |
| `BAKE_ERROR_MESHES_INVALID` | `6` |
| `BAKE_ERROR_CANT_CREATE_IMAGE` | `7` |
| `BAKE_ERROR_USER_ABORTED` | `8` |
| `BAKE_ERROR_TEXTURE_SIZE_TOO_SMALL` | `9` |
| `BAKE_ERROR_LIGHTMAP_TOO_SMALL` | `10` |
| `BAKE_ERROR_ATLAS_TOO_SMALL` | `11` |

## LightmapGI.EnvironmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENVIRONMENT_MODE_DISABLED` | `0` |
| `ENVIRONMENT_MODE_SCENE` | `1` |
| `ENVIRONMENT_MODE_CUSTOM_SKY` | `2` |
| `ENVIRONMENT_MODE_CUSTOM_COLOR` | `3` |

## LightmapGIData.ShadowmaskMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOWMASK_MODE_NONE` | `0` |
| `SHADOWMASK_MODE_REPLACE` | `1` |
| `SHADOWMASK_MODE_OVERLAY` | `2` |

## Line2D.LineJointMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LINE_JOINT_SHARP` | `0` |
| `LINE_JOINT_BEVEL` | `1` |
| `LINE_JOINT_ROUND` | `2` |

## Line2D.LineCapMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LINE_CAP_NONE` | `0` |
| `LINE_CAP_BOX` | `1` |
| `LINE_CAP_ROUND` | `2` |

## Line2D.LineTextureMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LINE_TEXTURE_NONE` | `0` |
| `LINE_TEXTURE_TILE` | `1` |
| `LINE_TEXTURE_STRETCH` | `2` |

## LineEdit.MenuItems

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MENU_CUT` | `0` |
| `MENU_COPY` | `1` |
| `MENU_PASTE` | `2` |
| `MENU_CLEAR` | `3` |
| `MENU_SELECT_ALL` | `4` |
| `MENU_UNDO` | `5` |
| `MENU_REDO` | `6` |
| `MENU_SUBMENU_TEXT_DIR` | `7` |
| `MENU_DIR_INHERITED` | `8` |
| `MENU_DIR_AUTO` | `9` |
| `MENU_DIR_LTR` | `10` |
| `MENU_DIR_RTL` | `11` |
| `MENU_DISPLAY_UCC` | `12` |
| `MENU_SUBMENU_INSERT_UCC` | `13` |
| `MENU_INSERT_LRM` | `14` |
| `MENU_INSERT_RLM` | `15` |
| `MENU_INSERT_LRE` | `16` |
| `MENU_INSERT_RLE` | `17` |
| `MENU_INSERT_LRO` | `18` |
| `MENU_INSERT_RLO` | `19` |
| `MENU_INSERT_PDF` | `20` |
| `MENU_INSERT_ALM` | `21` |
| `MENU_INSERT_LRI` | `22` |
| `MENU_INSERT_RLI` | `23` |
| `MENU_INSERT_FSI` | `24` |
| `MENU_INSERT_PDI` | `25` |
| `MENU_INSERT_ZWJ` | `26` |
| `MENU_INSERT_ZWNJ` | `27` |
| `MENU_INSERT_WJ` | `28` |
| `MENU_INSERT_SHY` | `29` |
| `MENU_EMOJI_AND_SYMBOL` | `30` |
| `MENU_MAX` | `31` |

## LineEdit.VirtualKeyboardType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `KEYBOARD_TYPE_DEFAULT` | `0` |
| `KEYBOARD_TYPE_MULTILINE` | `1` |
| `KEYBOARD_TYPE_NUMBER` | `2` |
| `KEYBOARD_TYPE_NUMBER_DECIMAL` | `3` |
| `KEYBOARD_TYPE_PHONE` | `4` |
| `KEYBOARD_TYPE_EMAIL_ADDRESS` | `5` |
| `KEYBOARD_TYPE_PASSWORD` | `6` |
| `KEYBOARD_TYPE_URL` | `7` |

## LineEdit.ExpandMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EXPAND_MODE_ORIGINAL_SIZE` | `0` |
| `EXPAND_MODE_FIT_TO_TEXT` | `1` |
| `EXPAND_MODE_FIT_TO_LINE_EDIT` | `2` |

## LinkButton.UnderlineMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UNDERLINE_MODE_ALWAYS` | `0` |
| `UNDERLINE_MODE_ON_HOVER` | `1` |
| `UNDERLINE_MODE_NEVER` | `2` |

## Logger.ErrorType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ERROR_TYPE_ERROR` | `0` |
| `ERROR_TYPE_WARNING` | `1` |
| `ERROR_TYPE_SCRIPT` | `2` |
| `ERROR_TYPE_SHADER` | `3` |

## LookAtModifier3D.OriginFrom

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ORIGIN_FROM_SELF` | `0` |
| `ORIGIN_FROM_SPECIFIC_BONE` | `1` |
| `ORIGIN_FROM_EXTERNAL_NODE` | `2` |

## Mesh.PrimitiveType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PRIMITIVE_POINTS` | `0` |
| `PRIMITIVE_LINES` | `1` |
| `PRIMITIVE_LINE_STRIP` | `2` |
| `PRIMITIVE_TRIANGLES` | `3` |
| `PRIMITIVE_TRIANGLE_STRIP` | `4` |

## Mesh.ArrayType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ARRAY_VERTEX` | `0` |
| `ARRAY_NORMAL` | `1` |
| `ARRAY_TANGENT` | `2` |
| `ARRAY_COLOR` | `3` |
| `ARRAY_TEX_UV` | `4` |
| `ARRAY_TEX_UV2` | `5` |
| `ARRAY_CUSTOM0` | `6` |
| `ARRAY_CUSTOM1` | `7` |
| `ARRAY_CUSTOM2` | `8` |
| `ARRAY_CUSTOM3` | `9` |
| `ARRAY_BONES` | `10` |
| `ARRAY_WEIGHTS` | `11` |
| `ARRAY_INDEX` | `12` |
| `ARRAY_MAX` | `13` |

## Mesh.ArrayCustomFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ARRAY_CUSTOM_RGBA8_UNORM` | `0` |
| `ARRAY_CUSTOM_RGBA8_SNORM` | `1` |
| `ARRAY_CUSTOM_RG_HALF` | `2` |
| `ARRAY_CUSTOM_RGBA_HALF` | `3` |
| `ARRAY_CUSTOM_R_FLOAT` | `4` |
| `ARRAY_CUSTOM_RG_FLOAT` | `5` |
| `ARRAY_CUSTOM_RGB_FLOAT` | `6` |
| `ARRAY_CUSTOM_RGBA_FLOAT` | `7` |
| `ARRAY_CUSTOM_MAX` | `8` |

## Mesh.ArrayFormat

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `ARRAY_FORMAT_VERTEX` | `1` |
| `ARRAY_FORMAT_NORMAL` | `2` |
| `ARRAY_FORMAT_TANGENT` | `4` |
| `ARRAY_FORMAT_COLOR` | `8` |
| `ARRAY_FORMAT_TEX_UV` | `16` |
| `ARRAY_FORMAT_TEX_UV2` | `32` |
| `ARRAY_FORMAT_CUSTOM0` | `64` |
| `ARRAY_FORMAT_CUSTOM1` | `128` |
| `ARRAY_FORMAT_CUSTOM2` | `256` |
| `ARRAY_FORMAT_CUSTOM3` | `512` |
| `ARRAY_FORMAT_BONES` | `1024` |
| `ARRAY_FORMAT_WEIGHTS` | `2048` |
| `ARRAY_FORMAT_INDEX` | `4096` |
| `ARRAY_FORMAT_BLEND_SHAPE_MASK` | `7` |
| `ARRAY_FORMAT_CUSTOM_BASE` | `13` |
| `ARRAY_FORMAT_CUSTOM_BITS` | `3` |
| `ARRAY_FORMAT_CUSTOM0_SHIFT` | `13` |
| `ARRAY_FORMAT_CUSTOM1_SHIFT` | `16` |
| `ARRAY_FORMAT_CUSTOM2_SHIFT` | `19` |
| `ARRAY_FORMAT_CUSTOM3_SHIFT` | `22` |
| `ARRAY_FORMAT_CUSTOM_MASK` | `7` |
| `ARRAY_COMPRESS_FLAGS_BASE` | `25` |
| `ARRAY_FLAG_USE_2D_VERTICES` | `33554432` |
| `ARRAY_FLAG_USE_DYNAMIC_UPDATE` | `67108864` |
| `ARRAY_FLAG_USE_8_BONE_WEIGHTS` | `134217728` |
| `ARRAY_FLAG_USES_EMPTY_VERTEX_ARRAY` | `268435456` |
| `ARRAY_FLAG_COMPRESS_ATTRIBUTES` | `536870912` |

## Mesh.BlendShapeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_SHAPE_MODE_NORMALIZED` | `0` |
| `BLEND_SHAPE_MODE_RELATIVE` | `1` |

## MeshConvexDecompositionSettings.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONVEX_DECOMPOSITION_MODE_VOXEL` | `0` |
| `CONVEX_DECOMPOSITION_MODE_TETRAHEDRON` | `1` |

## MultiMesh.TransformFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSFORM_2D` | `0` |
| `TRANSFORM_3D` | `1` |

## MultiMesh.PhysicsInterpolationQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INTERP_QUALITY_FAST` | `0` |
| `INTERP_QUALITY_HIGH` | `1` |

## MultiplayerAPI.RPCMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RPC_MODE_DISABLED` | `0` |
| `RPC_MODE_ANY_PEER` | `1` |
| `RPC_MODE_AUTHORITY` | `2` |

## MultiplayerPeer.ConnectionStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONNECTION_DISCONNECTED` | `0` |
| `CONNECTION_CONNECTING` | `1` |
| `CONNECTION_CONNECTED` | `2` |

## MultiplayerPeer.TransferMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANSFER_MODE_UNRELIABLE` | `0` |
| `TRANSFER_MODE_UNRELIABLE_ORDERED` | `1` |
| `TRANSFER_MODE_RELIABLE` | `2` |

## MultiplayerSynchronizer.VisibilityUpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_PROCESS_IDLE` | `0` |
| `VISIBILITY_PROCESS_PHYSICS` | `1` |
| `VISIBILITY_PROCESS_NONE` | `2` |

## NativeMenu.Feature

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEATURE_GLOBAL_MENU` | `0` |
| `FEATURE_POPUP_MENU` | `1` |
| `FEATURE_OPEN_CLOSE_CALLBACK` | `2` |
| `FEATURE_HOVER_CALLBACK` | `3` |
| `FEATURE_KEY_CALLBACK` | `4` |

## NativeMenu.SystemMenus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INVALID_MENU_ID` | `0` |
| `MAIN_MENU_ID` | `1` |
| `APPLICATION_MENU_ID` | `2` |
| `WINDOW_MENU_ID` | `3` |
| `HELP_MENU_ID` | `4` |
| `DOCK_MENU_ID` | `5` |

## NavigationMesh.SamplePartitionType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SAMPLE_PARTITION_WATERSHED` | `0` |
| `SAMPLE_PARTITION_MONOTONE` | `1` |
| `SAMPLE_PARTITION_LAYERS` | `2` |
| `SAMPLE_PARTITION_MAX` | `3` |

## NavigationMesh.ParsedGeometryType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARSED_GEOMETRY_MESH_INSTANCES` | `0` |
| `PARSED_GEOMETRY_STATIC_COLLIDERS` | `1` |
| `PARSED_GEOMETRY_BOTH` | `2` |
| `PARSED_GEOMETRY_MAX` | `3` |

## NavigationMesh.SourceGeometryMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SOURCE_GEOMETRY_ROOT_NODE_CHILDREN` | `0` |
| `SOURCE_GEOMETRY_GROUPS_WITH_CHILDREN` | `1` |
| `SOURCE_GEOMETRY_GROUPS_EXPLICIT` | `2` |
| `SOURCE_GEOMETRY_MAX` | `3` |

## NavigationPathQueryParameters2D.PathfindingAlgorithm

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATHFINDING_ALGORITHM_ASTAR` | `0` |

## NavigationPathQueryParameters2D.PathPostProcessing

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATH_POSTPROCESSING_CORRIDORFUNNEL` | `0` |
| `PATH_POSTPROCESSING_EDGECENTERED` | `1` |
| `PATH_POSTPROCESSING_NONE` | `2` |

## NavigationPathQueryParameters2D.PathMetadataFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `PATH_METADATA_INCLUDE_NONE` | `0` |
| `PATH_METADATA_INCLUDE_TYPES` | `1` |
| `PATH_METADATA_INCLUDE_RIDS` | `2` |
| `PATH_METADATA_INCLUDE_OWNERS` | `4` |
| `PATH_METADATA_INCLUDE_ALL` | `7` |

## NavigationPathQueryParameters3D.PathfindingAlgorithm

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATHFINDING_ALGORITHM_ASTAR` | `0` |

## NavigationPathQueryParameters3D.PathPostProcessing

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATH_POSTPROCESSING_CORRIDORFUNNEL` | `0` |
| `PATH_POSTPROCESSING_EDGECENTERED` | `1` |
| `PATH_POSTPROCESSING_NONE` | `2` |

## NavigationPathQueryParameters3D.PathMetadataFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `PATH_METADATA_INCLUDE_NONE` | `0` |
| `PATH_METADATA_INCLUDE_TYPES` | `1` |
| `PATH_METADATA_INCLUDE_RIDS` | `2` |
| `PATH_METADATA_INCLUDE_OWNERS` | `4` |
| `PATH_METADATA_INCLUDE_ALL` | `7` |

## NavigationPathQueryResult2D.PathSegmentType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATH_SEGMENT_TYPE_REGION` | `0` |
| `PATH_SEGMENT_TYPE_LINK` | `1` |

## NavigationPathQueryResult3D.PathSegmentType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PATH_SEGMENT_TYPE_REGION` | `0` |
| `PATH_SEGMENT_TYPE_LINK` | `1` |

## NavigationPolygon.SamplePartitionType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SAMPLE_PARTITION_CONVEX_PARTITION` | `0` |
| `SAMPLE_PARTITION_TRIANGULATE` | `1` |
| `SAMPLE_PARTITION_MAX` | `2` |

## NavigationPolygon.ParsedGeometryType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARSED_GEOMETRY_MESH_INSTANCES` | `0` |
| `PARSED_GEOMETRY_STATIC_COLLIDERS` | `1` |
| `PARSED_GEOMETRY_BOTH` | `2` |
| `PARSED_GEOMETRY_MAX` | `3` |

## NavigationPolygon.SourceGeometryMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SOURCE_GEOMETRY_ROOT_NODE_CHILDREN` | `0` |
| `SOURCE_GEOMETRY_GROUPS_WITH_CHILDREN` | `1` |
| `SOURCE_GEOMETRY_GROUPS_EXPLICIT` | `2` |
| `SOURCE_GEOMETRY_MAX` | `3` |

## NavigationServer2D.ProcessInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INFO_ACTIVE_MAPS` | `0` |
| `INFO_REGION_COUNT` | `1` |
| `INFO_AGENT_COUNT` | `2` |
| `INFO_LINK_COUNT` | `3` |
| `INFO_POLYGON_COUNT` | `4` |
| `INFO_EDGE_COUNT` | `5` |
| `INFO_EDGE_MERGE_COUNT` | `6` |
| `INFO_EDGE_CONNECTION_COUNT` | `7` |
| `INFO_EDGE_FREE_COUNT` | `8` |
| `INFO_OBSTACLE_COUNT` | `9` |

## NavigationServer3D.ProcessInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INFO_ACTIVE_MAPS` | `0` |
| `INFO_REGION_COUNT` | `1` |
| `INFO_AGENT_COUNT` | `2` |
| `INFO_LINK_COUNT` | `3` |
| `INFO_POLYGON_COUNT` | `4` |
| `INFO_EDGE_COUNT` | `5` |
| `INFO_EDGE_MERGE_COUNT` | `6` |
| `INFO_EDGE_CONNECTION_COUNT` | `7` |
| `INFO_EDGE_FREE_COUNT` | `8` |
| `INFO_OBSTACLE_COUNT` | `9` |

## NinePatchRect.AxisStretchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_STRETCH_MODE_STRETCH` | `0` |
| `AXIS_STRETCH_MODE_TILE` | `1` |
| `AXIS_STRETCH_MODE_TILE_FIT` | `2` |

## Node.ProcessMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PROCESS_MODE_INHERIT` | `0` |
| `PROCESS_MODE_PAUSABLE` | `1` |
| `PROCESS_MODE_WHEN_PAUSED` | `2` |
| `PROCESS_MODE_ALWAYS` | `3` |
| `PROCESS_MODE_DISABLED` | `4` |

## Node.ProcessThreadGroup

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PROCESS_THREAD_GROUP_INHERIT` | `0` |
| `PROCESS_THREAD_GROUP_MAIN_THREAD` | `1` |
| `PROCESS_THREAD_GROUP_SUB_THREAD` | `2` |

## Node.ProcessThreadMessages

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `FLAG_PROCESS_THREAD_MESSAGES` | `1` |
| `FLAG_PROCESS_THREAD_MESSAGES_PHYSICS` | `2` |
| `FLAG_PROCESS_THREAD_MESSAGES_ALL` | `3` |

## Node.PhysicsInterpolationMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PHYSICS_INTERPOLATION_MODE_INHERIT` | `0` |
| `PHYSICS_INTERPOLATION_MODE_ON` | `1` |
| `PHYSICS_INTERPOLATION_MODE_OFF` | `2` |

## Node.DuplicateFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DUPLICATE_SIGNALS` | `1` |
| `DUPLICATE_GROUPS` | `2` |
| `DUPLICATE_SCRIPTS` | `4` |
| `DUPLICATE_USE_INSTANTIATION` | `8` |
| `DUPLICATE_INTERNAL_STATE` | `16` |
| `DUPLICATE_DEFAULT` | `15` |

## Node.InternalMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INTERNAL_MODE_DISABLED` | `0` |
| `INTERNAL_MODE_FRONT` | `1` |
| `INTERNAL_MODE_BACK` | `2` |

## Node.AutoTranslateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AUTO_TRANSLATE_MODE_INHERIT` | `0` |
| `AUTO_TRANSLATE_MODE_ALWAYS` | `1` |
| `AUTO_TRANSLATE_MODE_DISABLED` | `2` |

## Node3D.RotationEditMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ROTATION_EDIT_MODE_EULER` | `0` |
| `ROTATION_EDIT_MODE_QUATERNION` | `1` |
| `ROTATION_EDIT_MODE_BASIS` | `2` |

## OS.RenderingDriver

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RENDERING_DRIVER_VULKAN` | `0` |
| `RENDERING_DRIVER_OPENGL3` | `1` |
| `RENDERING_DRIVER_D3D12` | `2` |
| `RENDERING_DRIVER_METAL` | `3` |

## OS.SystemDir

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SYSTEM_DIR_DESKTOP` | `0` |
| `SYSTEM_DIR_DCIM` | `1` |
| `SYSTEM_DIR_DOCUMENTS` | `2` |
| `SYSTEM_DIR_DOWNLOADS` | `3` |
| `SYSTEM_DIR_MOVIES` | `4` |
| `SYSTEM_DIR_MUSIC` | `5` |
| `SYSTEM_DIR_PICTURES` | `6` |
| `SYSTEM_DIR_RINGTONES` | `7` |

## OS.StdHandleType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STD_HANDLE_INVALID` | `0` |
| `STD_HANDLE_CONSOLE` | `1` |
| `STD_HANDLE_FILE` | `2` |
| `STD_HANDLE_PIPE` | `3` |
| `STD_HANDLE_UNKNOWN` | `4` |

## Object.ConnectFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `CONNECT_DEFERRED` | `1` |
| `CONNECT_PERSIST` | `2` |
| `CONNECT_ONE_SHOT` | `4` |
| `CONNECT_REFERENCE_COUNTED` | `8` |
| `CONNECT_APPEND_SOURCE_OBJECT` | `16` |

## OccluderPolygon2D.CullMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CULL_DISABLED` | `0` |
| `CULL_CLOCKWISE` | `1` |
| `CULL_COUNTER_CLOCKWISE` | `2` |

## OmniLight3D.ShadowMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_DUAL_PARABOLOID` | `0` |
| `SHADOW_CUBE` | `1` |

## OpenXRAPIExtension.OpenXRAlphaBlendModeSupport

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OPENXR_ALPHA_BLEND_MODE_SUPPORT_NONE` | `0` |
| `OPENXR_ALPHA_BLEND_MODE_SUPPORT_REAL` | `1` |
| `OPENXR_ALPHA_BLEND_MODE_SUPPORT_EMULATING` | `2` |

## OpenXRAction.ActionType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OPENXR_ACTION_BOOL` | `0` |
| `OPENXR_ACTION_FLOAT` | `1` |
| `OPENXR_ACTION_VECTOR2` | `2` |
| `OPENXR_ACTION_POSE` | `3` |

## OpenXRAndroidThreadSettingsExtension.ThreadType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `THREAD_TYPE_APPLICATION_MAIN` | `0` |
| `THREAD_TYPE_APPLICATION_WORKER` | `1` |
| `THREAD_TYPE_RENDERER_MAIN` | `2` |
| `THREAD_TYPE_RENDERER_WORKER` | `3` |

## OpenXRCompositionLayer.Filter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILTER_NEAREST` | `0` |
| `FILTER_LINEAR` | `1` |
| `FILTER_CUBIC` | `2` |

## OpenXRCompositionLayer.MipmapMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MIPMAP_MODE_DISABLED` | `0` |
| `MIPMAP_MODE_NEAREST` | `1` |
| `MIPMAP_MODE_LINEAR` | `2` |

## OpenXRCompositionLayer.Wrap

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WRAP_CLAMP_TO_BORDER` | `0` |
| `WRAP_CLAMP_TO_EDGE` | `1` |
| `WRAP_REPEAT` | `2` |
| `WRAP_MIRRORED_REPEAT` | `3` |
| `WRAP_MIRROR_CLAMP_TO_EDGE` | `4` |

## OpenXRCompositionLayer.Swizzle

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SWIZZLE_RED` | `0` |
| `SWIZZLE_GREEN` | `1` |
| `SWIZZLE_BLUE` | `2` |
| `SWIZZLE_ALPHA` | `3` |
| `SWIZZLE_ZERO` | `4` |
| `SWIZZLE_ONE` | `5` |

## OpenXRCompositionLayer.EyeVisibility

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EYE_VISIBILITY_BOTH` | `0` |
| `EYE_VISIBILITY_LEFT` | `1` |
| `EYE_VISIBILITY_RIGHT` | `2` |

## OpenXRFutureResult.ResultStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESULT_RUNNING` | `0` |
| `RESULT_FINISHED` | `1` |
| `RESULT_CANCELLED` | `2` |

## OpenXRHand.Hands

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_LEFT` | `0` |
| `HAND_RIGHT` | `1` |
| `HAND_MAX` | `2` |

## OpenXRHand.MotionRange

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MOTION_RANGE_UNOBSTRUCTED` | `0` |
| `MOTION_RANGE_CONFORM_TO_CONTROLLER` | `1` |
| `MOTION_RANGE_MAX` | `2` |

## OpenXRHand.SkeletonRig

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SKELETON_RIG_OPENXR` | `0` |
| `SKELETON_RIG_HUMANOID` | `1` |
| `SKELETON_RIG_MAX` | `2` |

## OpenXRHand.BoneUpdate

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BONE_UPDATE_FULL` | `0` |
| `BONE_UPDATE_ROTATION_ONLY` | `1` |
| `BONE_UPDATE_MAX` | `2` |

## OpenXRInterface.SessionState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SESSION_STATE_UNKNOWN` | `0` |
| `SESSION_STATE_IDLE` | `1` |
| `SESSION_STATE_READY` | `2` |
| `SESSION_STATE_SYNCHRONIZED` | `3` |
| `SESSION_STATE_VISIBLE` | `4` |
| `SESSION_STATE_FOCUSED` | `5` |
| `SESSION_STATE_STOPPING` | `6` |
| `SESSION_STATE_LOSS_PENDING` | `7` |
| `SESSION_STATE_EXITING` | `8` |

## OpenXRInterface.Hand

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_LEFT` | `0` |
| `HAND_RIGHT` | `1` |
| `HAND_MAX` | `2` |

## OpenXRInterface.HandMotionRange

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_MOTION_RANGE_UNOBSTRUCTED` | `0` |
| `HAND_MOTION_RANGE_CONFORM_TO_CONTROLLER` | `1` |
| `HAND_MOTION_RANGE_MAX` | `2` |

## OpenXRInterface.HandTrackedSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_TRACKED_SOURCE_UNKNOWN` | `0` |
| `HAND_TRACKED_SOURCE_UNOBSTRUCTED` | `1` |
| `HAND_TRACKED_SOURCE_CONTROLLER` | `2` |
| `HAND_TRACKED_SOURCE_MAX` | `3` |

## OpenXRInterface.HandJoints

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_JOINT_PALM` | `0` |
| `HAND_JOINT_WRIST` | `1` |
| `HAND_JOINT_THUMB_METACARPAL` | `2` |
| `HAND_JOINT_THUMB_PROXIMAL` | `3` |
| `HAND_JOINT_THUMB_DISTAL` | `4` |
| `HAND_JOINT_THUMB_TIP` | `5` |
| `HAND_JOINT_INDEX_METACARPAL` | `6` |
| `HAND_JOINT_INDEX_PROXIMAL` | `7` |
| `HAND_JOINT_INDEX_INTERMEDIATE` | `8` |
| `HAND_JOINT_INDEX_DISTAL` | `9` |
| `HAND_JOINT_INDEX_TIP` | `10` |
| `HAND_JOINT_MIDDLE_METACARPAL` | `11` |
| `HAND_JOINT_MIDDLE_PROXIMAL` | `12` |
| `HAND_JOINT_MIDDLE_INTERMEDIATE` | `13` |
| `HAND_JOINT_MIDDLE_DISTAL` | `14` |
| `HAND_JOINT_MIDDLE_TIP` | `15` |
| `HAND_JOINT_RING_METACARPAL` | `16` |
| `HAND_JOINT_RING_PROXIMAL` | `17` |
| `HAND_JOINT_RING_INTERMEDIATE` | `18` |
| `HAND_JOINT_RING_DISTAL` | `19` |
| `HAND_JOINT_RING_TIP` | `20` |
| `HAND_JOINT_LITTLE_METACARPAL` | `21` |
| `HAND_JOINT_LITTLE_PROXIMAL` | `22` |
| `HAND_JOINT_LITTLE_INTERMEDIATE` | `23` |
| `HAND_JOINT_LITTLE_DISTAL` | `24` |
| `HAND_JOINT_LITTLE_TIP` | `25` |
| `HAND_JOINT_MAX` | `26` |

## OpenXRInterface.PerfSettingsLevel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PERF_SETTINGS_LEVEL_POWER_SAVINGS` | `0` |
| `PERF_SETTINGS_LEVEL_SUSTAINED_LOW` | `1` |
| `PERF_SETTINGS_LEVEL_SUSTAINED_HIGH` | `2` |
| `PERF_SETTINGS_LEVEL_BOOST` | `3` |

## OpenXRInterface.PerfSettingsSubDomain

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PERF_SETTINGS_SUB_DOMAIN_COMPOSITING` | `0` |
| `PERF_SETTINGS_SUB_DOMAIN_RENDERING` | `1` |
| `PERF_SETTINGS_SUB_DOMAIN_THERMAL` | `2` |

## OpenXRInterface.PerfSettingsNotificationLevel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PERF_SETTINGS_NOTIF_LEVEL_NORMAL` | `0` |
| `PERF_SETTINGS_NOTIF_LEVEL_WARNING` | `1` |
| `PERF_SETTINGS_NOTIF_LEVEL_IMPAIRED` | `2` |

## OpenXRInterface.HandJointFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `HAND_JOINT_NONE` | `0` |
| `HAND_JOINT_ORIENTATION_VALID` | `1` |
| `HAND_JOINT_ORIENTATION_TRACKED` | `2` |
| `HAND_JOINT_POSITION_VALID` | `4` |
| `HAND_JOINT_POSITION_TRACKED` | `8` |
| `HAND_JOINT_LINEAR_VELOCITY_VALID` | `16` |
| `HAND_JOINT_ANGULAR_VELOCITY_VALID` | `32` |

## OpenXRRenderModelManager.RenderModelTracker

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RENDER_MODEL_TRACKER_ANY` | `0` |
| `RENDER_MODEL_TRACKER_NONE_SET` | `1` |
| `RENDER_MODEL_TRACKER_LEFT_HAND` | `2` |
| `RENDER_MODEL_TRACKER_RIGHT_HAND` | `3` |

## OpenXRSpatialAnchorCapability.PersistenceScope

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PERSISTENCE_SCOPE_SYSTEM_MANAGED` | `1` |
| `PERSISTENCE_SCOPE_LOCAL_ANCHORS` | `1000781000` |

## OpenXRSpatialCapabilityConfigurationAprilTag.AprilTagDict

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `APRIL_TAG_DICT_16H5` | `1` |
| `APRIL_TAG_DICT_25H9` | `2` |
| `APRIL_TAG_DICT_36H10` | `3` |
| `APRIL_TAG_DICT_36H11` | `4` |

## OpenXRSpatialCapabilityConfigurationAruco.ArucoDict

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ARUCO_DICT_4X4_50` | `1` |
| `ARUCO_DICT_4X4_100` | `2` |
| `ARUCO_DICT_4X4_250` | `3` |
| `ARUCO_DICT_4X4_1000` | `4` |
| `ARUCO_DICT_5X5_50` | `5` |
| `ARUCO_DICT_5X5_100` | `6` |
| `ARUCO_DICT_5X5_250` | `7` |
| `ARUCO_DICT_5X5_1000` | `8` |
| `ARUCO_DICT_6X6_50` | `9` |
| `ARUCO_DICT_6X6_100` | `10` |
| `ARUCO_DICT_6X6_250` | `11` |
| `ARUCO_DICT_6X6_1000` | `12` |
| `ARUCO_DICT_7X7_50` | `13` |
| `ARUCO_DICT_7X7_100` | `14` |
| `ARUCO_DICT_7X7_250` | `15` |
| `ARUCO_DICT_7X7_1000` | `16` |

## OpenXRSpatialComponentMarkerList.MarkerType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MARKER_TYPE_UNKNOWN` | `0` |
| `MARKER_TYPE_QRCODE` | `1` |
| `MARKER_TYPE_MICRO_QRCODE` | `2` |
| `MARKER_TYPE_ARUCO` | `3` |
| `MARKER_TYPE_APRIL_TAG` | `4` |
| `MARKER_TYPE_MAX` | `5` |

## OpenXRSpatialComponentPlaneAlignmentList.PlaneAlignment

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLANE_ALIGNMENT_HORIZONTAL_UPWARD` | `0` |
| `PLANE_ALIGNMENT_HORIZONTAL_DOWNWARD` | `1` |
| `PLANE_ALIGNMENT_VERTICAL` | `2` |
| `PLANE_ALIGNMENT_ARBITRARY` | `3` |

## OpenXRSpatialComponentPlaneSemanticLabelList.PlaneSemanticLabel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PLANE_SEMANTIC_LABEL_UNCATEGORIZED` | `1` |
| `PLANE_SEMANTIC_LABEL_FLOOR` | `2` |
| `PLANE_SEMANTIC_LABEL_WALL` | `3` |
| `PLANE_SEMANTIC_LABEL_CEILING` | `4` |
| `PLANE_SEMANTIC_LABEL_TABLE` | `5` |

## OpenXRSpatialEntityExtension.Capability

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CAPABILITY_PLANE_TRACKING` | `1000741000` |
| `CAPABILITY_MARKER_TRACKING_QR_CODE` | `1000743000` |
| `CAPABILITY_MARKER_TRACKING_MICRO_QR_CODE` | `1000743001` |
| `CAPABILITY_MARKER_TRACKING_ARUCO_MARKER` | `1000743002` |
| `CAPABILITY_MARKER_TRACKING_APRIL_TAG` | `1000743003` |
| `CAPABILITY_ANCHOR` | `1000762000` |

## OpenXRSpatialEntityExtension.ComponentType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPONENT_TYPE_BOUNDED_2D` | `1` |
| `COMPONENT_TYPE_BOUNDED_3D` | `2` |
| `COMPONENT_TYPE_PARENT` | `3` |
| `COMPONENT_TYPE_MESH_3D` | `4` |
| `COMPONENT_TYPE_PLANE_ALIGNMENT` | `1000741000` |
| `COMPONENT_TYPE_MESH_2D` | `1000741001` |
| `COMPONENT_TYPE_POLYGON_2D` | `1000741002` |
| `COMPONENT_TYPE_PLANE_SEMANTIC_LABEL` | `1000741003` |
| `COMPONENT_TYPE_MARKER` | `1000743000` |
| `COMPONENT_TYPE_ANCHOR` | `1000762000` |
| `COMPONENT_TYPE_PERSISTENCE` | `1000763000` |

## OpenXRSpatialEntityTracker.EntityTrackingState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENTITY_TRACKING_STATE_STOPPED` | `1` |
| `ENTITY_TRACKING_STATE_PAUSED` | `2` |
| `ENTITY_TRACKING_STATE_TRACKING` | `3` |

## PackedScene.GenEditState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GEN_EDIT_STATE_DISABLED` | `0` |
| `GEN_EDIT_STATE_INSTANCE` | `1` |
| `GEN_EDIT_STATE_MAIN` | `2` |
| `GEN_EDIT_STATE_MAIN_INHERITED` | `3` |

## PacketPeerDTLS.Status

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATUS_DISCONNECTED` | `0` |
| `STATUS_HANDSHAKING` | `1` |
| `STATUS_CONNECTED` | `2` |
| `STATUS_ERROR` | `3` |
| `STATUS_ERROR_HOSTNAME_MISMATCH` | `4` |

## ParticleProcessMaterial.Parameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_INITIAL_LINEAR_VELOCITY` | `0` |
| `PARAM_ANGULAR_VELOCITY` | `1` |
| `PARAM_ORBIT_VELOCITY` | `2` |
| `PARAM_LINEAR_ACCEL` | `3` |
| `PARAM_RADIAL_ACCEL` | `4` |
| `PARAM_TANGENTIAL_ACCEL` | `5` |
| `PARAM_DAMPING` | `6` |
| `PARAM_ANGLE` | `7` |
| `PARAM_SCALE` | `8` |
| `PARAM_HUE_VARIATION` | `9` |
| `PARAM_ANIM_SPEED` | `10` |
| `PARAM_ANIM_OFFSET` | `11` |
| `PARAM_RADIAL_VELOCITY` | `15` |
| `PARAM_DIRECTIONAL_VELOCITY` | `16` |
| `PARAM_SCALE_OVER_VELOCITY` | `17` |
| `PARAM_MAX` | `18` |
| `PARAM_TURB_VEL_INFLUENCE` | `13` |
| `PARAM_TURB_INIT_DISPLACEMENT` | `14` |
| `PARAM_TURB_INFLUENCE_OVER_LIFE` | `12` |

## ParticleProcessMaterial.ParticleFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLE_FLAG_ALIGN_Y_TO_VELOCITY` | `0` |
| `PARTICLE_FLAG_ROTATE_Y` | `1` |
| `PARTICLE_FLAG_DISABLE_Z` | `2` |
| `PARTICLE_FLAG_DAMPING_AS_FRICTION` | `3` |
| `PARTICLE_FLAG_INHERIT_EMITTER_SCALE` | `4` |
| `PARTICLE_FLAG_MAX` | `5` |

## ParticleProcessMaterial.EmissionShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMISSION_SHAPE_POINT` | `0` |
| `EMISSION_SHAPE_SPHERE` | `1` |
| `EMISSION_SHAPE_SPHERE_SURFACE` | `2` |
| `EMISSION_SHAPE_BOX` | `3` |
| `EMISSION_SHAPE_POINTS` | `4` |
| `EMISSION_SHAPE_DIRECTED_POINTS` | `5` |
| `EMISSION_SHAPE_RING` | `6` |
| `EMISSION_SHAPE_MAX` | `7` |

## ParticleProcessMaterial.SubEmitterMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SUB_EMITTER_DISABLED` | `0` |
| `SUB_EMITTER_CONSTANT` | `1` |
| `SUB_EMITTER_AT_END` | `2` |
| `SUB_EMITTER_AT_COLLISION` | `3` |
| `SUB_EMITTER_AT_START` | `4` |
| `SUB_EMITTER_MAX` | `5` |

## ParticleProcessMaterial.CollisionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COLLISION_DISABLED` | `0` |
| `COLLISION_RIGID` | `1` |
| `COLLISION_HIDE_ON_CONTACT` | `2` |
| `COLLISION_MAX` | `3` |

## PathFollow3D.RotationMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ROTATION_NONE` | `0` |
| `ROTATION_Y` | `1` |
| `ROTATION_XY` | `2` |
| `ROTATION_XYZ` | `3` |
| `ROTATION_ORIENTED` | `4` |

## Performance.Monitor

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TIME_FPS` | `0` |
| `TIME_PROCESS` | `1` |
| `TIME_PHYSICS_PROCESS` | `2` |
| `TIME_NAVIGATION_PROCESS` | `3` |
| `MEMORY_STATIC` | `4` |
| `MEMORY_STATIC_MAX` | `5` |
| `MEMORY_MESSAGE_BUFFER_MAX` | `6` |
| `OBJECT_COUNT` | `7` |
| `OBJECT_RESOURCE_COUNT` | `8` |
| `OBJECT_NODE_COUNT` | `9` |
| `OBJECT_ORPHAN_NODE_COUNT` | `10` |
| `RENDER_TOTAL_OBJECTS_IN_FRAME` | `11` |
| `RENDER_TOTAL_PRIMITIVES_IN_FRAME` | `12` |
| `RENDER_TOTAL_DRAW_CALLS_IN_FRAME` | `13` |
| `RENDER_VIDEO_MEM_USED` | `14` |
| `RENDER_TEXTURE_MEM_USED` | `15` |
| `RENDER_BUFFER_MEM_USED` | `16` |
| `PHYSICS_2D_ACTIVE_OBJECTS` | `17` |
| `PHYSICS_2D_COLLISION_PAIRS` | `18` |
| `PHYSICS_2D_ISLAND_COUNT` | `19` |
| `PHYSICS_3D_ACTIVE_OBJECTS` | `20` |
| `PHYSICS_3D_COLLISION_PAIRS` | `21` |
| `PHYSICS_3D_ISLAND_COUNT` | `22` |
| `AUDIO_OUTPUT_LATENCY` | `23` |
| `NAVIGATION_ACTIVE_MAPS` | `24` |
| `NAVIGATION_REGION_COUNT` | `25` |
| `NAVIGATION_AGENT_COUNT` | `26` |
| `NAVIGATION_LINK_COUNT` | `27` |
| `NAVIGATION_POLYGON_COUNT` | `28` |
| `NAVIGATION_EDGE_COUNT` | `29` |
| `NAVIGATION_EDGE_MERGE_COUNT` | `30` |
| `NAVIGATION_EDGE_CONNECTION_COUNT` | `31` |
| `NAVIGATION_EDGE_FREE_COUNT` | `32` |
| `NAVIGATION_OBSTACLE_COUNT` | `33` |
| `PIPELINE_COMPILATIONS_CANVAS` | `34` |
| `PIPELINE_COMPILATIONS_MESH` | `35` |
| `PIPELINE_COMPILATIONS_SURFACE` | `36` |
| `PIPELINE_COMPILATIONS_DRAW` | `37` |
| `PIPELINE_COMPILATIONS_SPECIALIZATION` | `38` |
| `NAVIGATION_2D_ACTIVE_MAPS` | `39` |
| `NAVIGATION_2D_REGION_COUNT` | `40` |
| `NAVIGATION_2D_AGENT_COUNT` | `41` |
| `NAVIGATION_2D_LINK_COUNT` | `42` |
| `NAVIGATION_2D_POLYGON_COUNT` | `43` |
| `NAVIGATION_2D_EDGE_COUNT` | `44` |
| `NAVIGATION_2D_EDGE_MERGE_COUNT` | `45` |
| `NAVIGATION_2D_EDGE_CONNECTION_COUNT` | `46` |
| `NAVIGATION_2D_EDGE_FREE_COUNT` | `47` |
| `NAVIGATION_2D_OBSTACLE_COUNT` | `48` |
| `NAVIGATION_3D_ACTIVE_MAPS` | `49` |
| `NAVIGATION_3D_REGION_COUNT` | `50` |
| `NAVIGATION_3D_AGENT_COUNT` | `51` |
| `NAVIGATION_3D_LINK_COUNT` | `52` |
| `NAVIGATION_3D_POLYGON_COUNT` | `53` |
| `NAVIGATION_3D_EDGE_COUNT` | `54` |
| `NAVIGATION_3D_EDGE_MERGE_COUNT` | `55` |
| `NAVIGATION_3D_EDGE_CONNECTION_COUNT` | `56` |
| `NAVIGATION_3D_EDGE_FREE_COUNT` | `57` |
| `NAVIGATION_3D_OBSTACLE_COUNT` | `58` |
| `MONITOR_MAX` | `59` |

## Performance.MonitorType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MONITOR_TYPE_QUANTITY` | `0` |
| `MONITOR_TYPE_MEMORY` | `1` |
| `MONITOR_TYPE_TIME` | `2` |
| `MONITOR_TYPE_PERCENTAGE` | `3` |

## PhysicalBone3D.DampMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DAMP_MODE_COMBINE` | `0` |
| `DAMP_MODE_REPLACE` | `1` |

## PhysicalBone3D.JointType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOINT_TYPE_NONE` | `0` |
| `JOINT_TYPE_PIN` | `1` |
| `JOINT_TYPE_CONE` | `2` |
| `JOINT_TYPE_HINGE` | `3` |
| `JOINT_TYPE_SLIDER` | `4` |
| `JOINT_TYPE_6DOF` | `5` |

## PhysicsServer2D.SpaceParameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPACE_PARAM_CONTACT_RECYCLE_RADIUS` | `0` |
| `SPACE_PARAM_CONTACT_MAX_SEPARATION` | `1` |
| `SPACE_PARAM_CONTACT_MAX_ALLOWED_PENETRATION` | `2` |
| `SPACE_PARAM_CONTACT_DEFAULT_BIAS` | `3` |
| `SPACE_PARAM_BODY_LINEAR_VELOCITY_SLEEP_THRESHOLD` | `4` |
| `SPACE_PARAM_BODY_ANGULAR_VELOCITY_SLEEP_THRESHOLD` | `5` |
| `SPACE_PARAM_BODY_TIME_TO_SLEEP` | `6` |
| `SPACE_PARAM_CONSTRAINT_DEFAULT_BIAS` | `7` |
| `SPACE_PARAM_SOLVER_ITERATIONS` | `8` |

## PhysicsServer2D.ShapeType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHAPE_WORLD_BOUNDARY` | `0` |
| `SHAPE_SEPARATION_RAY` | `1` |
| `SHAPE_SEGMENT` | `2` |
| `SHAPE_CIRCLE` | `3` |
| `SHAPE_RECTANGLE` | `4` |
| `SHAPE_CAPSULE` | `5` |
| `SHAPE_CONVEX_POLYGON` | `6` |
| `SHAPE_CONCAVE_POLYGON` | `7` |
| `SHAPE_CUSTOM` | `8` |

## PhysicsServer2D.AreaParameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AREA_PARAM_GRAVITY_OVERRIDE_MODE` | `0` |
| `AREA_PARAM_GRAVITY` | `1` |
| `AREA_PARAM_GRAVITY_VECTOR` | `2` |
| `AREA_PARAM_GRAVITY_IS_POINT` | `3` |
| `AREA_PARAM_GRAVITY_POINT_UNIT_DISTANCE` | `4` |
| `AREA_PARAM_LINEAR_DAMP_OVERRIDE_MODE` | `5` |
| `AREA_PARAM_LINEAR_DAMP` | `6` |
| `AREA_PARAM_ANGULAR_DAMP_OVERRIDE_MODE` | `7` |
| `AREA_PARAM_ANGULAR_DAMP` | `8` |
| `AREA_PARAM_PRIORITY` | `9` |

## PhysicsServer2D.AreaSpaceOverrideMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AREA_SPACE_OVERRIDE_DISABLED` | `0` |
| `AREA_SPACE_OVERRIDE_COMBINE` | `1` |
| `AREA_SPACE_OVERRIDE_COMBINE_REPLACE` | `2` |
| `AREA_SPACE_OVERRIDE_REPLACE` | `3` |
| `AREA_SPACE_OVERRIDE_REPLACE_COMBINE` | `4` |

## PhysicsServer2D.BodyMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_MODE_STATIC` | `0` |
| `BODY_MODE_KINEMATIC` | `1` |
| `BODY_MODE_RIGID` | `2` |
| `BODY_MODE_RIGID_LINEAR` | `3` |

## PhysicsServer2D.BodyParameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_PARAM_BOUNCE` | `0` |
| `BODY_PARAM_FRICTION` | `1` |
| `BODY_PARAM_MASS` | `2` |
| `BODY_PARAM_INERTIA` | `3` |
| `BODY_PARAM_CENTER_OF_MASS` | `4` |
| `BODY_PARAM_GRAVITY_SCALE` | `5` |
| `BODY_PARAM_LINEAR_DAMP_MODE` | `6` |
| `BODY_PARAM_ANGULAR_DAMP_MODE` | `7` |
| `BODY_PARAM_LINEAR_DAMP` | `8` |
| `BODY_PARAM_ANGULAR_DAMP` | `9` |
| `BODY_PARAM_MAX` | `10` |

## PhysicsServer2D.BodyDampMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_DAMP_MODE_COMBINE` | `0` |
| `BODY_DAMP_MODE_REPLACE` | `1` |

## PhysicsServer2D.BodyState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_STATE_TRANSFORM` | `0` |
| `BODY_STATE_LINEAR_VELOCITY` | `1` |
| `BODY_STATE_ANGULAR_VELOCITY` | `2` |
| `BODY_STATE_SLEEPING` | `3` |
| `BODY_STATE_CAN_SLEEP` | `4` |

## PhysicsServer2D.JointType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOINT_TYPE_PIN` | `0` |
| `JOINT_TYPE_GROOVE` | `1` |
| `JOINT_TYPE_DAMPED_SPRING` | `2` |
| `JOINT_TYPE_MAX` | `3` |

## PhysicsServer2D.JointParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOINT_PARAM_BIAS` | `0` |
| `JOINT_PARAM_MAX_BIAS` | `1` |
| `JOINT_PARAM_MAX_FORCE` | `2` |

## PhysicsServer2D.PinJointParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PIN_JOINT_SOFTNESS` | `0` |
| `PIN_JOINT_LIMIT_UPPER` | `1` |
| `PIN_JOINT_LIMIT_LOWER` | `2` |
| `PIN_JOINT_MOTOR_TARGET_VELOCITY` | `3` |

## PhysicsServer2D.PinJointFlag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PIN_JOINT_FLAG_ANGULAR_LIMIT_ENABLED` | `0` |
| `PIN_JOINT_FLAG_MOTOR_ENABLED` | `1` |

## PhysicsServer2D.DampedSpringParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DAMPED_SPRING_REST_LENGTH` | `0` |
| `DAMPED_SPRING_STIFFNESS` | `1` |
| `DAMPED_SPRING_DAMPING` | `2` |

## PhysicsServer2D.CCDMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CCD_MODE_DISABLED` | `0` |
| `CCD_MODE_CAST_RAY` | `1` |
| `CCD_MODE_CAST_SHAPE` | `2` |

## PhysicsServer2D.AreaBodyStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AREA_BODY_ADDED` | `0` |
| `AREA_BODY_REMOVED` | `1` |

## PhysicsServer2D.ProcessInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INFO_ACTIVE_OBJECTS` | `0` |
| `INFO_COLLISION_PAIRS` | `1` |
| `INFO_ISLAND_COUNT` | `2` |

## PhysicsServer3D.JointType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOINT_TYPE_PIN` | `0` |
| `JOINT_TYPE_HINGE` | `1` |
| `JOINT_TYPE_SLIDER` | `2` |
| `JOINT_TYPE_CONE_TWIST` | `3` |
| `JOINT_TYPE_6DOF` | `4` |
| `JOINT_TYPE_MAX` | `5` |

## PhysicsServer3D.PinJointParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PIN_JOINT_BIAS` | `0` |
| `PIN_JOINT_DAMPING` | `1` |
| `PIN_JOINT_IMPULSE_CLAMP` | `2` |

## PhysicsServer3D.HingeJointParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HINGE_JOINT_BIAS` | `0` |
| `HINGE_JOINT_LIMIT_UPPER` | `1` |
| `HINGE_JOINT_LIMIT_LOWER` | `2` |
| `HINGE_JOINT_LIMIT_BIAS` | `3` |
| `HINGE_JOINT_LIMIT_SOFTNESS` | `4` |
| `HINGE_JOINT_LIMIT_RELAXATION` | `5` |
| `HINGE_JOINT_MOTOR_TARGET_VELOCITY` | `6` |
| `HINGE_JOINT_MOTOR_MAX_IMPULSE` | `7` |

## PhysicsServer3D.HingeJointFlag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HINGE_JOINT_FLAG_USE_LIMIT` | `0` |
| `HINGE_JOINT_FLAG_ENABLE_MOTOR` | `1` |

## PhysicsServer3D.SliderJointParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SLIDER_JOINT_LINEAR_LIMIT_UPPER` | `0` |
| `SLIDER_JOINT_LINEAR_LIMIT_LOWER` | `1` |
| `SLIDER_JOINT_LINEAR_LIMIT_SOFTNESS` | `2` |
| `SLIDER_JOINT_LINEAR_LIMIT_RESTITUTION` | `3` |
| `SLIDER_JOINT_LINEAR_LIMIT_DAMPING` | `4` |
| `SLIDER_JOINT_LINEAR_MOTION_SOFTNESS` | `5` |
| `SLIDER_JOINT_LINEAR_MOTION_RESTITUTION` | `6` |
| `SLIDER_JOINT_LINEAR_MOTION_DAMPING` | `7` |
| `SLIDER_JOINT_LINEAR_ORTHOGONAL_SOFTNESS` | `8` |
| `SLIDER_JOINT_LINEAR_ORTHOGONAL_RESTITUTION` | `9` |
| `SLIDER_JOINT_LINEAR_ORTHOGONAL_DAMPING` | `10` |
| `SLIDER_JOINT_ANGULAR_LIMIT_UPPER` | `11` |
| `SLIDER_JOINT_ANGULAR_LIMIT_LOWER` | `12` |
| `SLIDER_JOINT_ANGULAR_LIMIT_SOFTNESS` | `13` |
| `SLIDER_JOINT_ANGULAR_LIMIT_RESTITUTION` | `14` |
| `SLIDER_JOINT_ANGULAR_LIMIT_DAMPING` | `15` |
| `SLIDER_JOINT_ANGULAR_MOTION_SOFTNESS` | `16` |
| `SLIDER_JOINT_ANGULAR_MOTION_RESTITUTION` | `17` |
| `SLIDER_JOINT_ANGULAR_MOTION_DAMPING` | `18` |
| `SLIDER_JOINT_ANGULAR_ORTHOGONAL_SOFTNESS` | `19` |
| `SLIDER_JOINT_ANGULAR_ORTHOGONAL_RESTITUTION` | `20` |
| `SLIDER_JOINT_ANGULAR_ORTHOGONAL_DAMPING` | `21` |
| `SLIDER_JOINT_MAX` | `22` |

## PhysicsServer3D.ConeTwistJointParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONE_TWIST_JOINT_SWING_SPAN` | `0` |
| `CONE_TWIST_JOINT_TWIST_SPAN` | `1` |
| `CONE_TWIST_JOINT_BIAS` | `2` |
| `CONE_TWIST_JOINT_SOFTNESS` | `3` |
| `CONE_TWIST_JOINT_RELAXATION` | `4` |

## PhysicsServer3D.G6DOFJointAxisParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `G6DOF_JOINT_LINEAR_LOWER_LIMIT` | `0` |
| `G6DOF_JOINT_LINEAR_UPPER_LIMIT` | `1` |
| `G6DOF_JOINT_LINEAR_LIMIT_SOFTNESS` | `2` |
| `G6DOF_JOINT_LINEAR_RESTITUTION` | `3` |
| `G6DOF_JOINT_LINEAR_DAMPING` | `4` |
| `G6DOF_JOINT_LINEAR_MOTOR_TARGET_VELOCITY` | `5` |
| `G6DOF_JOINT_LINEAR_MOTOR_FORCE_LIMIT` | `6` |
| `G6DOF_JOINT_LINEAR_SPRING_STIFFNESS` | `7` |
| `G6DOF_JOINT_LINEAR_SPRING_DAMPING` | `8` |
| `G6DOF_JOINT_LINEAR_SPRING_EQUILIBRIUM_POINT` | `9` |
| `G6DOF_JOINT_ANGULAR_LOWER_LIMIT` | `10` |
| `G6DOF_JOINT_ANGULAR_UPPER_LIMIT` | `11` |
| `G6DOF_JOINT_ANGULAR_LIMIT_SOFTNESS` | `12` |
| `G6DOF_JOINT_ANGULAR_DAMPING` | `13` |
| `G6DOF_JOINT_ANGULAR_RESTITUTION` | `14` |
| `G6DOF_JOINT_ANGULAR_FORCE_LIMIT` | `15` |
| `G6DOF_JOINT_ANGULAR_ERP` | `16` |
| `G6DOF_JOINT_ANGULAR_MOTOR_TARGET_VELOCITY` | `17` |
| `G6DOF_JOINT_ANGULAR_MOTOR_FORCE_LIMIT` | `18` |
| `G6DOF_JOINT_ANGULAR_SPRING_STIFFNESS` | `19` |
| `G6DOF_JOINT_ANGULAR_SPRING_DAMPING` | `20` |
| `G6DOF_JOINT_ANGULAR_SPRING_EQUILIBRIUM_POINT` | `21` |
| `G6DOF_JOINT_MAX` | `22` |

## PhysicsServer3D.G6DOFJointAxisFlag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `G6DOF_JOINT_FLAG_ENABLE_LINEAR_LIMIT` | `0` |
| `G6DOF_JOINT_FLAG_ENABLE_ANGULAR_LIMIT` | `1` |
| `G6DOF_JOINT_FLAG_ENABLE_ANGULAR_SPRING` | `2` |
| `G6DOF_JOINT_FLAG_ENABLE_LINEAR_SPRING` | `3` |
| `G6DOF_JOINT_FLAG_ENABLE_MOTOR` | `4` |
| `G6DOF_JOINT_FLAG_ENABLE_LINEAR_MOTOR` | `5` |
| `G6DOF_JOINT_FLAG_MAX` | `6` |

## PhysicsServer3D.ShapeType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHAPE_WORLD_BOUNDARY` | `0` |
| `SHAPE_SEPARATION_RAY` | `1` |
| `SHAPE_SPHERE` | `2` |
| `SHAPE_BOX` | `3` |
| `SHAPE_CAPSULE` | `4` |
| `SHAPE_CYLINDER` | `5` |
| `SHAPE_CONVEX_POLYGON` | `6` |
| `SHAPE_CONCAVE_POLYGON` | `7` |
| `SHAPE_HEIGHTMAP` | `8` |
| `SHAPE_SOFT_BODY` | `9` |
| `SHAPE_CUSTOM` | `10` |

## PhysicsServer3D.AreaParameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AREA_PARAM_GRAVITY_OVERRIDE_MODE` | `0` |
| `AREA_PARAM_GRAVITY` | `1` |
| `AREA_PARAM_GRAVITY_VECTOR` | `2` |
| `AREA_PARAM_GRAVITY_IS_POINT` | `3` |
| `AREA_PARAM_GRAVITY_POINT_UNIT_DISTANCE` | `4` |
| `AREA_PARAM_LINEAR_DAMP_OVERRIDE_MODE` | `5` |
| `AREA_PARAM_LINEAR_DAMP` | `6` |
| `AREA_PARAM_ANGULAR_DAMP_OVERRIDE_MODE` | `7` |
| `AREA_PARAM_ANGULAR_DAMP` | `8` |
| `AREA_PARAM_PRIORITY` | `9` |
| `AREA_PARAM_WIND_FORCE_MAGNITUDE` | `10` |
| `AREA_PARAM_WIND_SOURCE` | `11` |
| `AREA_PARAM_WIND_DIRECTION` | `12` |
| `AREA_PARAM_WIND_ATTENUATION_FACTOR` | `13` |

## PhysicsServer3D.AreaSpaceOverrideMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AREA_SPACE_OVERRIDE_DISABLED` | `0` |
| `AREA_SPACE_OVERRIDE_COMBINE` | `1` |
| `AREA_SPACE_OVERRIDE_COMBINE_REPLACE` | `2` |
| `AREA_SPACE_OVERRIDE_REPLACE` | `3` |
| `AREA_SPACE_OVERRIDE_REPLACE_COMBINE` | `4` |

## PhysicsServer3D.BodyMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_MODE_STATIC` | `0` |
| `BODY_MODE_KINEMATIC` | `1` |
| `BODY_MODE_RIGID` | `2` |
| `BODY_MODE_RIGID_LINEAR` | `3` |

## PhysicsServer3D.BodyParameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_PARAM_BOUNCE` | `0` |
| `BODY_PARAM_FRICTION` | `1` |
| `BODY_PARAM_MASS` | `2` |
| `BODY_PARAM_INERTIA` | `3` |
| `BODY_PARAM_CENTER_OF_MASS` | `4` |
| `BODY_PARAM_GRAVITY_SCALE` | `5` |
| `BODY_PARAM_LINEAR_DAMP_MODE` | `6` |
| `BODY_PARAM_ANGULAR_DAMP_MODE` | `7` |
| `BODY_PARAM_LINEAR_DAMP` | `8` |
| `BODY_PARAM_ANGULAR_DAMP` | `9` |
| `BODY_PARAM_MAX` | `10` |

## PhysicsServer3D.BodyDampMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_DAMP_MODE_COMBINE` | `0` |
| `BODY_DAMP_MODE_REPLACE` | `1` |

## PhysicsServer3D.BodyState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_STATE_TRANSFORM` | `0` |
| `BODY_STATE_LINEAR_VELOCITY` | `1` |
| `BODY_STATE_ANGULAR_VELOCITY` | `2` |
| `BODY_STATE_SLEEPING` | `3` |
| `BODY_STATE_CAN_SLEEP` | `4` |

## PhysicsServer3D.AreaBodyStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AREA_BODY_ADDED` | `0` |
| `AREA_BODY_REMOVED` | `1` |

## PhysicsServer3D.ProcessInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INFO_ACTIVE_OBJECTS` | `0` |
| `INFO_COLLISION_PAIRS` | `1` |
| `INFO_ISLAND_COUNT` | `2` |

## PhysicsServer3D.SpaceParameter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPACE_PARAM_CONTACT_RECYCLE_RADIUS` | `0` |
| `SPACE_PARAM_CONTACT_MAX_SEPARATION` | `1` |
| `SPACE_PARAM_CONTACT_MAX_ALLOWED_PENETRATION` | `2` |
| `SPACE_PARAM_CONTACT_DEFAULT_BIAS` | `3` |
| `SPACE_PARAM_BODY_LINEAR_VELOCITY_SLEEP_THRESHOLD` | `4` |
| `SPACE_PARAM_BODY_ANGULAR_VELOCITY_SLEEP_THRESHOLD` | `5` |
| `SPACE_PARAM_BODY_TIME_TO_SLEEP` | `6` |
| `SPACE_PARAM_SOLVER_ITERATIONS` | `7` |

## PhysicsServer3D.BodyAxis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BODY_AXIS_LINEAR_X` | `1` |
| `BODY_AXIS_LINEAR_Y` | `2` |
| `BODY_AXIS_LINEAR_Z` | `4` |
| `BODY_AXIS_ANGULAR_X` | `8` |
| `BODY_AXIS_ANGULAR_Y` | `16` |
| `BODY_AXIS_ANGULAR_Z` | `32` |

## PinJoint3D.Param

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_BIAS` | `0` |
| `PARAM_DAMPING` | `1` |
| `PARAM_IMPULSE_CLAMP` | `2` |

## PlaneMesh.Orientation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FACE_X` | `0` |
| `FACE_Y` | `1` |
| `FACE_Z` | `2` |

## PortableCompressedTexture2D.CompressionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPRESSION_MODE_LOSSLESS` | `0` |
| `COMPRESSION_MODE_LOSSY` | `1` |
| `COMPRESSION_MODE_BASIS_UNIVERSAL` | `2` |
| `COMPRESSION_MODE_S3TC` | `3` |
| `COMPRESSION_MODE_ETC2` | `4` |
| `COMPRESSION_MODE_BPTC` | `5` |
| `COMPRESSION_MODE_ASTC` | `6` |

## ProgressBar.FillMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILL_BEGIN_TO_END` | `0` |
| `FILL_END_TO_BEGIN` | `1` |
| `FILL_TOP_TO_BOTTOM` | `2` |
| `FILL_BOTTOM_TO_TOP` | `3` |

## ReflectionProbe.UpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UPDATE_ONCE` | `0` |
| `UPDATE_ALWAYS` | `1` |

## ReflectionProbe.AmbientMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AMBIENT_DISABLED` | `0` |
| `AMBIENT_ENVIRONMENT` | `1` |
| `AMBIENT_COLOR` | `2` |

## RenderingDevice.DeviceType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEVICE_TYPE_OTHER` | `0` |
| `DEVICE_TYPE_INTEGRATED_GPU` | `1` |
| `DEVICE_TYPE_DISCRETE_GPU` | `2` |
| `DEVICE_TYPE_VIRTUAL_GPU` | `3` |
| `DEVICE_TYPE_CPU` | `4` |
| `DEVICE_TYPE_MAX` | `5` |

## RenderingDevice.DriverResource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRIVER_RESOURCE_LOGICAL_DEVICE` | `0` |
| `DRIVER_RESOURCE_PHYSICAL_DEVICE` | `1` |
| `DRIVER_RESOURCE_TOPMOST_OBJECT` | `2` |
| `DRIVER_RESOURCE_COMMAND_QUEUE` | `3` |
| `DRIVER_RESOURCE_QUEUE_FAMILY` | `4` |
| `DRIVER_RESOURCE_TEXTURE` | `5` |
| `DRIVER_RESOURCE_TEXTURE_VIEW` | `6` |
| `DRIVER_RESOURCE_TEXTURE_DATA_FORMAT` | `7` |
| `DRIVER_RESOURCE_SAMPLER` | `8` |
| `DRIVER_RESOURCE_UNIFORM_SET` | `9` |
| `DRIVER_RESOURCE_BUFFER` | `10` |
| `DRIVER_RESOURCE_COMPUTE_PIPELINE` | `11` |
| `DRIVER_RESOURCE_RENDER_PIPELINE` | `12` |
| `DRIVER_RESOURCE_VULKAN_DEVICE` | `0` |
| `DRIVER_RESOURCE_VULKAN_PHYSICAL_DEVICE` | `1` |
| `DRIVER_RESOURCE_VULKAN_INSTANCE` | `2` |
| `DRIVER_RESOURCE_VULKAN_QUEUE` | `3` |
| `DRIVER_RESOURCE_VULKAN_QUEUE_FAMILY_INDEX` | `4` |
| `DRIVER_RESOURCE_VULKAN_IMAGE` | `5` |
| `DRIVER_RESOURCE_VULKAN_IMAGE_VIEW` | `6` |
| `DRIVER_RESOURCE_VULKAN_IMAGE_NATIVE_TEXTURE_FORMAT` | `7` |
| `DRIVER_RESOURCE_VULKAN_SAMPLER` | `8` |
| `DRIVER_RESOURCE_VULKAN_DESCRIPTOR_SET` | `9` |
| `DRIVER_RESOURCE_VULKAN_BUFFER` | `10` |
| `DRIVER_RESOURCE_VULKAN_COMPUTE_PIPELINE` | `11` |
| `DRIVER_RESOURCE_VULKAN_RENDER_PIPELINE` | `12` |

## RenderingDevice.DataFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DATA_FORMAT_R4G4_UNORM_PACK8` | `0` |
| `DATA_FORMAT_R4G4B4A4_UNORM_PACK16` | `1` |
| `DATA_FORMAT_B4G4R4A4_UNORM_PACK16` | `2` |
| `DATA_FORMAT_R5G6B5_UNORM_PACK16` | `3` |
| `DATA_FORMAT_B5G6R5_UNORM_PACK16` | `4` |
| `DATA_FORMAT_R5G5B5A1_UNORM_PACK16` | `5` |
| `DATA_FORMAT_B5G5R5A1_UNORM_PACK16` | `6` |
| `DATA_FORMAT_A1R5G5B5_UNORM_PACK16` | `7` |
| `DATA_FORMAT_R8_UNORM` | `8` |
| `DATA_FORMAT_R8_SNORM` | `9` |
| `DATA_FORMAT_R8_USCALED` | `10` |
| `DATA_FORMAT_R8_SSCALED` | `11` |
| `DATA_FORMAT_R8_UINT` | `12` |
| `DATA_FORMAT_R8_SINT` | `13` |
| `DATA_FORMAT_R8_SRGB` | `14` |
| `DATA_FORMAT_R8G8_UNORM` | `15` |
| `DATA_FORMAT_R8G8_SNORM` | `16` |
| `DATA_FORMAT_R8G8_USCALED` | `17` |
| `DATA_FORMAT_R8G8_SSCALED` | `18` |
| `DATA_FORMAT_R8G8_UINT` | `19` |
| `DATA_FORMAT_R8G8_SINT` | `20` |
| `DATA_FORMAT_R8G8_SRGB` | `21` |
| `DATA_FORMAT_R8G8B8_UNORM` | `22` |
| `DATA_FORMAT_R8G8B8_SNORM` | `23` |
| `DATA_FORMAT_R8G8B8_USCALED` | `24` |
| `DATA_FORMAT_R8G8B8_SSCALED` | `25` |
| `DATA_FORMAT_R8G8B8_UINT` | `26` |
| `DATA_FORMAT_R8G8B8_SINT` | `27` |
| `DATA_FORMAT_R8G8B8_SRGB` | `28` |
| `DATA_FORMAT_B8G8R8_UNORM` | `29` |
| `DATA_FORMAT_B8G8R8_SNORM` | `30` |
| `DATA_FORMAT_B8G8R8_USCALED` | `31` |
| `DATA_FORMAT_B8G8R8_SSCALED` | `32` |
| `DATA_FORMAT_B8G8R8_UINT` | `33` |
| `DATA_FORMAT_B8G8R8_SINT` | `34` |
| `DATA_FORMAT_B8G8R8_SRGB` | `35` |
| `DATA_FORMAT_R8G8B8A8_UNORM` | `36` |
| `DATA_FORMAT_R8G8B8A8_SNORM` | `37` |
| `DATA_FORMAT_R8G8B8A8_USCALED` | `38` |
| `DATA_FORMAT_R8G8B8A8_SSCALED` | `39` |
| `DATA_FORMAT_R8G8B8A8_UINT` | `40` |
| `DATA_FORMAT_R8G8B8A8_SINT` | `41` |
| `DATA_FORMAT_R8G8B8A8_SRGB` | `42` |
| `DATA_FORMAT_B8G8R8A8_UNORM` | `43` |
| `DATA_FORMAT_B8G8R8A8_SNORM` | `44` |
| `DATA_FORMAT_B8G8R8A8_USCALED` | `45` |
| `DATA_FORMAT_B8G8R8A8_SSCALED` | `46` |
| `DATA_FORMAT_B8G8R8A8_UINT` | `47` |
| `DATA_FORMAT_B8G8R8A8_SINT` | `48` |
| `DATA_FORMAT_B8G8R8A8_SRGB` | `49` |
| `DATA_FORMAT_A8B8G8R8_UNORM_PACK32` | `50` |
| `DATA_FORMAT_A8B8G8R8_SNORM_PACK32` | `51` |
| `DATA_FORMAT_A8B8G8R8_USCALED_PACK32` | `52` |
| `DATA_FORMAT_A8B8G8R8_SSCALED_PACK32` | `53` |
| `DATA_FORMAT_A8B8G8R8_UINT_PACK32` | `54` |
| `DATA_FORMAT_A8B8G8R8_SINT_PACK32` | `55` |
| `DATA_FORMAT_A8B8G8R8_SRGB_PACK32` | `56` |
| `DATA_FORMAT_A2R10G10B10_UNORM_PACK32` | `57` |
| `DATA_FORMAT_A2R10G10B10_SNORM_PACK32` | `58` |
| `DATA_FORMAT_A2R10G10B10_USCALED_PACK32` | `59` |
| `DATA_FORMAT_A2R10G10B10_SSCALED_PACK32` | `60` |
| `DATA_FORMAT_A2R10G10B10_UINT_PACK32` | `61` |
| `DATA_FORMAT_A2R10G10B10_SINT_PACK32` | `62` |
| `DATA_FORMAT_A2B10G10R10_UNORM_PACK32` | `63` |
| `DATA_FORMAT_A2B10G10R10_SNORM_PACK32` | `64` |
| `DATA_FORMAT_A2B10G10R10_USCALED_PACK32` | `65` |
| `DATA_FORMAT_A2B10G10R10_SSCALED_PACK32` | `66` |
| `DATA_FORMAT_A2B10G10R10_UINT_PACK32` | `67` |
| `DATA_FORMAT_A2B10G10R10_SINT_PACK32` | `68` |
| `DATA_FORMAT_R16_UNORM` | `69` |
| `DATA_FORMAT_R16_SNORM` | `70` |
| `DATA_FORMAT_R16_USCALED` | `71` |
| `DATA_FORMAT_R16_SSCALED` | `72` |
| `DATA_FORMAT_R16_UINT` | `73` |
| `DATA_FORMAT_R16_SINT` | `74` |
| `DATA_FORMAT_R16_SFLOAT` | `75` |
| `DATA_FORMAT_R16G16_UNORM` | `76` |
| `DATA_FORMAT_R16G16_SNORM` | `77` |
| `DATA_FORMAT_R16G16_USCALED` | `78` |
| `DATA_FORMAT_R16G16_SSCALED` | `79` |
| `DATA_FORMAT_R16G16_UINT` | `80` |
| `DATA_FORMAT_R16G16_SINT` | `81` |
| `DATA_FORMAT_R16G16_SFLOAT` | `82` |
| `DATA_FORMAT_R16G16B16_UNORM` | `83` |
| `DATA_FORMAT_R16G16B16_SNORM` | `84` |
| `DATA_FORMAT_R16G16B16_USCALED` | `85` |
| `DATA_FORMAT_R16G16B16_SSCALED` | `86` |
| `DATA_FORMAT_R16G16B16_UINT` | `87` |
| `DATA_FORMAT_R16G16B16_SINT` | `88` |
| `DATA_FORMAT_R16G16B16_SFLOAT` | `89` |
| `DATA_FORMAT_R16G16B16A16_UNORM` | `90` |
| `DATA_FORMAT_R16G16B16A16_SNORM` | `91` |
| `DATA_FORMAT_R16G16B16A16_USCALED` | `92` |
| `DATA_FORMAT_R16G16B16A16_SSCALED` | `93` |
| `DATA_FORMAT_R16G16B16A16_UINT` | `94` |
| `DATA_FORMAT_R16G16B16A16_SINT` | `95` |
| `DATA_FORMAT_R16G16B16A16_SFLOAT` | `96` |
| `DATA_FORMAT_R32_UINT` | `97` |
| `DATA_FORMAT_R32_SINT` | `98` |
| `DATA_FORMAT_R32_SFLOAT` | `99` |
| `DATA_FORMAT_R32G32_UINT` | `100` |
| `DATA_FORMAT_R32G32_SINT` | `101` |
| `DATA_FORMAT_R32G32_SFLOAT` | `102` |
| `DATA_FORMAT_R32G32B32_UINT` | `103` |
| `DATA_FORMAT_R32G32B32_SINT` | `104` |
| `DATA_FORMAT_R32G32B32_SFLOAT` | `105` |
| `DATA_FORMAT_R32G32B32A32_UINT` | `106` |
| `DATA_FORMAT_R32G32B32A32_SINT` | `107` |
| `DATA_FORMAT_R32G32B32A32_SFLOAT` | `108` |
| `DATA_FORMAT_R64_UINT` | `109` |
| `DATA_FORMAT_R64_SINT` | `110` |
| `DATA_FORMAT_R64_SFLOAT` | `111` |
| `DATA_FORMAT_R64G64_UINT` | `112` |
| `DATA_FORMAT_R64G64_SINT` | `113` |
| `DATA_FORMAT_R64G64_SFLOAT` | `114` |
| `DATA_FORMAT_R64G64B64_UINT` | `115` |
| `DATA_FORMAT_R64G64B64_SINT` | `116` |
| `DATA_FORMAT_R64G64B64_SFLOAT` | `117` |
| `DATA_FORMAT_R64G64B64A64_UINT` | `118` |
| `DATA_FORMAT_R64G64B64A64_SINT` | `119` |
| `DATA_FORMAT_R64G64B64A64_SFLOAT` | `120` |
| `DATA_FORMAT_B10G11R11_UFLOAT_PACK32` | `121` |
| `DATA_FORMAT_E5B9G9R9_UFLOAT_PACK32` | `122` |
| `DATA_FORMAT_D16_UNORM` | `123` |
| `DATA_FORMAT_X8_D24_UNORM_PACK32` | `124` |
| `DATA_FORMAT_D32_SFLOAT` | `125` |
| `DATA_FORMAT_S8_UINT` | `126` |
| `DATA_FORMAT_D16_UNORM_S8_UINT` | `127` |
| `DATA_FORMAT_D24_UNORM_S8_UINT` | `128` |
| `DATA_FORMAT_D32_SFLOAT_S8_UINT` | `129` |
| `DATA_FORMAT_BC1_RGB_UNORM_BLOCK` | `130` |
| `DATA_FORMAT_BC1_RGB_SRGB_BLOCK` | `131` |
| `DATA_FORMAT_BC1_RGBA_UNORM_BLOCK` | `132` |
| `DATA_FORMAT_BC1_RGBA_SRGB_BLOCK` | `133` |
| `DATA_FORMAT_BC2_UNORM_BLOCK` | `134` |
| `DATA_FORMAT_BC2_SRGB_BLOCK` | `135` |
| `DATA_FORMAT_BC3_UNORM_BLOCK` | `136` |
| `DATA_FORMAT_BC3_SRGB_BLOCK` | `137` |
| `DATA_FORMAT_BC4_UNORM_BLOCK` | `138` |
| `DATA_FORMAT_BC4_SNORM_BLOCK` | `139` |
| `DATA_FORMAT_BC5_UNORM_BLOCK` | `140` |
| `DATA_FORMAT_BC5_SNORM_BLOCK` | `141` |
| `DATA_FORMAT_BC6H_UFLOAT_BLOCK` | `142` |
| `DATA_FORMAT_BC6H_SFLOAT_BLOCK` | `143` |
| `DATA_FORMAT_BC7_UNORM_BLOCK` | `144` |
| `DATA_FORMAT_BC7_SRGB_BLOCK` | `145` |
| `DATA_FORMAT_ETC2_R8G8B8_UNORM_BLOCK` | `146` |
| `DATA_FORMAT_ETC2_R8G8B8_SRGB_BLOCK` | `147` |
| `DATA_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK` | `148` |
| `DATA_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK` | `149` |
| `DATA_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK` | `150` |
| `DATA_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK` | `151` |
| `DATA_FORMAT_EAC_R11_UNORM_BLOCK` | `152` |
| `DATA_FORMAT_EAC_R11_SNORM_BLOCK` | `153` |
| `DATA_FORMAT_EAC_R11G11_UNORM_BLOCK` | `154` |
| `DATA_FORMAT_EAC_R11G11_SNORM_BLOCK` | `155` |
| `DATA_FORMAT_ASTC_4x4_UNORM_BLOCK` | `156` |
| `DATA_FORMAT_ASTC_4x4_SRGB_BLOCK` | `157` |
| `DATA_FORMAT_ASTC_5x4_UNORM_BLOCK` | `158` |
| `DATA_FORMAT_ASTC_5x4_SRGB_BLOCK` | `159` |
| `DATA_FORMAT_ASTC_5x5_UNORM_BLOCK` | `160` |
| `DATA_FORMAT_ASTC_5x5_SRGB_BLOCK` | `161` |
| `DATA_FORMAT_ASTC_6x5_UNORM_BLOCK` | `162` |
| `DATA_FORMAT_ASTC_6x5_SRGB_BLOCK` | `163` |
| `DATA_FORMAT_ASTC_6x6_UNORM_BLOCK` | `164` |
| `DATA_FORMAT_ASTC_6x6_SRGB_BLOCK` | `165` |
| `DATA_FORMAT_ASTC_8x5_UNORM_BLOCK` | `166` |
| `DATA_FORMAT_ASTC_8x5_SRGB_BLOCK` | `167` |
| `DATA_FORMAT_ASTC_8x6_UNORM_BLOCK` | `168` |
| `DATA_FORMAT_ASTC_8x6_SRGB_BLOCK` | `169` |
| `DATA_FORMAT_ASTC_8x8_UNORM_BLOCK` | `170` |
| `DATA_FORMAT_ASTC_8x8_SRGB_BLOCK` | `171` |
| `DATA_FORMAT_ASTC_10x5_UNORM_BLOCK` | `172` |
| `DATA_FORMAT_ASTC_10x5_SRGB_BLOCK` | `173` |
| `DATA_FORMAT_ASTC_10x6_UNORM_BLOCK` | `174` |
| `DATA_FORMAT_ASTC_10x6_SRGB_BLOCK` | `175` |
| `DATA_FORMAT_ASTC_10x8_UNORM_BLOCK` | `176` |
| `DATA_FORMAT_ASTC_10x8_SRGB_BLOCK` | `177` |
| `DATA_FORMAT_ASTC_10x10_UNORM_BLOCK` | `178` |
| `DATA_FORMAT_ASTC_10x10_SRGB_BLOCK` | `179` |
| `DATA_FORMAT_ASTC_12x10_UNORM_BLOCK` | `180` |
| `DATA_FORMAT_ASTC_12x10_SRGB_BLOCK` | `181` |
| `DATA_FORMAT_ASTC_12x12_UNORM_BLOCK` | `182` |
| `DATA_FORMAT_ASTC_12x12_SRGB_BLOCK` | `183` |
| `DATA_FORMAT_G8B8G8R8_422_UNORM` | `184` |
| `DATA_FORMAT_B8G8R8G8_422_UNORM` | `185` |
| `DATA_FORMAT_G8_B8_R8_3PLANE_420_UNORM` | `186` |
| `DATA_FORMAT_G8_B8R8_2PLANE_420_UNORM` | `187` |
| `DATA_FORMAT_G8_B8_R8_3PLANE_422_UNORM` | `188` |
| `DATA_FORMAT_G8_B8R8_2PLANE_422_UNORM` | `189` |
| `DATA_FORMAT_G8_B8_R8_3PLANE_444_UNORM` | `190` |
| `DATA_FORMAT_R10X6_UNORM_PACK16` | `191` |
| `DATA_FORMAT_R10X6G10X6_UNORM_2PACK16` | `192` |
| `DATA_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16` | `193` |
| `DATA_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16` | `194` |
| `DATA_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16` | `195` |
| `DATA_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16` | `196` |
| `DATA_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16` | `197` |
| `DATA_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16` | `198` |
| `DATA_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16` | `199` |
| `DATA_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16` | `200` |
| `DATA_FORMAT_R12X4_UNORM_PACK16` | `201` |
| `DATA_FORMAT_R12X4G12X4_UNORM_2PACK16` | `202` |
| `DATA_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16` | `203` |
| `DATA_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16` | `204` |
| `DATA_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16` | `205` |
| `DATA_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16` | `206` |
| `DATA_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16` | `207` |
| `DATA_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16` | `208` |
| `DATA_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16` | `209` |
| `DATA_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16` | `210` |
| `DATA_FORMAT_G16B16G16R16_422_UNORM` | `211` |
| `DATA_FORMAT_B16G16R16G16_422_UNORM` | `212` |
| `DATA_FORMAT_G16_B16_R16_3PLANE_420_UNORM` | `213` |
| `DATA_FORMAT_G16_B16R16_2PLANE_420_UNORM` | `214` |
| `DATA_FORMAT_G16_B16_R16_3PLANE_422_UNORM` | `215` |
| `DATA_FORMAT_G16_B16R16_2PLANE_422_UNORM` | `216` |
| `DATA_FORMAT_G16_B16_R16_3PLANE_444_UNORM` | `217` |
| `DATA_FORMAT_ASTC_4x4_SFLOAT_BLOCK` | `218` |
| `DATA_FORMAT_ASTC_5x4_SFLOAT_BLOCK` | `219` |
| `DATA_FORMAT_ASTC_5x5_SFLOAT_BLOCK` | `220` |
| `DATA_FORMAT_ASTC_6x5_SFLOAT_BLOCK` | `221` |
| `DATA_FORMAT_ASTC_6x6_SFLOAT_BLOCK` | `222` |
| `DATA_FORMAT_ASTC_8x5_SFLOAT_BLOCK` | `223` |
| `DATA_FORMAT_ASTC_8x6_SFLOAT_BLOCK` | `224` |
| `DATA_FORMAT_ASTC_8x8_SFLOAT_BLOCK` | `225` |
| `DATA_FORMAT_ASTC_10x5_SFLOAT_BLOCK` | `226` |
| `DATA_FORMAT_ASTC_10x6_SFLOAT_BLOCK` | `227` |
| `DATA_FORMAT_ASTC_10x8_SFLOAT_BLOCK` | `228` |
| `DATA_FORMAT_ASTC_10x10_SFLOAT_BLOCK` | `229` |
| `DATA_FORMAT_ASTC_12x10_SFLOAT_BLOCK` | `230` |
| `DATA_FORMAT_ASTC_12x12_SFLOAT_BLOCK` | `231` |
| `DATA_FORMAT_MAX` | `232` |

## RenderingDevice.BarrierMask

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `BARRIER_MASK_VERTEX` | `1` |
| `BARRIER_MASK_FRAGMENT` | `8` |
| `BARRIER_MASK_COMPUTE` | `2` |
| `BARRIER_MASK_TRANSFER` | `4` |
| `BARRIER_MASK_RASTER` | `9` |
| `BARRIER_MASK_ALL_BARRIERS` | `32767` |
| `BARRIER_MASK_NO_BARRIER` | `32768` |

## RenderingDevice.TextureType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_TYPE_1D` | `0` |
| `TEXTURE_TYPE_2D` | `1` |
| `TEXTURE_TYPE_3D` | `2` |
| `TEXTURE_TYPE_CUBE` | `3` |
| `TEXTURE_TYPE_1D_ARRAY` | `4` |
| `TEXTURE_TYPE_2D_ARRAY` | `5` |
| `TEXTURE_TYPE_CUBE_ARRAY` | `6` |
| `TEXTURE_TYPE_MAX` | `7` |

## RenderingDevice.TextureSamples

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_SAMPLES_1` | `0` |
| `TEXTURE_SAMPLES_2` | `1` |
| `TEXTURE_SAMPLES_4` | `2` |
| `TEXTURE_SAMPLES_8` | `3` |
| `TEXTURE_SAMPLES_16` | `4` |
| `TEXTURE_SAMPLES_32` | `5` |
| `TEXTURE_SAMPLES_64` | `6` |
| `TEXTURE_SAMPLES_MAX` | `7` |

## RenderingDevice.TextureUsageBits

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `TEXTURE_USAGE_SAMPLING_BIT` | `1` |
| `TEXTURE_USAGE_COLOR_ATTACHMENT_BIT` | `2` |
| `TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT` | `4` |
| `TEXTURE_USAGE_DEPTH_RESOLVE_ATTACHMENT_BIT` | `4096` |
| `TEXTURE_USAGE_STORAGE_BIT` | `8` |
| `TEXTURE_USAGE_STORAGE_ATOMIC_BIT` | `16` |
| `TEXTURE_USAGE_CPU_READ_BIT` | `32` |
| `TEXTURE_USAGE_CAN_UPDATE_BIT` | `64` |
| `TEXTURE_USAGE_CAN_COPY_FROM_BIT` | `128` |
| `TEXTURE_USAGE_CAN_COPY_TO_BIT` | `256` |
| `TEXTURE_USAGE_INPUT_ATTACHMENT_BIT` | `512` |

## RenderingDevice.TextureSwizzle

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_SWIZZLE_IDENTITY` | `0` |
| `TEXTURE_SWIZZLE_ZERO` | `1` |
| `TEXTURE_SWIZZLE_ONE` | `2` |
| `TEXTURE_SWIZZLE_R` | `3` |
| `TEXTURE_SWIZZLE_G` | `4` |
| `TEXTURE_SWIZZLE_B` | `5` |
| `TEXTURE_SWIZZLE_A` | `6` |
| `TEXTURE_SWIZZLE_MAX` | `7` |

## RenderingDevice.TextureSliceType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_SLICE_2D` | `0` |
| `TEXTURE_SLICE_CUBEMAP` | `1` |
| `TEXTURE_SLICE_3D` | `2` |

## RenderingDevice.SamplerFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SAMPLER_FILTER_NEAREST` | `0` |
| `SAMPLER_FILTER_LINEAR` | `1` |

## RenderingDevice.SamplerRepeatMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SAMPLER_REPEAT_MODE_REPEAT` | `0` |
| `SAMPLER_REPEAT_MODE_MIRRORED_REPEAT` | `1` |
| `SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE` | `2` |
| `SAMPLER_REPEAT_MODE_CLAMP_TO_BORDER` | `3` |
| `SAMPLER_REPEAT_MODE_MIRROR_CLAMP_TO_EDGE` | `4` |
| `SAMPLER_REPEAT_MODE_MAX` | `5` |

## RenderingDevice.SamplerBorderColor

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SAMPLER_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK` | `0` |
| `SAMPLER_BORDER_COLOR_INT_TRANSPARENT_BLACK` | `1` |
| `SAMPLER_BORDER_COLOR_FLOAT_OPAQUE_BLACK` | `2` |
| `SAMPLER_BORDER_COLOR_INT_OPAQUE_BLACK` | `3` |
| `SAMPLER_BORDER_COLOR_FLOAT_OPAQUE_WHITE` | `4` |
| `SAMPLER_BORDER_COLOR_INT_OPAQUE_WHITE` | `5` |
| `SAMPLER_BORDER_COLOR_MAX` | `6` |

## RenderingDevice.VertexFrequency

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VERTEX_FREQUENCY_VERTEX` | `0` |
| `VERTEX_FREQUENCY_INSTANCE` | `1` |

## RenderingDevice.IndexBufferFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INDEX_BUFFER_FORMAT_UINT16` | `0` |
| `INDEX_BUFFER_FORMAT_UINT32` | `1` |

## RenderingDevice.StorageBufferUsage

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `STORAGE_BUFFER_USAGE_DISPATCH_INDIRECT` | `1` |

## RenderingDevice.BufferCreationBits

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `BUFFER_CREATION_DEVICE_ADDRESS_BIT` | `1` |
| `BUFFER_CREATION_AS_STORAGE_BIT` | `2` |
| `BUFFER_CREATION_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT` | `8` |

## RenderingDevice.AccelerationStructureFlagBits

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT` | `1` |
| `ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT` | `2` |
| `ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT` | `4` |
| `ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT` | `8` |
| `ACCELERATION_STRUCTURE_LOW_MEMORY_BIT` | `16` |

## RenderingDevice.AccelerationStructureGeometryFlagBits

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `ACCELERATION_STRUCTURE_GEOMETRY_OPAQUE_BIT` | `1` |
| `ACCELERATION_STRUCTURE_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT` | `2` |

## RenderingDevice.AccelerationStructureInstanceFlagBits

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `ACCELERATION_STRUCTURE_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT` | `1` |
| `ACCELERATION_STRUCTURE_INSTANCE_TRIANGLE_FLIP_FACING_BIT` | `2` |
| `ACCELERATION_STRUCTURE_INSTANCE_FORCE_OPAQUE_BIT` | `4` |
| `ACCELERATION_STRUCTURE_INSTANCE_FORCE_NO_OPAQUE_BIT` | `8` |

## RenderingDevice.UniformType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UNIFORM_TYPE_SAMPLER` | `0` |
| `UNIFORM_TYPE_SAMPLER_WITH_TEXTURE` | `1` |
| `UNIFORM_TYPE_TEXTURE` | `2` |
| `UNIFORM_TYPE_IMAGE` | `3` |
| `UNIFORM_TYPE_TEXTURE_BUFFER` | `4` |
| `UNIFORM_TYPE_SAMPLER_WITH_TEXTURE_BUFFER` | `5` |
| `UNIFORM_TYPE_IMAGE_BUFFER` | `6` |
| `UNIFORM_TYPE_UNIFORM_BUFFER` | `7` |
| `UNIFORM_TYPE_STORAGE_BUFFER` | `8` |
| `UNIFORM_TYPE_INPUT_ATTACHMENT` | `9` |
| `UNIFORM_TYPE_UNIFORM_BUFFER_DYNAMIC` | `10` |
| `UNIFORM_TYPE_STORAGE_BUFFER_DYNAMIC` | `11` |
| `UNIFORM_TYPE_ACCELERATION_STRUCTURE` | `12` |
| `UNIFORM_TYPE_MAX` | `13` |

## RenderingDevice.RenderPrimitive

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RENDER_PRIMITIVE_POINTS` | `0` |
| `RENDER_PRIMITIVE_LINES` | `1` |
| `RENDER_PRIMITIVE_LINES_WITH_ADJACENCY` | `2` |
| `RENDER_PRIMITIVE_LINESTRIPS` | `3` |
| `RENDER_PRIMITIVE_LINESTRIPS_WITH_ADJACENCY` | `4` |
| `RENDER_PRIMITIVE_TRIANGLES` | `5` |
| `RENDER_PRIMITIVE_TRIANGLES_WITH_ADJACENCY` | `6` |
| `RENDER_PRIMITIVE_TRIANGLE_STRIPS` | `7` |
| `RENDER_PRIMITIVE_TRIANGLE_STRIPS_WITH_AJACENCY` | `8` |
| `RENDER_PRIMITIVE_TRIANGLE_STRIPS_WITH_RESTART_INDEX` | `9` |
| `RENDER_PRIMITIVE_TESSELATION_PATCH` | `10` |
| `RENDER_PRIMITIVE_MAX` | `11` |

## RenderingDevice.PolygonCullMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `POLYGON_CULL_DISABLED` | `0` |
| `POLYGON_CULL_FRONT` | `1` |
| `POLYGON_CULL_BACK` | `2` |

## RenderingDevice.PolygonFrontFace

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `POLYGON_FRONT_FACE_CLOCKWISE` | `0` |
| `POLYGON_FRONT_FACE_COUNTER_CLOCKWISE` | `1` |

## RenderingDevice.StencilOperation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STENCIL_OP_KEEP` | `0` |
| `STENCIL_OP_ZERO` | `1` |
| `STENCIL_OP_REPLACE` | `2` |
| `STENCIL_OP_INCREMENT_AND_CLAMP` | `3` |
| `STENCIL_OP_DECREMENT_AND_CLAMP` | `4` |
| `STENCIL_OP_INVERT` | `5` |
| `STENCIL_OP_INCREMENT_AND_WRAP` | `6` |
| `STENCIL_OP_DECREMENT_AND_WRAP` | `7` |
| `STENCIL_OP_MAX` | `8` |

## RenderingDevice.CompareOperator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPARE_OP_NEVER` | `0` |
| `COMPARE_OP_LESS` | `1` |
| `COMPARE_OP_EQUAL` | `2` |
| `COMPARE_OP_LESS_OR_EQUAL` | `3` |
| `COMPARE_OP_GREATER` | `4` |
| `COMPARE_OP_NOT_EQUAL` | `5` |
| `COMPARE_OP_GREATER_OR_EQUAL` | `6` |
| `COMPARE_OP_ALWAYS` | `7` |
| `COMPARE_OP_MAX` | `8` |

## RenderingDevice.LogicOperation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOGIC_OP_CLEAR` | `0` |
| `LOGIC_OP_AND` | `1` |
| `LOGIC_OP_AND_REVERSE` | `2` |
| `LOGIC_OP_COPY` | `3` |
| `LOGIC_OP_AND_INVERTED` | `4` |
| `LOGIC_OP_NO_OP` | `5` |
| `LOGIC_OP_XOR` | `6` |
| `LOGIC_OP_OR` | `7` |
| `LOGIC_OP_NOR` | `8` |
| `LOGIC_OP_EQUIVALENT` | `9` |
| `LOGIC_OP_INVERT` | `10` |
| `LOGIC_OP_OR_REVERSE` | `11` |
| `LOGIC_OP_COPY_INVERTED` | `12` |
| `LOGIC_OP_OR_INVERTED` | `13` |
| `LOGIC_OP_NAND` | `14` |
| `LOGIC_OP_SET` | `15` |
| `LOGIC_OP_MAX` | `16` |

## RenderingDevice.BlendFactor

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_FACTOR_ZERO` | `0` |
| `BLEND_FACTOR_ONE` | `1` |
| `BLEND_FACTOR_SRC_COLOR` | `2` |
| `BLEND_FACTOR_ONE_MINUS_SRC_COLOR` | `3` |
| `BLEND_FACTOR_DST_COLOR` | `4` |
| `BLEND_FACTOR_ONE_MINUS_DST_COLOR` | `5` |
| `BLEND_FACTOR_SRC_ALPHA` | `6` |
| `BLEND_FACTOR_ONE_MINUS_SRC_ALPHA` | `7` |
| `BLEND_FACTOR_DST_ALPHA` | `8` |
| `BLEND_FACTOR_ONE_MINUS_DST_ALPHA` | `9` |
| `BLEND_FACTOR_CONSTANT_COLOR` | `10` |
| `BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR` | `11` |
| `BLEND_FACTOR_CONSTANT_ALPHA` | `12` |
| `BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA` | `13` |
| `BLEND_FACTOR_SRC_ALPHA_SATURATE` | `14` |
| `BLEND_FACTOR_SRC1_COLOR` | `15` |
| `BLEND_FACTOR_ONE_MINUS_SRC1_COLOR` | `16` |
| `BLEND_FACTOR_SRC1_ALPHA` | `17` |
| `BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA` | `18` |
| `BLEND_FACTOR_MAX` | `19` |

## RenderingDevice.BlendOperation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_OP_ADD` | `0` |
| `BLEND_OP_SUBTRACT` | `1` |
| `BLEND_OP_REVERSE_SUBTRACT` | `2` |
| `BLEND_OP_MINIMUM` | `3` |
| `BLEND_OP_MAXIMUM` | `4` |
| `BLEND_OP_MAX` | `5` |

## RenderingDevice.PipelineDynamicStateFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `DYNAMIC_STATE_LINE_WIDTH` | `1` |
| `DYNAMIC_STATE_DEPTH_BIAS` | `2` |
| `DYNAMIC_STATE_BLEND_CONSTANTS` | `4` |
| `DYNAMIC_STATE_DEPTH_BOUNDS` | `8` |
| `DYNAMIC_STATE_STENCIL_COMPARE_MASK` | `16` |
| `DYNAMIC_STATE_STENCIL_WRITE_MASK` | `32` |
| `DYNAMIC_STATE_STENCIL_REFERENCE` | `64` |

## RenderingDevice.InitialAction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INITIAL_ACTION_LOAD` | `0` |
| `INITIAL_ACTION_CLEAR` | `1` |
| `INITIAL_ACTION_DISCARD` | `2` |
| `INITIAL_ACTION_MAX` | `3` |
| `INITIAL_ACTION_CLEAR_REGION` | `1` |
| `INITIAL_ACTION_CLEAR_REGION_CONTINUE` | `1` |
| `INITIAL_ACTION_KEEP` | `0` |
| `INITIAL_ACTION_DROP` | `2` |
| `INITIAL_ACTION_CONTINUE` | `0` |

## RenderingDevice.FinalAction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FINAL_ACTION_STORE` | `0` |
| `FINAL_ACTION_DISCARD` | `1` |
| `FINAL_ACTION_MAX` | `2` |
| `FINAL_ACTION_READ` | `0` |
| `FINAL_ACTION_CONTINUE` | `0` |

## RenderingDevice.ShaderStage

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADER_STAGE_VERTEX` | `0` |
| `SHADER_STAGE_FRAGMENT` | `1` |
| `SHADER_STAGE_TESSELATION_CONTROL` | `2` |
| `SHADER_STAGE_TESSELATION_EVALUATION` | `3` |
| `SHADER_STAGE_COMPUTE` | `4` |
| `SHADER_STAGE_RAYGEN` | `5` |
| `SHADER_STAGE_ANY_HIT` | `6` |
| `SHADER_STAGE_CLOSEST_HIT` | `7` |
| `SHADER_STAGE_MISS` | `8` |
| `SHADER_STAGE_INTERSECTION` | `9` |
| `SHADER_STAGE_MAX` | `10` |
| `SHADER_STAGE_VERTEX_BIT` | `1` |
| `SHADER_STAGE_FRAGMENT_BIT` | `2` |
| `SHADER_STAGE_TESSELATION_CONTROL_BIT` | `4` |
| `SHADER_STAGE_TESSELATION_EVALUATION_BIT` | `8` |
| `SHADER_STAGE_COMPUTE_BIT` | `16` |
| `SHADER_STAGE_RAYGEN_BIT` | `32` |
| `SHADER_STAGE_ANY_HIT_BIT` | `64` |
| `SHADER_STAGE_CLOSEST_HIT_BIT` | `128` |
| `SHADER_STAGE_MISS_BIT` | `256` |
| `SHADER_STAGE_INTERSECTION_BIT` | `512` |

## RenderingDevice.ShaderLanguage

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADER_LANGUAGE_GLSL` | `0` |
| `SHADER_LANGUAGE_HLSL` | `1` |

## RenderingDevice.PipelineSpecializationConstantType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PIPELINE_SPECIALIZATION_CONSTANT_TYPE_BOOL` | `0` |
| `PIPELINE_SPECIALIZATION_CONSTANT_TYPE_INT` | `1` |
| `PIPELINE_SPECIALIZATION_CONSTANT_TYPE_FLOAT` | `2` |

## RenderingDevice.Features

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SUPPORTS_METALFX_SPATIAL` | `3` |
| `SUPPORTS_METALFX_TEMPORAL` | `4` |
| `SUPPORTS_BUFFER_DEVICE_ADDRESS` | `6` |
| `SUPPORTS_IMAGE_ATOMIC_32_BIT` | `7` |
| `SUPPORTS_RAY_QUERY` | `11` |
| `SUPPORTS_RAYTRACING_PIPELINE` | `12` |
| `SUPPORTS_HDR_OUTPUT` | `13` |

## RenderingDevice.Limit

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIMIT_MAX_BOUND_UNIFORM_SETS` | `0` |
| `LIMIT_MAX_FRAMEBUFFER_COLOR_ATTACHMENTS` | `1` |
| `LIMIT_MAX_TEXTURES_PER_UNIFORM_SET` | `2` |
| `LIMIT_MAX_SAMPLERS_PER_UNIFORM_SET` | `3` |
| `LIMIT_MAX_STORAGE_BUFFERS_PER_UNIFORM_SET` | `4` |
| `LIMIT_MAX_STORAGE_IMAGES_PER_UNIFORM_SET` | `5` |
| `LIMIT_MAX_UNIFORM_BUFFERS_PER_UNIFORM_SET` | `6` |
| `LIMIT_MAX_DRAW_INDEXED_INDEX` | `7` |
| `LIMIT_MAX_FRAMEBUFFER_HEIGHT` | `8` |
| `LIMIT_MAX_FRAMEBUFFER_WIDTH` | `9` |
| `LIMIT_MAX_TEXTURE_ARRAY_LAYERS` | `10` |
| `LIMIT_MAX_TEXTURE_SIZE_1D` | `11` |
| `LIMIT_MAX_TEXTURE_SIZE_2D` | `12` |
| `LIMIT_MAX_TEXTURE_SIZE_3D` | `13` |
| `LIMIT_MAX_TEXTURE_SIZE_CUBE` | `14` |
| `LIMIT_MAX_TEXTURES_PER_SHADER_STAGE` | `15` |
| `LIMIT_MAX_SAMPLERS_PER_SHADER_STAGE` | `16` |
| `LIMIT_MAX_STORAGE_BUFFERS_PER_SHADER_STAGE` | `17` |
| `LIMIT_MAX_STORAGE_IMAGES_PER_SHADER_STAGE` | `18` |
| `LIMIT_MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE` | `19` |
| `LIMIT_MAX_PUSH_CONSTANT_SIZE` | `20` |
| `LIMIT_MAX_UNIFORM_BUFFER_SIZE` | `21` |
| `LIMIT_MAX_VERTEX_INPUT_ATTRIBUTE_OFFSET` | `22` |
| `LIMIT_MAX_VERTEX_INPUT_ATTRIBUTES` | `23` |
| `LIMIT_MAX_VERTEX_INPUT_BINDINGS` | `24` |
| `LIMIT_MAX_VERTEX_INPUT_BINDING_STRIDE` | `25` |
| `LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT` | `26` |
| `LIMIT_MAX_COMPUTE_SHARED_MEMORY_SIZE` | `27` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_COUNT_X` | `28` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_COUNT_Y` | `29` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_COUNT_Z` | `30` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_INVOCATIONS` | `31` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_SIZE_X` | `32` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_SIZE_Y` | `33` |
| `LIMIT_MAX_COMPUTE_WORKGROUP_SIZE_Z` | `34` |
| `LIMIT_MAX_VIEWPORT_DIMENSIONS_X` | `35` |
| `LIMIT_MAX_VIEWPORT_DIMENSIONS_Y` | `36` |
| `LIMIT_METALFX_TEMPORAL_SCALER_MIN_SCALE` | `46` |
| `LIMIT_METALFX_TEMPORAL_SCALER_MAX_SCALE` | `47` |

## RenderingDevice.MemoryType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MEMORY_TEXTURES` | `0` |
| `MEMORY_BUFFERS` | `1` |
| `MEMORY_TOTAL` | `2` |

## RenderingDevice.BreadcrumbMarker

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `NONE` | `0` |
| `REFLECTION_PROBES` | `65536` |
| `SKY_PASS` | `131072` |
| `LIGHTMAPPER_PASS` | `196608` |
| `SHADOW_PASS_DIRECTIONAL` | `262144` |
| `SHADOW_PASS_CUBE` | `327680` |
| `OPAQUE_PASS` | `393216` |
| `ALPHA_PASS` | `458752` |
| `TRANSPARENT_PASS` | `524288` |
| `POST_PROCESSING_PASS` | `589824` |
| `BLIT_PASS` | `655360` |
| `UI_PASS` | `720896` |
| `DEBUG_PASS` | `786432` |

## RenderingDevice.DrawFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `DRAW_DEFAULT_ALL` | `0` |
| `DRAW_CLEAR_COLOR_0` | `1` |
| `DRAW_CLEAR_COLOR_1` | `2` |
| `DRAW_CLEAR_COLOR_2` | `4` |
| `DRAW_CLEAR_COLOR_3` | `8` |
| `DRAW_CLEAR_COLOR_4` | `16` |
| `DRAW_CLEAR_COLOR_5` | `32` |
| `DRAW_CLEAR_COLOR_6` | `64` |
| `DRAW_CLEAR_COLOR_7` | `128` |
| `DRAW_CLEAR_COLOR_MASK` | `255` |
| `DRAW_CLEAR_COLOR_ALL` | `255` |
| `DRAW_IGNORE_COLOR_0` | `256` |
| `DRAW_IGNORE_COLOR_1` | `512` |
| `DRAW_IGNORE_COLOR_2` | `1024` |
| `DRAW_IGNORE_COLOR_3` | `2048` |
| `DRAW_IGNORE_COLOR_4` | `4096` |
| `DRAW_IGNORE_COLOR_5` | `8192` |
| `DRAW_IGNORE_COLOR_6` | `16384` |
| `DRAW_IGNORE_COLOR_7` | `32768` |
| `DRAW_IGNORE_COLOR_MASK` | `65280` |
| `DRAW_IGNORE_COLOR_ALL` | `65280` |
| `DRAW_CLEAR_DEPTH` | `65536` |
| `DRAW_IGNORE_DEPTH` | `131072` |
| `DRAW_CLEAR_STENCIL` | `262144` |
| `DRAW_IGNORE_STENCIL` | `524288` |
| `DRAW_CLEAR_ALL` | `327935` |
| `DRAW_IGNORE_ALL` | `720640` |

## RenderingServer.TextureType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_TYPE_2D` | `0` |
| `TEXTURE_TYPE_LAYERED` | `1` |
| `TEXTURE_TYPE_3D` | `2` |

## RenderingServer.TextureLayeredType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_LAYERED_2D_ARRAY` | `0` |
| `TEXTURE_LAYERED_CUBEMAP` | `1` |
| `TEXTURE_LAYERED_CUBEMAP_ARRAY` | `2` |

## RenderingServer.CubeMapLayer

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CUBEMAP_LAYER_LEFT` | `0` |
| `CUBEMAP_LAYER_RIGHT` | `1` |
| `CUBEMAP_LAYER_BOTTOM` | `2` |
| `CUBEMAP_LAYER_TOP` | `3` |
| `CUBEMAP_LAYER_FRONT` | `4` |
| `CUBEMAP_LAYER_BACK` | `5` |

## RenderingServer.TextureDrawableFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TEXTURE_DRAWABLE_FORMAT_RGBA8` | `0` |
| `TEXTURE_DRAWABLE_FORMAT_RGBA8_SRGB` | `1` |
| `TEXTURE_DRAWABLE_FORMAT_RGBAH` | `2` |
| `TEXTURE_DRAWABLE_FORMAT_RGBAF` | `3` |

## RenderingServer.ShaderMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADER_SPATIAL` | `0` |
| `SHADER_CANVAS_ITEM` | `1` |
| `SHADER_PARTICLES` | `2` |
| `SHADER_SKY` | `3` |
| `SHADER_FOG` | `4` |
| `SHADER_TEXTURE_BLIT` | `5` |
| `SHADER_MAX` | `6` |

## RenderingServer.ArrayType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ARRAY_VERTEX` | `0` |
| `ARRAY_NORMAL` | `1` |
| `ARRAY_TANGENT` | `2` |
| `ARRAY_COLOR` | `3` |
| `ARRAY_TEX_UV` | `4` |
| `ARRAY_TEX_UV2` | `5` |
| `ARRAY_CUSTOM0` | `6` |
| `ARRAY_CUSTOM1` | `7` |
| `ARRAY_CUSTOM2` | `8` |
| `ARRAY_CUSTOM3` | `9` |
| `ARRAY_BONES` | `10` |
| `ARRAY_WEIGHTS` | `11` |
| `ARRAY_INDEX` | `12` |
| `ARRAY_MAX` | `13` |

## RenderingServer.ArrayCustomFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ARRAY_CUSTOM_RGBA8_UNORM` | `0` |
| `ARRAY_CUSTOM_RGBA8_SNORM` | `1` |
| `ARRAY_CUSTOM_RG_HALF` | `2` |
| `ARRAY_CUSTOM_RGBA_HALF` | `3` |
| `ARRAY_CUSTOM_R_FLOAT` | `4` |
| `ARRAY_CUSTOM_RG_FLOAT` | `5` |
| `ARRAY_CUSTOM_RGB_FLOAT` | `6` |
| `ARRAY_CUSTOM_RGBA_FLOAT` | `7` |
| `ARRAY_CUSTOM_MAX` | `8` |

## RenderingServer.ArrayFormat

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `ARRAY_FORMAT_VERTEX` | `1` |
| `ARRAY_FORMAT_NORMAL` | `2` |
| `ARRAY_FORMAT_TANGENT` | `4` |
| `ARRAY_FORMAT_COLOR` | `8` |
| `ARRAY_FORMAT_TEX_UV` | `16` |
| `ARRAY_FORMAT_TEX_UV2` | `32` |
| `ARRAY_FORMAT_CUSTOM0` | `64` |
| `ARRAY_FORMAT_CUSTOM1` | `128` |
| `ARRAY_FORMAT_CUSTOM2` | `256` |
| `ARRAY_FORMAT_CUSTOM3` | `512` |
| `ARRAY_FORMAT_BONES` | `1024` |
| `ARRAY_FORMAT_WEIGHTS` | `2048` |
| `ARRAY_FORMAT_INDEX` | `4096` |
| `ARRAY_FORMAT_BLEND_SHAPE_MASK` | `7` |
| `ARRAY_FORMAT_CUSTOM_BASE` | `13` |
| `ARRAY_FORMAT_CUSTOM_BITS` | `3` |
| `ARRAY_FORMAT_CUSTOM0_SHIFT` | `13` |
| `ARRAY_FORMAT_CUSTOM1_SHIFT` | `16` |
| `ARRAY_FORMAT_CUSTOM2_SHIFT` | `19` |
| `ARRAY_FORMAT_CUSTOM3_SHIFT` | `22` |
| `ARRAY_FORMAT_CUSTOM_MASK` | `7` |
| `ARRAY_COMPRESS_FLAGS_BASE` | `25` |
| `ARRAY_FLAG_USE_2D_VERTICES` | `33554432` |
| `ARRAY_FLAG_USE_DYNAMIC_UPDATE` | `67108864` |
| `ARRAY_FLAG_USE_8_BONE_WEIGHTS` | `134217728` |
| `ARRAY_FLAG_USES_EMPTY_VERTEX_ARRAY` | `268435456` |
| `ARRAY_FLAG_COMPRESS_ATTRIBUTES` | `536870912` |
| `ARRAY_FLAG_FORMAT_VERSION_BASE` | `35` |
| `ARRAY_FLAG_FORMAT_VERSION_SHIFT` | `35` |
| `ARRAY_FLAG_FORMAT_VERSION_1` | `0` |
| `ARRAY_FLAG_FORMAT_VERSION_2` | `34359738368` |
| `ARRAY_FLAG_FORMAT_CURRENT_VERSION` | `34359738368` |
| `ARRAY_FLAG_FORMAT_VERSION_MASK` | `255` |

## RenderingServer.PrimitiveType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PRIMITIVE_POINTS` | `0` |
| `PRIMITIVE_LINES` | `1` |
| `PRIMITIVE_LINE_STRIP` | `2` |
| `PRIMITIVE_TRIANGLES` | `3` |
| `PRIMITIVE_TRIANGLE_STRIP` | `4` |
| `PRIMITIVE_MAX` | `5` |

## RenderingServer.BlendShapeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BLEND_SHAPE_MODE_NORMALIZED` | `0` |
| `BLEND_SHAPE_MODE_RELATIVE` | `1` |

## RenderingServer.MultimeshTransformFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MULTIMESH_TRANSFORM_2D` | `0` |
| `MULTIMESH_TRANSFORM_3D` | `1` |

## RenderingServer.MultimeshPhysicsInterpolationQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MULTIMESH_INTERP_QUALITY_FAST` | `0` |
| `MULTIMESH_INTERP_QUALITY_HIGH` | `1` |

## RenderingServer.LightProjectorFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_PROJECTOR_FILTER_NEAREST` | `0` |
| `LIGHT_PROJECTOR_FILTER_LINEAR` | `1` |
| `LIGHT_PROJECTOR_FILTER_NEAREST_MIPMAPS` | `2` |
| `LIGHT_PROJECTOR_FILTER_LINEAR_MIPMAPS` | `3` |
| `LIGHT_PROJECTOR_FILTER_NEAREST_MIPMAPS_ANISOTROPIC` | `4` |
| `LIGHT_PROJECTOR_FILTER_LINEAR_MIPMAPS_ANISOTROPIC` | `5` |

## RenderingServer.LightType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_DIRECTIONAL` | `0` |
| `LIGHT_OMNI` | `1` |
| `LIGHT_SPOT` | `2` |
| `LIGHT_AREA` | `3` |

## RenderingServer.LightParam

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_PARAM_ENERGY` | `0` |
| `LIGHT_PARAM_INDIRECT_ENERGY` | `1` |
| `LIGHT_PARAM_VOLUMETRIC_FOG_ENERGY` | `2` |
| `LIGHT_PARAM_SPECULAR` | `3` |
| `LIGHT_PARAM_RANGE` | `4` |
| `LIGHT_PARAM_SIZE` | `5` |
| `LIGHT_PARAM_ATTENUATION` | `6` |
| `LIGHT_PARAM_SPOT_ANGLE` | `7` |
| `LIGHT_PARAM_SPOT_ATTENUATION` | `8` |
| `LIGHT_PARAM_SHADOW_MAX_DISTANCE` | `9` |
| `LIGHT_PARAM_SHADOW_SPLIT_1_OFFSET` | `10` |
| `LIGHT_PARAM_SHADOW_SPLIT_2_OFFSET` | `11` |
| `LIGHT_PARAM_SHADOW_SPLIT_3_OFFSET` | `12` |
| `LIGHT_PARAM_SHADOW_FADE_START` | `13` |
| `LIGHT_PARAM_SHADOW_NORMAL_BIAS` | `14` |
| `LIGHT_PARAM_SHADOW_BIAS` | `15` |
| `LIGHT_PARAM_SHADOW_PANCAKE_SIZE` | `16` |
| `LIGHT_PARAM_SHADOW_OPACITY` | `17` |
| `LIGHT_PARAM_SHADOW_BLUR` | `18` |
| `LIGHT_PARAM_TRANSMITTANCE_BIAS` | `19` |
| `LIGHT_PARAM_INTENSITY` | `20` |
| `LIGHT_PARAM_MAX` | `21` |

## RenderingServer.LightBakeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_BAKE_DISABLED` | `0` |
| `LIGHT_BAKE_STATIC` | `1` |
| `LIGHT_BAKE_DYNAMIC` | `2` |

## RenderingServer.LightOmniShadowMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_OMNI_SHADOW_DUAL_PARABOLOID` | `0` |
| `LIGHT_OMNI_SHADOW_CUBE` | `1` |

## RenderingServer.LightDirectionalShadowMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_DIRECTIONAL_SHADOW_ORTHOGONAL` | `0` |
| `LIGHT_DIRECTIONAL_SHADOW_PARALLEL_2_SPLITS` | `1` |
| `LIGHT_DIRECTIONAL_SHADOW_PARALLEL_4_SPLITS` | `2` |

## RenderingServer.LightDirectionalSkyMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIGHT_DIRECTIONAL_SKY_MODE_LIGHT_AND_SKY` | `0` |
| `LIGHT_DIRECTIONAL_SKY_MODE_LIGHT_ONLY` | `1` |
| `LIGHT_DIRECTIONAL_SKY_MODE_SKY_ONLY` | `2` |

## RenderingServer.ShadowQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_QUALITY_HARD` | `0` |
| `SHADOW_QUALITY_SOFT_VERY_LOW` | `1` |
| `SHADOW_QUALITY_SOFT_LOW` | `2` |
| `SHADOW_QUALITY_SOFT_MEDIUM` | `3` |
| `SHADOW_QUALITY_SOFT_HIGH` | `4` |
| `SHADOW_QUALITY_SOFT_ULTRA` | `5` |
| `SHADOW_QUALITY_MAX` | `6` |

## RenderingServer.ReflectionProbeUpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REFLECTION_PROBE_UPDATE_ONCE` | `0` |
| `REFLECTION_PROBE_UPDATE_ALWAYS` | `1` |

## RenderingServer.ReflectionProbeAmbientMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REFLECTION_PROBE_AMBIENT_DISABLED` | `0` |
| `REFLECTION_PROBE_AMBIENT_ENVIRONMENT` | `1` |
| `REFLECTION_PROBE_AMBIENT_COLOR` | `2` |

## RenderingServer.DecalTexture

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DECAL_TEXTURE_ALBEDO` | `0` |
| `DECAL_TEXTURE_NORMAL` | `1` |
| `DECAL_TEXTURE_ORM` | `2` |
| `DECAL_TEXTURE_EMISSION` | `3` |
| `DECAL_TEXTURE_MAX` | `4` |

## RenderingServer.DecalFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DECAL_FILTER_NEAREST` | `0` |
| `DECAL_FILTER_LINEAR` | `1` |
| `DECAL_FILTER_NEAREST_MIPMAPS` | `2` |
| `DECAL_FILTER_LINEAR_MIPMAPS` | `3` |
| `DECAL_FILTER_NEAREST_MIPMAPS_ANISOTROPIC` | `4` |
| `DECAL_FILTER_LINEAR_MIPMAPS_ANISOTROPIC` | `5` |

## RenderingServer.VoxelGIQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VOXEL_GI_QUALITY_LOW` | `0` |
| `VOXEL_GI_QUALITY_HIGH` | `1` |

## RenderingServer.ParticlesMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_MODE_2D` | `0` |
| `PARTICLES_MODE_3D` | `1` |

## RenderingServer.ParticlesTransformAlign

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_TRANSFORM_ALIGN_DISABLED` | `0` |
| `PARTICLES_TRANSFORM_ALIGN_Z_BILLBOARD` | `1` |
| `PARTICLES_TRANSFORM_ALIGN_Y_TO_VELOCITY` | `2` |
| `PARTICLES_TRANSFORM_ALIGN_Z_BILLBOARD_Y_TO_VELOCITY` | `3` |
| `PARTICLES_TRANSFORM_ALIGN_LOCAL_BILLBOARD` | `4` |

## RenderingServer.ParticlesTransformAlignCustomSrc

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_ALIGN_CHANNEL_FILTER_DISABLED` | `0` |
| `PARTICLES_ALIGN_CHANNEL_FILTER_X` | `1` |
| `PARTICLES_ALIGN_CHANNEL_FILTER_Y` | `2` |
| `PARTICLES_ALIGN_CHANNEL_FILTER_Z` | `3` |
| `PARTICLES_ALIGN_CHANNEL_FILTER_W` | `4` |

## RenderingServer.ParticlesTransformAlignAxis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_ALIGN_AXIS_X` | `0` |
| `PARTICLES_ALIGN_AXIS_Y` | `1` |

## RenderingServer.ParticlesDrawOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_DRAW_ORDER_INDEX` | `0` |
| `PARTICLES_DRAW_ORDER_LIFETIME` | `1` |
| `PARTICLES_DRAW_ORDER_REVERSE_LIFETIME` | `2` |
| `PARTICLES_DRAW_ORDER_VIEW_DEPTH` | `3` |

## RenderingServer.ParticlesCollisionType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_COLLISION_TYPE_SPHERE_ATTRACT` | `0` |
| `PARTICLES_COLLISION_TYPE_BOX_ATTRACT` | `1` |
| `PARTICLES_COLLISION_TYPE_VECTOR_FIELD_ATTRACT` | `2` |
| `PARTICLES_COLLISION_TYPE_SPHERE_COLLIDE` | `3` |
| `PARTICLES_COLLISION_TYPE_BOX_COLLIDE` | `4` |
| `PARTICLES_COLLISION_TYPE_SDF_COLLIDE` | `5` |
| `PARTICLES_COLLISION_TYPE_HEIGHTFIELD_COLLIDE` | `6` |

## RenderingServer.ParticlesCollisionHeightfieldResolution

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_256` | `0` |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_512` | `1` |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_1024` | `2` |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_2048` | `3` |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_4096` | `4` |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_8192` | `5` |
| `PARTICLES_COLLISION_HEIGHTFIELD_RESOLUTION_MAX` | `6` |

## RenderingServer.FogVolumeShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FOG_VOLUME_SHAPE_ELLIPSOID` | `0` |
| `FOG_VOLUME_SHAPE_CONE` | `1` |
| `FOG_VOLUME_SHAPE_CYLINDER` | `2` |
| `FOG_VOLUME_SHAPE_BOX` | `3` |
| `FOG_VOLUME_SHAPE_WORLD` | `4` |
| `FOG_VOLUME_SHAPE_MAX` | `5` |

## RenderingServer.ViewportScaling3DMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_SCALING_3D_MODE_BILINEAR` | `0` |
| `VIEWPORT_SCALING_3D_MODE_FSR` | `1` |
| `VIEWPORT_SCALING_3D_MODE_FSR2` | `2` |
| `VIEWPORT_SCALING_3D_MODE_METALFX_SPATIAL` | `3` |
| `VIEWPORT_SCALING_3D_MODE_METALFX_TEMPORAL` | `4` |
| `VIEWPORT_SCALING_3D_MODE_NEAREST` | `5` |
| `VIEWPORT_SCALING_3D_MODE_MAX` | `6` |

## RenderingServer.ViewportUpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_UPDATE_DISABLED` | `0` |
| `VIEWPORT_UPDATE_ONCE` | `1` |
| `VIEWPORT_UPDATE_WHEN_VISIBLE` | `2` |
| `VIEWPORT_UPDATE_WHEN_PARENT_VISIBLE` | `3` |
| `VIEWPORT_UPDATE_ALWAYS` | `4` |

## RenderingServer.ViewportClearMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_CLEAR_ALWAYS` | `0` |
| `VIEWPORT_CLEAR_NEVER` | `1` |
| `VIEWPORT_CLEAR_ONLY_NEXT_FRAME` | `2` |

## RenderingServer.ViewportEnvironmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_ENVIRONMENT_DISABLED` | `0` |
| `VIEWPORT_ENVIRONMENT_ENABLED` | `1` |
| `VIEWPORT_ENVIRONMENT_INHERIT` | `2` |
| `VIEWPORT_ENVIRONMENT_MAX` | `3` |

## RenderingServer.ViewportSDFOversize

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_SDF_OVERSIZE_100_PERCENT` | `0` |
| `VIEWPORT_SDF_OVERSIZE_120_PERCENT` | `1` |
| `VIEWPORT_SDF_OVERSIZE_150_PERCENT` | `2` |
| `VIEWPORT_SDF_OVERSIZE_200_PERCENT` | `3` |
| `VIEWPORT_SDF_OVERSIZE_MAX` | `4` |

## RenderingServer.ViewportSDFScale

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_SDF_SCALE_100_PERCENT` | `0` |
| `VIEWPORT_SDF_SCALE_50_PERCENT` | `1` |
| `VIEWPORT_SDF_SCALE_25_PERCENT` | `2` |
| `VIEWPORT_SDF_SCALE_MAX` | `3` |

## RenderingServer.ViewportMSAA

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_MSAA_DISABLED` | `0` |
| `VIEWPORT_MSAA_2X` | `1` |
| `VIEWPORT_MSAA_4X` | `2` |
| `VIEWPORT_MSAA_8X` | `3` |
| `VIEWPORT_MSAA_MAX` | `4` |

## RenderingServer.ViewportAnisotropicFiltering

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_ANISOTROPY_DISABLED` | `0` |
| `VIEWPORT_ANISOTROPY_2X` | `1` |
| `VIEWPORT_ANISOTROPY_4X` | `2` |
| `VIEWPORT_ANISOTROPY_8X` | `3` |
| `VIEWPORT_ANISOTROPY_16X` | `4` |
| `VIEWPORT_ANISOTROPY_MAX` | `5` |

## RenderingServer.ViewportScreenSpaceAA

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_SCREEN_SPACE_AA_DISABLED` | `0` |
| `VIEWPORT_SCREEN_SPACE_AA_FXAA` | `1` |
| `VIEWPORT_SCREEN_SPACE_AA_SMAA` | `2` |
| `VIEWPORT_SCREEN_SPACE_AA_MAX` | `3` |

## RenderingServer.ViewportOcclusionCullingBuildQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_OCCLUSION_BUILD_QUALITY_LOW` | `0` |
| `VIEWPORT_OCCLUSION_BUILD_QUALITY_MEDIUM` | `1` |
| `VIEWPORT_OCCLUSION_BUILD_QUALITY_HIGH` | `2` |

## RenderingServer.ViewportRenderInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME` | `0` |
| `VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME` | `1` |
| `VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME` | `2` |
| `VIEWPORT_RENDER_INFO_MAX` | `3` |

## RenderingServer.ViewportRenderInfoType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_RENDER_INFO_TYPE_VISIBLE` | `0` |
| `VIEWPORT_RENDER_INFO_TYPE_SHADOW` | `1` |
| `VIEWPORT_RENDER_INFO_TYPE_CANVAS` | `2` |
| `VIEWPORT_RENDER_INFO_TYPE_MAX` | `3` |

## RenderingServer.ViewportDebugDraw

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_DEBUG_DRAW_DISABLED` | `0` |
| `VIEWPORT_DEBUG_DRAW_UNSHADED` | `1` |
| `VIEWPORT_DEBUG_DRAW_LIGHTING` | `2` |
| `VIEWPORT_DEBUG_DRAW_OVERDRAW` | `3` |
| `VIEWPORT_DEBUG_DRAW_WIREFRAME` | `4` |
| `VIEWPORT_DEBUG_DRAW_NORMAL_BUFFER` | `5` |
| `VIEWPORT_DEBUG_DRAW_VOXEL_GI_ALBEDO` | `6` |
| `VIEWPORT_DEBUG_DRAW_VOXEL_GI_LIGHTING` | `7` |
| `VIEWPORT_DEBUG_DRAW_VOXEL_GI_EMISSION` | `8` |
| `VIEWPORT_DEBUG_DRAW_SHADOW_ATLAS` | `9` |
| `VIEWPORT_DEBUG_DRAW_DIRECTIONAL_SHADOW_ATLAS` | `10` |
| `VIEWPORT_DEBUG_DRAW_SCENE_LUMINANCE` | `11` |
| `VIEWPORT_DEBUG_DRAW_SSAO` | `12` |
| `VIEWPORT_DEBUG_DRAW_SSIL` | `13` |
| `VIEWPORT_DEBUG_DRAW_PSSM_SPLITS` | `14` |
| `VIEWPORT_DEBUG_DRAW_DECAL_ATLAS` | `15` |
| `VIEWPORT_DEBUG_DRAW_SDFGI` | `16` |
| `VIEWPORT_DEBUG_DRAW_SDFGI_PROBES` | `17` |
| `VIEWPORT_DEBUG_DRAW_GI_BUFFER` | `18` |
| `VIEWPORT_DEBUG_DRAW_DISABLE_LOD` | `19` |
| `VIEWPORT_DEBUG_DRAW_CLUSTER_OMNI_LIGHTS` | `20` |
| `VIEWPORT_DEBUG_DRAW_CLUSTER_SPOT_LIGHTS` | `21` |
| `VIEWPORT_DEBUG_DRAW_CLUSTER_DECALS` | `22` |
| `VIEWPORT_DEBUG_DRAW_CLUSTER_REFLECTION_PROBES` | `23` |
| `VIEWPORT_DEBUG_DRAW_OCCLUDERS` | `24` |
| `VIEWPORT_DEBUG_DRAW_MOTION_VECTORS` | `25` |
| `VIEWPORT_DEBUG_DRAW_INTERNAL_BUFFER` | `26` |

## RenderingServer.ViewportVRSMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_VRS_DISABLED` | `0` |
| `VIEWPORT_VRS_TEXTURE` | `1` |
| `VIEWPORT_VRS_XR` | `2` |
| `VIEWPORT_VRS_MAX` | `3` |

## RenderingServer.ViewportVRSUpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VIEWPORT_VRS_UPDATE_DISABLED` | `0` |
| `VIEWPORT_VRS_UPDATE_ONCE` | `1` |
| `VIEWPORT_VRS_UPDATE_ALWAYS` | `2` |
| `VIEWPORT_VRS_UPDATE_MAX` | `3` |

## RenderingServer.SkyMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SKY_MODE_AUTOMATIC` | `0` |
| `SKY_MODE_QUALITY` | `1` |
| `SKY_MODE_INCREMENTAL` | `2` |
| `SKY_MODE_REALTIME` | `3` |

## RenderingServer.CompositorEffectFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPOSITOR_EFFECT_FLAG_ACCESS_RESOLVED_COLOR` | `1` |
| `COMPOSITOR_EFFECT_FLAG_ACCESS_RESOLVED_DEPTH` | `2` |
| `COMPOSITOR_EFFECT_FLAG_NEEDS_MOTION_VECTORS` | `4` |
| `COMPOSITOR_EFFECT_FLAG_NEEDS_ROUGHNESS` | `8` |
| `COMPOSITOR_EFFECT_FLAG_NEEDS_SEPARATE_SPECULAR` | `16` |

## RenderingServer.CompositorEffectCallbackType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPOSITOR_EFFECT_CALLBACK_TYPE_PRE_OPAQUE` | `0` |
| `COMPOSITOR_EFFECT_CALLBACK_TYPE_POST_OPAQUE` | `1` |
| `COMPOSITOR_EFFECT_CALLBACK_TYPE_POST_SKY` | `2` |
| `COMPOSITOR_EFFECT_CALLBACK_TYPE_PRE_TRANSPARENT` | `3` |
| `COMPOSITOR_EFFECT_CALLBACK_TYPE_POST_TRANSPARENT` | `4` |
| `COMPOSITOR_EFFECT_CALLBACK_TYPE_ANY` | `-1` |

## RenderingServer.EnvironmentBG

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_BG_CLEAR_COLOR` | `0` |
| `ENV_BG_COLOR` | `1` |
| `ENV_BG_SKY` | `2` |
| `ENV_BG_CANVAS` | `3` |
| `ENV_BG_KEEP` | `4` |
| `ENV_BG_CAMERA_FEED` | `5` |
| `ENV_BG_MAX` | `6` |

## RenderingServer.EnvironmentAmbientSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_AMBIENT_SOURCE_BG` | `0` |
| `ENV_AMBIENT_SOURCE_DISABLED` | `1` |
| `ENV_AMBIENT_SOURCE_COLOR` | `2` |
| `ENV_AMBIENT_SOURCE_SKY` | `3` |

## RenderingServer.EnvironmentReflectionSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_REFLECTION_SOURCE_BG` | `0` |
| `ENV_REFLECTION_SOURCE_DISABLED` | `1` |
| `ENV_REFLECTION_SOURCE_SKY` | `2` |

## RenderingServer.EnvironmentGlowBlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_GLOW_BLEND_MODE_ADDITIVE` | `0` |
| `ENV_GLOW_BLEND_MODE_SCREEN` | `1` |
| `ENV_GLOW_BLEND_MODE_SOFTLIGHT` | `2` |
| `ENV_GLOW_BLEND_MODE_REPLACE` | `3` |
| `ENV_GLOW_BLEND_MODE_MIX` | `4` |

## RenderingServer.EnvironmentFogMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_FOG_MODE_EXPONENTIAL` | `0` |
| `ENV_FOG_MODE_DEPTH` | `1` |

## RenderingServer.EnvironmentToneMapper

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_TONE_MAPPER_LINEAR` | `0` |
| `ENV_TONE_MAPPER_REINHARD` | `1` |
| `ENV_TONE_MAPPER_FILMIC` | `2` |
| `ENV_TONE_MAPPER_ACES` | `3` |
| `ENV_TONE_MAPPER_AGX` | `4` |

## RenderingServer.EnvironmentSSRRoughnessQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SSR_ROUGHNESS_QUALITY_DISABLED` | `0` |
| `ENV_SSR_ROUGHNESS_QUALITY_LOW` | `1` |
| `ENV_SSR_ROUGHNESS_QUALITY_MEDIUM` | `2` |
| `ENV_SSR_ROUGHNESS_QUALITY_HIGH` | `3` |

## RenderingServer.EnvironmentSSAOQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SSAO_QUALITY_VERY_LOW` | `0` |
| `ENV_SSAO_QUALITY_LOW` | `1` |
| `ENV_SSAO_QUALITY_MEDIUM` | `2` |
| `ENV_SSAO_QUALITY_HIGH` | `3` |
| `ENV_SSAO_QUALITY_ULTRA` | `4` |

## RenderingServer.EnvironmentSSILQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SSIL_QUALITY_VERY_LOW` | `0` |
| `ENV_SSIL_QUALITY_LOW` | `1` |
| `ENV_SSIL_QUALITY_MEDIUM` | `2` |
| `ENV_SSIL_QUALITY_HIGH` | `3` |
| `ENV_SSIL_QUALITY_ULTRA` | `4` |

## RenderingServer.EnvironmentSDFGIYScale

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SDFGI_Y_SCALE_50_PERCENT` | `0` |
| `ENV_SDFGI_Y_SCALE_75_PERCENT` | `1` |
| `ENV_SDFGI_Y_SCALE_100_PERCENT` | `2` |

## RenderingServer.EnvironmentSDFGIRayCount

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SDFGI_RAY_COUNT_4` | `0` |
| `ENV_SDFGI_RAY_COUNT_8` | `1` |
| `ENV_SDFGI_RAY_COUNT_16` | `2` |
| `ENV_SDFGI_RAY_COUNT_32` | `3` |
| `ENV_SDFGI_RAY_COUNT_64` | `4` |
| `ENV_SDFGI_RAY_COUNT_96` | `5` |
| `ENV_SDFGI_RAY_COUNT_128` | `6` |
| `ENV_SDFGI_RAY_COUNT_MAX` | `7` |

## RenderingServer.EnvironmentSDFGIFramesToConverge

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SDFGI_CONVERGE_IN_5_FRAMES` | `0` |
| `ENV_SDFGI_CONVERGE_IN_10_FRAMES` | `1` |
| `ENV_SDFGI_CONVERGE_IN_15_FRAMES` | `2` |
| `ENV_SDFGI_CONVERGE_IN_20_FRAMES` | `3` |
| `ENV_SDFGI_CONVERGE_IN_25_FRAMES` | `4` |
| `ENV_SDFGI_CONVERGE_IN_30_FRAMES` | `5` |
| `ENV_SDFGI_CONVERGE_MAX` | `6` |

## RenderingServer.EnvironmentSDFGIFramesToUpdateLight

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENV_SDFGI_UPDATE_LIGHT_IN_1_FRAME` | `0` |
| `ENV_SDFGI_UPDATE_LIGHT_IN_2_FRAMES` | `1` |
| `ENV_SDFGI_UPDATE_LIGHT_IN_4_FRAMES` | `2` |
| `ENV_SDFGI_UPDATE_LIGHT_IN_8_FRAMES` | `3` |
| `ENV_SDFGI_UPDATE_LIGHT_IN_16_FRAMES` | `4` |
| `ENV_SDFGI_UPDATE_LIGHT_MAX` | `5` |

## RenderingServer.SubSurfaceScatteringQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SUB_SURFACE_SCATTERING_QUALITY_DISABLED` | `0` |
| `SUB_SURFACE_SCATTERING_QUALITY_LOW` | `1` |
| `SUB_SURFACE_SCATTERING_QUALITY_MEDIUM` | `2` |
| `SUB_SURFACE_SCATTERING_QUALITY_HIGH` | `3` |

## RenderingServer.DOFBokehShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOF_BOKEH_BOX` | `0` |
| `DOF_BOKEH_HEXAGON` | `1` |
| `DOF_BOKEH_CIRCLE` | `2` |

## RenderingServer.DOFBlurQuality

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DOF_BLUR_QUALITY_VERY_LOW` | `0` |
| `DOF_BLUR_QUALITY_LOW` | `1` |
| `DOF_BLUR_QUALITY_MEDIUM` | `2` |
| `DOF_BLUR_QUALITY_HIGH` | `3` |

## RenderingServer.InstanceType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INSTANCE_NONE` | `0` |
| `INSTANCE_MESH` | `1` |
| `INSTANCE_MULTIMESH` | `2` |
| `INSTANCE_PARTICLES` | `3` |
| `INSTANCE_PARTICLES_COLLISION` | `4` |
| `INSTANCE_LIGHT` | `5` |
| `INSTANCE_REFLECTION_PROBE` | `6` |
| `INSTANCE_DECAL` | `7` |
| `INSTANCE_VOXEL_GI` | `8` |
| `INSTANCE_LIGHTMAP` | `9` |
| `INSTANCE_OCCLUDER` | `10` |
| `INSTANCE_VISIBLITY_NOTIFIER` | `11` |
| `INSTANCE_FOG_VOLUME` | `12` |
| `INSTANCE_MAX` | `13` |
| `INSTANCE_GEOMETRY_MASK` | `14` |

## RenderingServer.InstanceFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `INSTANCE_FLAG_USE_BAKED_LIGHT` | `0` |
| `INSTANCE_FLAG_USE_DYNAMIC_GI` | `1` |
| `INSTANCE_FLAG_DRAW_NEXT_FRAME_IF_VISIBLE` | `2` |
| `INSTANCE_FLAG_IGNORE_OCCLUSION_CULLING` | `3` |
| `INSTANCE_FLAG_MAX` | `4` |

## RenderingServer.ShadowCastingSetting

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_CASTING_SETTING_OFF` | `0` |
| `SHADOW_CASTING_SETTING_ON` | `1` |
| `SHADOW_CASTING_SETTING_DOUBLE_SIDED` | `2` |
| `SHADOW_CASTING_SETTING_SHADOWS_ONLY` | `3` |

## RenderingServer.VisibilityRangeFadeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_RANGE_FADE_DISABLED` | `0` |
| `VISIBILITY_RANGE_FADE_SELF` | `1` |
| `VISIBILITY_RANGE_FADE_DEPENDENCIES` | `2` |

## RenderingServer.BakeChannels

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BAKE_CHANNEL_ALBEDO_ALPHA` | `0` |
| `BAKE_CHANNEL_NORMAL` | `1` |
| `BAKE_CHANNEL_ORM` | `2` |
| `BAKE_CHANNEL_EMISSION` | `3` |

## RenderingServer.CanvasTextureChannel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_TEXTURE_CHANNEL_DIFFUSE` | `0` |
| `CANVAS_TEXTURE_CHANNEL_NORMAL` | `1` |
| `CANVAS_TEXTURE_CHANNEL_SPECULAR` | `2` |

## RenderingServer.NinePatchAxisMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `NINE_PATCH_STRETCH` | `0` |
| `NINE_PATCH_TILE` | `1` |
| `NINE_PATCH_TILE_FIT` | `2` |

## RenderingServer.CanvasItemTextureFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_ITEM_TEXTURE_FILTER_DEFAULT` | `0` |
| `CANVAS_ITEM_TEXTURE_FILTER_NEAREST` | `1` |
| `CANVAS_ITEM_TEXTURE_FILTER_LINEAR` | `2` |
| `CANVAS_ITEM_TEXTURE_FILTER_NEAREST_WITH_MIPMAPS` | `3` |
| `CANVAS_ITEM_TEXTURE_FILTER_LINEAR_WITH_MIPMAPS` | `4` |
| `CANVAS_ITEM_TEXTURE_FILTER_NEAREST_WITH_MIPMAPS_ANISOTROPIC` | `5` |
| `CANVAS_ITEM_TEXTURE_FILTER_LINEAR_WITH_MIPMAPS_ANISOTROPIC` | `6` |
| `CANVAS_ITEM_TEXTURE_FILTER_MAX` | `7` |

## RenderingServer.CanvasItemTextureRepeat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_ITEM_TEXTURE_REPEAT_DEFAULT` | `0` |
| `CANVAS_ITEM_TEXTURE_REPEAT_DISABLED` | `1` |
| `CANVAS_ITEM_TEXTURE_REPEAT_ENABLED` | `2` |
| `CANVAS_ITEM_TEXTURE_REPEAT_MIRROR` | `3` |
| `CANVAS_ITEM_TEXTURE_REPEAT_MAX` | `4` |

## RenderingServer.CanvasGroupMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_GROUP_MODE_DISABLED` | `0` |
| `CANVAS_GROUP_MODE_CLIP_ONLY` | `1` |
| `CANVAS_GROUP_MODE_CLIP_AND_DRAW` | `2` |
| `CANVAS_GROUP_MODE_TRANSPARENT` | `3` |

## RenderingServer.CanvasLightMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_LIGHT_MODE_POINT` | `0` |
| `CANVAS_LIGHT_MODE_DIRECTIONAL` | `1` |

## RenderingServer.CanvasLightBlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_LIGHT_BLEND_MODE_ADD` | `0` |
| `CANVAS_LIGHT_BLEND_MODE_SUB` | `1` |
| `CANVAS_LIGHT_BLEND_MODE_MIX` | `2` |

## RenderingServer.CanvasLightShadowFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_LIGHT_FILTER_NONE` | `0` |
| `CANVAS_LIGHT_FILTER_PCF5` | `1` |
| `CANVAS_LIGHT_FILTER_PCF13` | `2` |
| `CANVAS_LIGHT_FILTER_MAX` | `3` |

## RenderingServer.CanvasOccluderPolygonCullMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CANVAS_OCCLUDER_POLYGON_CULL_DISABLED` | `0` |
| `CANVAS_OCCLUDER_POLYGON_CULL_CLOCKWISE` | `1` |
| `CANVAS_OCCLUDER_POLYGON_CULL_COUNTER_CLOCKWISE` | `2` |

## RenderingServer.GlobalShaderParameterType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GLOBAL_VAR_TYPE_BOOL` | `0` |
| `GLOBAL_VAR_TYPE_BVEC2` | `1` |
| `GLOBAL_VAR_TYPE_BVEC3` | `2` |
| `GLOBAL_VAR_TYPE_BVEC4` | `3` |
| `GLOBAL_VAR_TYPE_INT` | `4` |
| `GLOBAL_VAR_TYPE_IVEC2` | `5` |
| `GLOBAL_VAR_TYPE_IVEC3` | `6` |
| `GLOBAL_VAR_TYPE_IVEC4` | `7` |
| `GLOBAL_VAR_TYPE_RECT2I` | `8` |
| `GLOBAL_VAR_TYPE_UINT` | `9` |
| `GLOBAL_VAR_TYPE_UVEC2` | `10` |
| `GLOBAL_VAR_TYPE_UVEC3` | `11` |
| `GLOBAL_VAR_TYPE_UVEC4` | `12` |
| `GLOBAL_VAR_TYPE_FLOAT` | `13` |
| `GLOBAL_VAR_TYPE_VEC2` | `14` |
| `GLOBAL_VAR_TYPE_VEC3` | `15` |
| `GLOBAL_VAR_TYPE_VEC4` | `16` |
| `GLOBAL_VAR_TYPE_COLOR` | `17` |
| `GLOBAL_VAR_TYPE_RECT2` | `18` |
| `GLOBAL_VAR_TYPE_MAT2` | `19` |
| `GLOBAL_VAR_TYPE_MAT3` | `20` |
| `GLOBAL_VAR_TYPE_MAT4` | `21` |
| `GLOBAL_VAR_TYPE_TRANSFORM_2D` | `22` |
| `GLOBAL_VAR_TYPE_TRANSFORM` | `23` |
| `GLOBAL_VAR_TYPE_SAMPLER2D` | `24` |
| `GLOBAL_VAR_TYPE_SAMPLER2DARRAY` | `25` |
| `GLOBAL_VAR_TYPE_SAMPLER3D` | `26` |
| `GLOBAL_VAR_TYPE_SAMPLERCUBE` | `27` |
| `GLOBAL_VAR_TYPE_SAMPLEREXT` | `28` |
| `GLOBAL_VAR_TYPE_MAX` | `29` |

## RenderingServer.RenderingInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RENDERING_INFO_TOTAL_OBJECTS_IN_FRAME` | `0` |
| `RENDERING_INFO_TOTAL_PRIMITIVES_IN_FRAME` | `1` |
| `RENDERING_INFO_TOTAL_DRAW_CALLS_IN_FRAME` | `2` |
| `RENDERING_INFO_TEXTURE_MEM_USED` | `3` |
| `RENDERING_INFO_BUFFER_MEM_USED` | `4` |
| `RENDERING_INFO_VIDEO_MEM_USED` | `5` |
| `RENDERING_INFO_PIPELINE_COMPILATIONS_CANVAS` | `6` |
| `RENDERING_INFO_PIPELINE_COMPILATIONS_MESH` | `7` |
| `RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE` | `8` |
| `RENDERING_INFO_PIPELINE_COMPILATIONS_DRAW` | `9` |
| `RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION` | `10` |

## RenderingServer.PipelineSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PIPELINE_SOURCE_CANVAS` | `0` |
| `PIPELINE_SOURCE_MESH` | `1` |
| `PIPELINE_SOURCE_SURFACE` | `2` |
| `PIPELINE_SOURCE_DRAW` | `3` |
| `PIPELINE_SOURCE_SPECIALIZATION` | `4` |
| `PIPELINE_SOURCE_MAX` | `5` |

## RenderingServer.SplashStretchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPLASH_STRETCH_MODE_DISABLED` | `0` |
| `SPLASH_STRETCH_MODE_KEEP` | `1` |
| `SPLASH_STRETCH_MODE_KEEP_WIDTH` | `2` |
| `SPLASH_STRETCH_MODE_KEEP_HEIGHT` | `3` |
| `SPLASH_STRETCH_MODE_COVER` | `4` |
| `SPLASH_STRETCH_MODE_IGNORE` | `5` |

## RenderingServer.Features

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEATURE_SHADERS` | `0` |
| `FEATURE_MULTITHREADED` | `1` |

## Resource.DeepDuplicateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEEP_DUPLICATE_NONE` | `0` |
| `DEEP_DUPLICATE_INTERNAL` | `1` |
| `DEEP_DUPLICATE_ALL` | `2` |

## ResourceFormatLoader.CacheMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CACHE_MODE_IGNORE` | `0` |
| `CACHE_MODE_REUSE` | `1` |
| `CACHE_MODE_REPLACE` | `2` |
| `CACHE_MODE_IGNORE_DEEP` | `3` |
| `CACHE_MODE_REPLACE_DEEP` | `4` |

## ResourceImporter.ImportOrder

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `IMPORT_ORDER_DEFAULT` | `0` |
| `IMPORT_ORDER_SCENE` | `100` |

## ResourceLoader.ThreadLoadStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `THREAD_LOAD_INVALID_RESOURCE` | `0` |
| `THREAD_LOAD_IN_PROGRESS` | `1` |
| `THREAD_LOAD_FAILED` | `2` |
| `THREAD_LOAD_LOADED` | `3` |

## ResourceLoader.CacheMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CACHE_MODE_IGNORE` | `0` |
| `CACHE_MODE_REUSE` | `1` |
| `CACHE_MODE_REPLACE` | `2` |
| `CACHE_MODE_IGNORE_DEEP` | `3` |
| `CACHE_MODE_REPLACE_DEEP` | `4` |

## ResourceSaver.SaverFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `FLAG_NONE` | `0` |
| `FLAG_RELATIVE_PATHS` | `1` |
| `FLAG_BUNDLE_RESOURCES` | `2` |
| `FLAG_CHANGE_PATH` | `4` |
| `FLAG_OMIT_EDITOR_PROPERTIES` | `8` |
| `FLAG_SAVE_BIG_ENDIAN` | `16` |
| `FLAG_COMPRESS` | `32` |
| `FLAG_REPLACE_SUBRESOURCE_PATHS` | `64` |

## RetargetModifier3D.TransformFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `TRANSFORM_FLAG_POSITION` | `1` |
| `TRANSFORM_FLAG_ROTATION` | `2` |
| `TRANSFORM_FLAG_SCALE` | `4` |
| `TRANSFORM_FLAG_ALL` | `7` |

## RibbonTrailMesh.Shape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHAPE_FLAT` | `0` |
| `SHAPE_CROSS` | `1` |

## RichTextLabel.ListType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LIST_NUMBERS` | `0` |
| `LIST_LETTERS` | `1` |
| `LIST_ROMAN` | `2` |
| `LIST_DOTS` | `3` |

## RichTextLabel.MenuItems

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MENU_COPY` | `0` |
| `MENU_SELECT_ALL` | `1` |
| `MENU_MAX` | `2` |

## RichTextLabel.MetaUnderline

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `META_UNDERLINE_NEVER` | `0` |
| `META_UNDERLINE_ALWAYS` | `1` |
| `META_UNDERLINE_ON_HOVER` | `2` |

## RichTextLabel.ImageUpdateMask

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `UPDATE_TEXTURE` | `1` |
| `UPDATE_SIZE` | `2` |
| `UPDATE_COLOR` | `4` |
| `UPDATE_ALIGNMENT` | `8` |
| `UPDATE_REGION` | `16` |
| `UPDATE_PAD` | `32` |
| `UPDATE_TOOLTIP` | `64` |
| `UPDATE_WIDTH_UNIT` | `128` |

## RichTextLabel.ImageUnit

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `IMAGE_UNIT_PIXEL` | `0` |
| `IMAGE_UNIT_PERCENT` | `1` |
| `IMAGE_UNIT_EM` | `2` |

## RigidBody2D.FreezeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FREEZE_MODE_STATIC` | `0` |
| `FREEZE_MODE_KINEMATIC` | `1` |

## RigidBody2D.CenterOfMassMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CENTER_OF_MASS_MODE_AUTO` | `0` |
| `CENTER_OF_MASS_MODE_CUSTOM` | `1` |

## RigidBody2D.DampMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DAMP_MODE_COMBINE` | `0` |
| `DAMP_MODE_REPLACE` | `1` |

## RigidBody2D.CCDMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CCD_MODE_DISABLED` | `0` |
| `CCD_MODE_CAST_RAY` | `1` |
| `CCD_MODE_CAST_SHAPE` | `2` |

## RigidBody3D.FreezeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FREEZE_MODE_STATIC` | `0` |
| `FREEZE_MODE_KINEMATIC` | `1` |

## RigidBody3D.CenterOfMassMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CENTER_OF_MASS_MODE_AUTO` | `0` |
| `CENTER_OF_MASS_MODE_CUSTOM` | `1` |

## RigidBody3D.DampMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DAMP_MODE_COMBINE` | `0` |
| `DAMP_MODE_REPLACE` | `1` |

## SceneReplicationConfig.ReplicationMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REPLICATION_MODE_NEVER` | `0` |
| `REPLICATION_MODE_ALWAYS` | `1` |
| `REPLICATION_MODE_ON_CHANGE` | `2` |

## SceneState.GenEditState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GEN_EDIT_STATE_DISABLED` | `0` |
| `GEN_EDIT_STATE_INSTANCE` | `1` |
| `GEN_EDIT_STATE_MAIN` | `2` |
| `GEN_EDIT_STATE_MAIN_INHERITED` | `3` |

## SceneTree.GroupCallFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GROUP_CALL_DEFAULT` | `0` |
| `GROUP_CALL_REVERSE` | `1` |
| `GROUP_CALL_DEFERRED` | `2` |
| `GROUP_CALL_UNIQUE` | `4` |

## ScriptLanguage.ScriptNameCasing

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCRIPT_NAME_CASING_AUTO` | `0` |
| `SCRIPT_NAME_CASING_PASCAL_CASE` | `1` |
| `SCRIPT_NAME_CASING_SNAKE_CASE` | `2` |
| `SCRIPT_NAME_CASING_KEBAB_CASE` | `3` |
| `SCRIPT_NAME_CASING_CAMEL_CASE` | `4` |

## ScriptLanguageExtension.LookupResultType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOOKUP_RESULT_SCRIPT_LOCATION` | `0` |
| `LOOKUP_RESULT_CLASS` | `1` |
| `LOOKUP_RESULT_CLASS_CONSTANT` | `2` |
| `LOOKUP_RESULT_CLASS_PROPERTY` | `3` |
| `LOOKUP_RESULT_CLASS_METHOD` | `4` |
| `LOOKUP_RESULT_CLASS_SIGNAL` | `5` |
| `LOOKUP_RESULT_CLASS_ENUM` | `6` |
| `LOOKUP_RESULT_CLASS_TBD_GLOBALSCOPE` | `7` |
| `LOOKUP_RESULT_CLASS_ANNOTATION` | `8` |
| `LOOKUP_RESULT_LOCAL_CONSTANT` | `9` |
| `LOOKUP_RESULT_LOCAL_VARIABLE` | `10` |
| `LOOKUP_RESULT_MAX` | `11` |

## ScriptLanguageExtension.CodeCompletionLocation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOCATION_LOCAL` | `0` |
| `LOCATION_PARENT_MASK` | `256` |
| `LOCATION_OTHER_USER_CODE` | `512` |
| `LOCATION_OTHER` | `1024` |

## ScriptLanguageExtension.CodeCompletionKind

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CODE_COMPLETION_KIND_CLASS` | `0` |
| `CODE_COMPLETION_KIND_FUNCTION` | `1` |
| `CODE_COMPLETION_KIND_SIGNAL` | `2` |
| `CODE_COMPLETION_KIND_VARIABLE` | `3` |
| `CODE_COMPLETION_KIND_MEMBER` | `4` |
| `CODE_COMPLETION_KIND_ENUM` | `5` |
| `CODE_COMPLETION_KIND_CONSTANT` | `6` |
| `CODE_COMPLETION_KIND_NODE_PATH` | `7` |
| `CODE_COMPLETION_KIND_FILE_PATH` | `8` |
| `CODE_COMPLETION_KIND_PLAIN_TEXT` | `9` |
| `CODE_COMPLETION_KIND_KEYWORD` | `10` |
| `CODE_COMPLETION_KIND_MAX` | `11` |

## ScrollContainer.ScrollMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_MODE_DISABLED` | `0` |
| `SCROLL_MODE_AUTO` | `1` |
| `SCROLL_MODE_SHOW_ALWAYS` | `2` |
| `SCROLL_MODE_SHOW_NEVER` | `3` |
| `SCROLL_MODE_RESERVE` | `4` |
| `SCROLL_MODE_MAXIMIZE_FIRST` | `5` |

## ScrollContainer.ScrollHintMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_HINT_MODE_DISABLED` | `0` |
| `SCROLL_HINT_MODE_ALL` | `1` |
| `SCROLL_HINT_MODE_TOP_AND_LEFT` | `2` |
| `SCROLL_HINT_MODE_BOTTOM_AND_RIGHT` | `3` |

## Shader.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_SPATIAL` | `0` |
| `MODE_CANVAS_ITEM` | `1` |
| `MODE_PARTICLES` | `2` |
| `MODE_SKY` | `3` |
| `MODE_FOG` | `4` |
| `MODE_TEXTURE_BLIT` | `5` |

## Skeleton3D.ModifierCallbackModeProcess

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODIFIER_CALLBACK_MODE_PROCESS_PHYSICS` | `0` |
| `MODIFIER_CALLBACK_MODE_PROCESS_IDLE` | `1` |
| `MODIFIER_CALLBACK_MODE_PROCESS_MANUAL` | `2` |

## SkeletonModifier3D.BoneAxis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BONE_AXIS_PLUS_X` | `0` |
| `BONE_AXIS_MINUS_X` | `1` |
| `BONE_AXIS_PLUS_Y` | `2` |
| `BONE_AXIS_MINUS_Y` | `3` |
| `BONE_AXIS_PLUS_Z` | `4` |
| `BONE_AXIS_MINUS_Z` | `5` |

## SkeletonModifier3D.BoneDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BONE_DIRECTION_PLUS_X` | `0` |
| `BONE_DIRECTION_MINUS_X` | `1` |
| `BONE_DIRECTION_PLUS_Y` | `2` |
| `BONE_DIRECTION_MINUS_Y` | `3` |
| `BONE_DIRECTION_PLUS_Z` | `4` |
| `BONE_DIRECTION_MINUS_Z` | `5` |
| `BONE_DIRECTION_FROM_PARENT` | `6` |

## SkeletonModifier3D.SecondaryDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SECONDARY_DIRECTION_NONE` | `0` |
| `SECONDARY_DIRECTION_PLUS_X` | `1` |
| `SECONDARY_DIRECTION_MINUS_X` | `2` |
| `SECONDARY_DIRECTION_PLUS_Y` | `3` |
| `SECONDARY_DIRECTION_MINUS_Y` | `4` |
| `SECONDARY_DIRECTION_PLUS_Z` | `5` |
| `SECONDARY_DIRECTION_MINUS_Z` | `6` |
| `SECONDARY_DIRECTION_CUSTOM` | `7` |

## SkeletonModifier3D.RotationAxis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ROTATION_AXIS_X` | `0` |
| `ROTATION_AXIS_Y` | `1` |
| `ROTATION_AXIS_Z` | `2` |
| `ROTATION_AXIS_ALL` | `3` |
| `ROTATION_AXIS_CUSTOM` | `4` |

## SkeletonProfile.TailDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TAIL_DIRECTION_AVERAGE_CHILDREN` | `0` |
| `TAIL_DIRECTION_SPECIFIC_CHILD` | `1` |
| `TAIL_DIRECTION_END` | `2` |

## Sky.RadianceSize

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RADIANCE_SIZE_32` | `0` |
| `RADIANCE_SIZE_64` | `1` |
| `RADIANCE_SIZE_128` | `2` |
| `RADIANCE_SIZE_256` | `3` |
| `RADIANCE_SIZE_512` | `4` |
| `RADIANCE_SIZE_1024` | `5` |
| `RADIANCE_SIZE_2048` | `6` |
| `RADIANCE_SIZE_MAX` | `7` |

## Sky.ProcessMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PROCESS_MODE_AUTOMATIC` | `0` |
| `PROCESS_MODE_QUALITY` | `1` |
| `PROCESS_MODE_INCREMENTAL` | `2` |
| `PROCESS_MODE_REALTIME` | `3` |

## Slider.TickPosition

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TICK_POSITION_BOTTOM_RIGHT` | `0` |
| `TICK_POSITION_TOP_LEFT` | `1` |
| `TICK_POSITION_BOTH` | `2` |
| `TICK_POSITION_CENTER` | `3` |

## SliderJoint3D.Param

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PARAM_LINEAR_LIMIT_UPPER` | `0` |
| `PARAM_LINEAR_LIMIT_LOWER` | `1` |
| `PARAM_LINEAR_LIMIT_SOFTNESS` | `2` |
| `PARAM_LINEAR_LIMIT_RESTITUTION` | `3` |
| `PARAM_LINEAR_LIMIT_DAMPING` | `4` |
| `PARAM_LINEAR_MOTION_SOFTNESS` | `5` |
| `PARAM_LINEAR_MOTION_RESTITUTION` | `6` |
| `PARAM_LINEAR_MOTION_DAMPING` | `7` |
| `PARAM_LINEAR_ORTHOGONAL_SOFTNESS` | `8` |
| `PARAM_LINEAR_ORTHOGONAL_RESTITUTION` | `9` |
| `PARAM_LINEAR_ORTHOGONAL_DAMPING` | `10` |
| `PARAM_ANGULAR_LIMIT_UPPER` | `11` |
| `PARAM_ANGULAR_LIMIT_LOWER` | `12` |
| `PARAM_ANGULAR_LIMIT_SOFTNESS` | `13` |
| `PARAM_ANGULAR_LIMIT_RESTITUTION` | `14` |
| `PARAM_ANGULAR_LIMIT_DAMPING` | `15` |
| `PARAM_ANGULAR_MOTION_SOFTNESS` | `16` |
| `PARAM_ANGULAR_MOTION_RESTITUTION` | `17` |
| `PARAM_ANGULAR_MOTION_DAMPING` | `18` |
| `PARAM_ANGULAR_ORTHOGONAL_SOFTNESS` | `19` |
| `PARAM_ANGULAR_ORTHOGONAL_RESTITUTION` | `20` |
| `PARAM_ANGULAR_ORTHOGONAL_DAMPING` | `21` |
| `PARAM_MAX` | `22` |

## SoftBody3D.DisableMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DISABLE_MODE_REMOVE` | `0` |
| `DISABLE_MODE_KEEP_ACTIVE` | `1` |

## SplitContainer.DraggerVisibility

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DRAGGER_VISIBLE` | `0` |
| `DRAGGER_HIDDEN` | `1` |
| `DRAGGER_HIDDEN_COLLAPSED` | `2` |

## SpringBoneSimulator3D.CenterFrom

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CENTER_FROM_WORLD_ORIGIN` | `0` |
| `CENTER_FROM_NODE` | `1` |
| `CENTER_FROM_BONE` | `2` |

## SpriteBase3D.DrawFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_TRANSPARENT` | `0` |
| `FLAG_SHADED` | `1` |
| `FLAG_DOUBLE_SIDED` | `2` |
| `FLAG_DISABLE_DEPTH_TEST` | `3` |
| `FLAG_FIXED_SIZE` | `4` |
| `FLAG_MAX` | `5` |

## SpriteBase3D.AlphaCutMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALPHA_CUT_DISABLED` | `0` |
| `ALPHA_CUT_DISCARD` | `1` |
| `ALPHA_CUT_OPAQUE_PREPASS` | `2` |
| `ALPHA_CUT_HASH` | `3` |

## SpriteFrames.LoopMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LOOP_NONE` | `0` |
| `LOOP_LINEAR` | `1` |
| `LOOP_PINGPONG` | `2` |

## StreamPeerSocket.Status

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATUS_NONE` | `0` |
| `STATUS_CONNECTING` | `1` |
| `STATUS_CONNECTED` | `2` |
| `STATUS_ERROR` | `3` |

## StreamPeerTLS.Status

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATUS_DISCONNECTED` | `0` |
| `STATUS_HANDSHAKING` | `1` |
| `STATUS_CONNECTED` | `2` |
| `STATUS_ERROR` | `3` |
| `STATUS_ERROR_HOSTNAME_MISMATCH` | `4` |

## StyleBoxTexture.AxisStretchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AXIS_STRETCH_MODE_STRETCH` | `0` |
| `AXIS_STRETCH_MODE_TILE` | `1` |
| `AXIS_STRETCH_MODE_TILE_FIT` | `2` |

## SubViewport.ClearMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CLEAR_MODE_ALWAYS` | `0` |
| `CLEAR_MODE_NEVER` | `1` |
| `CLEAR_MODE_ONCE` | `2` |

## SubViewport.UpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UPDATE_DISABLED` | `0` |
| `UPDATE_ONCE` | `1` |
| `UPDATE_WHEN_VISIBLE` | `2` |
| `UPDATE_WHEN_PARENT_VISIBLE` | `3` |
| `UPDATE_ALWAYS` | `4` |

## SurfaceTool.CustomFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CUSTOM_RGBA8_UNORM` | `0` |
| `CUSTOM_RGBA8_SNORM` | `1` |
| `CUSTOM_RG_HALF` | `2` |
| `CUSTOM_RGBA_HALF` | `3` |
| `CUSTOM_R_FLOAT` | `4` |
| `CUSTOM_RG_FLOAT` | `5` |
| `CUSTOM_RGB_FLOAT` | `6` |
| `CUSTOM_RGBA_FLOAT` | `7` |
| `CUSTOM_MAX` | `8` |

## SurfaceTool.SkinWeightCount

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SKIN_4_WEIGHTS` | `0` |
| `SKIN_8_WEIGHTS` | `1` |

## TabBar.AlignmentMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ALIGNMENT_LEFT` | `0` |
| `ALIGNMENT_CENTER` | `1` |
| `ALIGNMENT_RIGHT` | `2` |
| `ALIGNMENT_MAX` | `3` |

## TabBar.CloseButtonDisplayPolicy

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CLOSE_BUTTON_SHOW_NEVER` | `0` |
| `CLOSE_BUTTON_SHOW_ACTIVE_ONLY` | `1` |
| `CLOSE_BUTTON_SHOW_ALWAYS` | `2` |
| `CLOSE_BUTTON_MAX` | `3` |

## TabContainer.TabPosition

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `POSITION_TOP` | `0` |
| `POSITION_BOTTOM` | `1` |
| `POSITION_MAX` | `2` |

## TextEdit.MenuItems

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MENU_CUT` | `0` |
| `MENU_COPY` | `1` |
| `MENU_PASTE` | `2` |
| `MENU_CLEAR` | `3` |
| `MENU_SELECT_ALL` | `4` |
| `MENU_UNDO` | `5` |
| `MENU_REDO` | `6` |
| `MENU_SUBMENU_TEXT_DIR` | `7` |
| `MENU_DIR_INHERITED` | `8` |
| `MENU_DIR_AUTO` | `9` |
| `MENU_DIR_LTR` | `10` |
| `MENU_DIR_RTL` | `11` |
| `MENU_DISPLAY_UCC` | `12` |
| `MENU_SUBMENU_INSERT_UCC` | `13` |
| `MENU_INSERT_LRM` | `14` |
| `MENU_INSERT_RLM` | `15` |
| `MENU_INSERT_LRE` | `16` |
| `MENU_INSERT_RLE` | `17` |
| `MENU_INSERT_LRO` | `18` |
| `MENU_INSERT_RLO` | `19` |
| `MENU_INSERT_PDF` | `20` |
| `MENU_INSERT_ALM` | `21` |
| `MENU_INSERT_LRI` | `22` |
| `MENU_INSERT_RLI` | `23` |
| `MENU_INSERT_FSI` | `24` |
| `MENU_INSERT_PDI` | `25` |
| `MENU_INSERT_ZWJ` | `26` |
| `MENU_INSERT_ZWNJ` | `27` |
| `MENU_INSERT_WJ` | `28` |
| `MENU_INSERT_SHY` | `29` |
| `MENU_EMOJI_AND_SYMBOL` | `30` |
| `MENU_MAX` | `31` |

## TextEdit.EditAction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ACTION_NONE` | `0` |
| `ACTION_TYPING` | `1` |
| `ACTION_BACKSPACE` | `2` |
| `ACTION_DELETE` | `3` |

## TextEdit.SearchFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SEARCH_MATCH_CASE` | `1` |
| `SEARCH_WHOLE_WORDS` | `2` |
| `SEARCH_BACKWARDS` | `4` |

## TextEdit.CaretType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CARET_TYPE_LINE` | `0` |
| `CARET_TYPE_BLOCK` | `1` |

## TextEdit.SelectionMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SELECTION_MODE_NONE` | `0` |
| `SELECTION_MODE_SHIFT` | `1` |
| `SELECTION_MODE_POINTER` | `2` |
| `SELECTION_MODE_WORD` | `3` |
| `SELECTION_MODE_LINE` | `4` |

## TextEdit.LineWrappingMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LINE_WRAPPING_NONE` | `0` |
| `LINE_WRAPPING_BOUNDARY` | `1` |

## TextEdit.GutterType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GUTTER_TYPE_STRING` | `0` |
| `GUTTER_TYPE_ICON` | `1` |
| `GUTTER_TYPE_CUSTOM` | `2` |

## TextServer.FontAntialiasing

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FONT_ANTIALIASING_NONE` | `0` |
| `FONT_ANTIALIASING_GRAY` | `1` |
| `FONT_ANTIALIASING_LCD` | `2` |

## TextServer.FontLCDSubpixelLayout

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FONT_LCD_SUBPIXEL_LAYOUT_NONE` | `0` |
| `FONT_LCD_SUBPIXEL_LAYOUT_HRGB` | `1` |
| `FONT_LCD_SUBPIXEL_LAYOUT_HBGR` | `2` |
| `FONT_LCD_SUBPIXEL_LAYOUT_VRGB` | `3` |
| `FONT_LCD_SUBPIXEL_LAYOUT_VBGR` | `4` |
| `FONT_LCD_SUBPIXEL_LAYOUT_MAX` | `5` |

## TextServer.Direction

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DIRECTION_AUTO` | `0` |
| `DIRECTION_LTR` | `1` |
| `DIRECTION_RTL` | `2` |
| `DIRECTION_INHERITED` | `3` |

## TextServer.Orientation

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ORIENTATION_HORIZONTAL` | `0` |
| `ORIENTATION_VERTICAL` | `1` |

## TextServer.JustificationFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `JUSTIFICATION_NONE` | `0` |
| `JUSTIFICATION_KASHIDA` | `1` |
| `JUSTIFICATION_WORD_BOUND` | `2` |
| `JUSTIFICATION_TRIM_EDGE_SPACES` | `4` |
| `JUSTIFICATION_AFTER_LAST_TAB` | `8` |
| `JUSTIFICATION_CONSTRAIN_ELLIPSIS` | `16` |
| `JUSTIFICATION_SKIP_LAST_LINE` | `32` |
| `JUSTIFICATION_SKIP_LAST_LINE_WITH_VISIBLE_CHARS` | `64` |
| `JUSTIFICATION_DO_NOT_SKIP_SINGLE_LINE` | `128` |

## TextServer.AutowrapMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `AUTOWRAP_OFF` | `0` |
| `AUTOWRAP_ARBITRARY` | `1` |
| `AUTOWRAP_WORD` | `2` |
| `AUTOWRAP_WORD_SMART` | `3` |

## TextServer.LineBreakFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `BREAK_NONE` | `0` |
| `BREAK_MANDATORY` | `1` |
| `BREAK_WORD_BOUND` | `2` |
| `BREAK_GRAPHEME_BOUND` | `4` |
| `BREAK_ADAPTIVE` | `8` |
| `BREAK_TRIM_EDGE_SPACES` | `16` |
| `BREAK_TRIM_INDENT` | `32` |
| `BREAK_TRIM_START_EDGE_SPACES` | `64` |
| `BREAK_TRIM_END_EDGE_SPACES` | `128` |

## TextServer.VisibleCharactersBehavior

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VC_CHARS_BEFORE_SHAPING` | `0` |
| `VC_CHARS_AFTER_SHAPING` | `1` |
| `VC_GLYPHS_AUTO` | `2` |
| `VC_GLYPHS_LTR` | `3` |
| `VC_GLYPHS_RTL` | `4` |

## TextServer.OverrunBehavior

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OVERRUN_NO_TRIMMING` | `0` |
| `OVERRUN_TRIM_CHAR` | `1` |
| `OVERRUN_TRIM_WORD` | `2` |
| `OVERRUN_TRIM_ELLIPSIS` | `3` |
| `OVERRUN_TRIM_WORD_ELLIPSIS` | `4` |
| `OVERRUN_TRIM_ELLIPSIS_FORCE` | `5` |
| `OVERRUN_TRIM_WORD_ELLIPSIS_FORCE` | `6` |

## TextServer.TextOverrunFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `OVERRUN_NO_TRIM` | `0` |
| `OVERRUN_TRIM` | `1` |
| `OVERRUN_TRIM_WORD_ONLY` | `2` |
| `OVERRUN_ADD_ELLIPSIS` | `4` |
| `OVERRUN_ENFORCE_ELLIPSIS` | `8` |
| `OVERRUN_JUSTIFICATION_AWARE` | `16` |
| `OVERRUN_SHORT_STRING_ELLIPSIS` | `32` |

## TextServer.GraphemeFlag

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `GRAPHEME_IS_VALID` | `1` |
| `GRAPHEME_IS_RTL` | `2` |
| `GRAPHEME_IS_VIRTUAL` | `4` |
| `GRAPHEME_IS_SPACE` | `8` |
| `GRAPHEME_IS_BREAK_HARD` | `16` |
| `GRAPHEME_IS_BREAK_SOFT` | `32` |
| `GRAPHEME_IS_TAB` | `64` |
| `GRAPHEME_IS_ELONGATION` | `128` |
| `GRAPHEME_IS_PUNCTUATION` | `256` |
| `GRAPHEME_IS_UNDERSCORE` | `512` |
| `GRAPHEME_IS_CONNECTED` | `1024` |
| `GRAPHEME_IS_SAFE_TO_INSERT_TATWEEL` | `2048` |
| `GRAPHEME_IS_EMBEDDED_OBJECT` | `4096` |
| `GRAPHEME_IS_SOFT_HYPHEN` | `8192` |

## TextServer.Hinting

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HINTING_NONE` | `0` |
| `HINTING_LIGHT` | `1` |
| `HINTING_NORMAL` | `2` |

## TextServer.SubpixelPositioning

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SUBPIXEL_POSITIONING_DISABLED` | `0` |
| `SUBPIXEL_POSITIONING_AUTO` | `1` |
| `SUBPIXEL_POSITIONING_ONE_HALF` | `2` |
| `SUBPIXEL_POSITIONING_ONE_QUARTER` | `3` |
| `SUBPIXEL_POSITIONING_ONE_HALF_MAX_SIZE` | `20` |
| `SUBPIXEL_POSITIONING_ONE_QUARTER_MAX_SIZE` | `16` |

## TextServer.Feature

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FEATURE_SIMPLE_LAYOUT` | `1` |
| `FEATURE_BIDI_LAYOUT` | `2` |
| `FEATURE_VERTICAL_LAYOUT` | `4` |
| `FEATURE_SHAPING` | `8` |
| `FEATURE_KASHIDA_JUSTIFICATION` | `16` |
| `FEATURE_BREAK_ITERATORS` | `32` |
| `FEATURE_FONT_BITMAP` | `64` |
| `FEATURE_FONT_DYNAMIC` | `128` |
| `FEATURE_FONT_MSDF` | `256` |
| `FEATURE_FONT_SYSTEM` | `512` |
| `FEATURE_FONT_VARIABLE` | `1024` |
| `FEATURE_CONTEXT_SENSITIVE_CASE_CONVERSION` | `2048` |
| `FEATURE_USE_SUPPORT_DATA` | `4096` |
| `FEATURE_UNICODE_IDENTIFIERS` | `8192` |
| `FEATURE_UNICODE_SECURITY` | `16384` |

## TextServer.ContourPointTag

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONTOUR_CURVE_TAG_ON` | `1` |
| `CONTOUR_CURVE_TAG_OFF_CONIC` | `0` |
| `CONTOUR_CURVE_TAG_OFF_CUBIC` | `2` |

## TextServer.SpacingType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SPACING_GLYPH` | `0` |
| `SPACING_SPACE` | `1` |
| `SPACING_TOP` | `2` |
| `SPACING_BOTTOM` | `3` |
| `SPACING_MAX` | `4` |

## TextServer.FontStyle

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `FONT_BOLD` | `1` |
| `FONT_ITALIC` | `2` |
| `FONT_FIXED_WIDTH` | `4` |

## TextServer.StructuredTextParser

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STRUCTURED_TEXT_DEFAULT` | `0` |
| `STRUCTURED_TEXT_URI` | `1` |
| `STRUCTURED_TEXT_FILE` | `2` |
| `STRUCTURED_TEXT_EMAIL` | `3` |
| `STRUCTURED_TEXT_LIST` | `4` |
| `STRUCTURED_TEXT_GDSCRIPT` | `5` |
| `STRUCTURED_TEXT_CUSTOM` | `6` |

## TextServer.FixedSizeScaleMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FIXED_SIZE_SCALE_DISABLE` | `0` |
| `FIXED_SIZE_SCALE_INTEGER_ONLY` | `1` |
| `FIXED_SIZE_SCALE_ENABLED` | `2` |

## TextureButton.StretchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STRETCH_SCALE` | `0` |
| `STRETCH_TILE` | `1` |
| `STRETCH_KEEP` | `2` |
| `STRETCH_KEEP_CENTERED` | `3` |
| `STRETCH_KEEP_ASPECT` | `4` |
| `STRETCH_KEEP_ASPECT_CENTERED` | `5` |
| `STRETCH_KEEP_ASPECT_COVERED` | `6` |

## TextureLayered.LayeredType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LAYERED_TYPE_2D_ARRAY` | `0` |
| `LAYERED_TYPE_CUBEMAP` | `1` |
| `LAYERED_TYPE_CUBEMAP_ARRAY` | `2` |

## TextureProgressBar.FillMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILL_LEFT_TO_RIGHT` | `0` |
| `FILL_RIGHT_TO_LEFT` | `1` |
| `FILL_TOP_TO_BOTTOM` | `2` |
| `FILL_BOTTOM_TO_TOP` | `3` |
| `FILL_CLOCKWISE` | `4` |
| `FILL_COUNTER_CLOCKWISE` | `5` |
| `FILL_BILINEAR_LEFT_AND_RIGHT` | `6` |
| `FILL_BILINEAR_TOP_AND_BOTTOM` | `7` |
| `FILL_CLOCKWISE_AND_COUNTER_CLOCKWISE` | `8` |

## TextureRect.ExpandMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EXPAND_KEEP_SIZE` | `0` |
| `EXPAND_IGNORE_SIZE` | `1` |
| `EXPAND_FIT_WIDTH` | `2` |
| `EXPAND_FIT_WIDTH_PROPORTIONAL` | `3` |
| `EXPAND_FIT_HEIGHT` | `4` |
| `EXPAND_FIT_HEIGHT_PROPORTIONAL` | `5` |

## TextureRect.StretchMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STRETCH_SCALE` | `0` |
| `STRETCH_TILE` | `1` |
| `STRETCH_KEEP` | `2` |
| `STRETCH_KEEP_CENTERED` | `3` |
| `STRETCH_KEEP_ASPECT` | `4` |
| `STRETCH_KEEP_ASPECT_CENTERED` | `5` |
| `STRETCH_KEEP_ASPECT_COVERED` | `6` |

## Theme.DataType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DATA_TYPE_COLOR` | `0` |
| `DATA_TYPE_CONSTANT` | `1` |
| `DATA_TYPE_FONT` | `2` |
| `DATA_TYPE_FONT_SIZE` | `3` |
| `DATA_TYPE_ICON` | `4` |
| `DATA_TYPE_STYLEBOX` | `5` |
| `DATA_TYPE_MAX` | `6` |

## Thread.Priority

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PRIORITY_LOW` | `0` |
| `PRIORITY_NORMAL` | `1` |
| `PRIORITY_HIGH` | `2` |

## TileMap.VisibilityMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_MODE_DEFAULT` | `0` |
| `VISIBILITY_MODE_FORCE_HIDE` | `2` |
| `VISIBILITY_MODE_FORCE_SHOW` | `1` |

## TileMapLayer.DebugVisibilityMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEBUG_VISIBILITY_MODE_DEFAULT` | `0` |
| `DEBUG_VISIBILITY_MODE_FORCE_HIDE` | `2` |
| `DEBUG_VISIBILITY_MODE_FORCE_SHOW` | `1` |

## TileSet.TileShape

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TILE_SHAPE_SQUARE` | `0` |
| `TILE_SHAPE_ISOMETRIC` | `1` |
| `TILE_SHAPE_HALF_OFFSET_SQUARE` | `2` |
| `TILE_SHAPE_HEXAGON` | `3` |

## TileSet.TileLayout

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TILE_LAYOUT_STACKED` | `0` |
| `TILE_LAYOUT_STACKED_OFFSET` | `1` |
| `TILE_LAYOUT_STAIRS_RIGHT` | `2` |
| `TILE_LAYOUT_STAIRS_DOWN` | `3` |
| `TILE_LAYOUT_DIAMOND_RIGHT` | `4` |
| `TILE_LAYOUT_DIAMOND_DOWN` | `5` |

## TileSet.TileOffsetAxis

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TILE_OFFSET_AXIS_HORIZONTAL` | `0` |
| `TILE_OFFSET_AXIS_VERTICAL` | `1` |

## TileSet.CellNeighbor

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CELL_NEIGHBOR_RIGHT_SIDE` | `0` |
| `CELL_NEIGHBOR_RIGHT_CORNER` | `1` |
| `CELL_NEIGHBOR_BOTTOM_RIGHT_SIDE` | `2` |
| `CELL_NEIGHBOR_BOTTOM_RIGHT_CORNER` | `3` |
| `CELL_NEIGHBOR_BOTTOM_SIDE` | `4` |
| `CELL_NEIGHBOR_BOTTOM_CORNER` | `5` |
| `CELL_NEIGHBOR_BOTTOM_LEFT_SIDE` | `6` |
| `CELL_NEIGHBOR_BOTTOM_LEFT_CORNER` | `7` |
| `CELL_NEIGHBOR_LEFT_SIDE` | `8` |
| `CELL_NEIGHBOR_LEFT_CORNER` | `9` |
| `CELL_NEIGHBOR_TOP_LEFT_SIDE` | `10` |
| `CELL_NEIGHBOR_TOP_LEFT_CORNER` | `11` |
| `CELL_NEIGHBOR_TOP_SIDE` | `12` |
| `CELL_NEIGHBOR_TOP_CORNER` | `13` |
| `CELL_NEIGHBOR_TOP_RIGHT_SIDE` | `14` |
| `CELL_NEIGHBOR_TOP_RIGHT_CORNER` | `15` |

## TileSet.TerrainMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TERRAIN_MODE_MATCH_CORNERS_AND_SIDES` | `0` |
| `TERRAIN_MODE_MATCH_CORNERS` | `1` |
| `TERRAIN_MODE_MATCH_SIDES` | `2` |

## TileSetAtlasSource.TileAnimationMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TILE_ANIMATION_MODE_DEFAULT` | `0` |
| `TILE_ANIMATION_MODE_RANDOM_START_TIMES` | `1` |
| `TILE_ANIMATION_MODE_MAX` | `2` |

## Time.Month

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MONTH_JANUARY` | `1` |
| `MONTH_FEBRUARY` | `2` |
| `MONTH_MARCH` | `3` |
| `MONTH_APRIL` | `4` |
| `MONTH_MAY` | `5` |
| `MONTH_JUNE` | `6` |
| `MONTH_JULY` | `7` |
| `MONTH_AUGUST` | `8` |
| `MONTH_SEPTEMBER` | `9` |
| `MONTH_OCTOBER` | `10` |
| `MONTH_NOVEMBER` | `11` |
| `MONTH_DECEMBER` | `12` |

## Time.Weekday

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WEEKDAY_SUNDAY` | `0` |
| `WEEKDAY_MONDAY` | `1` |
| `WEEKDAY_TUESDAY` | `2` |
| `WEEKDAY_WEDNESDAY` | `3` |
| `WEEKDAY_THURSDAY` | `4` |
| `WEEKDAY_FRIDAY` | `5` |
| `WEEKDAY_SATURDAY` | `6` |

## Timer.TimerProcessCallback

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TIMER_PROCESS_PHYSICS` | `0` |
| `TIMER_PROCESS_IDLE` | `1` |

## TouchScreenButton.VisibilityMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_ALWAYS` | `0` |
| `VISIBILITY_TOUCHSCREEN_ONLY` | `1` |

## Tree.SelectMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SELECT_SINGLE` | `0` |
| `SELECT_ROW` | `1` |
| `SELECT_MULTI` | `2` |

## Tree.DropModeFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DROP_MODE_DISABLED` | `0` |
| `DROP_MODE_ON_ITEM` | `1` |
| `DROP_MODE_INBETWEEN` | `2` |

## Tree.ScrollHintMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCROLL_HINT_MODE_DISABLED` | `0` |
| `SCROLL_HINT_MODE_BOTH` | `1` |
| `SCROLL_HINT_MODE_TOP` | `2` |
| `SCROLL_HINT_MODE_BOTTOM` | `3` |

## TreeItem.TreeCellMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CELL_MODE_STRING` | `0` |
| `CELL_MODE_CHECK` | `1` |
| `CELL_MODE_RANGE` | `2` |
| `CELL_MODE_ICON` | `3` |
| `CELL_MODE_CUSTOM` | `4` |

## Tween.TweenProcessMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TWEEN_PROCESS_PHYSICS` | `0` |
| `TWEEN_PROCESS_IDLE` | `1` |

## Tween.TweenPauseMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TWEEN_PAUSE_BOUND` | `0` |
| `TWEEN_PAUSE_STOP` | `1` |
| `TWEEN_PAUSE_PROCESS` | `2` |

## Tween.TransitionType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRANS_LINEAR` | `0` |
| `TRANS_SINE` | `1` |
| `TRANS_QUINT` | `2` |
| `TRANS_QUART` | `3` |
| `TRANS_QUAD` | `4` |
| `TRANS_EXPO` | `5` |
| `TRANS_ELASTIC` | `6` |
| `TRANS_CUBIC` | `7` |
| `TRANS_CIRC` | `8` |
| `TRANS_BOUNCE` | `9` |
| `TRANS_BACK` | `10` |
| `TRANS_SPRING` | `11` |

## Tween.EaseType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EASE_IN` | `0` |
| `EASE_OUT` | `1` |
| `EASE_IN_OUT` | `2` |
| `EASE_OUT_IN` | `3` |

## UPNP.UPNPResult

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `UPNP_RESULT_SUCCESS` | `0` |
| `UPNP_RESULT_NOT_AUTHORIZED` | `1` |
| `UPNP_RESULT_PORT_MAPPING_NOT_FOUND` | `2` |
| `UPNP_RESULT_INCONSISTENT_PARAMETERS` | `3` |
| `UPNP_RESULT_NO_SUCH_ENTRY_IN_ARRAY` | `4` |
| `UPNP_RESULT_ACTION_FAILED` | `5` |
| `UPNP_RESULT_SRC_IP_WILDCARD_NOT_PERMITTED` | `6` |
| `UPNP_RESULT_EXT_PORT_WILDCARD_NOT_PERMITTED` | `7` |
| `UPNP_RESULT_INT_PORT_WILDCARD_NOT_PERMITTED` | `8` |
| `UPNP_RESULT_REMOTE_HOST_MUST_BE_WILDCARD` | `9` |
| `UPNP_RESULT_EXT_PORT_MUST_BE_WILDCARD` | `10` |
| `UPNP_RESULT_NO_PORT_MAPS_AVAILABLE` | `11` |
| `UPNP_RESULT_CONFLICT_WITH_OTHER_MECHANISM` | `12` |
| `UPNP_RESULT_CONFLICT_WITH_OTHER_MAPPING` | `13` |
| `UPNP_RESULT_SAME_PORT_VALUES_REQUIRED` | `14` |
| `UPNP_RESULT_ONLY_PERMANENT_LEASE_SUPPORTED` | `15` |
| `UPNP_RESULT_INVALID_GATEWAY` | `16` |
| `UPNP_RESULT_INVALID_PORT` | `17` |
| `UPNP_RESULT_INVALID_PROTOCOL` | `18` |
| `UPNP_RESULT_INVALID_DURATION` | `19` |
| `UPNP_RESULT_INVALID_ARGS` | `20` |
| `UPNP_RESULT_INVALID_RESPONSE` | `21` |
| `UPNP_RESULT_INVALID_PARAM` | `22` |
| `UPNP_RESULT_HTTP_ERROR` | `23` |
| `UPNP_RESULT_SOCKET_ERROR` | `24` |
| `UPNP_RESULT_MEM_ALLOC_ERROR` | `25` |
| `UPNP_RESULT_NO_GATEWAY` | `26` |
| `UPNP_RESULT_NO_DEVICES` | `27` |
| `UPNP_RESULT_UNKNOWN_ERROR` | `28` |

## UPNPDevice.IGDStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `IGD_STATUS_OK` | `0` |
| `IGD_STATUS_HTTP_ERROR` | `1` |
| `IGD_STATUS_HTTP_EMPTY` | `2` |
| `IGD_STATUS_NO_URLS` | `3` |
| `IGD_STATUS_NO_IGD` | `4` |
| `IGD_STATUS_DISCONNECTED` | `5` |
| `IGD_STATUS_UNKNOWN_DEVICE` | `6` |
| `IGD_STATUS_INVALID_CONTROL` | `7` |
| `IGD_STATUS_MALLOC_ERROR` | `8` |
| `IGD_STATUS_UNKNOWN_ERROR` | `9` |

## UndoRedo.MergeMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MERGE_DISABLE` | `0` |
| `MERGE_ENDS` | `1` |
| `MERGE_ALL` | `2` |

## Viewport.PositionalShadowAtlasQuadrantSubdiv

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_DISABLED` | `0` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_1` | `1` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_4` | `2` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_16` | `3` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_64` | `4` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_256` | `5` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_1024` | `6` |
| `SHADOW_ATLAS_QUADRANT_SUBDIV_MAX` | `7` |

## Viewport.Scaling3DMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCALING_3D_MODE_BILINEAR` | `0` |
| `SCALING_3D_MODE_FSR` | `1` |
| `SCALING_3D_MODE_FSR2` | `2` |
| `SCALING_3D_MODE_METALFX_SPATIAL` | `3` |
| `SCALING_3D_MODE_METALFX_TEMPORAL` | `4` |
| `SCALING_3D_MODE_NEAREST` | `5` |
| `SCALING_3D_MODE_MAX` | `6` |

## Viewport.MSAA

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MSAA_DISABLED` | `0` |
| `MSAA_2X` | `1` |
| `MSAA_4X` | `2` |
| `MSAA_8X` | `3` |
| `MSAA_MAX` | `4` |

## Viewport.AnisotropicFiltering

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ANISOTROPY_DISABLED` | `0` |
| `ANISOTROPY_2X` | `1` |
| `ANISOTROPY_4X` | `2` |
| `ANISOTROPY_8X` | `3` |
| `ANISOTROPY_16X` | `4` |
| `ANISOTROPY_MAX` | `5` |

## Viewport.ScreenSpaceAA

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SCREEN_SPACE_AA_DISABLED` | `0` |
| `SCREEN_SPACE_AA_FXAA` | `1` |
| `SCREEN_SPACE_AA_SMAA` | `2` |
| `SCREEN_SPACE_AA_MAX` | `3` |

## Viewport.RenderInfo

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RENDER_INFO_OBJECTS_IN_FRAME` | `0` |
| `RENDER_INFO_PRIMITIVES_IN_FRAME` | `1` |
| `RENDER_INFO_DRAW_CALLS_IN_FRAME` | `2` |
| `RENDER_INFO_MAX` | `3` |

## Viewport.RenderInfoType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RENDER_INFO_TYPE_VISIBLE` | `0` |
| `RENDER_INFO_TYPE_SHADOW` | `1` |
| `RENDER_INFO_TYPE_CANVAS` | `2` |
| `RENDER_INFO_TYPE_MAX` | `3` |

## Viewport.DebugDraw

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEBUG_DRAW_DISABLED` | `0` |
| `DEBUG_DRAW_UNSHADED` | `1` |
| `DEBUG_DRAW_LIGHTING` | `2` |
| `DEBUG_DRAW_OVERDRAW` | `3` |
| `DEBUG_DRAW_WIREFRAME` | `4` |
| `DEBUG_DRAW_NORMAL_BUFFER` | `5` |
| `DEBUG_DRAW_VOXEL_GI_ALBEDO` | `6` |
| `DEBUG_DRAW_VOXEL_GI_LIGHTING` | `7` |
| `DEBUG_DRAW_VOXEL_GI_EMISSION` | `8` |
| `DEBUG_DRAW_SHADOW_ATLAS` | `9` |
| `DEBUG_DRAW_DIRECTIONAL_SHADOW_ATLAS` | `10` |
| `DEBUG_DRAW_SCENE_LUMINANCE` | `11` |
| `DEBUG_DRAW_SSAO` | `12` |
| `DEBUG_DRAW_SSIL` | `13` |
| `DEBUG_DRAW_PSSM_SPLITS` | `14` |
| `DEBUG_DRAW_DECAL_ATLAS` | `15` |
| `DEBUG_DRAW_SDFGI` | `16` |
| `DEBUG_DRAW_SDFGI_PROBES` | `17` |
| `DEBUG_DRAW_GI_BUFFER` | `18` |
| `DEBUG_DRAW_DISABLE_LOD` | `19` |
| `DEBUG_DRAW_CLUSTER_OMNI_LIGHTS` | `20` |
| `DEBUG_DRAW_CLUSTER_SPOT_LIGHTS` | `21` |
| `DEBUG_DRAW_CLUSTER_DECALS` | `22` |
| `DEBUG_DRAW_CLUSTER_REFLECTION_PROBES` | `23` |
| `DEBUG_DRAW_OCCLUDERS` | `24` |
| `DEBUG_DRAW_MOTION_VECTORS` | `25` |
| `DEBUG_DRAW_INTERNAL_BUFFER` | `26` |
| `DEBUG_DRAW_CLUSTER_AREA_LIGHTS` | `27` |
| `DEBUG_DRAW_AREA_LIGHT_ATLAS` | `28` |

## Viewport.DefaultCanvasItemTextureFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_NEAREST` | `0` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_LINEAR` | `1` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_LINEAR_WITH_MIPMAPS` | `2` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_NEAREST_WITH_MIPMAPS` | `3` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_PARENT_NODE` | `4` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_FILTER_MAX` | `5` |

## Viewport.DefaultCanvasItemTextureRepeat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `DEFAULT_CANVAS_ITEM_TEXTURE_REPEAT_DISABLED` | `0` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_REPEAT_ENABLED` | `1` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_REPEAT_MIRROR` | `2` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_REPEAT_PARENT_NODE` | `3` |
| `DEFAULT_CANVAS_ITEM_TEXTURE_REPEAT_MAX` | `4` |

## Viewport.SDFOversize

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SDF_OVERSIZE_100_PERCENT` | `0` |
| `SDF_OVERSIZE_120_PERCENT` | `1` |
| `SDF_OVERSIZE_150_PERCENT` | `2` |
| `SDF_OVERSIZE_200_PERCENT` | `3` |
| `SDF_OVERSIZE_MAX` | `4` |

## Viewport.SDFScale

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SDF_SCALE_100_PERCENT` | `0` |
| `SDF_SCALE_50_PERCENT` | `1` |
| `SDF_SCALE_25_PERCENT` | `2` |
| `SDF_SCALE_MAX` | `3` |

## Viewport.VRSMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VRS_DISABLED` | `0` |
| `VRS_TEXTURE` | `1` |
| `VRS_XR` | `2` |
| `VRS_MAX` | `3` |

## Viewport.VRSUpdateMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VRS_UPDATE_DISABLED` | `0` |
| `VRS_UPDATE_ONCE` | `1` |
| `VRS_UPDATE_ALWAYS` | `2` |
| `VRS_UPDATE_MAX` | `3` |

## VirtualJoystick.JoystickMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOYSTICK_FIXED` | `0` |
| `JOYSTICK_DYNAMIC` | `1` |
| `JOYSTICK_FOLLOWING` | `2` |

## VirtualJoystick.VisibilityMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VISIBILITY_ALWAYS` | `0` |
| `VISIBILITY_WHEN_TOUCHED` | `1` |

## VisibleOnScreenEnabler2D.EnableMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENABLE_MODE_INHERIT` | `0` |
| `ENABLE_MODE_ALWAYS` | `1` |
| `ENABLE_MODE_WHEN_PAUSED` | `2` |

## VisibleOnScreenEnabler3D.EnableMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `ENABLE_MODE_INHERIT` | `0` |
| `ENABLE_MODE_ALWAYS` | `1` |
| `ENABLE_MODE_WHEN_PAUSED` | `2` |

## VisualShader.Type

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_VERTEX` | `0` |
| `TYPE_FRAGMENT` | `1` |
| `TYPE_LIGHT` | `2` |
| `TYPE_START` | `3` |
| `TYPE_PROCESS` | `4` |
| `TYPE_COLLIDE` | `5` |
| `TYPE_START_CUSTOM` | `6` |
| `TYPE_PROCESS_CUSTOM` | `7` |
| `TYPE_SKY` | `8` |
| `TYPE_FOG` | `9` |
| `TYPE_TEXTURE_BLIT` | `10` |
| `TYPE_MAX` | `11` |

## VisualShader.VaryingMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VARYING_MODE_VERTEX_TO_FRAG_LIGHT` | `0` |
| `VARYING_MODE_FRAG_TO_LIGHT` | `1` |
| `VARYING_MODE_MAX` | `2` |

## VisualShader.VaryingType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `VARYING_TYPE_FLOAT` | `0` |
| `VARYING_TYPE_INT` | `1` |
| `VARYING_TYPE_UINT` | `2` |
| `VARYING_TYPE_VECTOR_2D` | `3` |
| `VARYING_TYPE_VECTOR_3D` | `4` |
| `VARYING_TYPE_VECTOR_4D` | `5` |
| `VARYING_TYPE_BOOLEAN` | `6` |
| `VARYING_TYPE_TRANSFORM` | `7` |
| `VARYING_TYPE_MAX` | `8` |

## VisualShaderNode.PortType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PORT_TYPE_SCALAR` | `0` |
| `PORT_TYPE_SCALAR_INT` | `1` |
| `PORT_TYPE_SCALAR_UINT` | `2` |
| `PORT_TYPE_VECTOR_2D` | `3` |
| `PORT_TYPE_VECTOR_3D` | `4` |
| `PORT_TYPE_VECTOR_4D` | `5` |
| `PORT_TYPE_BOOLEAN` | `6` |
| `PORT_TYPE_TRANSFORM` | `7` |
| `PORT_TYPE_SAMPLER` | `8` |
| `PORT_TYPE_MAX` | `9` |

## VisualShaderNodeBillboard.BillboardType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BILLBOARD_TYPE_DISABLED` | `0` |
| `BILLBOARD_TYPE_ENABLED` | `1` |
| `BILLBOARD_TYPE_FIXED_Y` | `2` |
| `BILLBOARD_TYPE_PARTICLES` | `3` |
| `BILLBOARD_TYPE_MAX` | `4` |

## VisualShaderNodeClamp.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_FLOAT` | `0` |
| `OP_TYPE_INT` | `1` |
| `OP_TYPE_UINT` | `2` |
| `OP_TYPE_VECTOR_2D` | `3` |
| `OP_TYPE_VECTOR_3D` | `4` |
| `OP_TYPE_VECTOR_4D` | `5` |
| `OP_TYPE_MAX` | `6` |

## VisualShaderNodeColorFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_GRAYSCALE` | `0` |
| `FUNC_HSV2RGB` | `1` |
| `FUNC_RGB2HSV` | `2` |
| `FUNC_SEPIA` | `3` |
| `FUNC_LINEAR_TO_SRGB` | `4` |
| `FUNC_SRGB_TO_LINEAR` | `5` |
| `FUNC_MAX` | `6` |

## VisualShaderNodeColorOp.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_SCREEN` | `0` |
| `OP_DIFFERENCE` | `1` |
| `OP_DARKEN` | `2` |
| `OP_LIGHTEN` | `3` |
| `OP_OVERLAY` | `4` |
| `OP_DODGE` | `5` |
| `OP_BURN` | `6` |
| `OP_SOFT_LIGHT` | `7` |
| `OP_HARD_LIGHT` | `8` |
| `OP_MAX` | `9` |

## VisualShaderNodeCompare.ComparisonType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CTYPE_SCALAR` | `0` |
| `CTYPE_SCALAR_INT` | `1` |
| `CTYPE_SCALAR_UINT` | `2` |
| `CTYPE_VECTOR_2D` | `3` |
| `CTYPE_VECTOR_3D` | `4` |
| `CTYPE_VECTOR_4D` | `5` |
| `CTYPE_BOOLEAN` | `6` |
| `CTYPE_TRANSFORM` | `7` |
| `CTYPE_MAX` | `8` |

## VisualShaderNodeCompare.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_EQUAL` | `0` |
| `FUNC_NOT_EQUAL` | `1` |
| `FUNC_GREATER_THAN` | `2` |
| `FUNC_GREATER_THAN_EQUAL` | `3` |
| `FUNC_LESS_THAN` | `4` |
| `FUNC_LESS_THAN_EQUAL` | `5` |
| `FUNC_MAX` | `6` |

## VisualShaderNodeCompare.Condition

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COND_ALL` | `0` |
| `COND_ANY` | `1` |
| `COND_MAX` | `2` |

## VisualShaderNodeCubemap.Source

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SOURCE_TEXTURE` | `0` |
| `SOURCE_PORT` | `1` |
| `SOURCE_MAX` | `2` |

## VisualShaderNodeCubemap.TextureType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_DATA` | `0` |
| `TYPE_COLOR` | `1` |
| `TYPE_NORMAL_MAP` | `2` |
| `TYPE_MAX` | `3` |

## VisualShaderNodeDerivativeFunc.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_3D` | `2` |
| `OP_TYPE_VECTOR_4D` | `3` |
| `OP_TYPE_MAX` | `4` |

## VisualShaderNodeDerivativeFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_SUM` | `0` |
| `FUNC_X` | `1` |
| `FUNC_Y` | `2` |
| `FUNC_MAX` | `3` |

## VisualShaderNodeDerivativeFunc.Precision

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `PRECISION_NONE` | `0` |
| `PRECISION_COARSE` | `1` |
| `PRECISION_FINE` | `2` |
| `PRECISION_MAX` | `3` |

## VisualShaderNodeFloatFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_SIN` | `0` |
| `FUNC_COS` | `1` |
| `FUNC_TAN` | `2` |
| `FUNC_ASIN` | `3` |
| `FUNC_ACOS` | `4` |
| `FUNC_ATAN` | `5` |
| `FUNC_SINH` | `6` |
| `FUNC_COSH` | `7` |
| `FUNC_TANH` | `8` |
| `FUNC_LOG` | `9` |
| `FUNC_EXP` | `10` |
| `FUNC_SQRT` | `11` |
| `FUNC_ABS` | `12` |
| `FUNC_SIGN` | `13` |
| `FUNC_FLOOR` | `14` |
| `FUNC_ROUND` | `15` |
| `FUNC_CEIL` | `16` |
| `FUNC_FRACT` | `17` |
| `FUNC_SATURATE` | `18` |
| `FUNC_NEGATE` | `19` |
| `FUNC_ACOSH` | `20` |
| `FUNC_ASINH` | `21` |
| `FUNC_ATANH` | `22` |
| `FUNC_DEGREES` | `23` |
| `FUNC_EXP2` | `24` |
| `FUNC_INVERSE_SQRT` | `25` |
| `FUNC_LOG2` | `26` |
| `FUNC_RADIANS` | `27` |
| `FUNC_RECIPROCAL` | `28` |
| `FUNC_ROUNDEVEN` | `29` |
| `FUNC_TRUNC` | `30` |
| `FUNC_ONEMINUS` | `31` |
| `FUNC_MAX` | `32` |

## VisualShaderNodeFloatOp.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_ADD` | `0` |
| `OP_SUB` | `1` |
| `OP_MUL` | `2` |
| `OP_DIV` | `3` |
| `OP_MOD` | `4` |
| `OP_POW` | `5` |
| `OP_MAX` | `6` |
| `OP_MIN` | `7` |
| `OP_ATAN2` | `8` |
| `OP_STEP` | `9` |
| `OP_ENUM_SIZE` | `10` |

## VisualShaderNodeFloatParameter.Hint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HINT_NONE` | `0` |
| `HINT_RANGE` | `1` |
| `HINT_RANGE_STEP` | `2` |
| `HINT_MAX` | `3` |

## VisualShaderNodeIntFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_ABS` | `0` |
| `FUNC_NEGATE` | `1` |
| `FUNC_SIGN` | `2` |
| `FUNC_BITWISE_NOT` | `3` |
| `FUNC_MAX` | `4` |

## VisualShaderNodeIntOp.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_ADD` | `0` |
| `OP_SUB` | `1` |
| `OP_MUL` | `2` |
| `OP_DIV` | `3` |
| `OP_MOD` | `4` |
| `OP_MAX` | `5` |
| `OP_MIN` | `6` |
| `OP_BITWISE_AND` | `7` |
| `OP_BITWISE_OR` | `8` |
| `OP_BITWISE_XOR` | `9` |
| `OP_BITWISE_LEFT_SHIFT` | `10` |
| `OP_BITWISE_RIGHT_SHIFT` | `11` |
| `OP_ENUM_SIZE` | `12` |

## VisualShaderNodeIntParameter.Hint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HINT_NONE` | `0` |
| `HINT_RANGE` | `1` |
| `HINT_RANGE_STEP` | `2` |
| `HINT_ENUM` | `3` |
| `HINT_MAX` | `4` |

## VisualShaderNodeIs.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_IS_INF` | `0` |
| `FUNC_IS_NAN` | `1` |
| `FUNC_MAX` | `2` |

## VisualShaderNodeMix.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_2D_SCALAR` | `2` |
| `OP_TYPE_VECTOR_3D` | `3` |
| `OP_TYPE_VECTOR_3D_SCALAR` | `4` |
| `OP_TYPE_VECTOR_4D` | `5` |
| `OP_TYPE_VECTOR_4D_SCALAR` | `6` |
| `OP_TYPE_MAX` | `7` |

## VisualShaderNodeMultiplyAdd.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_3D` | `2` |
| `OP_TYPE_VECTOR_4D` | `3` |
| `OP_TYPE_MAX` | `4` |

## VisualShaderNodeParameter.Qualifier

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `QUAL_NONE` | `0` |
| `QUAL_GLOBAL` | `1` |
| `QUAL_INSTANCE` | `2` |
| `QUAL_INSTANCE_INDEX` | `3` |
| `QUAL_MAX` | `4` |

## VisualShaderNodeParticleAccelerator.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_LINEAR` | `0` |
| `MODE_RADIAL` | `1` |
| `MODE_TANGENTIAL` | `2` |
| `MODE_MAX` | `3` |

## VisualShaderNodeParticleEmit.EmitFlags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `EMIT_FLAG_POSITION` | `1` |
| `EMIT_FLAG_ROT_SCALE` | `2` |
| `EMIT_FLAG_VELOCITY` | `4` |
| `EMIT_FLAG_COLOR` | `8` |
| `EMIT_FLAG_CUSTOM` | `16` |

## VisualShaderNodeParticleRandomness.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_3D` | `2` |
| `OP_TYPE_VECTOR_4D` | `3` |
| `OP_TYPE_MAX` | `4` |

## VisualShaderNodeRemap.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_2D_SCALAR` | `2` |
| `OP_TYPE_VECTOR_3D` | `3` |
| `OP_TYPE_VECTOR_3D_SCALAR` | `4` |
| `OP_TYPE_VECTOR_4D` | `5` |
| `OP_TYPE_VECTOR_4D_SCALAR` | `6` |
| `OP_TYPE_MAX` | `7` |

## VisualShaderNodeSample3D.Source

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SOURCE_TEXTURE` | `0` |
| `SOURCE_PORT` | `1` |
| `SOURCE_MAX` | `2` |

## VisualShaderNodeSmoothStep.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_2D_SCALAR` | `2` |
| `OP_TYPE_VECTOR_3D` | `3` |
| `OP_TYPE_VECTOR_3D_SCALAR` | `4` |
| `OP_TYPE_VECTOR_4D` | `5` |
| `OP_TYPE_VECTOR_4D_SCALAR` | `6` |
| `OP_TYPE_MAX` | `7` |

## VisualShaderNodeStep.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_SCALAR` | `0` |
| `OP_TYPE_VECTOR_2D` | `1` |
| `OP_TYPE_VECTOR_2D_SCALAR` | `2` |
| `OP_TYPE_VECTOR_3D` | `3` |
| `OP_TYPE_VECTOR_3D_SCALAR` | `4` |
| `OP_TYPE_VECTOR_4D` | `5` |
| `OP_TYPE_VECTOR_4D_SCALAR` | `6` |
| `OP_TYPE_MAX` | `7` |

## VisualShaderNodeSwitch.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_FLOAT` | `0` |
| `OP_TYPE_INT` | `1` |
| `OP_TYPE_UINT` | `2` |
| `OP_TYPE_VECTOR_2D` | `3` |
| `OP_TYPE_VECTOR_3D` | `4` |
| `OP_TYPE_VECTOR_4D` | `5` |
| `OP_TYPE_BOOLEAN` | `6` |
| `OP_TYPE_TRANSFORM` | `7` |
| `OP_TYPE_MAX` | `8` |

## VisualShaderNodeTexture.Source

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SOURCE_TEXTURE` | `0` |
| `SOURCE_SCREEN` | `1` |
| `SOURCE_2D_TEXTURE` | `2` |
| `SOURCE_2D_NORMAL` | `3` |
| `SOURCE_DEPTH` | `4` |
| `SOURCE_PORT` | `5` |
| `SOURCE_3D_NORMAL` | `6` |
| `SOURCE_ROUGHNESS` | `7` |
| `SOURCE_MAX` | `8` |

## VisualShaderNodeTexture.TextureType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_DATA` | `0` |
| `TYPE_COLOR` | `1` |
| `TYPE_NORMAL_MAP` | `2` |
| `TYPE_MAX` | `3` |

## VisualShaderNodeTextureParameter.TextureType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TYPE_DATA` | `0` |
| `TYPE_COLOR` | `1` |
| `TYPE_NORMAL_MAP` | `2` |
| `TYPE_ANISOTROPY` | `3` |
| `TYPE_MAX` | `4` |

## VisualShaderNodeTextureParameter.ColorDefault

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COLOR_DEFAULT_WHITE` | `0` |
| `COLOR_DEFAULT_BLACK` | `1` |
| `COLOR_DEFAULT_TRANSPARENT` | `2` |
| `COLOR_DEFAULT_MAX` | `3` |

## VisualShaderNodeTextureParameter.TextureFilter

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FILTER_DEFAULT` | `0` |
| `FILTER_NEAREST` | `1` |
| `FILTER_LINEAR` | `2` |
| `FILTER_NEAREST_MIPMAP` | `3` |
| `FILTER_LINEAR_MIPMAP` | `4` |
| `FILTER_NEAREST_MIPMAP_ANISOTROPIC` | `5` |
| `FILTER_LINEAR_MIPMAP_ANISOTROPIC` | `6` |
| `FILTER_MAX` | `7` |

## VisualShaderNodeTextureParameter.TextureRepeat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `REPEAT_DEFAULT` | `0` |
| `REPEAT_ENABLED` | `1` |
| `REPEAT_DISABLED` | `2` |
| `REPEAT_MAX` | `3` |

## VisualShaderNodeTextureParameter.TextureSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SOURCE_NONE` | `0` |
| `SOURCE_SCREEN` | `1` |
| `SOURCE_DEPTH` | `2` |
| `SOURCE_NORMAL_ROUGHNESS` | `3` |
| `SOURCE_MAX` | `4` |

## VisualShaderNodeTransformFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_INVERSE` | `0` |
| `FUNC_TRANSPOSE` | `1` |
| `FUNC_MAX` | `2` |

## VisualShaderNodeTransformOp.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_AxB` | `0` |
| `OP_BxA` | `1` |
| `OP_AxB_COMP` | `2` |
| `OP_BxA_COMP` | `3` |
| `OP_ADD` | `4` |
| `OP_A_MINUS_B` | `5` |
| `OP_B_MINUS_A` | `6` |
| `OP_A_DIV_B` | `7` |
| `OP_B_DIV_A` | `8` |
| `OP_MAX` | `9` |

## VisualShaderNodeTransformVecMult.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_AxB` | `0` |
| `OP_BxA` | `1` |
| `OP_3x3_AxB` | `2` |
| `OP_3x3_BxA` | `3` |
| `OP_MAX` | `4` |

## VisualShaderNodeUIntFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_NEGATE` | `0` |
| `FUNC_BITWISE_NOT` | `1` |
| `FUNC_MAX` | `2` |

## VisualShaderNodeUIntOp.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_ADD` | `0` |
| `OP_SUB` | `1` |
| `OP_MUL` | `2` |
| `OP_DIV` | `3` |
| `OP_MOD` | `4` |
| `OP_MAX` | `5` |
| `OP_MIN` | `6` |
| `OP_BITWISE_AND` | `7` |
| `OP_BITWISE_OR` | `8` |
| `OP_BITWISE_XOR` | `9` |
| `OP_BITWISE_LEFT_SHIFT` | `10` |
| `OP_BITWISE_RIGHT_SHIFT` | `11` |
| `OP_ENUM_SIZE` | `12` |

## VisualShaderNodeUVFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_PANNING` | `0` |
| `FUNC_SCALING` | `1` |
| `FUNC_MAX` | `2` |

## VisualShaderNodeVectorBase.OpType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_TYPE_VECTOR_2D` | `0` |
| `OP_TYPE_VECTOR_3D` | `1` |
| `OP_TYPE_VECTOR_4D` | `2` |
| `OP_TYPE_MAX` | `3` |

## VisualShaderNodeVectorFunc.Function

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FUNC_NORMALIZE` | `0` |
| `FUNC_SATURATE` | `1` |
| `FUNC_NEGATE` | `2` |
| `FUNC_RECIPROCAL` | `3` |
| `FUNC_ABS` | `4` |
| `FUNC_ACOS` | `5` |
| `FUNC_ACOSH` | `6` |
| `FUNC_ASIN` | `7` |
| `FUNC_ASINH` | `8` |
| `FUNC_ATAN` | `9` |
| `FUNC_ATANH` | `10` |
| `FUNC_CEIL` | `11` |
| `FUNC_COS` | `12` |
| `FUNC_COSH` | `13` |
| `FUNC_DEGREES` | `14` |
| `FUNC_EXP` | `15` |
| `FUNC_EXP2` | `16` |
| `FUNC_FLOOR` | `17` |
| `FUNC_FRACT` | `18` |
| `FUNC_INVERSE_SQRT` | `19` |
| `FUNC_LOG` | `20` |
| `FUNC_LOG2` | `21` |
| `FUNC_RADIANS` | `22` |
| `FUNC_ROUND` | `23` |
| `FUNC_ROUNDEVEN` | `24` |
| `FUNC_SIGN` | `25` |
| `FUNC_SIN` | `26` |
| `FUNC_SINH` | `27` |
| `FUNC_SQRT` | `28` |
| `FUNC_TAN` | `29` |
| `FUNC_TANH` | `30` |
| `FUNC_TRUNC` | `31` |
| `FUNC_ONEMINUS` | `32` |
| `FUNC_MAX` | `33` |

## VisualShaderNodeVectorOp.Operator

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `OP_ADD` | `0` |
| `OP_SUB` | `1` |
| `OP_MUL` | `2` |
| `OP_DIV` | `3` |
| `OP_MOD` | `4` |
| `OP_POW` | `5` |
| `OP_MAX` | `6` |
| `OP_MIN` | `7` |
| `OP_CROSS` | `8` |
| `OP_ATAN2` | `9` |
| `OP_REFLECT` | `10` |
| `OP_STEP` | `11` |
| `OP_ENUM_SIZE` | `12` |

## VoxelGI.Subdiv

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SUBDIV_64` | `0` |
| `SUBDIV_128` | `1` |
| `SUBDIV_256` | `2` |
| `SUBDIV_512` | `3` |
| `SUBDIV_MAX` | `4` |

## WebRTCDataChannel.WriteMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WRITE_MODE_TEXT` | `0` |
| `WRITE_MODE_BINARY` | `1` |

## WebRTCDataChannel.ChannelState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATE_CONNECTING` | `0` |
| `STATE_OPEN` | `1` |
| `STATE_CLOSING` | `2` |
| `STATE_CLOSED` | `3` |

## WebRTCPeerConnection.ConnectionState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATE_NEW` | `0` |
| `STATE_CONNECTING` | `1` |
| `STATE_CONNECTED` | `2` |
| `STATE_DISCONNECTED` | `3` |
| `STATE_FAILED` | `4` |
| `STATE_CLOSED` | `5` |

## WebRTCPeerConnection.GatheringState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `GATHERING_STATE_NEW` | `0` |
| `GATHERING_STATE_GATHERING` | `1` |
| `GATHERING_STATE_COMPLETE` | `2` |

## WebRTCPeerConnection.SignalingState

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `SIGNALING_STATE_STABLE` | `0` |
| `SIGNALING_STATE_HAVE_LOCAL_OFFER` | `1` |
| `SIGNALING_STATE_HAVE_REMOTE_OFFER` | `2` |
| `SIGNALING_STATE_HAVE_LOCAL_PRANSWER` | `3` |
| `SIGNALING_STATE_HAVE_REMOTE_PRANSWER` | `4` |
| `SIGNALING_STATE_CLOSED` | `5` |

## WebSocketPeer.WriteMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WRITE_MODE_TEXT` | `0` |
| `WRITE_MODE_BINARY` | `1` |

## WebSocketPeer.State

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `STATE_CONNECTING` | `0` |
| `STATE_OPEN` | `1` |
| `STATE_CLOSING` | `2` |
| `STATE_CLOSED` | `3` |

## WebXRInterface.TargetRayMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TARGET_RAY_MODE_UNKNOWN` | `0` |
| `TARGET_RAY_MODE_GAZE` | `1` |
| `TARGET_RAY_MODE_TRACKED_POINTER` | `2` |
| `TARGET_RAY_MODE_SCREEN` | `3` |

## Window.Mode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `MODE_WINDOWED` | `0` |
| `MODE_MINIMIZED` | `1` |
| `MODE_MAXIMIZED` | `2` |
| `MODE_FULLSCREEN` | `3` |
| `MODE_EXCLUSIVE_FULLSCREEN` | `4` |

## Window.Flags

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FLAG_RESIZE_DISABLED` | `0` |
| `FLAG_BORDERLESS` | `1` |
| `FLAG_ALWAYS_ON_TOP` | `2` |
| `FLAG_TRANSPARENT` | `3` |
| `FLAG_NO_FOCUS` | `4` |
| `FLAG_POPUP` | `5` |
| `FLAG_EXTEND_TO_TITLE` | `6` |
| `FLAG_MOUSE_PASSTHROUGH` | `7` |
| `FLAG_SHARP_CORNERS` | `8` |
| `FLAG_EXCLUDE_FROM_CAPTURE` | `9` |
| `FLAG_POPUP_WM_HINT` | `10` |
| `FLAG_MINIMIZE_DISABLED` | `11` |
| `FLAG_MAXIMIZE_DISABLED` | `12` |
| `FLAG_MAX` | `13` |

## Window.ContentScaleMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONTENT_SCALE_MODE_DISABLED` | `0` |
| `CONTENT_SCALE_MODE_CANVAS_ITEMS` | `1` |
| `CONTENT_SCALE_MODE_VIEWPORT` | `2` |

## Window.ContentScaleAspect

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONTENT_SCALE_ASPECT_IGNORE` | `0` |
| `CONTENT_SCALE_ASPECT_KEEP` | `1` |
| `CONTENT_SCALE_ASPECT_KEEP_WIDTH` | `2` |
| `CONTENT_SCALE_ASPECT_KEEP_HEIGHT` | `3` |
| `CONTENT_SCALE_ASPECT_EXPAND` | `4` |

## Window.ContentScaleStretch

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `CONTENT_SCALE_STRETCH_FRACTIONAL` | `0` |
| `CONTENT_SCALE_STRETCH_INTEGER` | `1` |

## Window.LayoutDirection

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `LAYOUT_DIRECTION_INHERITED` | `0` |
| `LAYOUT_DIRECTION_APPLICATION_LOCALE` | `1` |
| `LAYOUT_DIRECTION_LTR` | `2` |
| `LAYOUT_DIRECTION_RTL` | `3` |
| `LAYOUT_DIRECTION_SYSTEM_LOCALE` | `4` |
| `LAYOUT_DIRECTION_MAX` | `5` |
| `LAYOUT_DIRECTION_LOCALE` | `1` |

## Window.WindowInitialPosition

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `WINDOW_INITIAL_POSITION_ABSOLUTE` | `0` |
| `WINDOW_INITIAL_POSITION_CENTER_PRIMARY_SCREEN` | `1` |
| `WINDOW_INITIAL_POSITION_CENTER_MAIN_WINDOW_SCREEN` | `2` |
| `WINDOW_INITIAL_POSITION_CENTER_OTHER_SCREEN` | `3` |
| `WINDOW_INITIAL_POSITION_CENTER_SCREEN_WITH_MOUSE_FOCUS` | `4` |
| `WINDOW_INITIAL_POSITION_CENTER_SCREEN_WITH_KEYBOARD_FOCUS` | `5` |

## XMLParser.NodeType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `NODE_NONE` | `0` |
| `NODE_ELEMENT` | `1` |
| `NODE_ELEMENT_END` | `2` |
| `NODE_TEXT` | `3` |
| `NODE_COMMENT` | `4` |
| `NODE_CDATA` | `5` |
| `NODE_UNKNOWN` | `6` |

## XRBodyModifier3D.BodyUpdate

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `BODY_UPDATE_UPPER_BODY` | `1` |
| `BODY_UPDATE_LOWER_BODY` | `2` |
| `BODY_UPDATE_HANDS` | `4` |

## XRBodyModifier3D.BoneUpdate

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BONE_UPDATE_FULL` | `0` |
| `BONE_UPDATE_ROTATION_ONLY` | `1` |
| `BONE_UPDATE_MAX` | `2` |

## XRBodyTracker.BodyFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `BODY_FLAG_UPPER_BODY_SUPPORTED` | `1` |
| `BODY_FLAG_LOWER_BODY_SUPPORTED` | `2` |
| `BODY_FLAG_HANDS_SUPPORTED` | `4` |

## XRBodyTracker.Joint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `JOINT_ROOT` | `0` |
| `JOINT_HIPS` | `1` |
| `JOINT_SPINE` | `2` |
| `JOINT_CHEST` | `3` |
| `JOINT_UPPER_CHEST` | `4` |
| `JOINT_NECK` | `5` |
| `JOINT_HEAD` | `6` |
| `JOINT_HEAD_TIP` | `7` |
| `JOINT_LEFT_SHOULDER` | `8` |
| `JOINT_LEFT_UPPER_ARM` | `9` |
| `JOINT_LEFT_LOWER_ARM` | `10` |
| `JOINT_RIGHT_SHOULDER` | `11` |
| `JOINT_RIGHT_UPPER_ARM` | `12` |
| `JOINT_RIGHT_LOWER_ARM` | `13` |
| `JOINT_LEFT_UPPER_LEG` | `14` |
| `JOINT_LEFT_LOWER_LEG` | `15` |
| `JOINT_LEFT_FOOT` | `16` |
| `JOINT_LEFT_TOES` | `17` |
| `JOINT_RIGHT_UPPER_LEG` | `18` |
| `JOINT_RIGHT_LOWER_LEG` | `19` |
| `JOINT_RIGHT_FOOT` | `20` |
| `JOINT_RIGHT_TOES` | `21` |
| `JOINT_LEFT_HAND` | `22` |
| `JOINT_LEFT_PALM` | `23` |
| `JOINT_LEFT_WRIST` | `24` |
| `JOINT_LEFT_THUMB_METACARPAL` | `25` |
| `JOINT_LEFT_THUMB_PHALANX_PROXIMAL` | `26` |
| `JOINT_LEFT_THUMB_PHALANX_DISTAL` | `27` |
| `JOINT_LEFT_THUMB_TIP` | `28` |
| `JOINT_LEFT_INDEX_FINGER_METACARPAL` | `29` |
| `JOINT_LEFT_INDEX_FINGER_PHALANX_PROXIMAL` | `30` |
| `JOINT_LEFT_INDEX_FINGER_PHALANX_INTERMEDIATE` | `31` |
| `JOINT_LEFT_INDEX_FINGER_PHALANX_DISTAL` | `32` |
| `JOINT_LEFT_INDEX_FINGER_TIP` | `33` |
| `JOINT_LEFT_MIDDLE_FINGER_METACARPAL` | `34` |
| `JOINT_LEFT_MIDDLE_FINGER_PHALANX_PROXIMAL` | `35` |
| `JOINT_LEFT_MIDDLE_FINGER_PHALANX_INTERMEDIATE` | `36` |
| `JOINT_LEFT_MIDDLE_FINGER_PHALANX_DISTAL` | `37` |
| `JOINT_LEFT_MIDDLE_FINGER_TIP` | `38` |
| `JOINT_LEFT_RING_FINGER_METACARPAL` | `39` |
| `JOINT_LEFT_RING_FINGER_PHALANX_PROXIMAL` | `40` |
| `JOINT_LEFT_RING_FINGER_PHALANX_INTERMEDIATE` | `41` |
| `JOINT_LEFT_RING_FINGER_PHALANX_DISTAL` | `42` |
| `JOINT_LEFT_RING_FINGER_TIP` | `43` |
| `JOINT_LEFT_PINKY_FINGER_METACARPAL` | `44` |
| `JOINT_LEFT_PINKY_FINGER_PHALANX_PROXIMAL` | `45` |
| `JOINT_LEFT_PINKY_FINGER_PHALANX_INTERMEDIATE` | `46` |
| `JOINT_LEFT_PINKY_FINGER_PHALANX_DISTAL` | `47` |
| `JOINT_LEFT_PINKY_FINGER_TIP` | `48` |
| `JOINT_RIGHT_HAND` | `49` |
| `JOINT_RIGHT_PALM` | `50` |
| `JOINT_RIGHT_WRIST` | `51` |
| `JOINT_RIGHT_THUMB_METACARPAL` | `52` |
| `JOINT_RIGHT_THUMB_PHALANX_PROXIMAL` | `53` |
| `JOINT_RIGHT_THUMB_PHALANX_DISTAL` | `54` |
| `JOINT_RIGHT_THUMB_TIP` | `55` |
| `JOINT_RIGHT_INDEX_FINGER_METACARPAL` | `56` |
| `JOINT_RIGHT_INDEX_FINGER_PHALANX_PROXIMAL` | `57` |
| `JOINT_RIGHT_INDEX_FINGER_PHALANX_INTERMEDIATE` | `58` |
| `JOINT_RIGHT_INDEX_FINGER_PHALANX_DISTAL` | `59` |
| `JOINT_RIGHT_INDEX_FINGER_TIP` | `60` |
| `JOINT_RIGHT_MIDDLE_FINGER_METACARPAL` | `61` |
| `JOINT_RIGHT_MIDDLE_FINGER_PHALANX_PROXIMAL` | `62` |
| `JOINT_RIGHT_MIDDLE_FINGER_PHALANX_INTERMEDIATE` | `63` |
| `JOINT_RIGHT_MIDDLE_FINGER_PHALANX_DISTAL` | `64` |
| `JOINT_RIGHT_MIDDLE_FINGER_TIP` | `65` |
| `JOINT_RIGHT_RING_FINGER_METACARPAL` | `66` |
| `JOINT_RIGHT_RING_FINGER_PHALANX_PROXIMAL` | `67` |
| `JOINT_RIGHT_RING_FINGER_PHALANX_INTERMEDIATE` | `68` |
| `JOINT_RIGHT_RING_FINGER_PHALANX_DISTAL` | `69` |
| `JOINT_RIGHT_RING_FINGER_TIP` | `70` |
| `JOINT_RIGHT_PINKY_FINGER_METACARPAL` | `71` |
| `JOINT_RIGHT_PINKY_FINGER_PHALANX_PROXIMAL` | `72` |
| `JOINT_RIGHT_PINKY_FINGER_PHALANX_INTERMEDIATE` | `73` |
| `JOINT_RIGHT_PINKY_FINGER_PHALANX_DISTAL` | `74` |
| `JOINT_RIGHT_PINKY_FINGER_TIP` | `75` |
| `JOINT_LOWER_CHEST` | `76` |
| `JOINT_LEFT_SCAPULA` | `77` |
| `JOINT_LEFT_WRIST_TWIST` | `78` |
| `JOINT_RIGHT_SCAPULA` | `79` |
| `JOINT_RIGHT_WRIST_TWIST` | `80` |
| `JOINT_LEFT_FOOT_TWIST` | `81` |
| `JOINT_LEFT_HEEL` | `82` |
| `JOINT_LEFT_MIDDLE_FOOT` | `83` |
| `JOINT_RIGHT_FOOT_TWIST` | `84` |
| `JOINT_RIGHT_HEEL` | `85` |
| `JOINT_RIGHT_MIDDLE_FOOT` | `86` |
| `JOINT_MAX` | `87` |

## XRBodyTracker.JointFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `JOINT_FLAG_ORIENTATION_VALID` | `1` |
| `JOINT_FLAG_ORIENTATION_TRACKED` | `2` |
| `JOINT_FLAG_POSITION_VALID` | `4` |
| `JOINT_FLAG_POSITION_TRACKED` | `8` |

## XRFaceTracker.BlendShapeEntry

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `FT_EYE_LOOK_OUT_RIGHT` | `0` |
| `FT_EYE_LOOK_IN_RIGHT` | `1` |
| `FT_EYE_LOOK_UP_RIGHT` | `2` |
| `FT_EYE_LOOK_DOWN_RIGHT` | `3` |
| `FT_EYE_LOOK_OUT_LEFT` | `4` |
| `FT_EYE_LOOK_IN_LEFT` | `5` |
| `FT_EYE_LOOK_UP_LEFT` | `6` |
| `FT_EYE_LOOK_DOWN_LEFT` | `7` |
| `FT_EYE_CLOSED_RIGHT` | `8` |
| `FT_EYE_CLOSED_LEFT` | `9` |
| `FT_EYE_SQUINT_RIGHT` | `10` |
| `FT_EYE_SQUINT_LEFT` | `11` |
| `FT_EYE_WIDE_RIGHT` | `12` |
| `FT_EYE_WIDE_LEFT` | `13` |
| `FT_EYE_DILATION_RIGHT` | `14` |
| `FT_EYE_DILATION_LEFT` | `15` |
| `FT_EYE_CONSTRICT_RIGHT` | `16` |
| `FT_EYE_CONSTRICT_LEFT` | `17` |
| `FT_BROW_PINCH_RIGHT` | `18` |
| `FT_BROW_PINCH_LEFT` | `19` |
| `FT_BROW_LOWERER_RIGHT` | `20` |
| `FT_BROW_LOWERER_LEFT` | `21` |
| `FT_BROW_INNER_UP_RIGHT` | `22` |
| `FT_BROW_INNER_UP_LEFT` | `23` |
| `FT_BROW_OUTER_UP_RIGHT` | `24` |
| `FT_BROW_OUTER_UP_LEFT` | `25` |
| `FT_NOSE_SNEER_RIGHT` | `26` |
| `FT_NOSE_SNEER_LEFT` | `27` |
| `FT_NASAL_DILATION_RIGHT` | `28` |
| `FT_NASAL_DILATION_LEFT` | `29` |
| `FT_NASAL_CONSTRICT_RIGHT` | `30` |
| `FT_NASAL_CONSTRICT_LEFT` | `31` |
| `FT_CHEEK_SQUINT_RIGHT` | `32` |
| `FT_CHEEK_SQUINT_LEFT` | `33` |
| `FT_CHEEK_PUFF_RIGHT` | `34` |
| `FT_CHEEK_PUFF_LEFT` | `35` |
| `FT_CHEEK_SUCK_RIGHT` | `36` |
| `FT_CHEEK_SUCK_LEFT` | `37` |
| `FT_JAW_OPEN` | `38` |
| `FT_MOUTH_CLOSED` | `39` |
| `FT_JAW_RIGHT` | `40` |
| `FT_JAW_LEFT` | `41` |
| `FT_JAW_FORWARD` | `42` |
| `FT_JAW_BACKWARD` | `43` |
| `FT_JAW_CLENCH` | `44` |
| `FT_JAW_MANDIBLE_RAISE` | `45` |
| `FT_LIP_SUCK_UPPER_RIGHT` | `46` |
| `FT_LIP_SUCK_UPPER_LEFT` | `47` |
| `FT_LIP_SUCK_LOWER_RIGHT` | `48` |
| `FT_LIP_SUCK_LOWER_LEFT` | `49` |
| `FT_LIP_SUCK_CORNER_RIGHT` | `50` |
| `FT_LIP_SUCK_CORNER_LEFT` | `51` |
| `FT_LIP_FUNNEL_UPPER_RIGHT` | `52` |
| `FT_LIP_FUNNEL_UPPER_LEFT` | `53` |
| `FT_LIP_FUNNEL_LOWER_RIGHT` | `54` |
| `FT_LIP_FUNNEL_LOWER_LEFT` | `55` |
| `FT_LIP_PUCKER_UPPER_RIGHT` | `56` |
| `FT_LIP_PUCKER_UPPER_LEFT` | `57` |
| `FT_LIP_PUCKER_LOWER_RIGHT` | `58` |
| `FT_LIP_PUCKER_LOWER_LEFT` | `59` |
| `FT_MOUTH_UPPER_UP_RIGHT` | `60` |
| `FT_MOUTH_UPPER_UP_LEFT` | `61` |
| `FT_MOUTH_LOWER_DOWN_RIGHT` | `62` |
| `FT_MOUTH_LOWER_DOWN_LEFT` | `63` |
| `FT_MOUTH_UPPER_DEEPEN_RIGHT` | `64` |
| `FT_MOUTH_UPPER_DEEPEN_LEFT` | `65` |
| `FT_MOUTH_UPPER_RIGHT` | `66` |
| `FT_MOUTH_UPPER_LEFT` | `67` |
| `FT_MOUTH_LOWER_RIGHT` | `68` |
| `FT_MOUTH_LOWER_LEFT` | `69` |
| `FT_MOUTH_CORNER_PULL_RIGHT` | `70` |
| `FT_MOUTH_CORNER_PULL_LEFT` | `71` |
| `FT_MOUTH_CORNER_SLANT_RIGHT` | `72` |
| `FT_MOUTH_CORNER_SLANT_LEFT` | `73` |
| `FT_MOUTH_FROWN_RIGHT` | `74` |
| `FT_MOUTH_FROWN_LEFT` | `75` |
| `FT_MOUTH_STRETCH_RIGHT` | `76` |
| `FT_MOUTH_STRETCH_LEFT` | `77` |
| `FT_MOUTH_DIMPLE_RIGHT` | `78` |
| `FT_MOUTH_DIMPLE_LEFT` | `79` |
| `FT_MOUTH_RAISER_UPPER` | `80` |
| `FT_MOUTH_RAISER_LOWER` | `81` |
| `FT_MOUTH_PRESS_RIGHT` | `82` |
| `FT_MOUTH_PRESS_LEFT` | `83` |
| `FT_MOUTH_TIGHTENER_RIGHT` | `84` |
| `FT_MOUTH_TIGHTENER_LEFT` | `85` |
| `FT_TONGUE_OUT` | `86` |
| `FT_TONGUE_UP` | `87` |
| `FT_TONGUE_DOWN` | `88` |
| `FT_TONGUE_RIGHT` | `89` |
| `FT_TONGUE_LEFT` | `90` |
| `FT_TONGUE_ROLL` | `91` |
| `FT_TONGUE_BLEND_DOWN` | `92` |
| `FT_TONGUE_CURL_UP` | `93` |
| `FT_TONGUE_SQUISH` | `94` |
| `FT_TONGUE_FLAT` | `95` |
| `FT_TONGUE_TWIST_RIGHT` | `96` |
| `FT_TONGUE_TWIST_LEFT` | `97` |
| `FT_SOFT_PALATE_CLOSE` | `98` |
| `FT_THROAT_SWALLOW` | `99` |
| `FT_NECK_FLEX_RIGHT` | `100` |
| `FT_NECK_FLEX_LEFT` | `101` |
| `FT_EYE_CLOSED` | `102` |
| `FT_EYE_WIDE` | `103` |
| `FT_EYE_SQUINT` | `104` |
| `FT_EYE_DILATION` | `105` |
| `FT_EYE_CONSTRICT` | `106` |
| `FT_BROW_DOWN_RIGHT` | `107` |
| `FT_BROW_DOWN_LEFT` | `108` |
| `FT_BROW_DOWN` | `109` |
| `FT_BROW_UP_RIGHT` | `110` |
| `FT_BROW_UP_LEFT` | `111` |
| `FT_BROW_UP` | `112` |
| `FT_NOSE_SNEER` | `113` |
| `FT_NASAL_DILATION` | `114` |
| `FT_NASAL_CONSTRICT` | `115` |
| `FT_CHEEK_PUFF` | `116` |
| `FT_CHEEK_SUCK` | `117` |
| `FT_CHEEK_SQUINT` | `118` |
| `FT_LIP_SUCK_UPPER` | `119` |
| `FT_LIP_SUCK_LOWER` | `120` |
| `FT_LIP_SUCK` | `121` |
| `FT_LIP_FUNNEL_UPPER` | `122` |
| `FT_LIP_FUNNEL_LOWER` | `123` |
| `FT_LIP_FUNNEL` | `124` |
| `FT_LIP_PUCKER_UPPER` | `125` |
| `FT_LIP_PUCKER_LOWER` | `126` |
| `FT_LIP_PUCKER` | `127` |
| `FT_MOUTH_UPPER_UP` | `128` |
| `FT_MOUTH_LOWER_DOWN` | `129` |
| `FT_MOUTH_OPEN` | `130` |
| `FT_MOUTH_RIGHT` | `131` |
| `FT_MOUTH_LEFT` | `132` |
| `FT_MOUTH_SMILE_RIGHT` | `133` |
| `FT_MOUTH_SMILE_LEFT` | `134` |
| `FT_MOUTH_SMILE` | `135` |
| `FT_MOUTH_SAD_RIGHT` | `136` |
| `FT_MOUTH_SAD_LEFT` | `137` |
| `FT_MOUTH_SAD` | `138` |
| `FT_MOUTH_STRETCH` | `139` |
| `FT_MOUTH_DIMPLE` | `140` |
| `FT_MOUTH_TIGHTENER` | `141` |
| `FT_MOUTH_PRESS` | `142` |
| `FT_MAX` | `143` |

## XRHandModifier3D.BoneUpdate

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `BONE_UPDATE_FULL` | `0` |
| `BONE_UPDATE_ROTATION_ONLY` | `1` |
| `BONE_UPDATE_MAX` | `2` |

## XRHandTracker.HandTrackingSource

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_TRACKING_SOURCE_UNKNOWN` | `0` |
| `HAND_TRACKING_SOURCE_UNOBSTRUCTED` | `1` |
| `HAND_TRACKING_SOURCE_CONTROLLER` | `2` |
| `HAND_TRACKING_SOURCE_NOT_TRACKED` | `3` |
| `HAND_TRACKING_SOURCE_MAX` | `4` |

## XRHandTracker.HandJoint

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `HAND_JOINT_PALM` | `0` |
| `HAND_JOINT_WRIST` | `1` |
| `HAND_JOINT_THUMB_METACARPAL` | `2` |
| `HAND_JOINT_THUMB_PHALANX_PROXIMAL` | `3` |
| `HAND_JOINT_THUMB_PHALANX_DISTAL` | `4` |
| `HAND_JOINT_THUMB_TIP` | `5` |
| `HAND_JOINT_INDEX_FINGER_METACARPAL` | `6` |
| `HAND_JOINT_INDEX_FINGER_PHALANX_PROXIMAL` | `7` |
| `HAND_JOINT_INDEX_FINGER_PHALANX_INTERMEDIATE` | `8` |
| `HAND_JOINT_INDEX_FINGER_PHALANX_DISTAL` | `9` |
| `HAND_JOINT_INDEX_FINGER_TIP` | `10` |
| `HAND_JOINT_MIDDLE_FINGER_METACARPAL` | `11` |
| `HAND_JOINT_MIDDLE_FINGER_PHALANX_PROXIMAL` | `12` |
| `HAND_JOINT_MIDDLE_FINGER_PHALANX_INTERMEDIATE` | `13` |
| `HAND_JOINT_MIDDLE_FINGER_PHALANX_DISTAL` | `14` |
| `HAND_JOINT_MIDDLE_FINGER_TIP` | `15` |
| `HAND_JOINT_RING_FINGER_METACARPAL` | `16` |
| `HAND_JOINT_RING_FINGER_PHALANX_PROXIMAL` | `17` |
| `HAND_JOINT_RING_FINGER_PHALANX_INTERMEDIATE` | `18` |
| `HAND_JOINT_RING_FINGER_PHALANX_DISTAL` | `19` |
| `HAND_JOINT_RING_FINGER_TIP` | `20` |
| `HAND_JOINT_PINKY_FINGER_METACARPAL` | `21` |
| `HAND_JOINT_PINKY_FINGER_PHALANX_PROXIMAL` | `22` |
| `HAND_JOINT_PINKY_FINGER_PHALANX_INTERMEDIATE` | `23` |
| `HAND_JOINT_PINKY_FINGER_PHALANX_DISTAL` | `24` |
| `HAND_JOINT_PINKY_FINGER_TIP` | `25` |
| `HAND_JOINT_MAX` | `26` |

## XRHandTracker.HandJointFlags

Bitfield group. Combine numeric `.Value` values with `bit32`.

| Native constant | Value |
| --- | --- |
| `HAND_JOINT_FLAG_ORIENTATION_VALID` | `1` |
| `HAND_JOINT_FLAG_ORIENTATION_TRACKED` | `2` |
| `HAND_JOINT_FLAG_POSITION_VALID` | `4` |
| `HAND_JOINT_FLAG_POSITION_TRACKED` | `8` |
| `HAND_JOINT_FLAG_LINEAR_VELOCITY_VALID` | `16` |
| `HAND_JOINT_FLAG_ANGULAR_VELOCITY_VALID` | `32` |

## XRInterface.Capabilities

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `XR_NONE` | `0` |
| `XR_MONO` | `1` |
| `XR_STEREO` | `2` |
| `XR_QUAD` | `4` |
| `XR_VR` | `8` |
| `XR_AR` | `16` |
| `XR_EXTERNAL` | `32` |

## XRInterface.TrackingStatus

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `XR_NORMAL_TRACKING` | `0` |
| `XR_EXCESSIVE_MOTION` | `1` |
| `XR_INSUFFICIENT_FEATURES` | `2` |
| `XR_UNKNOWN_TRACKING` | `3` |
| `XR_NOT_TRACKING` | `4` |

## XRInterface.PlayAreaMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `XR_PLAY_AREA_UNKNOWN` | `0` |
| `XR_PLAY_AREA_3DOF` | `1` |
| `XR_PLAY_AREA_SITTING` | `2` |
| `XR_PLAY_AREA_ROOMSCALE` | `3` |
| `XR_PLAY_AREA_STAGE` | `4` |
| `XR_PLAY_AREA_CUSTOM` | `2147483647` |

## XRInterface.EnvironmentBlendMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `XR_ENV_BLEND_MODE_OPAQUE` | `0` |
| `XR_ENV_BLEND_MODE_ADDITIVE` | `1` |
| `XR_ENV_BLEND_MODE_ALPHA_BLEND` | `2` |

## XRInterface.VRSTextureFormat

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `XR_VRS_TEXTURE_FORMAT_UNIFIED` | `0` |
| `XR_VRS_TEXTURE_FORMAT_FRAGMENT_SHADING_RATE` | `1` |
| `XR_VRS_TEXTURE_FORMAT_FRAGMENT_DENSITY_MAP` | `2` |

## XRPose.TrackingConfidence

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `XR_TRACKING_CONFIDENCE_NONE` | `0` |
| `XR_TRACKING_CONFIDENCE_LOW` | `1` |
| `XR_TRACKING_CONFIDENCE_HIGH` | `2` |

## XRPositionalTracker.TrackerHand

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRACKER_HAND_UNKNOWN` | `0` |
| `TRACKER_HAND_LEFT` | `1` |
| `TRACKER_HAND_RIGHT` | `2` |
| `TRACKER_HAND_MAX` | `3` |

## XRServer.TrackerType

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `TRACKER_HEAD` | `1` |
| `TRACKER_CONTROLLER` | `2` |
| `TRACKER_BASESTATION` | `4` |
| `TRACKER_ANCHOR` | `8` |
| `TRACKER_HAND` | `16` |
| `TRACKER_BODY` | `32` |
| `TRACKER_FACE` | `64` |
| `TRACKER_ANY_KNOWN` | `127` |
| `TRACKER_UNKNOWN` | `128` |
| `TRACKER_ANY` | `255` |

## XRServer.RotationMode

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `RESET_FULL_ROTATION` | `0` |
| `RESET_BUT_KEEP_TILT` | `1` |
| `DONT_RESET_ROTATION` | `2` |

## ZIPPacker.ZipAppend

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `APPEND_CREATE` | `0` |
| `APPEND_CREATEAFTER` | `1` |
| `APPEND_ADDINZIP` | `2` |

## ZIPPacker.CompressionLevel

Non-bitfield enum group.

| Native constant | Value |
| --- | --- |
| `COMPRESSION_DEFAULT` | `-1` |
| `COMPRESSION_NONE` | `0` |
| `COMPRESSION_FAST` | `1` |
| `COMPRESSION_BEST` | `9` |
