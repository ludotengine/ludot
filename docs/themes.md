# Editor themes

Editor Settings > Themes provides Kanagawa, Catppuccin Mocha, Nord, Everforest
Dark, and Gruvbox Dark adaptations. These apply editor window base and accent
colors as well as every script highlighting color. Adjust individual colors under
General > Text Editor > Theme > Highlighting and General > Interface > Theme.
Advanced Settings exposes the full settings list.

Enter a new name and choose Save Theme to keep a local copy. Delete Saved Theme
removes that named copy. Copy Current Theme Code exports the current colors to
the clipboard. Paste a code into the text box and choose Apply Theme Code to
import it, then save it under a name if desired. Codes start with `LDT1:` and use
DEFLATE-compressed JSON encoded as Base64. They include color values and selected
appearance settings, but no scripts, fonts, paths, or executable objects. Imports
are validated before settings change. Settings persist across editor launches.

Palette references:

- Kanagawa: https://github.com/rebelot/kanagawa.nvim
- Catppuccin: https://github.com/catppuccin/catppuccin
- Nord: https://github.com/nordtheme/nord
- Everforest: https://github.com/sainnhe/everforest
- Gruvbox: https://github.com/morhetz/gruvbox

These are Ludot palette adaptations, rather than ports of those editors' theme
plugins. Theme authors retain their original names and attribution.
