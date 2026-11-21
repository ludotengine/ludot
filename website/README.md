# Ludot documentation site

Custom VitePress site for GitHub Pages. The landing page is at `/ludot/`, the
manual at `/ludot/docs/`, and the API reference at `/ludot/docs/api/`. The site
follows the latest source and has no version picker.

## Local development

Use Node 22 or newer and Python 3.12 or newer:

```bash
cd website
npm ci
npm run dev
```

Open the Local URL printed by VitePress. `npm run dev` generates the reference
before starting the server. Re-run `npm run docs:generate` after changing the
source manuals, engine manifest, or class XML. Site-only Markdown and theme
components reload automatically.

The playground needs a browser runtime build. Install Emscripten 4.0.23 using the
official [emsdk instructions](https://emscripten.org/docs/getting_started/downloads.html),
load `emsdk_env.sh`, and run this from the repository root:

```bash
bash website/scripts/build-runtime.sh
```

This compiles the committed Luau fork's existing web entry point, including its
compiler, VM, and type checker. It writes a self-contained module with embedded
WebAssembly to `website/public/runtime/ludot.mjs`. Do not substitute an upstream
Luau package, which would omit Ludot extensions. Runtime files are ignored by Git
and rebuilt in CI. Use `DOCS_BUILD_JOBS` to change the build parallelism.

## Content ownership

- Edit `docs/language.md` for the language guide and engine library guides. The
  generator splits its sections into site topics.
- Edit `docs/building.md`, `docs/shaders.md`, `docs/status.md`, and `docs/themes.md`
  for the existing manuals. Generated copies live in `docs/reference/` here.
- Edit `website/docs/` for site-only guides, overviews, and the playground page.
- Edit engine class XML and regenerate `types/engine.api` for native API changes.
  `scripts/generate.py` builds one page per manifest class. Generated files are
  ignored by Git. Linked summaries expose own members; parent-member tables load from local static JSON when expanded. `docs/api-notes/` supplies maintained Luau usage examples.
- `highlighting/languages.mjs` extends the Luau TextMate grammar with native Ludot
  keywords and a separate lushader grammar. Markdown uses these grammars. The editable CodeMirror editor uses Luau tokenization and shared light/dark colors, with a WebAssembly analyzer worker for IntelliSense.

The generator copies local license notices into the published site. The source
logo retains its attribution. Landing-page stills come from the repository's
Vesper demo capture; they depict real Ludot output.

## Build and verify

After building the browser runtime:

```bash
cd website
npm run docs:generate
npm test
npm run build
npm run preview
```

Tests run all playground presets against the actual WebAssembly compiler,
checker, and VM. They check real diagnostics, syntax token colors, reference
coverage, and search entries. VitePress rejects broken internal links during the
production build. Inspect the landing page, a manual page, a class page, and the
playground at desktop and phone widths after layout changes.

The sandbox runs each action in a separate browser worker, terminates it after
10 seconds, and limits WebAssembly memory to 256 MiB. User code has no engine,
filesystem, or module loader. No execution backend is required. Engine checking mode loads the committed API manifest and provides completion, hover, and inline errors without running a scene. `runtime/intellisense.cpp` shares the standalone analyzer APIs. Browser positions convert UTF-16 offsets to Luau UTF-8 columns.

## GitHub Pages

`.github/workflows/docs.yml` builds on relevant pull requests. Pushes to `main`
and manual dispatch also upload and deploy a Pages artifact. The workflow builds
the runtime from source rather than downloading a precompiled VM.

In the repository's **Settings > Pages**, select **GitHub Actions** as the build
source. The workflow's `github-pages` environment may require approval according
to repository rules. This setting and a successful deployment are required before
the public URL exists. A local build does not verify that deployment.

The workflow uses `/<repository>/` as the base. For a user/organization root site
or a custom domain, set `DOCS_BASE=/` in the build step. For another subpath, use
a leading and trailing slash. Public links, image URLs, search, and the worker
runtime resolve through the configured base. See
[VitePress deployment documentation](https://vitepress.dev/guide/deploy).

Do not commit `node_modules`, generated reference pages, or `.vitepress/dist`.


## Live landing-page demo

The page embeds the actual Vesper project through its experimental single-threaded
web target. Build the web template described in `docs/building.md`, then run
`LUDOT=/absolute/path/to/editor bash website/scripts/export-vesper-web.sh` from the
repository root. The generated `public/vesper/` directory is ignored by Git. The
Pages workflow builds both the editor and template before exporting, so publishing
does not depend on an ignored local build. The shell uses `.htm` to avoid
VitePress dev-server interception of `.html` requests.
