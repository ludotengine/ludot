import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile, readdir } from 'node:fs/promises'

const site = new URL('../', import.meta.url)
const manifest = await readFile(new URL('../types/engine.api', site), 'utf8')

test('generated reference covers the committed manifest', async () => {
	const classes = JSON.parse(
		await readFile(new URL('.generated/classes.json', site), 'utf8'),
	)
	const names = manifest
		.split('\n')
		.filter((line) => line.startsWith('C\t'))
		.map((line) => line.split('\t')[1])
		.sort()
	assert.deepEqual(classes.map((entry) => entry.name).sort(), names)
	const pages = (await readdir(new URL('docs/api/engine/', site)))
		.map((file) => file.replace(/\.md$/, ''))
		.sort()
	assert.deepEqual(pages, names)
	const node = await readFile(
		new URL('docs/api/engine/Node.md', site),
		'utf8',
	)
	assert.match(node, /Read getNode/)
	assert.match(node, /Native name: <code>get_node<\/code>/)
	assert.match(node, /## Signals/)
	assert.match(node, /<p class="inheritance">.*<a href="\.\/Object\.html">Object<\/a><\/p>/)
	assert.match(node, /v-pre/)
})

test('search includes manual topics and converted API methods', async () => {
	const entries = JSON.parse(
		await readFile(new URL('public/search.json', site), 'utf8'),
	)
	assert.ok(
		entries.find((entry) => entry.title === 'Your first Ludot project'),
	)
	assert.ok(
		entries.find(
			(entry) => entry.title === 'Node' && entry.text.includes('getNode'),
		),
	)
	assert.ok(entries.every((entry) => entry.path.startsWith('/docs/')))
})

test('inherited BBCode produces valid escaped Vue HTML', async () => {
	const { parse } = await import('@vue/compiler-dom')
	for (const file of ['AESContext', 'CanvasItem', 'RichTextLabel', 'Node']) {
		const text = await readFile(
			new URL(`docs/api/engine/${file}.md`, site),
			'utf8',
		)
		for (const line of text
			.split('\n')
			.filter((line) =>
				line.startsWith('<div class="api-description"'),
			)) {
			assert.doesNotThrow(
				() => parse(line),
				`${file}: invalid inherited description`,
			)
		}
	}
})

test('API summaries and inherited references stay local', async () => {
	const node = await readFile(
		new URL('docs/api/engine/Node.md', site),
		'utf8',
	)
	assert.match(node, /## Summary/)
	assert.match(node, /class="api-summary-table"/)
	assert.match(node, /<InheritedMembers :owners=/)
	assert.match(node, /href="\.\/Node.html#method-getnode"/)
	assert.match(node, /href="\.\/Object.html#method-/)
	const vector = await readFile(
		new URL('docs/api/engine/Vector3.md', site),
		'utf8',
	)
	assert.match(vector, /href="#constructor-1"/)
	assert.match(vector, /## Usage in Luau/)
	assert.match(vector, /only nil and false are falsey/)
	assert.doesNotMatch(vector, /docs\.godotengine\.org/)
})
