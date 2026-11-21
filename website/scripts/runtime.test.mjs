import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'
import createLudot from '../public/runtime/ludot.mjs'
import { createHighlighterCore } from 'shiki/core'
import { createOnigurumaEngine } from 'shiki/engine/oniguruma'
import githubDark from '@shikijs/themes/github-dark'
import { luau, lushader } from '../highlighting/languages.mjs'

const source = await readFile(
	new URL('../.vitepress/theme/components/examples.ts', import.meta.url),
	'utf8',
)
const { examples } = await import(
	`data:text/javascript,${encodeURIComponent(source)}`
)

for (const [name, code] of Object.entries(examples)) {
	test(`fork runtime and checker: ${name}`, async () => {
		const output = []
		const runtime = await createLudot({
			print: (line) => output.push(line),
		})
		const diagnostics = runtime.ccall(
			'checkScript',
			'string',
			['string', 'number'],
			[code, 1],
		)
		assert.equal(diagnostics, '', diagnostics)
		const error = runtime.ccall(
			'executeScript',
			'string',
			['string'],
			[code],
		)
		assert.equal(error, '', error)
		assert.ok(output.length > 0)
		if (name === 'Enums & matching')
			assert.deepEqual(output, [
				'Game in progress',
				'Idle\t1',
				'Running\t2',
				'Finished\t3',
			])
		if (name === 'Classes') assert.deepEqual(output, ['Count:\t8'])
		if (name === 'Ranges & pipelines')
			assert.deepEqual(output, ['Score: 2', 'Score: 4', 'Score: 6'])
		if (name === 'Optional values')
			assert.deepEqual(output, [
				'Sound:\tfalse',
				'Title:\tMy game',
				'No player yet',
			])
	})
}

test('checker and VM report actual errors', async () => {
	const runtime = await createLudot({ print() {} })
	assert.match(
		runtime.ccall(
			'checkScript',
			'string',
			['string', 'number'],
			['const x: number = "wrong"', 1],
		),
		/number.*string/,
	)
	assert.match(
		runtime.ccall(
			'executeScript',
			'string',
			['string'],
			['error("test failure")'],
		),
		/test failure/,
	)
	assert.match(
		runtime.ccall('executeScript', 'string', ['string'], ['const =']),
		/Expected|expected/,
	)
})

test('custom grammars color fork and shader syntax', async () => {
	const highlighter = await createHighlighterCore({
		themes: [githubDark],
		langs: [luau, lushader],
		engine: createOnigurumaEngine(import('shiki/wasm')),
	})
	const script = highlighter.codeToTokens(
		'const count: number = 3\n-- class stays a comment\nprint("hello")',
		{ lang: 'luau', theme: 'github-dark' },
	)
	const firstLine = script.tokens[0]
	assert.ok(new Set(firstLine.map((token) => token.color)).size >= 3)
	const shader = highlighter.codeToTokens(
		'shader spatial\nexport Cutoff: number = 0.5\nAlbedo = vec3(Time)',
		{ lang: 'lushader', theme: 'github-dark' },
	)
	assert.ok(
		new Set(shader.tokens.flat().map((token) => token.color)).size >= 3,
	)
	assert.notEqual(shader.tokens[0][0].color, '#E1E4E8')
	highlighter.dispose()
})

test('IntelliSense infers table members, types, and live diagnostics', async () => {
	const runtime = await createLudot({ print() {}, printErr() {} })
	const query = (code, line, column, api = '') =>
		JSON.parse(
			runtime.ccall(
				'queryScript',
				'string',
				['string', 'number', 'number', 'string'],
				[code, line, column, api],
			),
		)
	const completed = query('const item = { Health = 100 }\nitem.', 1, 5)
	assert.ok(
		completed.completions.find(
			(item) => item.label === 'Health' && item.detail === 'number',
		),
	)
	const checked = query('const health: number = "wrong"\nprint(health)', 1, 8)
	assert.equal(checked.hover, 'number')
	assert.match(checked.diagnostics[0].message, /number.*string/)
	const api = await readFile(
		new URL('../../types/engine.api', import.meta.url),
		'utf8',
	)
	const native = query('const node = Node3D.new()\nnode.', 1, 5, api)
	assert.ok(
		native.completions.find(
			(item) => item.label === 'Position' && item.detail === 'Vector3',
		),
	)
	assert.ok(native.completions.find((item) => item.label === 'getNode'))
})

test('editor positions convert between UTF-16 and analyzer UTF-8', async () => {
	const { analyzerPosition, editorOffset } =
		await import('../.vitepress/theme/components/positions.mjs')
	const text = '-- 😀\nconst title = "é"\nprint(title)'
	for (const offset of [0, 3, 5, 6, 20, text.length]) {
		const position = analyzerPosition(text, offset)
		assert.equal(editorOffset(text, position.line, position.column), offset)
	}
})

test('maintained engine guides and API usage examples type-check', async () => {
	const { readdir } = await import('node:fs/promises')
	const runtime = await createLudot({ print() {}, printErr() {} })
	const api = await readFile(
		new URL('../../types/engine.api', import.meta.url),
		'utf8',
	)
	for (const directory of ['docs/api-notes', 'docs/engine']) {
		for (const file of await readdir(
			new URL(`../${directory}/`, import.meta.url),
		)) {
			if (
				['tasks.md', 'signals.md', 'naming.md', 'reload.md'].includes(
					file,
				)
			)
				continue
			const text = await readFile(
				new URL(`../${directory}/${file}`, import.meta.url),
				'utf8',
			)
			for (const match of text.matchAll(/```luau\n([\s\S]*?)```/g)) {
				const result = JSON.parse(
					runtime.ccall(
						'queryScript',
						'string',
						['string', 'number', 'number', 'string'],
						[match[1], 0, 0, api],
					),
				)
				assert.equal(result.error, undefined, file)
				assert.deepEqual(
					result.diagnostics,
					[],
					`${file}: ${JSON.stringify(result.diagnostics)}`,
				)
			}
		}
	}
})
