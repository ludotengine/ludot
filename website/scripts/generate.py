#!/usr/bin/env python3
"""Build source-derived reference pages. Run from any working directory."""

import html
import json
import re
import shutil
import xml.etree.ElementTree as ET
from pathlib import Path

SITE = Path(__file__).resolve().parents[1]
ROOT = SITE.parent
GENERATED = SITE / ".generated"
API = SITE / "docs/api/engine"
GENERATED.mkdir(exist_ok=True)
API.mkdir(parents=True, exist_ok=True)


def write(path, text, source=None):
    target = SITE / path
    target.parent.mkdir(parents=True, exist_ok=True)
    prefix = f"---\nsource: {source}\n---\n\n" if source else ""
    target.write_text(
        prefix
        + re.sub(
            r"(?m)^( {4})+", lambda match: "\t" * (len(match[0]) // 4), text.strip()
        ).replace("```lua\n", "```luau\n")
        + "\n",
        encoding="utf-8",
    )


def name_case(name, property=False):
    leading = name[: len(name) - len(name.lstrip("_"))]
    words = re.sub(r"([a-z])([A-Z])", r"\1_\2", name.lstrip("_"))
    words = re.sub(r"([A-Z])([A-Z][a-z])", r"\1_\2", words).lower().split("_")
    result = "".join(word.capitalize() for word in words)
    if not property:
        result = result[:1].lower() + result[1:]
    return leading + result


# Use the analyzer's manifest as the authority for available members. XML supplies
# prose, argument names, defaults, and constructor documentation, never extra APIs.
classes = {}
enums = []
for row in (ROOT / "types/engine.api").read_text().splitlines()[1:]:
    fields = row.split("\t")
    kind = fields[0]
    if kind == "C":
        classes[fields[1]] = {
            "name": fields[1],
            "parent": fields[2],
            "builtin": fields[3] == "1",
            "rows": [],
        }
    elif kind in ("G", "E"):
        enums.append(fields)
    elif kind in ("F", "P", "X", "K", "O"):
        classes[fields[1]]["rows"].append(fields)

xmls = {}
for directory in [ROOT / "engine/doc/classes", ROOT / "engine/modules"]:
    for path in sorted(directory.rglob("*.xml")):
        if directory.name == "modules" and "doc_classes" not in path.parts:
            continue
        tree = ET.parse(path).getroot()
        if tree.tag == "class":
            xmls[tree.attrib["name"]] = (tree, path.relative_to(ROOT).as_posix())


def category(entry):
    chain = []
    current = entry
    while current:
        chain.append(current["name"])
        current = classes.get(current["parent"])
    if entry["builtin"]:
        return "Value types"
    if any(name.startswith("Editor") for name in chain):
        return "Editor"
    if "Control" in chain:
        return "UI"
    if any("3D" in name for name in chain):
        return "3D"
    if any("2D" in name for name in chain):
        return "2D"
    if "Resource" in chain:
        return "Resources"
    return "Other"


from reference import generate_reference

for entry in classes.values():
    entry["category"] = category(entry)
index = generate_reference(classes, xmls, name_case, write, SITE, enums)

(GENERATED / "classes.json").write_text(
    json.dumps(index, ensure_ascii=False), encoding="utf-8"
)
# Remove obsolete generated pages when a class is deleted from the manifest.
for path in API.glob("*.md"):
    if path.stem not in classes:
        path.unlink()

# Split the existing hand-maintained language manual without copying its text into
# a second tracked source. Source links point back to the authoritative document.
language = (ROOT / "docs/language.md").read_text()
sections = {}
for match in re.finditer(r"^## (.+)\n([\s\S]*?)(?=^## |\Z)", language, re.M):
    sections[match[1]] = match[2].strip()
plans = {
    "luau/classes": ("Classes and properties", ["Classes"]),
    "luau/imports": ("Modules and imports", ["Imports"]),
    "luau/enums": (
        "Enums and pattern matching",
        ["Enums", "Match expressions and statements"],
    ),
    "luau/operators": (
        "Operators and control flow",
        [
            "Optional access and nil coalescing",
            "Inclusive ranges",
            "Pipelines and precedence",
            "Godot class identity",
            "Tooling for the extended syntax",
        ],
    ),
    "engine/tasks": ("Tasks and scheduling", ["Tasks"]),
    "engine/signals": ("Signals and connections", ["Signals"]),
    "engine/naming": ("Native API and naming", ["Engine naming"]),
    "engine/reload": (
        "Hot reload and debugging",
        ["Hot reload", "Debugging and profiling", "Function hover help"],
    ),
}
for path, (title, included) in plans.items():
    body = "\n\n".join(f"## {heading}\n\n{sections[heading]}" for heading in included)
    write(f"docs/{path}.md", f"# {title}\n\n{body}", "docs/language.md")

for name in ("building", "shaders", "themes", "status"):
    text = (ROOT / f"docs/{name}.md").read_text()
    if name == "shaders":
        text = text.replace("```lua", "```lushader")
    text = text.replace(
        "(../website/docs/guide/installation.md)", "(../guide/installation.md)"
    )
    text = text.replace(
        "(../website/README.md)",
        "(https://github.com/ludotengine/ludot/blob/main/website/README.md)",
    )
    text = (
        text.replace("(language.md", "(../engine/naming.md")
        .replace("(themes.md)", "(themes.md)")
        .replace(
            "(development/language-features-checklist.md)",
            "(language-features-checklist.md)",
        )
    )
    write(f"docs/reference/{name}.md", text, f"docs/{name}.md")
write(
    "docs/reference/language-features-checklist.md",
    (ROOT / "docs/development/language-features-checklist.md").read_text(),
    "docs/development/language-features-checklist.md",
)

# Publish checked-in analyzer declarations with their comments and complete
# signatures. Keep conditional variants clearly named instead of flattening flags.
builtins = (ROOT / "luau/Analysis/src/EmbeddedBuiltinDefinitions.cpp").read_text()
blocks = re.findall(
    r'(kBuiltinDefinition\w+)\s*=\s*R"(BUILTIN_SRC|CLASS_SRC)\(([\s\S]*?)\)\2"',
    builtins,
)
selected = {
    "BaseSrc": "Global functions",
    "Bit32Src": "bit32",
    "MathSrc": "math",
    "OsSrc": "os",
    "CoroutineSrc_DEPRECATED": "coroutine",
    "TableSrc": "table",
    "DebugSrc": "debug",
    "Utf8Src": "utf8",
    "BufferSrc_NOINTEGER": "buffer",
    "VectorSrc": "vector",
}
body = [
    "# Luau standard library",
    "",
    "The language keeps Luau's standard libraries. These declarations come directly from the committed analyzer and describe argument and return types. `T?` accepts nil, `...` is variadic, and generic type packs preserve multiple return values.",
    "",
    "Use direct table iteration, `for key, value in values do`, for ordinary tables. The browser playground exposes VM libraries; engine-specific `task`, `Signal`, `Connection`, and `Enum` are documented separately.",
    "",
    "## Common operations",
    "",
    "| Task | API |",
    "| --- | --- |",
    "| Print a value | `print(value)` |",
    "| Convert text to a number | `tonumber(text)` returns a number or nil |",
    "| Catch an error | `pcall(callback, ...)` returns success followed by results or an error |",
    "| Clamp a value | `math.clamp(value, minimum, maximum)` |",
    "| Build a sequence | `table.insert(values, value)` |",
    "| Join text | `table.concat(values, separator)` |",
    "| Format text | `string.format(format, ...)` |",
    "| Work with UTF-8 | `utf8.len(text)`, `utf8.codes(text)` |",
    "",
    "The Luau `vector` library is distinct from Godot's `Vector2` and `Vector3` types. The `os` library is sandboxed and does not provide process execution or file access. Coroutines alone do not schedule engine frames; use [tasks](../engine/tasks.md) in the engine.",
    "",
]
for var, _, code in blocks:
    suffix = var.removeprefix("kBuiltinDefinition")
    if suffix in selected:
        label = selected[suffix]
        code = re.sub(r"^ {4}", "\t", code.strip(), flags=re.M)
        body += [f"## {label}", "", "```lua\n" + code + "\n```", ""]
body += [
    "## string",
    "",
    "The analyzer registers string overloads in `luau/Analysis/src/BuiltinDefinitions.cpp`; the VM implementation is `luau/VM/src/lstrlib.cpp`.",
    "",
    "| Function | Purpose |",
    "| --- | --- |",
    "| `string.byte(text, start?, finish?)` | Read byte values. |",
    "| `string.char(...)` | Build text from byte values. |",
    "| `string.find(text, pattern, start?, plain?)` | Find a match; returns positions and captures, or nil. |",
    "| `string.match(text, pattern, start?)` | Return the pattern captures. |",
    "| `string.gmatch(text, pattern)` | Iterate over matches. |",
    "| `string.gsub(text, pattern, replacement, count?)` | Replace matches; returns text and replacement count. |",
    "| `string.format(format, ...)` | Format values into text. |",
    "| `string.len(text)` | Count bytes, not Unicode characters. |",
    "| `string.sub(text, start, finish?)` | Slice using byte positions. |",
    "| `string.lower(text)` / `string.upper(text)` | Change ASCII letter casing. |",
    "| `string.rep(text, count, separator?)` | Repeat text. |",
    "| `string.reverse(text)` | Reverse bytes. |",
    "| `string.split(text, separator?)` | Split text into an array. |",
    "",
    "Positions are 1-based; negative positions count from the end. Pattern functions use Lua patterns, not regular expressions. Use `utf8` for Unicode code points.",
    "",
    "## Runtime availability",
    "",
    "The engine installs its own sandbox and module loader. A declaration in the analyzer does not grant filesystem or operating system access. Internal feature-flag variants, experimental integer APIs, and type-function implementation helpers are not presented as stable library contracts. The exact definitions remain in the linked source.",
    "",
    "## Try it",
    "",
    '<Playground example="Optional values" />',
]
write(
    "docs/api/luau.md",
    "\n".join(body),
    "luau/Analysis/src/EmbeddedBuiltinDefinitions.cpp",
)

# Native enum groups are useful when reading reflected signatures and the manifest.
body = [
    "# Native enum groups",
    "",
    "This is the complete set of enum groups recorded by the engine manifest. Names here are native reflection names, including class ownership. The [engine enum guide](../luau/enums.md#engine-enums) explains global `Enum` names, collision resolution, and bitfields.",
    "",
]
for row in enums:
    if row[0] == "G":
        name, bitfield, values = row[1], row[2], row[3:]
    else:
        name, bitfield, values = row[1] + "." + row[2], row[3], row[4:]
    body += [
        f"## {name}",
        "",
        "Bitfield group. Combine numeric `.Value` values with `bit32`."
        if bitfield == "1"
        else "Non-bitfield enum group.",
        "",
        "| Native constant | Value |",
        "| --- | --- |",
    ]
    body += [f"| `{values[i]}` | `{values[i + 1]}` |" for i in range(0, len(values), 2)]
    body += [""]
write("docs/api/enums.md", "\n".join(body), "types/engine.api")

# A small static search index covers full prose and every reflected member name.
search = []
for path in sorted((SITE / "docs").rglob("*.md")):
    if "api-notes" in path.parts:
        continue
    text = path.read_text()
    title = re.search(r"^# (.+)$", text, re.M)
    if not title:
        continue
    relative = path.relative_to(SITE).as_posix()
    route = "/" + relative.removesuffix(".md") + ".html"
    if route.endswith("/index.html"):
        route = route.removesuffix("index.html")
    if relative.startswith("docs/api/engine/"):
        entry = classes[path.stem]
        brief = next(item["brief"] for item in index if item["name"] == path.stem)
        content = (
            brief
            + " "
            + " ".join(
                row[2] + " " + name_case(row[2], row[0] in ("P", "X"))
                for row in entry["rows"]
            )
        )
        section = "Engine API"
    else:
        content = re.sub(r"---[\s\S]*?---|<[^>]+>|[`#*\[\]]", " ", text)
        content = re.sub(r"\s+", " ", content).strip()
        section = "Language & libraries" if "/luau" in relative else "Manual & tools"
    search.append(
        {"title": title[1], "path": route, "text": content, "section": section}
    )
(SITE / "public/search.json").write_text(
    json.dumps(search, ensure_ascii=False, separators=(",", ":")), encoding="utf-8"
)
print(
    f"Generated {len(classes)} class references, {len(enums)} enum groups, and {len(search)} searchable pages."
)

# License notices travel with the static browser runtime and copied reference.
licenses = SITE / "public/licenses"
licenses.mkdir(exist_ok=True)
for original, destination in [
    ("LICENSE", "ludot.txt"),
    ("engine/LICENSE.txt", "godot.txt"),
    ("luau/LICENSE.txt", "luau.txt"),
    ("luau/lua_LICENSE.txt", "lua.txt"),
]:
    shutil.copyfile(ROOT / original, licenses / destination)

shutil.copyfile(ROOT / "types/engine.api", SITE / "public/engine.api")
