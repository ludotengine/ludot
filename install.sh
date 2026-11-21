#!/bin/sh
# Install or update the latest published Ludot release for the current user.
set -eu
repository=https://github.com/ludotengine/ludot
case "$(uname -s):$(uname -m)" in
	Linux:x86_64) platform=linux; architecture=x86_64; extension=tar.gz ;;
	Darwin:arm64) platform=macos; architecture=arm64; extension=zip ;;
	*) printf '%s\n' 'Supported packages: Linux x86_64 and macOS Apple Silicon.' >&2; exit 1 ;;
esac
for tool in curl tar awk; do
	command -v "$tool" >/dev/null 2>&1 || { printf 'Required command: %s\n' "$tool" >&2; exit 1; }
done
release_url=$(curl --fail --silent --show-error --location --output /dev/null --write-out '%{url_effective}' "$repository/releases/latest")
tag=${release_url##*/}
case "$tag" in v[0-9]*) ;; *) printf '%s\n' 'No published Ludot release is available.' >&2; exit 1 ;; esac
case "$tag" in *[!A-Za-z0-9._-]*) printf '%s\n' 'Invalid release tag.' >&2; exit 1 ;; esac
version=${tag#v}
asset=ludot-$version-$platform-$architecture
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM
curl --fail --silent --show-error --location "$repository/releases/download/$tag/$asset.$extension" --output "$work/$asset.$extension"
curl --fail --silent --show-error --location "$repository/releases/download/$tag/$asset.sha256" --output "$work/checksums"
awk -v name="$asset.$extension" '$2 == name { print; found = 1 } END { if (!found) exit 1 }' "$work/checksums" > "$work/selected.sha256"
if [ "$platform" = macos ]; then
	(cd "$work" && shasum -a 256 --check selected.sha256)
	ditto -x -k "$work/$asset.zip" "$work/unpack"
	source_directory="$work/unpack/$asset/Ludot.app"
	parent="$HOME/Applications"
	target="$parent/Ludot.app"
else
	(cd "$work" && sha256sum --check selected.sha256)
	mkdir -p "$work/unpack"
	tar -xzf "$work/$asset.tar.gz" -C "$work/unpack"
	source_directory="$work/unpack/$asset"
	parent="${XDG_DATA_HOME:-$HOME/.local/share}"
	target="$parent/ludot"
fi
mkdir -p "$parent" "$HOME/.local/bin"
staged="$target.new.$$"
backup="$target.previous.$$"
if [ "$platform" = macos ]; then ditto "$source_directory" "$staged"; else cp -R "$source_directory" "$staged"; fi
if [ -e "$target" ]; then mv "$target" "$backup"; fi
if ! mv "$staged" "$target"; then
	if [ -e "$backup" ]; then mv "$backup" "$target"; fi
	exit 1
fi
rm -rf "$backup"
if [ "$platform" = macos ]; then
	executable=$(/usr/libexec/PlistBuddy -c 'Print CFBundleExecutable' "$target/Contents/Info.plist")
	ln -sfn "$target/Contents/MacOS/$executable" "$HOME/.local/bin/ludot"
else
	chmod +x "$target/ludot"
	ln -sfn "$target/ludot" "$HOME/.local/bin/ludot"
	mkdir -p "$parent/applications"
	cat > "$parent/applications/org.ludotengine.Ludot.desktop" <<DESKTOP
[Desktop Entry]
Type=Application
Name=Ludot
Comment=Experimental game engine with Luau scripting
Exec="$target/ludot" %f
Icon=$target/ludot.svg
Terminal=false
Categories=Development;IDE;
DESKTOP
fi
printf 'Installed Ludot %s to %s\n' "$version" "$target"
printf 'Run %s/.local/bin/ludot, or use ludot if ~/.local/bin is on PATH.\n' "$HOME"
