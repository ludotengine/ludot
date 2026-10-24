#!/usr/bin/env python3
"""Regenerate Ludot's desktop artwork from logo.svg. Requires rsvg-convert and Pillow."""
from pathlib import Path
from io import BytesIO
import re
import subprocess
from PIL import Image

root = Path(__file__).resolve().parents[1]
source = (root / 'logo.svg').read_text()

def svg(size):
    return source.replace('width="128" height="128"', f'width="{size}" height="{size}"')

def png(size):
    return subprocess.check_output(['rsvg-convert', '-w', str(size), '-h', str(size), str(root / 'logo.svg')])

for name, size in {
    'engine/icon.svg': 128,
    'engine/editor/icons/Godot.svg': 64,
    'engine/editor/icons/GodotMonochrome.svg': 64,
    'engine/editor/icons/GodotFile.svg': 64,
    'engine/editor/icons/DefaultProjectIcon.svg': 128,
    'engine/editor/icons/Logo.svg': 69,
    'engine/editor/icons/TitleBarLogo.svg': 24,
    'engine/misc/dist/windows/icon_console.svg': 128,
    'engine/misc/dist/html/logo.svg': 128,
}.items():
    (root / name).write_text(svg(size))
for name in ['icon', 'icon_outlined', 'logo', 'logo_outlined']:
    (root / f'engine/misc/logo/{name}.svg').write_text(source)
    (root / f'engine/misc/logo/{name}.png').write_bytes(png(256))
for name in ['engine/icon.png', 'engine/main/app_icon.png']:
    (root / name).write_bytes(png(128))
mark = Image.open(BytesIO(png(256)))
for name in ['splash', 'splash_editor']:
    canvas = Image.new('RGBA', (512, 256))
    canvas.alpha_composite(mark, (128, 0))
    canvas.save(root / f'engine/main/{name}.png')
large = Image.open(BytesIO(png(1024)))
for name in ['godot.ico', 'godot_console.ico']:
    large.save(root / 'engine/platform/windows' / name, format='ICO', sizes=[(n,n) for n in [16,24,32,48,64,128,256]])
resources = root / 'engine/misc/dist/macos_tools.app/Contents/Resources'
large.save(resources / 'GodotLG.icns', format='ICNS')
# Every macOS document icon previously contained the original Godot robot.
colors = {'project':'#5071ea', 'scene':'#a5efac', 'resource':'#e0b6ff', 'shader':'#ffca5f', 'gdscript':'#9dd6ff'}
for kind, color in colors.items():
    for suffix, size in [('',128), ('_small',32), ('_extra_small',16)]:
        document = '<!-- Ludot document icon. Godot robot derivative: Andrea Calabro, CC BY 4.0. Luau gradient: Roblox Corporation, MIT. See logo.svg. -->\n' + f'''<svg xmlns="http://www.w3.org/2000/svg" width="{size}" height="{size}" viewBox="0 0 128 128"><path fill="{color}" d="M16 4h64l32 32v88H16z"/><path fill="#fff" opacity=".5" d="M80 4v32h32z"/><g transform="translate(20 38) scale(.66)">{re.search(r'<defs>.*</g>', source, re.S)[0]}</g></svg>'''
        (root / f'engine/misc/dist/document_icons/{kind}{suffix}.svg').write_text(document)
    rendered = subprocess.check_output(['rsvg-convert', '-w','1024','-h','1024', str(root / f'engine/misc/dist/document_icons/{kind}.svg')])
    icon_name = {'project':'Project', 'scene':'Scene', 'resource':'Resource', 'shader':'Shader', 'gdscript':'GDScript'}[kind]
    Image.open(BytesIO(rendered)).save(resources / (icon_name + '.icns'), format='ICNS')
# The old precompiled asset catalog could override the supplied application icon.
(resources / 'Assets.car').unlink(missing_ok=True)
assets = root / 'engine/misc/dist/macos/GodotLG.icon/Assets'
for name in ['layer_0.svg', 'layer_1.svg', 'layer_2.svg']:
    (assets / name).write_text(source if name == 'layer_0.svg' else '<svg xmlns="http://www.w3.org/2000/svg" width="128" height="128"/>\n')
