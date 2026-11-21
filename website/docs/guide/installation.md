# Install Ludot

Run the command for your system. Run it again later to update.

<InstallOptions>

<div id="install-panel-0" class="install-panel" role="tabpanel" aria-labelledby="install-tab-0">

```bash
curl -fsSL https://github.com/ludotengine/ludot/releases/latest/download/install.sh | sh
```

- **Linux x86_64:** installs to `~/.local/share/ludot`, with the `ludot` command in `~/.local/bin`.
- **macOS Apple Silicon:** installs to `~/Applications/Ludot.app`.

</div>

<div id="install-panel-1" class="install-panel" role="tabpanel" aria-labelledby="install-tab-1">

```powershell
irm https://github.com/ludotengine/ludot/releases/latest/download/install.ps1 | iex
```

For Windows x86_64. Open a new terminal afterwards to use the `ludot` command.

</div>

</InstallOptions>

The scripts verify package checksums. Ludot is experimental, so back up your projects before updating.

## Windows EXE

<ReleaseDownload suffix="-windows-x86_64-setup.exe">Download EXE</ReleaseDownload>

Installs for the current user and can add Ludot to your PATH. The installer isn't signed, so Windows may warn you before running it.

## macOS DMG

<ReleaseDownload suffix="-macos-arm64.dmg">Download DMG</ReleaseDownload>

For Apple Silicon. Open the image and drag Ludot to Applications. The app isn't signed or notarized, so macOS may ask you to confirm the first launch.

## Other options

[Portable archives](https://github.com/ludotengine/ludot/releases/latest) · [Build from source](../reference/building.md)

Once it's installed, continue with [your first project](./first-project.md).
