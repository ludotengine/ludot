import { basicSetup } from 'codemirror'
import { EditorState, Compartment } from '@codemirror/state'
import { EditorView, keymap, hoverTooltip } from '@codemirror/view'
import {
	StreamLanguage,
	HighlightStyle,
	syntaxHighlighting,
} from '@codemirror/language'
import { lua } from '@codemirror/legacy-modes/mode/lua'
import {
	autocompletion,
	type CompletionContext,
} from '@codemirror/autocomplete'
import { indentWithTab } from '@codemirror/commands'
import { linter, lintGutter, forceLinting } from '@codemirror/lint'
import { tags } from '@lezer/highlight'
import { analyzerPosition, editorOffset } from './positions.mjs'

type Query = {
	completions: { label: string; detail?: string; type: string }[]
	hover: string
	diagnostics: {
		line: number
		column: number
		endLine: number
		endColumn: number
		message: string
	}[]
}

export function mountEditor(
	parent: HTMLElement,
	options: {
		code: string
		dark: boolean
		engine: boolean
		runtime: string
		api: string
		change: (code: string) => void
		run: () => void
		status: (message: string) => void
	},
) {
	let engine = options.engine
	let worker: Worker | undefined
	let requestId = 0
	const pending = new Map<
		number,
		{
			resolve: (query: Query) => void
			reject: (error: Error) => void
			timer: ReturnType<typeof setTimeout>
		}
	>()
	const colorMode = new Compartment()
	function stopService() {
		worker?.terminate()
		worker = undefined
		for (const request of pending.values()) {
			clearTimeout(request.timer)
			request.reject(new Error('Language service stopped.'))
		}
		pending.clear()
	}
	function query(code: string, offset: number): Promise<Query> {
		if (code.length > 50000)
			return Promise.reject(
				new Error('Code exceeds the 50,000-character limit.'),
			)
		if (!worker) {
			worker = new Worker(
				new URL('./intellisense.worker.ts', import.meta.url),
				{ type: 'module' },
			)
			worker.onmessage = ({ data }) => {
				const request = pending.get(data.id)
				if (!request) return
				pending.delete(data.id)
				clearTimeout(request.timer)
				if (data.error) request.reject(new Error(data.error))
				else {
					options.status('IntelliSense ready')
					request.resolve(data)
				}
			}
			worker.onerror = () => {
				options.status(
					'IntelliSense could not load. Try editing again.',
				)
				stopService()
			}
		}
		const id = ++requestId
		return new Promise((resolve, reject) => {
			const timer = setTimeout(() => {
				options.status('IntelliSense timed out. Try a smaller program.')
				stopService()
			}, 15000)
			pending.set(id, { resolve, reject, timer })
			worker!.postMessage({
				id,
				code,
				...analyzerPosition(code, offset),
				engine,
				runtime: options.runtime,
				api: options.api,
			})
		})
	}
	async function complete(context: CompletionContext) {
		const word = context.matchBefore(/[A-Za-z_][A-Za-z0-9_]*/)
		const previous = context.state.sliceDoc(
			Math.max(0, context.pos - 1),
			context.pos,
		)
		if (!word && !context.explicit && previous !== '.' && previous !== ':')
			return null
		const code = context.state.doc.toString()
		try {
			const result = await query(code, context.pos)
			if (context.aborted) return null
			return {
				from: word?.from ?? context.pos,
				options: result.completions.map((entry) => ({
					...entry,
					type: entry.detail?.includes('->')
						? 'function'
						: entry.type,
					detail:
						entry.detail?.length && entry.detail.length > 100
							? `${entry.detail.slice(0, 100)}…`
							: entry.detail,
					info: entry.detail,
				})),
				validFor: /^[A-Za-z0-9_]*$/,
			}
		} catch {
			return null
		}
	}
	const parser = StreamLanguage.define({
		...lua,
		token(stream, state) {
			const token = lua.token(stream, state)
			if (token === 'variable' || token === 'variable-2') {
				if (
					/^(const|class|extends|export|public|private|onready|import|from|as|enum|match|with|case|otherwise|to|step|continue)$/.test(
						stream.current(),
					)
				)
					return 'keyword'
				if (
					/^(number|string|boolean|any|unknown|never)$/.test(
						stream.current(),
					)
				)
					return 'typeName'
				if (/^[A-Z]/.test(stream.current())) return 'typeName'
			}
			return token
		},
	})
	const colors = HighlightStyle.define([
		{ tag: tags.keyword, color: 'var(--code-keyword)' },
		{ tag: tags.typeName, color: 'var(--code-type)' },
		{ tag: tags.string, color: 'var(--code-string)' },
		{ tag: tags.number, color: 'var(--code-number)' },
		{ tag: tags.comment, color: 'var(--muted)', fontStyle: 'italic' },
		{
			tag: tags.function(tags.variableName),
			color: 'var(--code-function)',
		},
	])
	const view = new EditorView({
		parent,
		state: EditorState.create({
			doc: options.code,
			extensions: [
				basicSetup,
				parser,
				syntaxHighlighting(colors),
				colorMode.of(EditorView.theme({}, { dark: options.dark })),
				EditorState.tabSize.of(4),
				EditorView.contentAttributes.of({
					'aria-label': 'Editable Ludot Luau code',
					spellcheck: 'false',
				}),
				keymap.of([
					{
						key: 'Mod-Enter',
						run() {
							options.run()
							return true
						},
					},
					indentWithTab,
				]),
				autocompletion({
					override: [complete],
					activateOnTyping: true,
				}),
				lintGutter(),
				linter(
					async (editor) => {
						const doc = editor.state.doc.toString()
						try {
							const result = await query(
								doc,
								editor.state.selection.main.head,
							)
							if (editor.state.doc.toString() !== doc) return []
							return result.diagnostics.map((item) => {
								const from = editorOffset(
									doc,
									item.line,
									item.column,
								)
								const to = Math.max(
									from,
									editorOffset(
										doc,
										item.endLine,
										item.endColumn,
									),
								)
								return {
									from,
									to,
									severity: 'error' as const,
									message: item.message,
								}
							})
						} catch {
							return []
						}
					},
					{ delay: 700 },
				),
				hoverTooltip(
					async (editor, position) => {
						const doc = editor.state.doc.toString()
						const line = editor.state.doc.lineAt(position)
						const word = /[A-Za-z_][A-Za-z0-9_]*/g
						let match: RegExpExecArray | null
						while ((match = word.exec(line.text))) {
							const from = line.from + match.index
							if (
								position < from ||
								position > from + match[0].length
							)
								continue
							try {
								const result = await query(
									doc,
									Math.min(from + 1, doc.length),
								)
								if (!result.hover) return null
								return {
									pos: from,
									end: from + match[0].length,
									create() {
										const dom =
											document.createElement('pre')
										dom.className = 'type-hover'
										dom.textContent = result.hover
										return { dom }
									},
								}
							} catch {
								return null
							}
						}
						return null
					},
					{ hoverTime: 350 },
				),
				EditorView.updateListener.of((update) => {
					if (update.docChanged)
						options.change(update.state.doc.toString())
				}),
			],
		}),
	})
	return {
		check() {
			return query(
				view.state.doc.toString(),
				view.state.selection.main.head,
			)
		},
		setCode(code: string) {
			if (view.state.doc.toString() !== code)
				view.dispatch({
					changes: {
						from: 0,
						to: view.state.doc.length,
						insert: code,
					},
				})
		},
		setEngine(value: boolean) {
			engine = value
			forceLinting(view)
		},
		setDark(value: boolean) {
			view.dispatch({
				effects: colorMode.reconfigure(
					EditorView.theme({}, { dark: value }),
				),
			})
		},
		destroy() {
			stopService()
			view.destroy()
		},
	}
}
