import { createHighlighterCore } from 'shiki/core'
import { createOnigurumaEngine } from 'shiki/engine/oniguruma'
import githubDark from '@shikijs/themes/github-dark'
import githubLight from '@shikijs/themes/github-light'
import { luau, lushader } from './languages.mjs'

export const highlighter = createHighlighterCore({
	themes: [githubDark, githubLight],
	langs: [luau, lushader],
	engine: createOnigurumaEngine(import('shiki/wasm')),
})
