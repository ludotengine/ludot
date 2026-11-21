export const navigation = [
	{
		title: 'Ludot manual',
		items: [
			['Documentation overview', '/docs/'],
			['Install or update', '/docs/guide/installation'],
			['First steps', '/docs/guide/first-project'],
			['Build from source', '/docs/reference/building'],
			['Support & limitations', '/docs/guide/support'],
		],
	},
	{
		title: 'Luau language',
		items: [
			['Language overview', '/docs/luau/'],
			['Classes & properties', '/docs/luau/classes'],
			['Modules & imports', '/docs/luau/imports'],
			['Enums & matching', '/docs/luau/enums'],
			['Operators & control flow', '/docs/luau/operators'],
			['Standard library', '/docs/api/luau'],
			['Playground', '/docs/playground'],
		],
	},
	{
		title: 'Engine · Experimental',
		items: [
			['Engine scripting', '/docs/engine/'],
			['Nodes & scenes', '/docs/engine/scenes'],
			['Values & collections', '/docs/engine/values'],
			['Coordinates & transforms', '/docs/engine/transforms'],
			['Input actions', '/docs/engine/input'],
			['Physics & movement', '/docs/engine/physics'],
			['UI, text & themes', '/docs/engine/ui'],
			['Resources & files', '/docs/engine/resources'],
			['Animation & tweens', '/docs/engine/animation'],
			['Audio', '/docs/engine/audio'],
			['Rendering & materials', '/docs/engine/rendering'],
			['Exporting', '/docs/engine/exporting'],
			['Native API & naming', '/docs/engine/naming'],
			['Tasks & scheduling', '/docs/engine/tasks'],
			['Signals & connections', '/docs/engine/signals'],
			['Hot reload & debugging', '/docs/engine/reload'],
			['Class reference', '/docs/api/'],
			['Node', '/docs/api/engine/Node'],
			['Node2D', '/docs/api/engine/Node2D'],
			['Node3D', '/docs/api/engine/Node3D'],
			['CharacterBody3D', '/docs/api/engine/CharacterBody3D'],
			['Vector3', '/docs/api/engine/Vector3'],
		],
	},
	{
		title: 'Shaders & tools',
		items: [
			['Shader language', '/docs/reference/shaders'],
			['Standalone LSP', '/docs/tools/lsp'],
			['Editor & themes', '/docs/reference/themes'],
			['Editor MCP', '/docs/tools/mcp'],
			['Contributing', '/docs/guide/contributing'],
			['Licenses & attribution', '/docs/guide/licenses'],
		],
	},
]

export function href(path: string) {
	return path.endsWith('/') ? path : `${path}.html`
}
