// CodeMirror offsets use UTF-16; the Ludot analyzer counts UTF-8 bytes.
export function analyzerPosition(source, offset) {
	const before = source.slice(0, offset)
	const start = before.lastIndexOf('\n') + 1
	return {
		line: before.slice(0, start).split('\n').length - 1,
		column: new TextEncoder().encode(before.slice(start)).length,
	}
}

export function editorOffset(source, line, column) {
	const lines = source.split('\n')
	let offset = 0
	for (let index = 0; index < Math.min(line, lines.length - 1); index++) offset += lines[index].length + 1
	const text = lines[Math.min(line, lines.length - 1)]
	let bytes = 0
	for (const character of text) {
		const length = new TextEncoder().encode(character).length
		if (bytes + length > column) break
		bytes += length
		offset += character.length
	}
	return offset
}
