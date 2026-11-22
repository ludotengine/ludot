"""Render the committed engine API as linked, Luau-oriented reference pages."""

import html
import json
import re
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'tools'))
from ludot_lsp.documentation import Documentation, example, select_examples

KINDS = {
    "P": "property",
    "F": "method",
    "X": "signal",
    "K": "constant",
    "O": "operator",
}
PRIMITIVES = {
    "float": "number",
    "int": "number",
    "bool": "boolean",
    "String": "string",
    "StringName": "string",
    "NodePath": "string",
    "void": "()",
    "Nil": "nil",
    "Variant": "any",
}


def anchor(kind, name):
    return kind + "-" + re.sub(r"[^a-z0-9_-]", "-", name.lower())


class Reference:
    def __init__(self, classes, xmls, casing, enum_rows):
        self.classes = classes
        self.xmls = xmls
        self.casing = casing
        self.examples = Documentation().examples
        self.enum_groups = {
            row[1] if row[0] == "G" else row[1] + "." + row[2] for row in enum_rows
        }
        self.symbols = {}
        self.details = {}
        for owner, entry in classes.items():
            symbols = self.symbols[owner] = {}
            details = self.details[owner] = {}
            if owner in xmls:
                for kind, path in [
                    ("F", "methods/method"),
                    ("P", "members/member"),
                    ("X", "signals/signal"),
                    ("K", "constants/constant"),
                ]:
                    for node in xmls[owner][0].findall(path):
                        details[(kind, node.attrib["name"])] = node
                        if kind == "K":
                            details[(kind, casing(node.attrib["name"], True))] = node
            for row in entry["rows"]:
                kind = row[0]
                name = self.name(row)
                symbols[(kind, row[2])] = name
                symbols[(kind, name)] = name
                if kind == "K":
                    xml = xmls.get(owner)
                    if xml:
                        for constant in xml[0].findall("constants/constant"):
                            raw = constant.attrib["name"]
                            if casing(raw, True) == name:
                                symbols[(kind, raw)] = name

    def name(self, row):
        return (
            self.casing(row[2], row[0] in ("P", "X"))
            if row[0] not in ("K", "O")
            else row[2]
        )

    def resolve(self, owner, kind, native):
        while owner in self.classes:
            name = self.symbols[owner].get((kind, native))
            if name:
                return owner, name
            owner = self.classes[owner]["parent"]
        return None

    def member_link(self, owner, kind, native, label=None):
        resolved = self.resolve(owner, kind, native)
        if not resolved:
            return "<code>" + html.escape(label or native) + "</code>"
        owner, name = resolved
        return f'<a href="./{owner}.html#{anchor(KINDS[kind], name)}"><code>{html.escape(label or name)}</code></a>'

    def type_name(self, value):
        optional = value.endswith("?")
        value = value.removesuffix("?")
        if value.startswith("enum::"):
            label = "EnumItem"
        elif value.startswith("bitfield::"):
            label = "number"
        elif value.startswith("typedarray::"):
            label = "Array"
        else:
            label = PRIMITIVES.get(value, value)
        return label + ("?" if optional else "")

    def type_link(self, value):
        label = self.type_name(value)
        raw = value.removesuffix("?")
        if raw.startswith(("enum::", "bitfield::")):
            group = raw.split("::", 1)[1]
            if group not in self.enum_groups:
                return "<code>" + html.escape(label) + "</code>"
            target = "../enums.html#" + re.sub(r"[^a-z0-9_-]", "-", group.lower())
            return f'<a href="{target}" title="Native enum group: {html.escape(group)}"><code>{label}</code></a>'
        if raw.startswith("typedarray::"):
            return f'<a href="./Array.html"><code>Array</code></a>&lt;{self.type_link(raw.split("::", 1)[1])}&gt;'
        if raw in self.classes:
            return f'<a href="./{raw}.html"><code>{label}</code></a>'
        if raw in ("Enum", "EnumItem"):
            return (
                f'<a href="../../luau/enums.html#engine-enums"><code>{label}</code></a>'
            )
        return (
            f'<a href="../../engine/values.html"><code>{html.escape(label)}</code></a>'
        )

    def prose(self, text, owner, member=''):
        """Escape source, resolve native references locally, and protect code spans."""
        snippets = []
        key = owner + ('.' + member if member else '')
        text = select_examples(text, example(key, self.examples))

        def protect(content):
            snippets.append(content)
            return f"LUDOTSNIPPET{len(snippets) - 1}END"

        def block(match):
            language, content = match[1], match[2]
            lines = content.strip("\n").splitlines()
            indent = min(
                (len(line) - len(line.lstrip()) for line in lines if line.strip()),
                default=0,
            )
            code = html.escape("\n".join(line[indent:] for line in lines)).replace(
                "\n", "&#10;"
            )
            label = 'Luau' if language == 'codeblock lang=luau' else 'Upstream'
            return protect(
                f'<details class="upstream-example"><summary>{label} example</summary><pre><code>{code}</code></pre></details>'
            )

        text = re.sub(
            r"\[(codeblock lang=luau|codeblock|gdscript|csharp)(?:[^\]]*)\]([\s\S]*?)\[/(?:codeblock|gdscript|csharp)\]", block, text
        )
        text = html.escape(text)
        text = re.sub(
            r"\[code(?:\s+[^\]]*)?\]([\s\S]*?)\[/code\]",
            lambda match: protect("<code>" + match[1] + "</code>"),
            text,
        )

        def reference(match):
            kind, target = match[1], match[2]
            if kind == "param":
                return "<code>" + target + "</code>"
            if kind == "enum":
                qualified = (
                    target if target in self.enum_groups else owner + "." + target
                )
                if qualified not in self.enum_groups:
                    return "<code>" + html.escape(target) + "</code>"
                return f'<a href="../enums.html#{re.sub(r"[^a-z0-9_-]", "-", qualified.lower())}"><code>{target}</code></a>'
            ref_owner, native = (
                target.rsplit(".", 1) if "." in target else (owner, target)
            )
            row_kind = {"method": "F", "member": "P", "signal": "X", "constant": "K"}[
                kind
            ]
            return self.member_link(ref_owner, row_kind, native)

        text = re.sub(
            r"\[(method|member|signal|constant|enum|param) ([^\]]+)\]", reference, text
        )
        for tag, output in [("b", "strong"), ("i", "em"), ("kbd", "kbd")]:
            text = text.replace(f"[{tag}]", f"<{output}>").replace(
                f"[/{tag}]", f"</{output}>"
            )
        text = re.sub(
            r"\[([A-Za-z][A-Za-z0-9]*)\]",
            lambda m: (
                self.type_link(m[1])
                if m[1] in self.classes or m[1] in PRIMITIVES
                else m[0]
            ),
            text,
        )

        def url(match):
            address, label = match[1], match[2]
            if "$DOCS_URL" in address or "docs.godotengine.org" in address:
                topic = self.local_topic(address)
                if topic:
                    return f'<a href="../../engine/{topic}.html">{label}</a>'
                return label + ' <small class="upstream-note">upstream topic</small>'
            if address.startswith(("https://", "http://")):
                return f'<a href="{address}">{label}</a>'
            return label

        text = re.sub(r"\[url=([^\]]+)\](.*?)\[/url\]", url, text, flags=re.S)
        text = re.sub(
            r"\[/?(?:codeblocks|center|u|s|color[^\]]*|font[^\]]*)\]", "", text
        )
        # Source prose sometimes describes GDScript truthiness, which differs from Luau.
        text = re.sub(
            r"In a boolean context,[\s\S]*?(?=\n|$)",
            "In Luau, only nil and false are falsey. An engine value, including a zero vector or empty collection, is truthy.",
            text,
        )
        text = re.sub(r"\[br\]", "<br />", text)
        paragraphs = [
            part.strip() for part in re.split(r"\n\s*\n|\n\t+", text) if part.strip()
        ]
        result = "".join("<p>" + part + "</p>" for part in paragraphs)
        for index, content in enumerate(snippets):
            token = f"LUDOTSNIPPET{index}END"
            if content.startswith("<details"):
                result = result.replace("<p>" + token + "</p>", content).replace(
                    token, "</p>" + content + "<p>"
                )
            else:
                result = result.replace(token, content)
        return (
            '<div class="api-description" v-pre>'
            + result.replace("<p></p>", "").replace("\n", " ")
            + "</div>"
        )

    @staticmethod
    def local_topic(address):
        for needles, topic in [
            (("math/", "vector", "transform"), "transforms"),
            (("nodes_and_scenes", "scene", "groups", "instancing"), "scenes"),
            (("input",), "input"),
            (("gui/", "ui/", "control", "theme", "font"), "ui"),
            (("physics", "collision", "character"), "physics"),
            (("resource", "file", "saving", "io/"), "resources"),
            (("animation", "tween"), "animation"),
            (("audio",), "audio"),
            (
                (
                    "2d/",
                    "3d/",
                    "rendering",
                    "shading",
                    "environment",
                    "material",
                    "light",
                ),
                "rendering",
            ),
            (("export", "deploy"), "exporting"),
        ]:
            if any(needle in address for needle in needles):
                return topic
        return None

    def detail(self, owner, row):
        return self.details[owner].get((row[0], row[2]))

    def signature(self, owner, row, linked=False):
        name = self.name(row)
        render_type = self.type_link if linked else self.type_name
        escape = html.escape if linked else lambda value: value
        label = self.member_link(owner, row[0], row[2]) if linked else name
        if row[0] == "P":
            return label + ": " + render_type(row[3])
        if row[0] == "X":
            arguments = ", ".join(render_type(t) for t in row[3:])
            return (
                label
                + ": Signal"
                + ("&lt;" if linked else "<")
                + arguments
                + ("&gt;" if linked else ">")
            )
        if row[0] == "K":
            return label + ": " + render_type(row[3])
        if row[0] == "O":
            return label + "(" + render_type(row[3]) + ") → " + render_type(row[4])
        detail = self.detail(owner, row)
        params = detail.findall("param") if detail is not None else []
        arguments = []
        for index, kind in enumerate(row[6:]):
            param = params[index] if index < len(params) else None
            label = param.attrib["name"] if param is not None else f"arg{index + 1}"
            arguments.append(escape(label) + ": " + render_type(kind))
        if row[5] == "1":
            arguments.append("...: any")
        prefix = owner + "." if row[4] == "1" else ""
        return (
            prefix + label + "(" + ", ".join(arguments) + ") → " + render_type(row[3])
        )

    def table(self, owner, rows):
        items = []
        for row in rows:
            target = f"./{owner}.html#{anchor(KINDS[row[0]], self.name(row))}"
            detail = self.detail(owner, row)
            flags = detail.attrib.get("qualifiers", "") if detail is not None else ""
            badge = (
                '<span class="member-tag">callback</span>' if "virtual" in flags else ""
            )
            items.append(
                f'<div class="api-summary-row"><a class="member-permalink" href="{target}" aria-label="Read {html.escape(self.name(row))}">#</a><div class="api-summary-signature">{self.signature(owner, row, True)}</div>{badge}</div>'
            )
        return '<div class="api-summary-table" v-pre>' + "".join(items) + "</div>"


