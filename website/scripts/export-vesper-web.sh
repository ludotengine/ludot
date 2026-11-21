#!/usr/bin/env bash
# Exports demos/vesper for the landing page into website/public/vesper.
# Needs a Ludot editor binary (LUDOT, default: ludot on PATH) and the web template
# from docs/building.md in engine/bin.
set -euo pipefail
repo=$(cd "$(dirname "$0")/../.." && pwd)
ludot=${LUDOT:-ludot}
template="$repo/engine/bin/godot.web.template_release.wasm32.nothreads.zip"
out="$repo/website/public/vesper"
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

cp -r "$repo/demos/vesper" "$work/vesper"
rm -rf "$work/vesper/.godot"
cat > "$work/vesper/export_presets.cfg" <<PRESET
[preset.0]

name="Web"
platform="Web"
runnable=true
export_filter="all_resources"
include_filter=""
exclude_filter=""
export_path=""

[preset.0.options]

custom_template/debug=""
custom_template/release="$template"
variant/extensions_support=false
variant/thread_support=false
vram_texture_compression/for_desktop=true
vram_texture_compression/for_mobile=false
html/export_icon=false
html/custom_html_shell=""
html/head_include="<style>html, body { background: #000; }</style>"
html/canvas_resize_policy=2
html/focus_canvas_on_start=false
html/experimental_virtual_keyboard=false
progressive_web_app/enabled=false
PRESET

# Clear the folder's contents but keep the folder, so a running dev server's file
# watcher keeps serving it.
mkdir -p "$out"
find "$out" -mindepth 1 -delete
"$ludot" --headless --path "$work/vesper" --import
# The page is play.htm because VitePress's dev server routes every .html request
# to its own pages. Godot also writes a play.html copy of the shell; drop it.
"$ludot" --headless --path "$work/vesper" --export-release Web "$out/play.htm"
rm -f "$out/play.html"
