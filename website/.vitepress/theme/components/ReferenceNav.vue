<script setup lang="ts">
import { computed, ref } from 'vue'
import { useRoute, withBase } from 'vitepress'
import { PhMagnifyingGlass } from '@phosphor-icons/vue'
import classes from '../../../.generated/classes.json'
const route = useRoute()
const query = ref('')
const current = computed(() =>
	decodeURIComponent(
		route.path
			.split('/')
			.at(-1)
			?.replace(/\.html$/, '') || '',
	),
)
const categories = ['2D', '3D', 'UI', 'Resources', 'Editor', 'Other']
const filtered = computed(() =>
	classes.filter((entry) =>
		`${entry.name} ${entry.brief}`
			.toLowerCase()
			.includes(query.value.toLowerCase()),
	),
)
const currentEntry = computed(() =>
	classes.find((entry) => entry.name === current.value),
)
</script>

<template>
	<a class="sidebar-home" :href="withBase('/docs/')">Documentation</a>
	<label class="sidebar-filter"
		><PhMagnifyingGlass :size="15" aria-hidden="true" /><input
			v-model="query"
			type="search"
			aria-label="Search engine reference"
			placeholder="Search reference"
	/></label>
	<nav class="reference-navigation" aria-label="Engine reference">
		<h2>Engine API</h2>
		<a :href="withBase('/docs/api/')">Overview</a>
		<details :open="!currentEntry?.builtin || !!query">
			<summary>Classes</summary>
			<details
				v-for="category in categories"
				:key="category"
				:open="currentEntry?.category === category || !!query"
			>
				<summary>{{ category }}</summary>
				<a
					v-for="entry in filtered.filter(
						(item) => !item.builtin && item.category === category,
					)"
					:key="entry.name"
					:href="withBase(`/docs/api/engine/${entry.name}.html`)"
					:class="{ active: current === entry.name }"
					:aria-current="current === entry.name ? 'page' : undefined"
					>{{ entry.name }}</a
				>
			</details>
		</details>
		<details :open="currentEntry?.builtin || !!query">
			<summary>Data types</summary>
			<a
				v-for="entry in filtered.filter((item) => item.builtin)"
				:key="entry.name"
				:href="withBase(`/docs/api/engine/${entry.name}.html`)"
				:class="{ active: current === entry.name }"
				:aria-current="current === entry.name ? 'page' : undefined"
				>{{ entry.name }}</a
			>
		</details>
		<a :href="withBase('/docs/api/enums.html')">Enums</a>
		<h2>Luau libraries</h2>
		<a :href="withBase('/docs/api/luau.html')">Standard library</a>
		<a :href="withBase('/docs/engine/tasks.html')">task</a>
		<a :href="withBase('/docs/api/engine/Signal.html')"
			>Signal &amp; Connection</a
		>
		<a :href="withBase('/docs/luau/enums.html#engine-enums')"
			>Enum &amp; EnumItem</a
		>
	</nav>
</template>
