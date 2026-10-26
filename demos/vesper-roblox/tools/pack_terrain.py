"""Pack Ludot's terrain export into the Roblox StringValue asset model."""
import argparse
import base64
import hashlib
import json
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("input", nargs="?", type=Path, default=ROOT.parents[1] / "build/roblox")
args = parser.parse_args()
recipes = {
    "Ember": (9, .09, None), "Earth": (14, .06, .04), "Moon": (3.4, .07, None),
    "Mars": (11, .1, None), "Pluto": (7, .045, None),
}
children, manifest = [], {}
for name, (radius, relief, sea) in recipes.items():
    data = json.loads((args.input / (name + ".json")).read_text())
    n = data["resolution"]
    heights = [((sum(c * c for c in p) ** .5) / radius - 1) / relief for p in data["positions"]]
    low, high = min(heights), max(heights)
    normalized_sea = (sea - low) / (high - low) if sea is not None else -1
    faces = []
    for side in range(6):
        triangles = data["indices"][side * n * n * 6:(side + 1) * n * n * 6]
        lookup, ids, indices = {}, [], []
        for old in triangles:
            if old not in lookup:
                lookup[old] = len(ids)
                ids.append(old)
            indices.append(lookup[old])
        blob = bytearray(struct.pack("<IIf", len(ids), len(indices), normalized_sea))
        for old in ids:
            blob.extend(struct.pack("<8f", *data["positions"][old], *data["normals"][old], *data["colors"][old]))
        # Godot is clockwise; Roblox front faces are counterclockwise.
        for i in range(0, len(indices), 3):
            blob.extend(struct.pack("<3H", indices[i], indices[i + 2], indices[i + 1]))
        faces.append({"Name": f"Face{side + 1}", "ClassName": "StringValue",
                      "Properties": {"Value": base64.b64encode(blob).decode()}})
    children.append({"Name": name, "ClassName": "Folder", "Children": faces})
    manifest[name] = {
        "resolution": n, "triangles": len(data["indices"]) // 3,
        "source_vertices": len(data["positions"]), "sea_normalized": normalized_sea,
        "sha256": hashlib.sha256(json.dumps(data, sort_keys=True).encode()).hexdigest(),
    }
output = ROOT / "src/TerrainData.model.json"
output.write_text(json.dumps({"ClassName": "Folder", "Children": children}, separators=(",", ":")) + "\n")
(ROOT / "assets/manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
print("Packed", len(children), "original terrain meshes;", output.stat().st_size, "bytes")
