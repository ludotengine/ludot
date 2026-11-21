import luauGrammar from '@shikijs/langs/luau'

const base = luauGrammar[0]
const extensions = [
	{
		name: 'keyword.control.ludot',
		match: '\\b(?:const|class|extends|export|private|public|onready|import|from|as|enum|match|with|case|otherwise|to|step)\\b',
	},
	{
		name: 'storage.type.ludot',
		match: '\\b(?:number|boolean|string|int|uint|float|bool|vec[234]|ivec[234]|uvec[234]|bvec[234]|mat[234]|sampler2D|samplerCube|DepthTexture|ScreenTexture|NormalRoughnessTexture)\\b',
	},
	{
		name: 'entity.name.type.ludot',
		match: '\\b(?:[A-Z][A-Za-z0-9_]*|EnumItem|Connection)\\b',
	},
	{ name: 'keyword.operator.ludot', match: '\\?\\?=?|\\?\\.|\\?:|\\|>' },
]

export const luau = {
	...base,
	name: 'luau',
	displayName: 'Luau',
	aliases: ['lua'],
	patterns: [
		{ include: '#comment' },
		{ include: '#string' },
		...extensions,
		...base.patterns,
	],
}

export const lushader = {
	...base,
	name: 'lushader',
	displayName: 'Luau shader',
	scopeName: 'source.lushader',
	aliases: [],
	patterns: [
		{ include: '#comment' },
		{ include: '#string' },
		{
			name: 'keyword.control.lushader',
			match: '\\b(?:shader|canvas_item|spatial|particles|include|export|flat|smooth)\\b',
		},
		{
			name: 'variable.language.lushader',
			match: '\\b(?:Color|Texture|Vertex|Albedo|Velocity|Time|UV|Normal|Roughness|Metallic|Alpha|Emission|CameraPosition|CameraDirection|NodePosition|View|FragCoord|ViewportSize|InverseViewMatrix|InverseProjectionMatrix)\\b',
		},
		{
			name: 'support.function.lushader',
			match: '\\b(?:renderMode|discard|mix|clamp|normalize|texture|sin|cos|pow|floor|float|int|vec[234])(?=\\s*\\()',
		},
		...extensions,
		...base.patterns,
	],
}
