<p align="center">
  <a href="https://ludotengine.github.io/ludot/">
    <img src="logo.svg" width="88" alt="Ludot">
  </a>
</p>

<h1 align="center">Ludot</h1>

<p align="center">A game engine with native Luau scripting.</p>

<p align="center">
  <a href="https://github.com/ludotengine/ludot/actions/workflows/desktop.yml"><img src="https://github.com/ludotengine/ludot/actions/workflows/desktop.yml/badge.svg" alt="Desktop builds"></a>
  <a href="https://github.com/ludotengine/ludot/releases/latest"><img src="https://img.shields.io/github/v/release/ludotengine/ludot?color=438de3&label=release" alt="Latest release"></a>
  <a href="https://ludotengine.github.io/ludot/docs/reference/status.html"><img src="https://img.shields.io/badge/engine-experimental-438de3" alt="Experimental engine"></a>
</p>

<p align="center">
  <a href="https://ludotengine.github.io/ludot/"><img src="website/public/home/vesper-desert.webp" width="960" alt="The Vesper demo running in Ludot"></a>
</p>

<table>
  <tr>
    <td width="33%" valign="top">
      <h3><a href="https://ludotengine.github.io/ludot/">Website</a></h3>
      <p>Explore Ludot and the live Vesper demo.</p>
    </td>
    <td width="33%" valign="top">
      <h3><a href="https://ludotengine.github.io/ludot/docs/">Documentation</a></h3>
      <p>Guides, API reference, and an editable playground.</p>
    </td>
    <td width="33%" valign="top">
      <h3><a href="https://github.com/ludotengine/ludot/releases/latest">Downloads</a></h3>
      <p>Linux, macOS, and Windows releases.</p>
    </td>
  </tr>
</table>

Ludot pairs Godot's scene system with a native Luau fork. Scripts support typed
brace classes, imports, enums, signals, and hot reload. `.lushader` files compile a
typed Luau subset to engine shaders. The editor includes checking, completion,
debugging, and profiling; a standalone LSP supports other editors. GDScript remains
available.

> The engine is experimental. Expect breaking changes and keep project backups
> before updating.

## Install or update

**Linux x86_64 · macOS Apple Silicon**

```sh
curl -fsSL https://github.com/ludotengine/ludot/releases/latest/download/install.sh | sh
```

**Windows x86_64 · PowerShell**

```powershell
irm https://github.com/ludotengine/ludot/releases/latest/download/install.ps1 | iex
```

Run the same command to update. Prefer an installer? Get the [Windows EXE or macOS
DMG](https://github.com/ludotengine/ludot/releases/latest).
[Installation details](https://ludotengine.github.io/ludot/docs/guide/installation.html).

## Start building

- [Your first project](https://ludotengine.github.io/ludot/docs/guide/first-project.html)
- [Luau language guide](https://ludotengine.github.io/ludot/docs/luau/)
- [Engine API reference](https://ludotengine.github.io/ludot/docs/api/)
- [Build from source](docs/building.md)
- [Contribute](https://ludotengine.github.io/ludot/docs/guide/contributing.html)

## Source and licenses

Ludot contains independent Godot and Luau forks, committed directly in this
repository. [Upstream provenance](upstreams.json) records their original versions.
Original copyright and dependency notices remain in their source trees.

[Ludot license](LICENSE) · [Godot license](engine/LICENSE.txt) ·
[Luau license](luau/LICENSE.txt) · [Logo attribution](logo.svg)
