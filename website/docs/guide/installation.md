# Install Ludot

Run the command again to update to the latest release.

<InstallOptions>

<div id="install-panel-0" class="install-panel" role="tabpanel" aria-labelledby="install-tab-0">

```bash
curl -fsSL https://github.com/ludotengine/ludot/releases/latest/download/install.sh | sh
```

Linux x86_64. Installs to `~/.local/share/ludot`; command at `~/.local/bin/ludot`.

</div>

<div id="install-panel-1" class="install-panel" role="tabpanel" aria-labelledby="install-tab-1">

```bash
curl -fsSL https://github.com/ludotengine/ludot/releases/latest/download/install.sh | sh
```

macOS Apple Silicon. Installs to `~/Applications/Ludot.app`.

</div>

<div id="install-panel-2" class="install-panel" role="tabpanel" aria-labelledby="install-tab-2">

```powershell
irm https://github.com/ludotengine/ludot/releases/latest/download/install.ps1 | iex
```

Windows x86_64. Open a new terminal after installation to use `ludot`.

</div>

</InstallOptions>

The scripts verify package checksums. Ludot is experimental; back up projects before updating.

## Windows EXE

<ReleaseDownload suffix="-windows-x86_64-setup.exe">Download EXE</ReleaseDownload>

Installs for the current user and can add Ludot to PATH. The installer is not signed.

## macOS DMG

<ReleaseDownload suffix="-macos-arm64.dmg">Download DMG</ReleaseDownload>

Apple Silicon. Open the image and drag Ludot to Applications. The app is not signed or notarized.

[Portable archives](https://github.com/ludotengine/ludot/releases/latest) · [Build from source](../reference/building.md) · [Your first project](./first-project.md)
