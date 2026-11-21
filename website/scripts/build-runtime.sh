#!/usr/bin/env bash
set -euo pipefail
# Run with emsdk_env.sh loaded. This builds the committed fork, never upstream Luau.
site_dir="$(cd "$(dirname "$0")/.." && pwd)"
repo_dir="$(dirname "$site_dir")"
emcmake cmake -S "$site_dir/runtime" -B "$repo_dir/build/docs-runtime" -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build "$repo_dir/build/docs-runtime" --target Luau.Web -j "${DOCS_BUILD_JOBS:-4}"
mkdir -p "$site_dir/public/runtime"
cp "$repo_dir/build/docs-runtime/luau/ludot.mjs" "$site_dir/public/runtime/ludot.mjs"
