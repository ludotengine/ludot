<script setup lang="ts">
import { reactive } from 'vue'
import { withBase } from 'vitepress'
const props = defineProps<{ owners: string[] }>()
const pages = reactive<
	Record<string, { html?: string; error?: string; loading?: boolean }>
>({})
async function load(owner: string, event: Event) {
	if (
		!(event.target as HTMLDetailsElement).open ||
		pages[owner]?.html ||
		pages[owner]?.loading
	)
		return
	pages[owner] = { loading: true }
	try {
		const response = await fetch(withBase(`/api-members/${owner}.json`))
		if (!response.ok) throw new Error()
		pages[owner] = await response.json()
	} catch {
		pages[owner] = {
			error: 'Could not load members. Use the parent reference below.',
		}
	}
}
</script>

<template>
	<details
		v-for="owner in props.owners"
		:key="owner"
		class="inherited-members"
		@toggle="load(owner, $event)"
	>
		<summary>{{ owner }} members</summary>
		<p>
			<a :href="withBase(`/docs/api/engine/${owner}.html`)"
				>Open {{ owner }} reference</a
			>
		</p>
		<p v-if="pages[owner]?.loading">Loading members…</p>
		<p v-else-if="pages[owner]?.error">{{ pages[owner].error }}</p>
		<div v-else v-html="pages[owner]?.html"></div>
	</details>
</template>
