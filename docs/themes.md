# Editor themes

Ludot's editor ships with five color themes. Pick one under Editor Settings >
Themes:

- Kanagawa
- Catppuccin Mocha
- Nord
- Everforest Dark
- Gruvbox Dark

A theme sets the editor window's base and accent colors and every script
highlighting color.

## Adjusting colors

Change individual colors under:

- General > Text Editor > Theme > Highlighting
- General > Interface > Theme

Turn on Advanced Settings to see the full list.

## Saving themes

Enter a name and choose **Save Theme** to keep a local copy. **Delete Saved Theme**
removes it. Saved themes persist across editor launches.

## Sharing themes

**Copy Current Theme Code** puts the current colors on the clipboard as a code. To
import one, paste it into the text box and choose **Apply Theme Code**. Save it under
a name if you want to keep it.

A theme code starts with `LDT1:`, followed by DEFLATE-compressed JSON encoded as
Base64. It holds color values and some appearance settings, never scripts, fonts,
paths, or executable objects. Codes are validated before any setting changes.

## Palette credits

These themes are Ludot's adaptations of the original palettes, not ports of those
editors' theme plugins. The palettes keep their original names and attribution.

- Kanagawa: https://github.com/rebelot/kanagawa.nvim
- Catppuccin: https://github.com/catppuccin/catppuccin
- Nord: https://github.com/nordtheme/nord
- Everforest: https://github.com/sainnhe/everforest
- Gruvbox: https://github.com/morhetz/gruvbox
