import { defineConfig } from 'vitepress'
import { luau, lushader } from '../highlighting/languages.mjs'

const base = process.env.DOCS_BASE || '/ludot/'

export default defineConfig({
	title: 'Ludot',
	titleTemplate: ':title · Ludot documentation',
	description:
		'Learn Ludot, the Luau language fork, native engine scripting, shaders, and tools.',
	base,
	cleanUrls: false,
	appearance: 'dark',
	head: [
		[
			'link',
			{ rel: 'icon', type: 'image/svg+xml', href: `${base}logo.svg` },
		],
	],
	markdown: {
		languages: [
			luau,
			lushader,
			'json',
			'bash',
			'ini',
			'python',
			'cpp',
			'glsl',
		],
		theme: { dark: 'github-dark', light: 'github-light' },
	},
	srcExclude: [
		'README.md',
		'node_modules/**',
		'runtime/**',
		'docs/api-notes/**',
	],
	vite: { build: { chunkSizeWarningLimit: 1000 } },
	transformPageData(page) {
		if (page.relativePath.startsWith('docs/api/engine/')) {
			page.frontmatter.api = true
		}
	},
})
