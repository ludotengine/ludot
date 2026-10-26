"""Emit an edit-mode command-bar preview builder without executing ModuleScript requires."""
from pathlib import Path
import argparse,re
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--output',type=Path,default=Path('vesper-preview.luau'))
args=parser.parse_args()
parts=[]
for name in ['Math','Config','Tour','Geometry','Textures','Worlds','Flight','Environment']:
    source=(root/'src'/(name+'.luau')).read_text()
    source=re.sub(r'^const \w+ = require\([^\n]*\)\n','',source,flags=re.M)
    source=source.replace('export type ','type ')
    parts.append(f'const {name} = (function()\n{source}\nend)()\n')
parts.append('''const root = Instance.new("Folder")
root.Name = "VesperPreview"
root:SetAttribute("VesperPreview",true)
root.Parent = workspace
const air = Environment.setup(true)
const system = Worlds.build(root)
workspace.CurrentCamera.CFrame = CFrame.lookAt(Vector3.new(-300,420,980),Vector3.new(120,0,0))
_G.VesperPreview = {System=system,Worlds=Worlds,Flight=Flight,Environment=Environment,Air=air}
print("VESPER_PREVIEW_BUILT",#system.Bodies,#root:GetDescendants())
''')
args.output.parent.mkdir(parents=True,exist_ok=True)
args.output.write_text('\n'.join(parts))
print(args.output)
