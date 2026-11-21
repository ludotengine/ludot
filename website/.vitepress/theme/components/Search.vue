<script setup lang="ts">
import { computed, nextTick, onMounted, ref } from 'vue'
import { withBase } from 'vitepress'
const emit = defineEmits(['close'])
const query = ref('')
const input = ref<HTMLInputElement>()
const entries = ref<
	{ title: string; path: string; text: string; section: string }[]
>([])
const state = ref('Loading search index…')
const results = computed(() => {
	const terms = query.value.toLowerCase().trim().split(/\s+/).filter(Boolean)
	if (!terms.length) return []
	return entries.value
		.map((entry) => {
			const title = entry.title.toLowerCase()
			const haystack = `${title} ${entry.text}`.toLowerCase()
			return {
				...entry,
				score: terms.every((term) => haystack.includes(term))
					? terms.reduce(
							(score, term) =>
								score +
								(title === term
									? 100
									: title.includes(term)
										? 20
										: 1),
							0,
						)
					: 0,
			}
		})
		.filter((entry) => entry.score > 0)
		.sort((a, b) => b.score - a.score)
		.slice(0, 30)
})
onMounted(async () => {
	await nextTick()
	input.value?.focus()
	try {
		const response = await fetch(withBase('/search.json'))
		if (!response.ok) throw new Error()
		entries.value = await response.json()
		state.value = ''
	} catch {
		state.value = 'Search could not load. Close this dialog and try again.'
	}
})
function trapFocus(event: KeyboardEvent) {
	if (event.key !== 'Tab') return
	const controls = Array.from(
		document.querySelectorAll<HTMLElement>(
			'.search-dialog input, .search-dialog button, .search-dialog a',
		),
	)
	const first = controls[0]
	const last = controls.at(-1)
	if (event.shiftKey && document.activeElement === first) {
		event.preventDefault()
		last?.focus()
	}
	if (!event.shiftKey && document.activeElement === last) {
		event.preventDefault()
		first.focus()
	}
}
</script>

<template>
	<div class="search-backdrop" @click.self="emit('close')">
		<section
			class="search-dialog"
			role="dialog"
			aria-modal="true"
			aria-labelledby="search-title"
			@keydown="trapFocus"
			@keydown.esc="emit('close')"
		>
			<div class="search-heading">
				<h2 id="search-title">Search documentation</h2>
				<button aria-label="Close search" @click="emit('close')">
					Esc
				</button>
			</div>
			<input
				ref="input"
				v-model="query"
				aria-label="Search all documentation and API classes"
				placeholder="Search a class, method, or topic…"
				@keydown.enter="
					results.length &&
					($event.currentTarget as HTMLInputElement).parentElement
						?.querySelector('a')
						?.click()
				"
			/>
			<p class="search-status" role="status">
				{{
					state ||
					(query.trim()
						? `${results.length} results${results.length === 30 ? ' shown' : ''}`
						: 'Search the manual, language guide, and engine API.')
				}}
			</p>
			<div class="search-results">
				<a
					v-for="result in results"
					:key="result.path"
					:href="withBase(result.path)"
					@click="emit('close')"
					><small>{{ result.section }}</small
					><strong>{{ result.title }}</strong
					><span>{{ result.text.slice(0, 150) }}</span></a
				>
			</div>
		</section>
	</div>
</template>
