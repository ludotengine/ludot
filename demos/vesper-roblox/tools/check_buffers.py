"""Check the real exported terrain buffers used by the Roblox port."""
from pathlib import Path
import base64,json,math,struct
root=Path(__file__).resolve().parents[1]
data=json.loads((root/'assets/TerrainData.model.json').read_text())
triangles=0
for world in data['Children']:
    seams={}
    for face in world['Children']:
        raw=base64.b64decode(face['Properties']['Value'])
        vertices,indices,sea=struct.unpack_from('<IIf',raw)
        assert indices%3==0 and indices//3<20_000 and vertices<60_000
        assert len(raw)==12+vertices*32+indices*2
        values=[struct.unpack_from('<8f',raw,12+i*32) for i in range(vertices)]
        for p in values:
            assert all(math.isfinite(v) for v in p)
            assert sum(p[i]*p[i+3] for i in range(3))>0,'Normal points inward'
            key=tuple(round(x,5) for x in p[:3])
            if key in seams:
                assert max(abs(a-b) for a,b in zip(seams[key],p))<1e-4,'Split seam'
            seams[key]=p
        ids=struct.unpack_from('<'+'H'*indices,raw,12+vertices*32)
        assert max(ids)<vertices
        for index in range(0,len(ids),3):
            a,b,c=(values[ids[index+j]][:3] for j in range(3))
            u=[b[i]-a[i] for i in range(3)];v=[c[i]-a[i] for i in range(3)]
            n=[u[1]*v[2]-u[2]*v[1],u[2]*v[0]-u[0]*v[2],u[0]*v[1]-u[1]*v[0]]
            assert sum(n[i]*a[i] for i in range(3))>0,'Winding points inward'
        triangles+=indices//3
    print(world['Name'],len(seams),'unique vertices; shared seams match')
print('VESPER_BUFFER_CHECKS_PASSED',triangles,'terrain triangles')