def generate_reference(classes, xmls, casing, write, site, enum_rows):
    reference = Reference(classes, xmls, casing, enum_rows)
    index = []
    member_data = site / "public/api-members"
    member_data.mkdir(exist_ok=True)
    for name, entry in sorted(classes.items()):
        xml, source = xmls.get(name, (None, "types/engine.api"))
        raw_brief = (
            "".join(xml.find("brief_description").itertext()).strip()
            if xml is not None and xml.find("brief_description") is not None
            else "Native engine type."
        )
        brief = re.sub(r"\[/?[^\]]+\]", "", raw_brief)
        category = entry["category"]
        index.append(
            {
                "name": name,
                "parent": entry["parent"],
                "category": category,
                "brief": brief,
                "builtin": entry["builtin"],
            }
        )
        if name == "Signal":
            index[-1]["brief"] = (
                "Typed signals and connections for Luau and engine events."
            )
            signal_section = re.search(
                r"^## Signals\n([\s\S]*?)(?=^## |\Z)",
                (site.parent / "docs/language.md").read_text(),
                re.M,
            )[1]
            write(
                "docs/api/engine/Signal.md",
                "# Signal\n\n" + signal_section,
                "engine/modules/ludot/luau_signal.cpp",
            )
            continue
        parts = [
            f'<div class="reference-kind">Engine {"data type" if entry["builtin"] else "class"}</div>',
            "",
            f"# {name}",
            "",
            '<p class="lead">' + html.escape(brief) + "</p>",
            "",
        ]
        chain = []
        parent = entry["parent"]
        while parent in classes:
            chain.append(f'<a href="./{parent}.html">{parent}</a>')
            parent = classes[parent]["parent"]
        if chain:
            parts += [
                '<p class="inheritance"><span class="inheritance-label">Inherits</span>'
                + " → ".join(chain)
                + "</p>",
                "",
            ]
        if category == "Editor":
            parts += [
                "::: warning Editor API",
                "Available to editor integrations. Luau editor tool scripts are not supported.",
                ":::",
                "",
            ]
        constructors = (
            xml.findall("constructors/constructor") if xml is not None else []
        )
        parts += ["## Summary", ""]
        if constructors:
            parts += ["#### Constructors", ""]
            signatures = []
            for position, constructor in enumerate(constructors):
                arguments = ", ".join(
                    html.escape(param.attrib["name"])
                    + ": "
                    + reference.type_link(param.attrib["type"])
                    for param in constructor.findall("param")
                )
                signatures.append(
                    f'<div class="api-summary-row"><a class="member-permalink" href="#constructor-{position + 1}" aria-label="Read constructor overload {position + 1}">#</a><div class="api-summary-signature">new({arguments}) → {reference.type_link(name)}</div></div>'
                )
            parts += [
                '<div class="api-summary-table" v-pre>'
                + "".join(signatures)
                + "</div>",
                "",
            ]
        for kind, title in [
            ("P", "Properties"),
            ("F", "Methods"),
            ("X", "Signals"),
            ("K", "Constants"),
            ("O", "Operators"),
        ]:
            rows = [row for row in entry["rows"] if row[0] == kind]
            if rows:
                parts += [f"#### {title}", "", reference.table(name, rows), ""]
        ancestors = []
        parent = entry["parent"]
        while parent in classes:
            ancestors.append(parent)
            parent = classes[parent]["parent"]
        if ancestors:
            parts += [
                "### Inherited members",
                "",
                "<InheritedMembers :owners='" + json.dumps(ancestors) + "' />",
                "",
            ]
        inherited_rows = [row for row in entry["rows"] if row[0] in ("P", "F", "X")]
        (member_data / (name + ".json")).write_text(
            json.dumps({"html": reference.table(name, inherited_rows)})
        )
        note = site / "docs/api-notes" / f"{name}.md"
        if note.exists():
            parts += ["## Usage in Luau", "", note.read_text().strip(), ""]
        if xml is not None and xml.find("description") is not None:
            parts += [
                "## Description",
                "",
                reference.prose(
                    "".join(xml.find("description").itertext()).strip(), name
                ),
                "",
            ]
        if constructors:
            parts += [
                "## Constructors",
                "",
                "Construct values with `.new(...)`. Overloads below accept the listed argument types.",
                "",
            ]
            for position, constructor in enumerate(constructors):
                arguments = ", ".join(
                    param.attrib["name"]
                    + ": "
                    + reference.type_name(param.attrib["type"])
                    for param in constructor.findall("param")
                )
                parts += [
                    f'<h3 id="constructor-{position + 1}" data-member="constructor">new({arguments})</h3>',
                    "",
                    reference.prose(
                        "".join(constructor.find("description").itertext()).strip(),
                        name,
                    ),
                    "",
                ]
        for kind, title in [
            ("P", "Properties"),
            ("F", "Methods"),
            ("X", "Signals"),
            ("K", "Constants"),
            ("O", "Operators"),
        ]:
            rows = [row for row in entry["rows"] if row[0] == kind]
            if not rows:
                continue
            parts += [f"## {title}", ""]
            for row in rows:
                renamed = reference.name(row)
                detail = reference.detail(name, row)
                parts += [
                    f'<h3 id="{anchor(KINDS[kind], renamed)}" data-member="{KINDS[kind]}">{html.escape(renamed)}</h3>',
                    "",
                    '<div class="api-detail-signature" v-pre>'
                    + reference.signature(name, row, True)
                    + "</div>",
                    "",
                ]
                if detail is not None and "virtual" in detail.attrib.get(
                    "qualifiers", ""
                ):
                    parts += [
                        "Implement this callback in an attached Luau class. The engine calls it at the corresponding lifecycle event.",
                        "",
                    ]
                if detail is not None:
                    if "default" in detail.attrib:
                        parts += ["Default: `" + detail.attrib["default"] + "`.", ""]
                    if "value" in detail.attrib:
                        parts += ["Native value: `" + detail.attrib["value"] + "`.", ""]
                    params = detail.findall("param")
                    if params:
                        parts += [
                            '<table class="api-parameters" v-pre><thead><tr><th>Parameter</th><th>Type</th><th>Default</th></tr></thead><tbody>'
                            + "".join(
                                "<tr><td><code>"
                                + html.escape(p.attrib["name"])
                                + "</code></td><td>"
                                + reference.type_link(p.attrib["type"])
                                + "</td><td><code>"
                                + html.escape(p.attrib.get("default", "Required"))
                                + "</code></td></tr>"
                                for p in params
                            )
                            + "</tbody></table>",
                            "",
                        ]
                    body = detail if kind in ("P", "K") else detail.find("description")
                    if body is not None:
                        parts += [
                            reference.prose("".join(body.itertext()).strip(), name, row[2]),
                            "",
                        ]
                parts += [
                    f'<details class="native-spelling"><summary>Native spelling</summary><p>Native name: <code>{html.escape(row[2])}</code></p></details>',
                    "",
                ]
        parts += [
            "## Related guides",
            "",
            "[Scene tree](../../engine/scenes.md) · [Native values](../../engine/values.md) · [API naming](../../engine/naming.md)",
            "",
            '<details class="reference-source"><summary>Source and attribution</summary><p>Generated from <code>types/engine.api</code> and <code>'
            + source
            + '</code>. Engine class descriptions remain under the engine MIT license. <a href="../../guide/licenses.html">Licenses and attribution</a>. Luau examples share their source with editor and language-server hovers.</p></details>',
            "",
        ]
        write(f"docs/api/engine/{name}.md", "\n".join(parts), source)
    return index
