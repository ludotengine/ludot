extends SceneTree
func _initialize():
    var args = OS.get_cmdline_user_args()
    var output = args[0] if args.size() > 0 else ProjectSettings.globalize_path("res://../../build/roblox")
    DirAccess.make_dir_recursive_absolute(output)
    var script = ClassDB.instantiate("LuauScript")
    script.set_source_code('''import "res://scripts/Terrain" as Terrain
import "res://scripts/System" as System
class ExportTerrain extends Node = {
    function mesh(name: string, resolution: number): ArrayMesh?
        for _, world in System.Worlds do
            if world.Name == name and world.Shape ~= nil then
                const shape = table.clone(world.Shape)
                shape.Resolution = resolution
                return Terrain.build(shape).Mesh
            end
            for _, moon in world.Moons or {} do
                if moon.Name == name and moon.Shape ~= nil then
                    const shape = table.clone(moon.Shape)
                    shape.Resolution = resolution
                    return Terrain.build(shape).Mesh
                end
            end
        end
        return nil
    end
}
''')
    if script.reload() != OK:
        quit(1)
        return
    var node = Node.new()
    node.set_script(script)
    for world in ["Ember", "Earth", "Moon", "Mars", "Pluto"]:
        var mesh = node.call("mesh", world, 48)
        if mesh == null:
            quit(1)
            return
        var arrays = mesh.surface_get_arrays(0)
        var data = {"name":world,"resolution":48,"positions":[],"normals":[],"colors":[],"indices":Array(arrays[12])}
        for p in arrays[0]: data.positions.append([p.x,p.y,p.z])
        for n in arrays[1]: data.normals.append([n.x,n.y,n.z])
        for c in arrays[3]: data.colors.append([c.r,c.g])
        var file = FileAccess.open(output.path_join(world+".json"),FileAccess.WRITE)
        file.store_string(JSON.stringify(data))
        file.close()
        print("EXPORTED ",world," ",data.positions.size()," vertices, ",data.indices.size()/3," triangles")
    node.free()
    quit()
